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
	 Sound3DBuffer.hpp
				�T�E���h�R�c�o�b�t�@�N���X�w�b�_�[�t�@�C��

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
//	�T�E���h�R�c�o�b�t�@�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class ISound3DBuffer {
public:
	virtual ~ISound3DBuffer() {}

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) = 0;
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) = 0;

	virtual void SetConeAngles(const DWORD inInside, const DWORD inOutside) = 0;
	virtual void SetConeOrientation(const D3DVECTOR& inOrientation) = 0;
	virtual void SetConeOutsideVolume(const LONG inVolume) = 0;
	virtual void SetConeParameters(const DWORD inInside, const DWORD inOutside,
								   const D3DVECTOR& inOrientation, const LONG inVolume) = 0;

	virtual void SetDistance(const D3DVALUE inMin, const D3DVALUE inMax) = 0;
	virtual void SetMode(const DWORD inMode) = 0;

	virtual void SetAllParameters(const DS3DBUFFER& in3DBuffer) = 0;
};

//------------------------------------------------------------------------------
//	�T�E���h�R�c�o�b�t�@�N���X��`
//------------------------------------------------------------------------------
class CSound3DBuffer : public ISound3DBuffer {
public:
	CSound3DBuffer(IDirectSound3DBuffer8* pDS3DBuffer);
	virtual ~CSound3DBuffer() { m_pDS3DBuffer->Release(); }

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z);
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z);

	virtual void SetConeAngles(const DWORD inInside, const DWORD inOutside);
	virtual void SetConeOrientation(const D3DVECTOR& inOrientation);
	virtual void SetConeOutsideVolume(const LONG inVolume);
	virtual void SetConeParameters(const DWORD inInside, const DWORD inOutside,
								   const D3DVECTOR& inOrientation, const LONG inVolume);

	virtual void SetDistance(const D3DVALUE inMin, const D3DVALUE inMax);
	virtual void SetMode(const DWORD inMode);

	virtual void SetAllParameters(const DS3DBUFFER& in3DBuffer);

private:
	IDirectSound3DBuffer8*   m_pDS3DBuffer;
};

//------------------------------------------------------------------------------
//	NULL�T�E���h�R�c�o�b�t�@�N���X��`
//------------------------------------------------------------------------------
class CNullSound3DBuffer : public ISound3DBuffer {
public:
	CNullSound3DBuffer() {}
	virtual ~CNullSound3DBuffer() {}

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) {}
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) {}

	virtual void SetConeAngles(const DWORD inInside, const DWORD inOutside) {}
	virtual void SetConeOrientation(const D3DVECTOR& inOrientation) {}
	virtual void SetConeOutsideVolume(const LONG inVolume) {}
	virtual void SetConeParameters(const DWORD inInside, const DWORD inOutside,
								   const D3DVECTOR& inOrientation, const LONG inVolume) {}

	virtual void SetDistance(const D3DVALUE inMin, const D3DVALUE inMax) {}
	virtual void SetMode(const DWORD inMode) {}

	virtual void SetAllParameters(const DS3DBUFFER& in3DBuffer) {}
};
