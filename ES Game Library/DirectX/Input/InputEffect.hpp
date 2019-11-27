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
	 InputEffect.hpp
				���̓f�o�C�X�G�t�F�N�g�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�o�[�W�����錾
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <dinput.h>

//------------------------------------------------------------------------------
//	���̓f�o�C�X�G�t�F�N�g�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IInputEffect {
public:
	virtual ~IInputEffect() {}

	virtual bool IsNull() const = 0;

	virtual bool Start(const DWORD inIterations) = 0;
	virtual void Stop() = 0;
	virtual bool IsPlaying() = 0;
};

//------------------------------------------------------------------------------
//	���̓f�o�C�X�G�t�F�N�g�N���X��`
//------------------------------------------------------------------------------
class CInputEffect : public IInputEffect {
public:
	CInputEffect(IDirectInputEffect* pDIEffect);
	virtual ~CInputEffect();

	virtual bool IsNull() const { return false; }

	virtual bool Start(const DWORD inIterations);
	virtual void Stop() { m_pDIEffect->Stop(); }
	virtual bool IsPlaying();

private:
	IDirectInputEffect*   m_pDIEffect;
};

//------------------------------------------------------------------------------
//	NULL���̓f�o�C�X�G�t�F�N�g�N���X��`
//------------------------------------------------------------------------------
class CNullInputEffect : public IInputEffect {
public:
	CNullInputEffect() {}
	virtual ~CNullInputEffect() {}

	virtual bool IsNull() const { return true; }

	virtual bool Start(const DWORD inIterations) { return false; }
	virtual void Stop() {}
	virtual bool IsPlaying() { return false; }
};
