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
	 MusicSegment.hpp
				�~���[�W�b�N�Z�O�����g�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <dmusici.h>

//------------------------------------------------------------------------------
//	�~���[�W�b�N�Z�O�����g���t�t���O
//------------------------------------------------------------------------------
enum DXAMUSICSEGMENT_PLAYFLAGS {
	DXAMS_PRIMARY   = 0,						// �v���C�}��
	DXAMS_SECONDARY = DMUS_SEGF_SECONDARY,		// �Z�J���_��
	DXAMS_QUEUE     = DMUS_SEGF_QUEUE			// ���t�L���[�ɓo�^
};

//------------------------------------------------------------------------------
//	�~���[�W�b�N�Z�O�����g�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IMusicSegment {
public:
	virtual ~IMusicSegment() {}

	virtual bool Play(const DWORD inFlag = 0, const int inLoop = -1) = 0;
	virtual bool PlayOnce() = 0;
	virtual void Stop() = 0;

	virtual bool IsPlaying() = 0;
};

//------------------------------------------------------------------------------
//	�~���[�W�b�N�Z�O�����g�N���X��`
//------------------------------------------------------------------------------
class CMusicSegment : public IMusicSegment {
public:
	CMusicSegment(IDirectMusicLoader8*      pDMLoader,
				  IDirectMusicPerformance8* pDMPerformance,
				  IDirectMusicSegment8*     pDMSegment,
				  BYTE*                     pWaveData);
	virtual ~CMusicSegment();

	virtual bool Play(const DWORD inFlag, const int inLoop);
	virtual bool PlayOnce() { return Play(DXAMS_SECONDARY, 0); }
	virtual void Stop() { m_pDMPerformance->StopEx(m_pDMSegment, 0, 0); }

	virtual bool IsPlaying();

private:
	IDirectMusicLoader8*        m_pDMLoader;
	IDirectMusicPerformance8*   m_pDMPerformance;
	IDirectMusicSegment8*       m_pDMSegment;
	BYTE*                       m_pWaveData;
};

//------------------------------------------------------------------------------
//	NULL�~���[�W�b�N�Z�O�����g�N���X��`
//------------------------------------------------------------------------------
class CNullMusicSegment : public IMusicSegment {
public:
	CNullMusicSegment() {}
	virtual ~CNullMusicSegment() {}

	virtual bool Play(const DWORD inFlag, const int inLoop) { return false; }
	virtual bool PlayOnce() { return Play(DXAMS_SECONDARY, 0); }
	virtual void Stop() {}

	virtual bool IsPlaying() { return false; }
};
