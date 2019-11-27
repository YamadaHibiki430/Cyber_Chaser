/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 DXAudio8.cpp
				DirectX Audio8�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "DXAudio8.hpp"
#include "../Common/ComLibrary.hpp"

#include <mmreg.h>
#include <msacm.h>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msacm32.lib")

//------------------------------------------------------------------------------
//	OGG�g�p��
//------------------------------------------------------------------------------
#ifdef DXA_OGG_OPT
	#include <vorbis/codec.h>
	#include <vorbis/vorbisfile.h>

	#pragma comment(lib, "ogg_static.lib")
	#pragma comment(lib, "vorbis_static.lib")
	#pragma comment(lib, "vorbisfile_static.lib")
#endif

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CDXAudio8::CDXAudio8() : m_pDSound  (NULL), m_pDSBPrimary   (NULL), m_pListener(NULL),
						 m_pDMLoader(NULL), m_pDMPerformance(NULL),
						 m_SuspendPrimary(DSBVOLUME_MAX), m_SuspendSegment(0)
{
	CComLibrary::Initialize();		// COM���C�u����������
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CDXAudio8::~CDXAudio8()
{
	Release();
	CComLibrary::Uninitialize();	// COM���C�u�������
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CDXAudio8::Initialize(const HWND hWnd)
{
	Release();

	try {
		// DirectSound�I�u�W�F�N�g�̐����ƃC���^�t�F�[�X�̎擾
		if(::CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC_SERVER,
							  IID_IDirectSound8, (LPVOID*)&m_pDSound) != S_OK)
			throw TEXT("*** Error - DirectSound�I�u�W�F�N�g�������s(CDXAudio8_Initialize)\n");

		// DirectSound�I�u�W�F�N�g������
		if(m_pDSound->Initialize(NULL) != DS_OK)
			throw TEXT("*** Error - DirectSound�I�u�W�F�N�g���������s(CDXAudio8_Initialize)\n");

		// DirectSound�������x���ݒ�
		if(m_pDSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY) != DS_OK)
			throw TEXT("*** Error - DirectSound�������x���ݒ莸�s(CDXAudio8_Initialize)\n");

		// DirectMusicLoader�I�u�W�F�N�g�̐����ƃC���^�t�F�[�X�̎擾
		if(::CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC_SERVER,
							  IID_IDirectMusicLoader8, (LPVOID*)&m_pDMLoader) != S_OK)
			throw TEXT("*** Error - ���[�_�I�u�W�F�N�g�������s(CDXAudio8_Initialize)\n");

		// DirectMusicPerformance�I�u�W�F�N�g�̐����ƃC���^�t�F�[�X�̎擾
		if(::CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC_SERVER,
							  IID_IDirectMusicPerformance8, (LPVOID*)&m_pDMPerformance) != S_OK)
			throw TEXT("*** Error - �p�t�H�[�}���X�I�u�W�F�N�g�������s(CDXAudio8_Initialize)\n");

		// �p�t�H�[�}���X�̏������ƃf�t�H���g�I�[�f�B�I�p�X�̃Z�b�g�A�b�v
		if(m_pDMPerformance->InitAudio(NULL, (LPDIRECTSOUND*)&m_pDSound, NULL,
									   DMUS_APATH_DYNAMIC_STEREO, 64, DMUS_AUDIOF_ALL, NULL) != S_OK)
			throw TEXT("*** Error - �p�t�H�[�}���X���������s(CDXAudio8_Initialize)\n");

		// �v���C�}���o�b�t�@�擾
		DSBUFFERDESC   dsbd;
		::ZeroMemory(&dsbd, sizeof(dsbd));
		dsbd.dwSize        = sizeof(dsbd);
		dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
		dsbd.dwBufferBytes = 0;
		dsbd.lpwfxFormat   = NULL;
		if(FAILED(m_pDSound->CreateSoundBuffer(&dsbd, &m_pDSBPrimary, NULL)))
			throw TEXT("*** Error - �v���C�}���o�b�t�@�擾���s(CDXAudio8_Initialize)\n");

		// ���X�i����
		IDirectSound3DListener8*   pDS3DListener;
		if(m_pDSBPrimary->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&pDS3DListener) == S_OK) {
			m_pListener = new CSoundListener(pDS3DListener);
			pDS3DListener->Release();
		} else {
			::OutputDebugString(TEXT("*** Error - ���X�i�擾���s(CDXAudio8_Initialize)\n"));
			m_pListener = new CNullSoundListener();
		}
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
		Release();

		// �������Ɏ��s�����ꍇ�́A���X�i��NULL�I�u�W�F�N�g��ݒ�
		m_pListener = new CNullSoundListener();

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CDXAudio8::Release()
{
	m_ProtectedResource.clear();

	// ���X�i���
	delete m_pListener;
	m_pListener = NULL;

	// �v���C�}���o�b�t�@���
	if(m_pDSBPrimary != NULL) {
		m_pDSBPrimary->Release();
		m_pDSBPrimary = NULL;
	}

	// �Z�O�����g���
	ReleaseAllSegments();

	// �p�t�H�[�}���X�I�u�W�F�N�g���
	if(m_pDMPerformance != NULL) {
		m_pDMPerformance->CloseDown();
		m_pDMPerformance->Release();
		m_pDMPerformance = NULL;
	}

	// ���[�_�I�u�W�F�N�g���
	if(m_pDMLoader != NULL) {
		m_pDMLoader->Release();
		m_pDMLoader = NULL;
	}

	// �o�b�t�@���
	ReleaseAllBuffers();

	// DirectSound�I�u�W�F�N�g���
	if(m_pDSound != NULL) {
		m_pDSound->Release();
		m_pDSound = NULL;
	}
}

