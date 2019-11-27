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
	 SoundListener.cpp
				�T�E���h���X�i�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "SoundListener.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CSoundListener::CSoundListener(IDirectSound3DListener8* pDS3DListener)
{
	assert(pDS3DListener != NULL);
	m_pDS3DListener = pDS3DListener;
	m_pDS3DListener->AddRef();			// �Q�ƃJ�E���^�C���N�������g
}

//------------------------------------------------------------------------------
//	�ʒu�ݒ�
//------------------------------------------------------------------------------
void CSoundListener::SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z)
{
	m_pDS3DListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	���x�ݒ�
//------------------------------------------------------------------------------
void CSoundListener::SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z)
{
	m_pDS3DListener->SetVelocity(x, y, z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�����ݒ�
//------------------------------------------------------------------------------
void CSoundListener::SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop)
{
	m_pDS3DListener->SetOrientation(inFront.x, inFront.y, inFront.z,
									inTop  .x, inTop  .y, inTop  .z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	���[���I�t�W���ݒ�
//------------------------------------------------------------------------------
void CSoundListener::SetRolloffFactor(const D3DVALUE inRolloff)
{
	m_pDS3DListener->SetRolloffFactor(inRolloff, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�h�b�v���[�搔�ݒ�
//------------------------------------------------------------------------------
void CSoundListener::SetDopplerFactor(const D3DVALUE inDoppler)
{
	m_pDS3DListener->SetDopplerFactor(inDoppler, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�����W���ݒ�
//------------------------------------------------------------------------------
void CSoundListener::SetDistanceFactor(const D3DVALUE inDistance)
{
	m_pDS3DListener->SetDistanceFactor(inDistance, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�S�p�����[�^�ݒ�
//------------------------------------------------------------------------------
void CSoundListener::SetAllParameters(const DS3DLISTENER& inListener)
{
	m_pDS3DListener->SetAllParameters(&inListener, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�p�����[�^������
//------------------------------------------------------------------------------
void CSoundListener::Initialize()
{
	DS3DLISTENER   Listener;
	::ZeroMemory(&Listener, sizeof(Listener));
	Listener.dwSize           = sizeof(Listener);

	Listener.vPosition.x      = 0.0f;							// �ʒu
	Listener.vPosition.y      = 0.0f;
	Listener.vPosition.z      = 0.0f;

	Listener.vVelocity.x      = 0.0f;							// ���x
	Listener.vVelocity.y      = 0.0f;
	Listener.vVelocity.z      = 0.0f;

	Listener.vOrientFront.x   = 0.0f;							// �O���x�N�g���ݒ�
	Listener.vOrientFront.y   = 0.0f;
	Listener.vOrientFront.z   = 1.0f;

	Listener.vOrientTop.x     = 0.0f;							// ����x�N�g���ݒ�
	Listener.vOrientTop.y     = 1.0f;
	Listener.vOrientTop.z     = 0.0f;

	Listener.flDistanceFactor = DS3D_DEFAULTDISTANCEFACTOR;		// �W���ݒ�
	Listener.flRolloffFactor  = DS3D_DEFAULTROLLOFFFACTOR;
	Listener.flDopplerFactor  = DS3D_DEFAULTDOPPLERFACTOR;

	m_pDS3DListener->SetAllParameters(&Listener, DS3D_IMMEDIATE);
}
