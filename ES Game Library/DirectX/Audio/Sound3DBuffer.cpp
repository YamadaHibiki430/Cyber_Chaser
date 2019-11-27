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
	 Sound3DBuffer.cpp
				�T�E���h�R�c�o�b�t�@�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Sound3DBuffer.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CSound3DBuffer::CSound3DBuffer(IDirectSound3DBuffer8* pDS3DBuffer)
{
	assert(pDS3DBuffer != NULL);
	m_pDS3DBuffer = pDS3DBuffer;
	m_pDS3DBuffer->AddRef();		// �Q�ƃJ�E���^�C���N�������g
}

//------------------------------------------------------------------------------
//	�ʒu�ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z)
{
	m_pDS3DBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	���x�ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z)
{
	m_pDS3DBuffer->SetVelocity(x, y, z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�R�[���p�x�ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetConeAngles(const DWORD inInside, const DWORD inOutside)
{
	m_pDS3DBuffer->SetConeAngles(inInside, inOutside, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�R�[�������ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetConeOrientation(const D3DVECTOR& inOrientation)
{
	m_pDS3DBuffer->SetConeOrientation(inOrientation.x, inOrientation.y, inOrientation.z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�R�[���O�����ʐݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetConeOutsideVolume(const LONG inVolume)
{
	m_pDS3DBuffer->SetConeOutsideVolume(inVolume, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�R�[���p�����[�^�ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetConeParameters(const DWORD inInside, const DWORD inOutside,
									   const D3DVECTOR& inOrientation, const LONG inVolume)
{
	SetConeAngles(inInside, inOutside);
	SetConeOrientation(inOrientation);
	SetConeOutsideVolume(inVolume);
}

//------------------------------------------------------------------------------
//	�����ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetDistance(const D3DVALUE inMin, const D3DVALUE inMax)
{
	m_pDS3DBuffer->SetMinDistance(inMin, DS3D_IMMEDIATE);
	m_pDS3DBuffer->SetMaxDistance(inMax, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	���[�h�ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetMode(const DWORD inMode)
{
	m_pDS3DBuffer->SetMode(inMode, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	�S�p�����[�^�ݒ�
//------------------------------------------------------------------------------
void CSound3DBuffer::SetAllParameters(const DS3DBUFFER& inDS3DBuf)
{
	m_pDS3DBuffer->SetAllParameters(&inDS3DBuf, DS3D_IMMEDIATE);
}