//------------------------------------------------------------------------------
//	�T�X�y���h����
//------------------------------------------------------------------------------
void CDXAudio8::Suspend()
{ 
	m_SuspendPrimary = GetMasterVolume();	SetMasterVolume(DSBVOLUME_MIN);
	m_SuspendSegment = GetSegmentVolume();	SetSegmentVolume(-10000);
}

//------------------------------------------------------------------------------
//	�}�X�^�[�{�����[���擾
//------------------------------------------------------------------------------
LONG CDXAudio8::GetMasterVolume()
{
	if(m_pDSBPrimary == NULL) {
		::OutputDebugString(TEXT("*** Error - �v���C�}���o�b�t�@������(CDXAudio8_GetMasterVolume)\n"));
		return DSBVOLUME_MIN;
	}

	LONG   volume;
	if(m_pDSBPrimary->GetVolume(&volume) != DS_OK)
		return DSBVOLUME_MAX;
	
	return volume;
}

//------------------------------------------------------------------------------
//	�}�X�^�[�{�����[���ݒ�
//------------------------------------------------------------------------------
void CDXAudio8::SetMasterVolume(LONG inVolume)
{
	if(m_pDSBPrimary == NULL) {
		::OutputDebugString(TEXT("*** Error - �v���C�}���o�b�t�@������(CDXAudio8_SetMasterVolume)\n"));
		return;
	}
	m_pDSBPrimary->SetVolume(inVolume);
}

//------------------------------------------------------------------------------
//	�Z�O�����g�}�X�^�[�{�����[���擾
//------------------------------------------------------------------------------
int CDXAudio8::GetSegmentVolume()
{
	if(m_pDMPerformance == NULL) {
		::OutputDebugString(TEXT("*** Error - �p�t�H�[�}���X������(CDXAudio8_GetSegmentVolume)\n"));
		return 0;
	}

	int   volume;
	if(m_pDMPerformance->GetGlobalParam(GUID_PerfMasterVolume, (LPVOID)&volume, sizeof(volume)) != S_OK)
		return 0;

	return volume;
}

//------------------------------------------------------------------------------
//	�Z�O�����g�}�X�^�[�{�����[���ݒ�
//------------------------------------------------------------------------------
void CDXAudio8::SetSegmentVolume(int inVolume)
{
	if(m_pDMPerformance == NULL) {
		::OutputDebugString(TEXT("*** Error - �p�t�H�[�}���X������(CDXAudio8_GetSegmentVolume)\n"));
		return;
	}
	m_pDMPerformance->SetGlobalParam(GUID_PerfMasterVolume, (LPVOID)&inVolume, sizeof(inVolume));
}

//------------------------------------------------------------------------------
//	�Z�O�����g����
//------------------------------------------------------------------------------
IMusicSegment* CDXAudio8::CreateSegmentFromFile(LPTSTR inFileName)
{
	// MusicSegment�I�u�W�F�N�g����
	IMusicSegment*          pSegment   = NULL;
	IDirectMusicSegment8*   pDMSegment = NULL;
	BYTE*                   pWaveData  = NULL;
	try {
		if(m_pDMLoader == NULL)
			throw TEXT("*** Error - DirectX Audio��������(CDXAudio8_CreateSegmentFromFile)\n");

		// DirectMusicSegment�I�u�W�F�N�g����
		HRESULT   hr;
#ifdef UNICODE
		// �t�@�C���ǂݍ���
		hr = m_pDMLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8,
											 inFileName, (LPVOID*)&pDMSegment);
#else
		// �t�@�C������UNICODE�֕ϊ�
		WCHAR   wzFileName[MAX_PATH];
		::MultiByteToWideChar(CP_ACP, 0, inFileName, -1, wzFileName, MAX_PATH);

		// �t�@�C���ǂݍ���
		hr = m_pDMLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8,
											 wzFileName, (LPVOID*)&pDMSegment);
#endif

#ifndef DXA_OGG_OPT
		if(FAILED(hr))
			throw TEXT("*** Error - �t�@�C���ǂݍ��ݎ��s(CDXAudio8_CreateSegmentFromFile)\n");
#else
		// OGG�t�@�C���Ƃ��ĊJ��
		if(FAILED(hr)) {
			if(CreateMusicSegmentFromOggFile(inFileName, pDMSegment, pWaveData) == false)
				throw TEXT("*** Error - �t�@�C���ǂݍ��ݎ��s(CDXAudio8_CreateSegmentFromFile)\n");
		}
#endif

		// �W��MIDI�t�@�C���ݒ�
		MUSIC_TIME   mt;
		pDMSegment->GetLength(&mt);
		if(mt != 1)
			pDMSegment->SetParam(GUID_StandardMIDIFile, 0xFFFFFFFF, 0, 0, NULL);

		// �p�t�H�[�}���X�Ƀ_�E�����[�h
		if(pDMSegment->Download(m_pDMPerformance) != S_OK)
			throw TEXT("*** Error - �_�E�����[�h���s(CDXAudio8_CreateSegmentFromFile)\n");

		pSegment = new CMusicSegment(m_pDMLoader, m_pDMPerformance, pDMSegment, pWaveData);
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);

		delete[] pWaveData;
		pSegment = new CNullMusicSegment();
	}
	SafeRelease(pDMSegment);

	// �Z�O�����g���X�g�֒ǉ�
	m_SegmentList.push_back(pSegment);

	return pSegment;
}

