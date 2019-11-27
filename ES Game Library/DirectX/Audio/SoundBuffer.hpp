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
	 SoundBuffer.hpp
				�T�E���h�o�b�t�@�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Sound3DBuffer.hpp"

//------------------------------------------------------------------------------
//	�G�t�F�N�g�t���O
//------------------------------------------------------------------------------
enum DXAFX {
	DXAFX_CHORUS      = 1 << 0,		// �R�[���X
	DXAFX_COMPRESSOR  = 1 << 1,		// �R���v���b�V����
	DXAFX_DISTORTION  = 1 << 2,		// �f�B�X�g�[�V����
	DXAFX_ECHO        = 1 << 3,		// �G�R�[
	DXAFX_FLANGER     = 1 << 4,		// �t�����W
	DXAFX_GARGLE      = 1 << 5,		// �K�[�O��
	DXAFX_ENVREVERB   = 1 << 6,		// �����o�[�u
	DXAFX_PARAMEQ     = 1 << 7,		// �p�����g���b�N�C�R���C�U
	DXAFX_WAVESREVERB = 1 << 8,		// �~���[�W�b�N���o�[�u
	DXAFX_MAX         = 9
};

//------------------------------------------------------------------------------
//	�T�E���h�o�b�t�@�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class ISoundBuffer {
public:
	virtual ~ISoundBuffer() {}

	virtual bool Play(const DWORD inPriority = 0, const DWORD inFlags = 0) = 0;
	virtual bool PlayLooping() = 0;
	virtual void Stop() = 0;

	virtual void SetVolume(LONG inVolume) = 0;
	virtual void SetPan(LONG inPan) = 0;
	virtual void SetFrequency(DWORD inFrequency) = 0;
	virtual bool SetFX(const DWORD inFXFlags) = 0;

	inline void SetVolume(const float inVolume)
	{ SetVolume(DSBVOLUME_MAX - LONG( (DSBVOLUME_MAX - DSBVOLUME_MIN) * (1.0f - inVolume) )); }

	inline void SetPan(const float inPan)
	{ SetPan( LONG(DSBPAN_RIGHT * inPan) ); }

	virtual LONG  GetVolume() = 0;
	virtual LONG  GetPan() = 0;
	virtual DWORD GetFrequency() = 0;
	virtual DWORD GetState() = 0;

	virtual bool IsPlaying() = 0;
	virtual bool IsLooping() = 0;

	virtual ISound3DBuffer* GetSound3D() = 0;
};

//------------------------------------------------------------------------------
//	�T�E���h�o�b�t�@�N���X��`
//------------------------------------------------------------------------------
class CSoundBuffer : public ISoundBuffer {
public:
	CSoundBuffer(IDirectSoundBuffer8* pDSBuffer, LPCTSTR inFileName);
	virtual ~CSoundBuffer() { delete m_p3DBuffer; m_pDSBuffer->Release(); }

	virtual bool Play(const DWORD inPriority, const DWORD inFlags);
	virtual bool PlayLooping() { return Play(0, DSBPLAY_LOOPING); }
	virtual void Stop() { m_pDSBuffer->Stop(); }

	virtual void SetVolume   (LONG  inVolume)    { m_pDSBuffer->SetVolume(inVolume); }
	virtual void SetPan      (LONG  inPan)       { m_pDSBuffer->SetPan(inPan); }
	virtual void SetFrequency(DWORD inFrequency) { m_pDSBuffer->SetFrequency(inFrequency); }
	virtual bool SetFX       (const DWORD inFXFlags);

	virtual LONG  GetVolume();
	virtual LONG  GetPan();
	virtual DWORD GetFrequency();
	virtual DWORD GetState();

	virtual bool IsPlaying() { return (GetState() & DSBSTATUS_PLAYING) != 0; }
	virtual bool IsLooping() { return (GetState() & DSBSTATUS_LOOPING) != 0; }

	virtual ISound3DBuffer* GetSound3D() { return m_p3DBuffer; }

private:
	void Restore();

	IDirectSoundBuffer8*   m_pDSBuffer;
	ISound3DBuffer*        m_p3DBuffer;

	TCHAR                  m_FileName[MAX_PATH + 1];
};

//------------------------------------------------------------------------------
//	NULL�T�E���h�o�b�t�@�N���X��`
//------------------------------------------------------------------------------
class CNullSoundBuffer : public ISoundBuffer {
public:
	CNullSoundBuffer() {}
	virtual ~CNullSoundBuffer() {}

	virtual bool Play(const DWORD inPriority, const DWORD inFlags) { return false; }
	virtual bool PlayLooping() { return Play(0, DSBPLAY_LOOPING); }
	virtual void Stop() {}

	virtual void SetVolume(LONG inVolume) {}
	virtual void SetPan(LONG inPan) {}
	virtual void SetFrequency(DWORD inFrequency) {}
	virtual bool SetFX(const DWORD inFXFlags) { return false; }

	virtual LONG  GetVolume()    { return DSBVOLUME_MIN; }
	virtual LONG  GetPan()       { return DSBPAN_CENTER; }
	virtual DWORD GetFrequency() { return 0; }
	virtual DWORD GetState()     { return 0; }

	virtual bool IsPlaying() { return false; }
	virtual bool IsLooping() { return false; }

	virtual ISound3DBuffer* GetSound3D() { return &m_3DBuffer; }

private:
	CNullSound3DBuffer   m_3DBuffer;
};
