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
	 SoundBuffer.cpp
				�T�E���h�o�b�t�@�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "SoundBuffer.hpp"
#include <mmreg.h>
#include <msacm.h>
#include <cassert>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msacm32.lib")

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CSoundBuffer::CSoundBuffer(IDirectSoundBuffer8* pDSBuffer, LPCTSTR inFileName)
{
	assert(pDSBuffer != NULL);

	m_pDSBuffer = pDSBuffer;
	m_pDSBuffer->AddRef();		// �Q�ƃJ�E���^�C���N�������g

	::ZeroMemory(m_FileName, sizeof(m_FileName));
	if(inFileName != NULL)
		::lstrcpy(m_FileName, inFileName);

	// �R�c�o�b�t�@�ݒ�
	IDirectSound3DBuffer8*   pDS3DBuffer;
	if(m_pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&pDS3DBuffer) == S_OK) {
		m_p3DBuffer = new CSound3DBuffer(pDS3DBuffer);
		pDS3DBuffer->Release();
	} else {
		m_p3DBuffer = new CNullSound3DBuffer();
	}
}

//------------------------------------------------------------------------------
//	�Đ�
//------------------------------------------------------------------------------
bool CSoundBuffer::Play(const DWORD inPriority, const DWORD inFlags)
{
	m_pDSBuffer->SetCurrentPosition(0);

	// �Đ�
	HRESULT   hr = m_pDSBuffer->Play(0, inPriority, inFlags);
	if(hr == DS_OK)
		return true;
	if(hr != DSERR_BUFFERLOST) {
		::OutputDebugString(TEXT("*** Error - �o�b�t�@�Đ����s(CSoundBuffer_Play)\n"));
		return false;
	}

	// �o�b�t�@�𕜌����A�ēx�Đ������݂�
	Restore();
	if(m_pDSBuffer->Play(0, inPriority, inFlags) != DS_OK) {
		::OutputDebugString(TEXT("*** Error - �o�b�t�@�Đ����s(CSoundBuffer_Play)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�ݒ�
//------------------------------------------------------------------------------
bool CSoundBuffer::SetFX(const DWORD inFXFlags)
{
	Stop();

	// �G�t�F�N�g�폜
	m_pDSBuffer->SetFX(0, NULL, NULL);
	if(inFXFlags == 0)
		return true;

	// �G�t�F�N�g�ݒ�
	int            FXCnt = 0;
	DSEFFECTDESC   EffectDesc[DXAFX_MAX];
	::ZeroMemory(EffectDesc, sizeof(EffectDesc));
	for(int i = 0; i < DXAFX_MAX; i++) {
		EffectDesc[i].dwSize        = sizeof(DSEFFECTDESC);
		EffectDesc[i].guidDSFXClass = GUID_NULL;
	}

	// �R�[���X
	if((inFXFlags & DXAFX_CHORUS) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_CHORUS;
		FXCnt++;
	}
	// �R���v���b�V����
	if((inFXFlags & DXAFX_COMPRESSOR) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_COMPRESSOR;
		FXCnt++;
	}
	// �f�B�X�g�[�V����
	if((inFXFlags & DXAFX_DISTORTION) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_DISTORTION;
		FXCnt++;
	}
	// �G�R�[
	if((inFXFlags & DXAFX_ECHO) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_ECHO;
		FXCnt++;
	}
	// �t�����W
	if((inFXFlags & DXAFX_FLANGER) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_FLANGER;
		FXCnt++;
	}
	// �K�[�O��
	if((inFXFlags & DXAFX_GARGLE) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_GARGLE;
		FXCnt++;
	}
	// �����o�[�u
	if((inFXFlags & DXAFX_ENVREVERB) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_I3DL2REVERB;
		FXCnt++;
	}
	// �p�����g���b�N�C�R���C�U
	if((inFXFlags & DXAFX_PARAMEQ) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_PARAMEQ;
		FXCnt++;
	}
	// �~���[�W�b�N���o�[�u
	if((inFXFlags & DXAFX_WAVESREVERB) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_WAVES_REVERB;
		FXCnt++;
	}

	if(FAILED(m_pDSBuffer->SetFX(FXCnt, EffectDesc, NULL))) {
		::OutputDebugString(TEXT("*** Error - �G�t�F�N�g�ݒ莸�s(CSoundBuffer_SetFX)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�{�����[���擾
//------------------------------------------------------------------------------
LONG CSoundBuffer::GetVolume()
{
	LONG   volume;
	if(m_pDSBuffer->GetVolume(&volume) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �{�����[���擾���s(CSoundBuffer_GetVolume)\n"));
		return DSBVOLUME_MAX;
	}

	return volume;
}

//------------------------------------------------------------------------------
//	�p���擾
//------------------------------------------------------------------------------
LONG CSoundBuffer::GetPan()
{
	LONG   pan;
	if(m_pDSBuffer->GetPan(&pan) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �p���擾���s(CSoundBuffer_GetPan)\n"));
		return DSBPAN_CENTER;
	}

	return pan;
}

//------------------------------------------------------------------------------
//	���g���擾
//------------------------------------------------------------------------------
DWORD CSoundBuffer::GetFrequency()
{
	DWORD   freq;
	if(m_pDSBuffer->GetFrequency(&freq) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ���g���擾���s(CSoundBuffer_GetFrequency)\n"));
		return 0;
	}

	return freq;
}

//------------------------------------------------------------------------------
//	�X�e�[�^�X�擾
//------------------------------------------------------------------------------
DWORD CSoundBuffer::GetState()
{
	DWORD   status;
	if(m_pDSBuffer->GetStatus(&status) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �X�e�[�^�X�擾���s(CSoundBuffer_GetState)\n"));
		return 0;
	}

	return status;
}

//------------------------------------------------------------------------------
//	�o�b�t�@����
//------------------------------------------------------------------------------
void CSoundBuffer::Restore()
{
	// �o�b�t�@����
	if(m_pDSBuffer->Restore() != DS_OK) {
		::OutputDebugString(TEXT("*** Error - �X�e�[�^�X�擾���s(CSoundBuffer_Restore)\n"));
		return;
	}

	// ���L�������͕������Ȃ�
	if(m_FileName[0] == '\0')
		return;

	// �C���[�W�ēǍ�
	HMMIO                 hFile      = NULL;
	HACMSTREAM            hACMStream = NULL;	// ACM�X�g���[��
	LPVOID                pFormat    = NULL;	// �t�H�[�}�b�g
	LPVOID                pImage     = NULL;	// �C���[�W

	try {
		// �t�@�C���I�[�v��
		hFile = ::mmioOpen(m_FileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
		if(hFile == NULL)
			throw TEXT("*** Error - �t�@�C���I�[�v�����s(CSoundBuffer_Restore)\n");

		MMCKINFO   ParentChunk, ChildChunk;
		::ZeroMemory(&ParentChunk, sizeof(ParentChunk));
		::ZeroMemory(&ChildChunk,  sizeof(ChildChunk ));

		// 'WAVE'�`�����N�֐i��
		ParentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if(::mmioDescend(hFile, &ParentChunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'WAVE'�`�����N��������܂���(CSoundBuffer_Restore)\n");

		// 'WAVE'�`�����N��'fmt '�`�����N�֐i��
		ChildChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'fmt '�`�����N��������܂���(CSoundBuffer_Restore)\n");

		// �t�H�[�}�b�g���擾
		pFormat = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pFormat, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - �t�H�[�}�b�g���擾���s(CSoundBuffer_Restore)\n");

		::mmioAscend(hFile, &ChildChunk, 0);	// 'fmt '�`�����N�ޏo

		// 'WAVE'�`�����N��'data'�`�����N�֐i��
		ChildChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'data'�`�����N��������܂���(CSoundBuffer_Restore)\n");

		// ���j�APCM�ɕϊ�
		WAVEFORMATEX   NewFormat;
		::ZeroMemory(&NewFormat, sizeof(NewFormat));

		// �����t�H�[�}�b�g�擾
		NewFormat.wFormatTag = WAVE_FORMAT_PCM;
		if(::acmFormatSuggest(NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, sizeof(NewFormat),
							  ACM_FORMATSUGGESTF_WFORMATTAG) != 0)
			throw TEXT("*** Error - �����t�H�[�}�b�g�擾���s(CSoundBuffer_Restore)\n");

		// ACM�X�g���[���I�[�v��
		if(::acmStreamOpen(&hACMStream, NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, NULL, 0, 0,
						   ACM_STREAMOPENF_NONREALTIME) != 0)
			throw TEXT("*** Error - ACM�X�g���[���I�[�v�����s(CSoundBuffer_Restore)\n");

		// �t�H�[�}�b�g�ϊ���擾
		DWORD   NewSize;
		if(::acmStreamSize(hACMStream, ChildChunk.cksize, &NewSize, ACM_STREAMSIZEF_SOURCE) != 0)
			throw TEXT("*** Error - �f�R�[�h�T�C�Y�擾���s(CSoundBuffer_Restore)\n");

		// �ϊ����C���[�W�ǂݍ���
		pImage = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pImage, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - �ϊ����C���[�W�ǂݍ��ݎ��s(CSoundBuffer_Restore)\n");

		// �o�b�t�@�̃��b�N
		LPVOID    pBuf[2];
		DWORD     BufBytes[2];
		if(m_pDSBuffer->Lock(0, 0, &pBuf[0], &BufBytes[0], &pBuf[1], &BufBytes[1], DSBLOCK_ENTIREBUFFER)
		   != DS_OK)
			throw TEXT("*** Error - �o�b�t�@���b�N���s(CSoundBuffer_Restore)\n");

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
			m_pDSBuffer->Unlock(pBuf[0], 0, pBuf[1], 0);
			throw TEXT("*** Error - ACM�X�g���[���w�b�_�ݒ莸�s(CSoundBuffer_Restore)\n");
		}

		// �ϊ����Ȃ���o�b�t�@�֓]��
		const MMRESULT   mmr = ::acmStreamConvert(hACMStream, &acmsh, 0);

		::acmStreamUnprepareHeader(hACMStream, &acmsh, 0);
		m_pDSBuffer->Unlock(pBuf[0], BufBytes[0], pBuf[1], 0);
		if(mmr != 0)
			throw TEXT("*** Error - �C���[�W�ϊ����s(CSoundBuffer_Restore)\n");
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
	}

	delete[] pImage;
	delete[] pFormat;
	if(hACMStream != NULL)	::acmStreamClose(hACMStream, 0);;
	if(hFile      != NULL)	::mmioClose(hFile, 0);
}
