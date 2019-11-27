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
	 InputEffect.cpp
				���̓f�o�C�X�G�t�F�N�g�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "InputEffect.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CInputEffect::CInputEffect(IDirectInputEffect* pDIEffect)
{
	assert(pDIEffect != NULL);
	m_pDIEffect = pDIEffect;
	m_pDIEffect->AddRef();			// �Q�ƃJ�E���^�C���N�������g
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CInputEffect::~CInputEffect()
{
	m_pDIEffect->Unload();
	m_pDIEffect->Release();
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�J�n
//------------------------------------------------------------------------------
bool CInputEffect::Start(const DWORD inIterations)
{
	if(m_pDIEffect->Start(inIterations, DIES_SOLO) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - �G�t�F�N�g�J�n���s(CInputEffect_Star)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�Đ��`�F�b�N
//------------------------------------------------------------------------------
bool CInputEffect::IsPlaying()
{
	DWORD   State;
	m_pDIEffect->GetEffectStatus(&State);
	if((State & DIEGES_PLAYING) == 0)
		return false;

	return true;
}
