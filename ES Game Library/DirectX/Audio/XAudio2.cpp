/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft WindowsXP�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2008

 �y�v���O�����z
	 XAudio2.cpp
				XAudio2�N���X

 �y�o�[�W�����z
	 * Version    2014.03.00      2014/03/14  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "XAudio2.hpp"
#include "../Common/ComLibrary.hpp"

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#ifdef NDEBUG
#pragma comment(lib, "XAPOBase.lib")
#else
#pragma comment(lib, "XAPOBased.lib")
#endif

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CXAudio2::CXAudio2() : m_pXAudio(NULL), m_pMasterVoice(NULL)
{
	CComLibrary::Initialize();		// COM���C�u����������
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CXAudio2::~CXAudio2()
{
	Release();
	CComLibrary::Uninitialize();	// COM���C�u�������
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CXAudio2::Initialize()
{
	Release();

	// XAudio2�C���^�t�F�[�X����
#ifdef NDEBUG
	if(::XAudio2Create(&m_pXAudio) != S_OK) {
#else
	if(::XAudio2Create(&m_pXAudio, XAUDIO2_DEBUG_ENGINE) != S_OK) {
		::OutputDebugString(TEXT("*** Error - XAudio2�C���^�t�F�[�X����(CXAudio2_Initialize)\n"));
#endif
		return false;
	}

	// �}�X�^�[�{�C�X����
	if(m_pXAudio->CreateMasteringVoice(&m_pMasterVoice) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �}�X�^�[�{�C�X����(CXAudio2_Initialize)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CXAudio2::Release()
{
	// �}�X�^�[�{�C�X���
	if(m_pMasterVoice != NULL) {
		m_pMasterVoice->DestroyVoice();
		m_pMasterVoice = NULL;
	}

	SafeRelease(m_pXAudio);
}

//------------------------------------------------------------------------------
//	�}�X�^�[�{�����[���ݒ�
//------------------------------------------------------------------------------
void CXAudio2::SetMasterVolume(const float inVolume)
{
	if(m_pMasterVoice == NULL) {
		::OutputDebugString(TEXT("*** Error - �}�X�^�[�{�C�X��������(CXAudio2_SetMasterVolume)\n"));
		return;
	}

	m_pMasterVoice->SetVolume(inVolume);
}

//------------------------------------------------------------------------------
//	�}�X�^�[�{�����[���擾
//------------------------------------------------------------------------------
float CXAudio2::GetMasterVolume() const
{
	if(m_pMasterVoice == NULL) {
		::OutputDebugString(TEXT("*** Error - �}�X�^�[�{�C�X��������(CXAudio2_GetMasterVolume)\n"));
		return 0.0f;
	}

	float   volume = 0.0f;
	m_pMasterVoice->GetVolume(&volume);

	return volume;
}

//------------------------------------------------------------------------------
//	�T�E���h�{�C�X����
//------------------------------------------------------------------------------
ISoundVoice* CXAudio2::CreateVoiceFromFile(LPTSTR inFileName)
{
	HANDLE   hFile    = INVALID_HANDLE_VALUE;
	HANDLE   hMapFile = NULL;
	void*    pMemFile = NULL;
	try {
		if(m_pXAudio == NULL)
			throw TEXT("*** Error - XAudio2��������(CXAudio2_CreateVoiceFromFile)\n");

		// �t�@�C���I�[�v��
		hFile = ::CreateFile(inFileName, GENERIC_READ,
							 FILE_SHARE_READ, NULL, OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			throw TEXT("*** Error - �t�@�C���I�[�v�����s(CXAudio2_CreateVoiceFromFile)\n");

		// �t�@�C���}�b�s���O�I�u�W�F�N�g����
		hMapFile = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if(hMapFile == NULL)
			throw TEXT("*** Error - �t�@�C���}�b�s���O�I�u�W�F�N�g�������s(CXAudio2_CreateVoiceFromFile)\n");

		// �t�@�C���}�b�s���O
		pMemFile = ::MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
		if(pMemFile == NULL)
			throw TEXT("*** Error - �t�@�C���}�b�s���O���s(CXAudio2_CreateVoiceFromFile)\n");

		// WAVE�t�@�C�����
		BYTE*   pWave = (BYTE*)pMemFile;

		// RIFF
		if(::memcmp(pWave, "RIFF", 4) != 0)
			throw TEXT("*** Error - RIFF�w�b�_�[�G���[(CXAudio2_CreateVoiceFromFile)\n");

		// �f�[�^�T�C�Y
		const DWORD   WAVE_SIZE = *(DWORD*)pWave;
		pWave += 4;

		// WAVE
		if(::memcmp(pWave, "WAVE", 4) != 0)
			throw TEXT("*** Error - WAVE�`�����N�G���[(CXAudio2_CreateVoiceFromFile)\n");

		// �T�u�`�����N
	} catch(LPCTSTR err_str) {
		::OutputDebugString(err_str);

		// NULL
	}

	if(pMemFile != NULL)
		::UnmapViewOfFile(pMemFile);
	if(hMapFile != NULL)
		::CloseHandle(hMapFile);
	if(hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hMapFile);

	return NULL;
}

/*
//------------------------------------------------------------------------------
//	�T�E���h�{�C�X����
//------------------------------------------------------------------------------
ISoundVoice* CXAudio2::CreateVoice(WAVEFORMATEX& inFormat)
{
	IXAudio2SourceVoice*   pSrcVoice;
	const HRESULT   hr = m_pXAudio->CreateSourceVoice(&pSrcVoice, &inFormat);

	ISourceVoice*   pVoice;
	if(hr == S_OK) {
		pVoice = new CSourceVoice(pSrcVoice);
		pSrcVoice->DestroyVoice();
	} else {
		::OutputDebugString(TEXT("*** Error - �\�[�X�{�C�X�������s(CXAudio2_CreateVoice)\n"));
		pVoice = new CNullSourceVoice();
	}

	// �{�C�X���X�g�֒ǉ�
	;

	return pVoice;
}

//------------------------------------------------------------------------------
//	�T�E���h�{�C�X����
//------------------------------------------------------------------------------
ISoundVoice* CXAudio2::CreateVoice(const int inSamplesPerSec,
									const int inBitsPerSample,
									const int inChannels)
{
	WAVEFORMATEX   wav_fmt;
	wav_fmt.wFormatTag      = WAVE_FORMAT_PCM;
	wav_fmt.nChannels       = inChannels;
	wav_fmt.nSamplesPerSec  = inSamplesPerSec;
	wav_fmt.wBitsPerSample  = inBitsPerSample;
	wav_fmt.nBlockAlign     = (wav_fmt.wBitsPerSample * wav_fmt.nChannels) / 8;
	wav_fmt.nAvgBytesPerSec =  wav_fmt.nSamplesPerSec * wav_fmt.nBlockAlign;
	wav_fmt.cbSize          = 0;

	return CreateVoice(wav_fmt);
}
*/