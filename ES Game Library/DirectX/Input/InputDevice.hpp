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
	 InputDevice.hpp
				�C���v�b�g�f�o�C�X�N���X�w�b�_�[�t�@�C��

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
#include "InputEffect.hpp"

//------------------------------------------------------------------------------
//	�萔
//------------------------------------------------------------------------------
enum {
	DIDEV_BUFSIZE       =     16,	// �o�b�t�@�T�C�Y
//	DIDEV_AXIS_MIN      =      0,	// �Q�[���p�b�h�͈�
//	DIDEV_AXIS_MAX      =  65535,
	DIDEV_AXIS_MIN      = -32767,	// �Q�[���p�b�h�͈�
	DIDEV_AXIS_MAX      =  32767,
	DIDEV_AXIS_CENTER   = (DIDEV_AXIS_MIN + DIDEV_AXIS_MAX + 1) / 2,
	DIDEV_AXIS_DEADZONE =   1000	// �f�b�h�]�[��(10%)
};

//------------------------------------------------------------------------------
//	�C���v�b�g�f�o�C�X�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IInputDevice {
public:
	virtual ~IInputDevice() {}

	virtual bool  IsNull() const = 0;

	virtual bool  GetState(LPVOID pState) = 0;
	virtual DWORD GetBuffer(DIDEVICEOBJECTDATA outDIData[], const DWORD inElements = DIDEV_BUFSIZE) = 0;
	virtual void  FlushBuffer() = 0;
	virtual void  Polling()     = 0;

	virtual void  Acquire()     = 0;
	virtual void  Unacquire()   = 0;

	virtual bool  CreateMagnitudeEffect(REFGUID inGUID,
										const DWORD inMagnitude, const double inPeriod) = 0;
	virtual void  ReleaseEffect() = 0;

	virtual IInputEffect* GetEffect() = 0;
	virtual IInputDevice& GetState () = 0;
	virtual IInputDevice& GetBuffer() = 0;
};

//------------------------------------------------------------------------------
//	�C���v�b�g�f�o�C�X�N���X��`
//------------------------------------------------------------------------------
class CInputDevice : public IInputDevice {
public:
	CInputDevice(IDirectInputDevice8* pDIDevice);
	virtual ~CInputDevice();

	virtual bool  IsNull() const { return false; }

	virtual DWORD GetBuffer(DIDEVICEOBJECTDATA outDIData[], const DWORD inElements);
	virtual void  FlushBuffer();
	virtual void  Polling() {}

	virtual void  Acquire()   { m_pDIDevice->Acquire();   }
	virtual void  Unacquire() { m_pDIDevice->Unacquire(); }

	virtual bool  CreateMagnitudeEffect(REFGUID inGUID,
										const DWORD inMagnitude, const double inPeriod);
	virtual void  ReleaseEffect();

	virtual IInputEffect* GetEffect() { return m_pEffect; }
	virtual IInputDevice& GetState () { return *this;     }
	virtual IInputDevice& GetBuffer() { return *this;     }

protected:
	virtual bool GetState(DWORD inSize, LPVOID pState);

	IDirectInputDevice8*   m_pDIDevice;
	IInputEffect*          m_pEffect;
};

//------------------------------------------------------------------------------
//	�L�[�{�[�h�N���X��`
//------------------------------------------------------------------------------
class CKeyboard : public CInputDevice {
public:
	CKeyboard(IDirectInputDevice8* pDIDKeyboard) : CInputDevice(pDIDKeyboard) {}
	virtual ~CKeyboard() {}

	virtual bool GetState(LPVOID pState) { return CInputDevice::GetState(256, pState); }
};

//------------------------------------------------------------------------------
//	�}�E�X�N���X��`
//------------------------------------------------------------------------------
class CMouse : public CInputDevice {
public:
	CMouse(IDirectInputDevice8* pDIDMouse) : CInputDevice(pDIDMouse) {}
	virtual ~CMouse() {}

	virtual bool GetState(LPVOID pState)
	{ return CInputDevice::GetState(sizeof(DIMOUSESTATE2), pState); }
};

//------------------------------------------------------------------------------
//	�Q�[���p�b�h�N���X��`
//------------------------------------------------------------------------------
class CGamePad : public CInputDevice {
public:
	CGamePad(IDirectInputDevice8* pDIDGamePad) : CInputDevice(pDIDGamePad) {}
	virtual ~CGamePad() {}

	virtual bool GetState(LPVOID pState) { return CInputDevice::GetState(sizeof(DIJOYSTATE), pState); }
	virtual void Polling() { m_pDIDevice->Poll(); }
};

//------------------------------------------------------------------------------
//	NULL�C���v�b�g�f�o�C�X�N���X��`
//------------------------------------------------------------------------------
class CNullInputDevice : public IInputDevice {
public:
	CNullInputDevice() : m_ClearBytes(0) {}
	CNullInputDevice(const DWORD inClearBytes) : m_ClearBytes(inClearBytes) {}
	virtual ~CNullInputDevice() {}

	virtual bool  IsNull() const { return true; }

	virtual bool  GetState(LPVOID pState) { ::ZeroMemory(pState, m_ClearBytes); return true; }
	virtual DWORD GetBuffer(DIDEVICEOBJECTDATA outDIData[], const DWORD inElements) { return 0; }
	virtual void  FlushBuffer() {}
	virtual void  Polling()     {}

	virtual void  Acquire()     {}
	virtual void  Unacquire()   {}

	virtual bool  CreateMagnitudeEffect(REFGUID inGUID,
										const DWORD inMagnitude, const double inPeriod) { return false; }
	virtual void  ReleaseEffect() {}

	virtual IInputEffect* GetEffect() { return &m_Effect; }
	virtual IInputDevice& GetState () { return *this;     }
	virtual IInputDevice& GetBuffer() { return *this;     }

protected:
	DWORD              m_ClearBytes;
	CNullInputEffect   m_Effect;
};

//------------------------------------------------------------------------------
//	NULL�Q�[���p�b�h�N���X��`
//------------------------------------------------------------------------------
class CNullGamePad : public CNullInputDevice {
public:
	CNullGamePad() : CNullInputDevice(0) {}
	CNullGamePad(const DWORD inClearBytes) : CNullInputDevice(inClearBytes) {}

	virtual bool GetState(LPVOID pState);
};
