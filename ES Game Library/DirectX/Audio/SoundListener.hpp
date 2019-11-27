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
	 SoundListener.hpp
				�T�E���h���X�i�N���X�w�b�_

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <dsound.h>

//------------------------------------------------------------------------------
//	�T�E���h���X�i�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class ISoundListener {
public:
	virtual ~ISoundListener() {}

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) = 0;
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) = 0;
	virtual void SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop) = 0;
	virtual void SetRolloffFactor(const D3DVALUE inRolloff) = 0;
	virtual void SetDopplerFactor(const D3DVALUE inDoppler) = 0;
	virtual void SetDistanceFactor(const D3DVALUE inDistance) = 0;
	virtual void SetAllParameters(const DS3DLISTENER& inListener) = 0;

	virtual void Initialize() = 0;
};

//------------------------------------------------------------------------------
//	�T�E���h���X�i�N���X��`
//------------------------------------------------------------------------------
class CSoundListener : public ISoundListener {
public:
	CSoundListener(IDirectSound3DListener8* pDS3DListener);
	virtual ~CSoundListener() { m_pDS3DListener->Release(); }

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z);
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z);
	virtual void SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop);
	virtual void SetRolloffFactor(const D3DVALUE inRolloff);
	virtual void SetDopplerFactor(const D3DVALUE inDoppler);
	virtual void SetDistanceFactor(const D3DVALUE inDistance);
	virtual void SetAllParameters(const DS3DLISTENER& inListener);

	virtual void Initialize();

private:
	IDirectSound3DListener8*   m_pDS3DListener;
};

//------------------------------------------------------------------------------
//	NULL�T�E���h���X�i�N���X��`
//------------------------------------------------------------------------------
class CNullSoundListener : public ISoundListener {
public:
	CNullSoundListener() {}
	virtual ~CNullSoundListener() {}

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) {}
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) {}
	virtual void SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop)  {}
	virtual void SetRolloffFactor(const D3DVALUE inRolloff) {}
	virtual void SetDopplerFactor(const D3DVALUE inDoppler) {}
	virtual void SetDistanceFactor(const D3DVALUE inDistance) {}
	virtual void SetAllParameters(const DS3DLISTENER& inListener) {}

	virtual void Initialize() {}
};