//------------------------------------------------------------------------------
//	�Z�O�����g���
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseSegment(IMusicSegment*& pSegment)
{
	m_ProtectedResource.erase(pSegment);	// �v���e�N�g����

	m_SegmentList.remove(pSegment);
	delete pSegment;
	pSegment = NULL;
}

//------------------------------------------------------------------------------
//	�S�Z�O�����g���
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseAllSegments()
{
	std::list<IMusicSegment*>::iterator   list_it = m_SegmentList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_SegmentList.end()) {
			delete *list_it;
			list_it++;
		}
		m_SegmentList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_SegmentList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_SegmentList.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�o�b�t�@����
//------------------------------------------------------------------------------
ISoundBuffer* CDXAudio8::CreateBufferFromFile(LPTSTR inFileName, const bool in3DBuffer)
{
/*
	HMMIO                 hFile      = NULL;
	HACMSTREAM            hACMStream = NULL;	// ACM�X�g���[��
	LPVOID                pFormat    = NULL;	// �t�H�[�}�b�g
	LPVOID                pImage     = NULL;	// �C���[�W

	ISoundBuffer*         pBuffer    = NULL;
	IDirectSoundBuffer8*  pDSBuffer  = NULL;

	try {
		if(m_pDSound == NULL)
			throw TEXT("*** Error - DirectX Audio��������(CDXAudio8_CreateBufferFromFile)\n");

		// �t�@�C���I�[�v��
		hFile = ::mmioOpen(inFileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
		if(hFile == NULL)
			throw TEXT("*** Error - �t�@�C���I�[�v�����s(CDXAudio8_CreateBufferFromFile)\n");

		MMCKINFO   ParentChunk, ChildChunk;
		::ZeroMemory(&ParentChunk, sizeof(ParentChunk));
		::ZeroMemory(&ChildChunk,  sizeof(ChildChunk ));

		// 'WAVE'�`�����N�֐i��
		ParentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if(::mmioDescend(hFile, &ParentChunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'WAVE'�`�����N��������܂���(CDXAudio8_CreateBufferFromFile)\n");

		// 'WAVE'�`�����N��'fmt '�`�����N�֐i��
		ChildChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'fmt '�`�����N��������܂���(CDXAudio8_CreateBufferFromFile)\n");

		// �t�H�[�}�b�g���擾
		pFormat = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pFormat, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - �t�H�[�}�b�g���擾���s(CDXAudio8_CreateBufferFromFile)\n");

		::mmioAscend(hFile, &ChildChunk, 0);	// 'fmt '�`�����N�ޏo

		// 'WAVE'�`�����N��'data'�`�����N�֐i��
		ChildChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'data'�`�����N��������܂���(CDXAudio8_CreateBufferFromFile)\n");

		// ���j�APCM�ɕϊ�
		WAVEFORMATEX   NewFormat;
		::ZeroMemory(&NewFormat, sizeof(NewFormat));

		// �����t�H�[�}�b�g�擾
		NewFormat.wFormatTag = WAVE_FORMAT_PCM;
		if(::acmFormatSuggest(NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, sizeof(NewFormat),
								ACM_FORMATSUGGESTF_WFORMATTAG) != 0)
			throw TEXT("*** Error - �����t�H�[�}�b�g�擾���s(CDXAudio8_CreateBufferFromFile)\n");

		// ACM�X�g���[���I�[�v��
		if(::acmStreamOpen(&hACMStream, NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, NULL, 0, 0,
							ACM_STREAMOPENF_NONREALTIME) != 0)
			throw TEXT("*** Error - ACM�X�g���[���I�[�v�����s(CDXAudio8_CreateBufferFromFile)\n");

		// �t�H�[�}�b�g�ϊ���擾
		DWORD   NewSize;
		if(::acmStreamSize(hACMStream, ChildChunk.cksize, &NewSize, ACM_STREAMSIZEF_SOURCE) != 0)
			throw TEXT("*** Error - �f�R�[�h�T�C�Y�擾���s(CDXAudio8_CreateBufferFromFile)\n");

		// �ϊ����C���[�W�ǂݍ���
		pImage = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pImage, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - �ϊ����C���[�W�ǂݍ��ݎ��s(CDXAudio8_CreateBufferFromFile)\n");

		// �Z�J���_���o�b�t�@����
		DSBUFFERDESC   dsbd;
		::ZeroMemory(&dsbd, sizeof(dsbd));
		dsbd.dwSize          = sizeof(dsbd);
		dsbd.dwBufferBytes   = ChildChunk.cksize;
		dsbd.dwReserved      = 0;
		dsbd.lpwfxFormat     = (LPWAVEFORMATEX)pFormat;
		dsbd.dwFlags         = DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
							   DSBCAPS_CTRLFX   | DSBCAPS_GETCURRENTPOSITION2;
		if(in3DBuffer == false) {
			dsbd.dwFlags        |= DSBCAPS_CTRLPAN;
			dsbd.guid3DAlgorithm = GUID_NULL;
		} else {
			dsbd.dwFlags        |= DSBCAPS_CTRL3D;
			dsbd.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
		}

		// DirectSoundBuffer����
		IDirectSoundBuffer*   pdsb;
		if(FAILED(m_pDSound->CreateSoundBuffer(&dsbd, &pdsb, NULL)))
			throw TEXT("*** Error - DirectSoundBuffer�������s(CDXAudio8_CreateBufferFromFile)\n");

		// DirectSoundBuffer8�擾
		const HRESULT   hr = pdsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pDSBuffer);
		pdsb->Release();
		if(hr != S_OK)
			throw TEXT("*** Error - DirectSoundBuffer8�擾���s(CDXAudio8_CreateBufferFromFile)\n");

		// �o�b�t�@�̃��b�N
		LPVOID    pBuf[2];
		DWORD     BufBytes[2];
		if(pDSBuffer->Lock(0, 0, &pBuf[0], &BufBytes[0], &pBuf[1], &BufBytes[1], DSBLOCK_ENTIREBUFFER)
			!= DS_OK)
			throw TEXT("*** Error - �o�b�t�@���b�N���s(CDXAudio8_CreateBufferFromFile)\n");

		// ACM�X�g���[���w�b�_�ݒ�
		ACMSTREAMHEADER   acmsh;
		::ZeroMemory(&acmsh, sizeof(acmsh));
		acmsh.cbStruct    = sizeof(acmsh);
		acmsh.fdwStatus   = 0;
		acmsh.pbSrc       = (LPBYTE)pImage;			// �ϊ����C���[�W�̈�
		acmsh.cbSrcLength = ChildChunk.cksize;		// �ϊ����C���[�W�̈�̃T�C�Y
		acmsh.pbDst       = (LPBYTE)pBuf[0];		// �ϊ���C���[�W�̈�
		acmsh.cbDstLength = BufBytes[0];			// �ϊ���C���[�W�̈�̃T�C�Y
		if(::acmStreamPrepareHeader(hACMStream, &acmsh, 0) != 0) {
			pDSBuffer->Unlock(pBuf[0], 0, pBuf[1], 0);
			throw TEXT("*** Error - ACM�X�g���[���w�b�_�ݒ莸�s(CDXAudio8_CreateBufferFromFile)\n");
		}

		// �ϊ����Ȃ���o�b�t�@�֓]��
		const MMRESULT   mmr = ::acmStreamConvert(hACMStream, &acmsh, 0);

		::acmStreamUnprepareHeader(hACMStream, &acmsh, 0);
		pDSBuffer->Unlock(pBuf[0], BufBytes[0], pBuf[1], 0);
		if(mmr != 0)
			throw TEXT("*** Error - �C���[�W�ϊ����s(CDXAudio8_CreateBufferFromFile)\n");

		pBuffer = new CSoundBuffer(pDSBuffer, inFileName);
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
		// ��O�����������ꍇ�́ANULL�T�E���h�o�b�t�@�𐶐�
		pBuffer = new CNullSoundBuffer();
	}

	SafeRelease(pDSBuffer);
	delete[] pImage;
	delete[] pFormat;
	if(hACMStream != NULL)	::acmStreamClose(hACMStream, 0);;
	if(hFile      != NULL)	::mmioClose(hFile, 0);
*/

	// DirectSoundBuffer8����
	IDirectSoundBuffer8*  pDSBuffer = CreateDirectSoundBufferFromFile(inFileName, false, in3DBuffer);
	ISoundBuffer*         pBuffer;
	if(pDSBuffer != NULL)
		pBuffer = new CSoundBuffer(pDSBuffer, inFileName);
	else
		pBuffer = new CNullSoundBuffer();

	// �o�b�t�@���X�g�֒ǉ�
	m_BufferList.push_back(pBuffer);
	SafeRelease(pDSBuffer);

	return pBuffer;
}

