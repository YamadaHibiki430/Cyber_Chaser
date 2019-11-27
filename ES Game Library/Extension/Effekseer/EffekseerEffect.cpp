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
	 EffekseerEffect.hpp
				EffekseerEffect�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "EffekseerEffect.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CEffekseerEffect::CEffekseerEffect(Effekseer::Manager* pManager,
								   Effekseer::Effect*  pEffect)
	: m_manager(pManager), m_effect(pEffect)
{
	assert(m_manager != NULL && m_effect != NULL);
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CEffekseerEffect::~CEffekseerEffect()
{
	ES_SAFE_RELEASE(m_effect);
	m_manager = NULL;
}

//------------------------------------------------------------------------------
//	�Đ�
//------------------------------------------------------------------------------
Effekseer::Handle CEffekseerEffect::Play(const float inX, const float inY, const float inZ)
{
	return m_manager->Play(m_effect, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	��~
//------------------------------------------------------------------------------
void CEffekseerEffect::Stop(const Effekseer::Handle inHandle)
{
	 m_manager->StopEffect(inHandle);
}

//------------------------------------------------------------------------------
//	���[�g��~
//------------------------------------------------------------------------------
void CEffekseerEffect::StopRoot()
{
	 m_manager->StopRoot(m_effect);
}

//------------------------------------------------------------------------------
//	�ꎞ��~
//------------------------------------------------------------------------------
void CEffekseerEffect::Pause(const Effekseer::Handle inHandle)
{
	m_manager->SetPaused(inHandle, true);
}

//------------------------------------------------------------------------------
//	�ĊJ
//------------------------------------------------------------------------------
void CEffekseerEffect::Resume(const Effekseer::Handle inHandle)
{
	m_manager->SetPaused(inHandle, false);
}

//------------------------------------------------------------------------------
//	���ݒ�
//------------------------------------------------------------------------------
void CEffekseerEffect::SetVisible(const Effekseer::Handle inHandle, const bool inVisible)
{
	m_manager->SetShown(inHandle, inVisible);
}

//------------------------------------------------------------------------------
//	�Đ��X�s�[�h�ݒ�
//------------------------------------------------------------------------------
void CEffekseerEffect::SetSpeed(const Effekseer::Handle inHandle, const float inSpeed)
{
	m_manager->SetSpeed(inHandle, inSpeed);
}

//------------------------------------------------------------------------------
//	���W�ݒ�
//------------------------------------------------------------------------------
void CEffekseerEffect::SetPosition(const Effekseer::Handle inHandle,
								   const float inX, const float inY, const float inZ)
{
	m_manager->SetLocation(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	�p�x�ݒ�
//------------------------------------------------------------------------------
void CEffekseerEffect::SetRotation(const Effekseer::Handle inHandle,
								   const float inX, const float inY, const float inZ)
{
	m_manager->SetRotation(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	�X�P�[�����O�ݒ�
//------------------------------------------------------------------------------
void CEffekseerEffect::SetScale(const Effekseer::Handle inHandle,
								const float inX, const float inY, const float inZ)
{
	m_manager->SetScale(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	�^�[�Q�b�g���W�ݒ�
//------------------------------------------------------------------------------
void CEffekseerEffect::SetTargetPostion(const Effekseer::Handle inHandle,
										const float inX, const float inY, const float inZ)
{
	m_manager->SetTargetLocation(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	�ړ�
//------------------------------------------------------------------------------
void CEffekseerEffect::Move(const Effekseer::Handle inHandle,
							const float inAddX, const float inAddY, const float inAddZ)
{
	m_manager->AddLocation(inHandle, Effekseer::Vector3D(inAddX, inAddY, inAddZ));
}

//------------------------------------------------------------------------------
//	���W�擾
//------------------------------------------------------------------------------
D3DVECTOR CEffekseerEffect::GetPosition(const Effekseer::Handle inHandle)
{
	Effekseer::Vector3D   location = m_manager->GetLocation(inHandle);
	D3DVECTOR   pos = {location.X, location.Y, location.Z};
	return pos;
}

//------------------------------------------------------------------------------
//	���W�擾
//------------------------------------------------------------------------------
D3DVECTOR CNullEffekseerEffect::GetPosition(const Effekseer::Handle inHandle)
{
	D3DVECTOR   pos = {0.0f, 0.0f, 0.0f};
	return pos;
}