//------------------------------------------------------------------------------
//	���L�o�b�t�@����
//------------------------------------------------------------------------------
DWORD CDXAudio8::CreateSharedBufferFromFile(LPTSTR inFileName,
											ISoundBuffer* pSoundBufferArray[],
											const DWORD inArrayCount,
											const bool  in3DBuffer)
{
#ifdef _DEBUG
	if(inArrayCount == 0) {
		::OutputDebugString(TEXT("*** Error - �z��v�f���[��(CDXAudio8_CreateSharedBufferFromFile)\n"));
		return 0;
	}
#endif

	ISoundBuffer*   pBuffer;
	DWORD           actual = 0;	// ���ۂɐ����ł�����
	DWORD           index  = 0;	// �z��C���f�b�N�X

	// DirectSoundBuffer8����
	IDirectSoundBuffer8*  pDSBuffer = CreateDirectSoundBufferFromFile(inFileName, true, in3DBuffer);
	if(pDSBuffer != NULL) {
		pBuffer = new CSoundBuffer(pDSBuffer, inFileName);
		actual++;
	} else {
		pBuffer = new CNullSoundBuffer();
	}

	// �o�b�t�@���X�g�֒ǉ�
	m_BufferList.push_back(pBuffer);
	pSoundBufferArray[index] = pBuffer;
	index++;

	// ���L�o�b�t�@����
	IDirectSoundBuffer*    pdsb  = NULL;
	IDirectSoundBuffer8*   pdsb8 = NULL;
	HRESULT                hr;
	while(index < inArrayCount) {
		try {
			if(m_pDSound == NULL)
				throw TEXT("*** Error - DirectX Audio��������(CDXAudio8_CreateSharedBufferFromFile)\n");
			if(pDSBuffer == NULL)
				throw TEXT("*** Error - DirectSoundBuffer8�������s(CDXAudio8_CreateSharedBufferFromFile)\n");

			// ���L�o�b�t�@����
			if(m_pDSound->DuplicateSoundBuffer(pDSBuffer, &pdsb) != DS_OK)
				throw TEXT("*** Error - ���L�o�b�t�@�������s(CDXAudio8_CreateSharedBufferFromFile)\n");
			// IDirectSoundBuffer8�擾
			hr = pdsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pdsb8);
			pdsb->Release();
			if(hr != S_OK)
				throw TEXT("*** Error - DirectSoundBuffer8�擾���s(CDXAudio8_CreateSharedBufferFromFile)\n");

			pBuffer = new CSoundBuffer(pdsb8, TEXT(""));
			pdsb8->Release();
			actual++;
		} catch(LPCTSTR ErrorString) {
			::OutputDebugString(ErrorString);
			pBuffer = new CNullSoundBuffer();
		}

		// �o�b�t�@���X�g�֒ǉ�
		m_BufferList.push_back(pBuffer);
		pSoundBufferArray[index] = pBuffer;
		index++;
	}
	SafeRelease(pDSBuffer);

	return actual;
}

//------------------------------------------------------------------------------
//	�o�b�t�@���
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseBuffer(ISoundBuffer*& pBuffer)
{
	m_ProtectedResource.erase(pBuffer);		// �v���e�N�g����

	m_BufferList.remove(pBuffer);
	delete pBuffer;
	pBuffer = NULL;
}

//------------------------------------------------------------------------------
//	�S�o�b�t�@���
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseAllBuffers()
{
	std::list<ISoundBuffer*>::iterator   list_it = m_BufferList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_BufferList.end()) {
			delete *list_it;
			list_it++;
		}
		m_BufferList.clear();
	} else {
		std::set <void*>::iterator   set_it;
		std::set <void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_BufferList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_BufferList.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

#ifdef DXA_OGG_OPT
//------------------------------------------------------------------------------
//	OGG�t�@�C������IDirectMusicSegment8�C���^�t�F�[�X����
//------------------------------------------------------------------------------
bool CDXAudio8::CreateMusicSegmentFromOggFile(LPTSTR inFileName,
											  IDirectMusicSegment8*& pDMSegment,
											  BYTE*& pWaveImage)
{
/*
	// OGG�t�@�C���I�[�v��
	FILE*       pFile = NULL;
#if UNICODE
	char        file_name[MAX_PATH + 1];
	const int   len = ::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, 0, NULL, NULL);
	::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, len, NULL, NULL);
	::fopen_s(&pFile, file_name,  "rb");
#else
	::fopen_s(&pFile, inFileName, "rb);
#endif
	if(pFile == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		return false;
	}

	// OGG�t�@�C�����擾
	OggVorbis_File   ovf;
	if(::ov_open(pFile, &ovf, NULL, 0) < 0) {
		::OutputDebugString(TEXT("*** Error - OGG�t�@�C�����擾���s(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		::fclose(pFile);
		return false;
	}

	// OGG���擾
	vorbis_info*   vi = ov_info(&ovf, -1);
	if(vi == NULL) {
		::OutputDebugString(TEXT("*** Error - OGG���擾���s(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// WAVE�t�@�C���w�b�_�[�T�C�Y�ݒ�
	struct WAVEFILEHEADER {
		char         cRIFF[4];
		int          iSizeRIFF;
		char         cType[4];
		char         cFmt [4];
		int          iSizeFmt;
		WAVEFORMATEX WaveFmt;
		char         cData[4];
		int          iSizeData;
	} wh;
	const long   wh_size = sizeof(wh.cRIFF)    + sizeof(wh.iSizeRIFF) +
						   sizeof(wh.cType)    + sizeof(wh.cFmt)      +
						   sizeof(wh.iSizeFmt) + sizeof(wh.WaveFmt)   +
						   sizeof(wh.cData)    + sizeof(wh.iSizeData);

	// �C���[�W�T�C�Y�ݒ�
//	const long   data_size = static_cast<long>(::ceil(vi->channels * vi->rate   *
//													  ::ov_time_total(&ovf, -1) * sizeof(WORD)));
	const long   data_size = static_cast<long>(::ov_pcm_total(&ovf, -1)) * vi->channels * sizeof(WORD);

	// �������m��
	pWaveImage = new BYTE[wh_size + data_size];
	if(pWaveImage == NULL) {
		::OutputDebugString(TEXT("*** Error - �������m�ێ��s(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// OGG�f�R�[�h
	int    bitstream;
	long   read_size, write_size = 0;
	while(true) {
		// �f�R�[�h
//		read_size = ov_read(&ovf, (char*)pWaveImage + wh_size + write_size,
//							data_size - write_size, 0, sizeof(WORD), 1, &bitstream);
		read_size = ov_read(&ovf, (char*)pWaveImage + wh_size + write_size,
							4096, 0, sizeof(WORD), 1, &bitstream);
		if(read_size == 0)
			break;	// �f�R�[�h�I��
		if(read_size < 0) {
			// �f�R�[�h���s
			::OutputDebugString(TEXT("*** Error - �f�R�[�h���s(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));

			delete[] pWaveImage;
			pWaveImage = NULL;
			::ov_clear(&ovf);

			return false;
		}

		write_size += read_size;
	}

	// �w�b�_�[������
	::CopyMemory(wh.cRIFF, "RIFF", 4);
	::CopyMemory(wh.cType, "WAVE", 4);
	::CopyMemory(wh.cFmt,  "fmt ", 4);
	::CopyMemory(wh.cData, "data", 4);

	wh.iSizeRIFF = wh_size + write_size - 8;
	wh.iSizeFmt  = sizeof(WAVEFORMATEX);
	wh.iSizeData = write_size;

	wh.WaveFmt.cbSize          = sizeof(WAVEFORMATEX);
	wh.WaveFmt.wFormatTag      = WAVE_FORMAT_PCM;
	wh.WaveFmt.nChannels       = vi->channels;
	wh.WaveFmt.nSamplesPerSec  = vi->rate;
	wh.WaveFmt.nAvgBytesPerSec = vi->rate * vi->channels * sizeof(WORD);
	wh.WaveFmt.nBlockAlign     = vi->channels * sizeof(WORD);
	wh.WaveFmt.wBitsPerSample  = sizeof(WORD) * 8;

	// �m�ۂ����������֓]��
	int  copy_size = 0;
	::CopyMemory(pWaveImage,              wh.cRIFF,     sizeof(wh.cRIFF    ));	copy_size += sizeof(wh.cRIFF    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeRIFF, sizeof(wh.iSizeRIFF));	copy_size += sizeof(wh.iSizeRIFF);
	::CopyMemory(pWaveImage + copy_size,  wh.cType,     sizeof(wh.cType    ));	copy_size += sizeof(wh.cType    );
	::CopyMemory(pWaveImage + copy_size,  wh.cFmt,      sizeof(wh.cFmt     ));	copy_size += sizeof(wh.cFmt     );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeFmt,  sizeof(wh.iSizeFmt ));	copy_size += sizeof(wh.iSizeFmt );
	::CopyMemory(pWaveImage + copy_size, &wh.WaveFmt,   sizeof(wh.WaveFmt  ));	copy_size += sizeof(wh.WaveFmt  );
	::CopyMemory(pWaveImage + copy_size,  wh.cData,     sizeof(wh.cData    ));	copy_size += sizeof(wh.cData    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeData, sizeof(wh.iSizeData));

	// OGG�t�@�C���N���[�Y
	::ov_clear(&ovf);
*/

	// OGG�t�@�C���f�R�[�h
	DWORD   image_size;
	if(DecodeOggFile(inFileName, pWaveImage, image_size) == false) {
		::OutputDebugString(TEXT("*** Error - OGG�t�@�C���f�R�[�h���s(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		return false;
	}

	// �Z�O�����g�p�����[�^�ݒ�
	DMUS_OBJECTDESC  desc;
	::ZeroMemory(&desc, sizeof(desc));
	desc.dwSize      = sizeof(desc);
	desc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_MEMORY;
	desc.guidClass   = CLSID_DirectMusicSegment;
	desc.pbMemData   = pWaveImage;
	desc.llMemLength = image_size;

	// �Z�O�����g����
	if(FAILED(m_pDMLoader->GetObject(&desc, IID_IDirectMusicSegment8, (LPVOID*)&pDMSegment))) {
		::OutputDebugString(TEXT("*** Error - �Z�O�����g�������s(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));

		delete[] pWaveImage;
		pWaveImage = NULL;

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	OGG�t�@�C�����������Ƀf�R�[�h
//------------------------------------------------------------------------------
bool CDXAudio8::DecodeOggFile(LPTSTR inFileName, BYTE*& pWaveImage, DWORD& outImageSize)
{
	pWaveImage   = NULL;
	outImageSize = 0;

	// OGG�t�@�C���I�[�v��
	FILE*       pFile = NULL;
#if UNICODE
	char        file_name[MAX_PATH + 1];
	const int   len = ::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, 0, NULL, NULL);
	::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, len, NULL, NULL);
	::fopen_s(&pFile, file_name,  "rb");
#else
	::fopen_s(&pFile, inFileName, "rb);
#endif
	if(pFile == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CDXAudio8_DecodeOggFile)\n"));
		return false;
	}

	// OGG�t�@�C�����擾
	OggVorbis_File   ovf;
	if(::ov_open(pFile, &ovf, NULL, 0) < 0) {
		::OutputDebugString(TEXT("*** Error - OGG�t�@�C�����擾���s(CDXAudio8_DecodeOggFile)\n"));
		::fclose(pFile);
		return false;
	}

	// OGG���擾
	vorbis_info*   vi = ov_info(&ovf, -1);
	if(vi == NULL) {
		::OutputDebugString(TEXT("*** Error - OGG���擾���s(CDXAudio8_DecodeOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// WAVE�t�@�C���w�b�_�[�T�C�Y�ݒ�
	struct WAVEFILEHEADER {
		char         cRIFF[4];
		int          iSizeRIFF;
		char         cType[4];
		char         cFmt [4];
		int          iSizeFmt;
		WAVEFORMATEX WaveFmt;
		char         cData[4];
		int          iSizeData;
	} wh;
	const long   wh_size = sizeof(wh.cRIFF)    + sizeof(wh.iSizeRIFF) +
						   sizeof(wh.cType)    + sizeof(wh.cFmt)      +
						   sizeof(wh.iSizeFmt) + sizeof(wh.WaveFmt)   +
						   sizeof(wh.cData)    + sizeof(wh.iSizeData);

	// �C���[�W�T�C�Y�ݒ�
	const long   data_size = static_cast<long>(::ov_pcm_total(&ovf, -1)) * vi->channels * sizeof(WORD);

	// �������m��
	pWaveImage = new BYTE[wh_size + data_size];
	if(pWaveImage == NULL) {
		::OutputDebugString(TEXT("*** Error - �������m�ێ��s(CDXAudio8_DecodeOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// OGG�f�R�[�h
	int    bitstream;
	long   read_size, write_size = 0;
	while(true) {
		// �f�R�[�h
		read_size = ov_read(&ovf, (char*)pWaveImage + wh_size + write_size,
							4096, 0, sizeof(WORD), 1, &bitstream);
		if(read_size == 0)
			break;	// �f�R�[�h�I��
		if(read_size < 0) {
			// �f�R�[�h���s
			::OutputDebugString(TEXT("*** Error - �f�R�[�h���s(CDXAudio8_DecodeOggFile)\n"));

			delete[] pWaveImage;
			pWaveImage = NULL;
			::ov_clear(&ovf);

			return false;
		}

		write_size += read_size;
	}

	// �C���[�W�T�C�Y�ݒ�
	outImageSize = wh_size + write_size;

	// �w�b�_�[������
	::CopyMemory(wh.cRIFF, "RIFF", 4);
	::CopyMemory(wh.cType, "WAVE", 4);
	::CopyMemory(wh.cFmt,  "fmt ", 4);
	::CopyMemory(wh.cData, "data", 4);

	wh.iSizeRIFF = outImageSize - 8;		// "RIFF"��"WAVE"�̕�������
	wh.iSizeFmt  = sizeof(WAVEFORMATEX);
	wh.iSizeData = write_size;

	wh.WaveFmt.cbSize          = sizeof(WAVEFORMATEX);
	wh.WaveFmt.wFormatTag      = WAVE_FORMAT_PCM;
	wh.WaveFmt.nChannels       = vi->channels;
	wh.WaveFmt.nSamplesPerSec  = vi->rate;
	wh.WaveFmt.nAvgBytesPerSec = vi->rate * vi->channels * sizeof(WORD);
	wh.WaveFmt.nBlockAlign     = vi->channels * static_cast<WORD>(sizeof(WORD));
	wh.WaveFmt.wBitsPerSample  = sizeof(WORD) * 8;

	// �m�ۂ����������փw�b�_�[����]��
	int  copy_size = 0;
	::CopyMemory(pWaveImage,              wh.cRIFF,     sizeof(wh.cRIFF    ));	copy_size += sizeof(wh.cRIFF    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeRIFF, sizeof(wh.iSizeRIFF));	copy_size += sizeof(wh.iSizeRIFF);
	::CopyMemory(pWaveImage + copy_size,  wh.cType,     sizeof(wh.cType    ));	copy_size += sizeof(wh.cType    );
	::CopyMemory(pWaveImage + copy_size,  wh.cFmt,      sizeof(wh.cFmt     ));	copy_size += sizeof(wh.cFmt     );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeFmt,  sizeof(wh.iSizeFmt ));	copy_size += sizeof(wh.iSizeFmt );
	::CopyMemory(pWaveImage + copy_size, &wh.WaveFmt,   sizeof(wh.WaveFmt  ));	copy_size += sizeof(wh.WaveFmt  );
	::CopyMemory(pWaveImage + copy_size,  wh.cData,     sizeof(wh.cData    ));	copy_size += sizeof(wh.cData    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeData, sizeof(wh.iSizeData));

	// OGG�t�@�C���N���[�Y
	::ov_clear(&ovf);

	return true;
}
#endif

//------------------------------------------------------------------------------
//	DirectSoundBuffer8�C���^�t�F�[�X����
//------------------------------------------------------------------------------
IDirectSoundBuffer8* CDXAudio8::CreateDirectSoundBufferFromFile(LPTSTR inFileName,
																const bool inShared, const bool in3DBuffer)
{
	IDirectSoundBuffer8*  pDSBuffer  = NULL;

	HMMIO                 hFile      = NULL;
	HACMSTREAM            hACMStream = NULL;	// ACM�X�g���[��
	LPVOID                pFormat    = NULL;	// �t�H�[�}�b�g
	LPVOID                pImage     = NULL;	// �C���[�W

	try {
		if(m_pDSound == NULL)
			throw TEXT("*** Error - DirectX Audio��������(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// �t�@�C���I�[�v��
		hFile = ::mmioOpen(inFileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
		if(hFile == NULL)
			throw TEXT("*** Error - �t�@�C���I�[�v�����s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		MMCKINFO   ParentChunk, ChildChunk;
		::ZeroMemory(&ParentChunk, sizeof(ParentChunk));
		::ZeroMemory(&ChildChunk,  sizeof(ChildChunk ));

		// 'WAVE'�`�����N�֐i��
		ParentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if(::mmioDescend(hFile, &ParentChunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'WAVE'�`�����N��������܂���(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// 'WAVE'�`�����N��'fmt '�`�����N�֐i��
		ChildChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'fmt '�`�����N��������܂���(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// �t�H�[�}�b�g���擾
		pFormat = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pFormat, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - �t�H�[�}�b�g���擾���s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		::mmioAscend(hFile, &ChildChunk, 0);	// 'fmt '�`�����N�ޏo

		// 'WAVE'�`�����N��'data'�`�����N�֐i��
		ChildChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'data'�`�����N��������܂���(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// ���j�APCM�ɕϊ�
		WAVEFORMATEX   NewFormat;
		::ZeroMemory(&NewFormat, sizeof(NewFormat));

		// �����t�H�[�}�b�g�擾
		NewFormat.wFormatTag = WAVE_FORMAT_PCM;
		if(::acmFormatSuggest(NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, sizeof(NewFormat),
							  ACM_FORMATSUGGESTF_WFORMATTAG) != 0)
			throw TEXT("*** Error - �����t�H�[�}�b�g�擾���s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// ACM�X�g���[���I�[�v��
		if(::acmStreamOpen(&hACMStream, NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, NULL, 0, 0,
						   ACM_STREAMOPENF_NONREALTIME) != 0)
			throw TEXT("*** Error - ACM�X�g���[���I�[�v�����s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// �t�H�[�}�b�g�ϊ���擾
		DWORD   NewSize;
		if(::acmStreamSize(hACMStream, ChildChunk.cksize, &NewSize, ACM_STREAMSIZEF_SOURCE) != 0)
			throw TEXT("*** Error - �f�R�[�h�T�C�Y�擾���s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// �ϊ����C���[�W�ǂݍ���
		pImage = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pImage, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - �ϊ����C���[�W�ǂݍ��ݎ��s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// �Z�J���_���o�b�t�@����
		DSBUFFERDESC   dsbd;
		::ZeroMemory(&dsbd, sizeof(dsbd));
		dsbd.dwSize          = sizeof(dsbd);
		dsbd.dwBufferBytes   = NewSize;
		dsbd.dwReserved      = 0;
		dsbd.lpwfxFormat     = &NewFormat;
		dsbd.dwFlags         = DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
							   DSBCAPS_GETCURRENTPOSITION2;
		if(inShared   == false) {
			dsbd.dwFlags        |= DSBCAPS_CTRLFX;
		}
		if(in3DBuffer == false) {
			dsbd.dwFlags        |= DSBCAPS_CTRLPAN;
			dsbd.guid3DAlgorithm = GUID_NULL;
		} else {
			dsbd.dwFlags        |= DSBCAPS_CTRL3D;
			dsbd.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
		}

		// DirectSoundBuffer����
		IDirectSoundBuffer*   pdsb;
		if(FAILED(m_pDSound->CreateSoundBuffer(&dsbd, &pdsb, NULL)))
			throw TEXT("*** Error - DirectSoundBuffer�������s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// DirectSoundBuffer8�擾
		const HRESULT   hr = pdsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pDSBuffer);
		pdsb->Release();
		if(hr != S_OK)
			throw TEXT("*** Error - DirectSoundBuffer8�擾���s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// �o�b�t�@���b�N
		LPVOID    pBuf[2];
		DWORD     BufBytes[2];
		if(pDSBuffer->Lock(0, 0, &pBuf[0], &BufBytes[0], &pBuf[1], &BufBytes[1], DSBLOCK_ENTIREBUFFER)
			!= DS_OK)
			throw TEXT("*** Error - �o�b�t�@���b�N���s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// ACM�X�g���[���w�b�_�ݒ�
		ACMSTREAMHEADER   acmsh;
		::ZeroMemory(&acmsh, sizeof(acmsh));
		acmsh.cbStruct    = sizeof(acmsh);
		acmsh.fdwStatus   = 0;
		acmsh.pbSrc       = (LPBYTE)pImage;			// �ϊ����C���[�W�̈�
		acmsh.cbSrcLength = ChildChunk.cksize;		// �ϊ����C���[�W�̈�̃T�C�Y
		acmsh.pbDst       = (LPBYTE)pBuf[0];		// �ϊ���C���[�W�̈�
		acmsh.cbDstLength = BufBytes[0];			// �ϊ���C���[�W�̈�̃T�C�Y
		if(::acmStreamPrepareHeader(hACMStream, &acmsh, 0) != 0) {
			pDSBuffer->Unlock(pBuf[0], 0, pBuf[1], 0);
			throw TEXT("*** Error - ACM�X�g���[���w�b�_�ݒ莸�s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");
		}

		// �ϊ����Ȃ���o�b�t�@�֓]��
		const MMRESULT   mmr = ::acmStreamConvert(hACMStream, &acmsh, 0);

		::acmStreamUnprepareHeader(hACMStream, &acmsh, 0);
		pDSBuffer->Unlock(pBuf[0], BufBytes[0], pBuf[1], 0);
		if(mmr != 0)
			throw TEXT("*** Error - �C���[�W�ϊ����s(CDXAudio8_CreateDirectSoundBufferFromFile)\n");
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
		SafeRelease(pDSBuffer);
	}

	delete[] pImage;
	delete[] pFormat;
	if(hACMStream != NULL)	::acmStreamClose(hACMStream, 0);
	if(hFile      != NULL)	::mmioClose(hFile, 0);

	return pDSBuffer;
}

//------------------------------------------------------------------------------
//	���\�[�X�v���e�N�g�ݒ�
//------------------------------------------------------------------------------
void CDXAudio8::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_ProtectedResource.erase (pResource);		// �v���e�N�g����
	}
}
