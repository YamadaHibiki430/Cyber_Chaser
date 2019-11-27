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
	 InputDevice.cpp
				�C���v�b�g�f�o�C�X�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "InputDevice.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CInputDevice::CInputDevice(IDirectInputDevice8* pDIDevice) : m_pEffect(new CNullInputEffect())
{
	assert(pDIDevice != NULL);
	m_pDIDevice = pDIDevice;
	m_pDIDevice->AddRef();
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CInputDevice::~CInputDevice()
{
	delete m_pEffect;	// �G�t�F�N�g���

	m_pDIDevice->Unacquire();
	m_pDIDevice->Release();
}

//------------------------------------------------------------------------------
//	��Ԏ擾
//------------------------------------------------------------------------------
bool CInputDevice::GetState(const DWORD inSize, LPVOID pState)
{
	const HRESULT   hr = m_pDIDevice->GetDeviceState(inSize, pState);
	if(hr == DI_OK)
		return true;

	::OutputDebugString(TEXT("*** Error - ��Ԏ擾���s(CInputDevice_GetState)\n"));
	::ZeroMemory(pState, inSize);

	// �A�N�Z�X���Ď擾
	if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		m_pDIDevice->Acquire();

	return false;
}

//------------------------------------------------------------------------------
//	�o�b�t�@�擾
//------------------------------------------------------------------------------
DWORD CInputDevice::GetBuffer(DIDEVICEOBJECTDATA outBufData[], const DWORD inElements)
{
	DWORD   Items = inElements;
	const HRESULT   hr = m_pDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), outBufData, &Items, 0);
	if(SUCCEEDED(hr))
		return Items;

	::OutputDebugString(TEXT("*** Error - �o�b�t�@�擾���s(CInputDevice_GetBuffer)\n"));
	::ZeroMemory(outBufData, sizeof(DIDEVICEOBJECTDATA) * inElements);

	// �A�N�Z�X���Ď擾
	if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		m_pDIDevice->Acquire();

	return 0;
}

//------------------------------------------------------------------------------
//	�o�b�t�@����
//------------------------------------------------------------------------------
void CInputDevice::FlushBuffer()
{
	DWORD   Items = INFINITE;
	const HRESULT   hr = m_pDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &Items, 0);

	// �A�N�Z�X���Ď擾
	if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		m_pDIDevice->Acquire();
}

//------------------------------------------------------------------------------
//	�U���G�t�F�N�g����
//------------------------------------------------------------------------------
bool CInputDevice::CreateMagnitudeEffect(REFGUID inGUID,
										 const DWORD inMagnitude, const double inPeriod)
{
	// �G�t�F�N�g�����ݒ�
	DIPERIODIC   diprd;
	::ZeroMemory(&diprd, sizeof(diprd));
	diprd.dwMagnitude = inMagnitude;
	diprd.lOffset     = 0;
	diprd.dwPhase     = 0;
	diprd.dwPeriod    = (DWORD)(inPeriod * DI_SECONDS);
/*
	// �G���x���[�v�ݒ�
	DIENVELOPE   dienv;
	::ZeroMemory(&dienv, sizeof(dienv));
	dienv.dwSize        = sizeof(dienv);
	dienv.dwAttackLevel = 0;
	dienv.dwAttackTime  = (DWORD)(2.5 * DISECONDS);
	dienv.dwFadeLevel   = 0;
	dienv.dwFadeTime    = (DWORD)(2.5 * DISECONDS);
*/
	// �G�t�F�N�g�ݒ�
	DWORD      Axes[]      = {DIJOFS_X, DIJOFS_Y};		// �G�t�F�N�g��
	LONG       Direction[] = {0, 0};					// �G�t�F�N�g����

	DIEFFECT   Effect;
	::ZeroMemory(&Effect, sizeof(Effect));
	Effect.dwSize                  = sizeof(Effect);
	Effect.dwFlags                 = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
	Effect.dwDuration              = (DWORD)(inPeriod * DI_SECONDS);
	Effect.dwSamplePeriod          = 0;
	Effect.dwGain                  = DI_FFNOMINALMAX;
	Effect.dwTriggerButton         = DIEB_NOTRIGGER;
	Effect.dwTriggerRepeatInterval = 0;
	Effect.cAxes                   = sizeof(Axes) / sizeof(Axes[0]);
	Effect.rgdwAxes                = Axes;
	Effect.rglDirection            = Direction;
	Effect.lpEnvelope              = NULL;	// &dienv;
	Effect.cbTypeSpecificParams    = sizeof(diprd);
	Effect.lpvTypeSpecificParams   = &diprd;

	// �G�t�F�N�g����
	IDirectInputEffect*   pDIEffect;
	if(m_pDIDevice->CreateEffect(inGUID, &Effect, &pDIEffect, NULL) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - �G�t�F�N�g�������s(CInputDevice_CreateMagnitudeEffect)\n"));
		ReleaseEffect();
		return false;
	}
	pDIEffect->AddRef();

	// �G�t�F�N�g�I�u�W�F�N�g����
	delete m_pEffect;
	m_pEffect = new CInputEffect(pDIEffect);
	pDIEffect->Release();

	return true;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g���
//------------------------------------------------------------------------------
void CInputDevice::ReleaseEffect()
{
	// pDevice��NULL�I�u�W�F�N�g�̏ꍇ�͉�����Ȃ�
	if(m_pEffect->IsNull())
		return;

	// �G�t�F�N�g��������ANULL�I�u�W�F�N�g��ݒ�
	delete m_pEffect;
	m_pEffect = new CNullInputEffect();
}

//------------------------------------------------------------------------------
//	NULL�Q�[���p�b�h��Ԏ擾
//------------------------------------------------------------------------------
bool CNullGamePad::GetState(LPVOID pState)
{
	::ZeroMemory(pState, m_ClearBytes);

	DIJOYSTATE*   state = (DIJOYSTATE*)pState;
	state->rgdwPOV[0]   = -1;
	state->rgdwPOV[1]   = -1;
	state->rgdwPOV[2]   = -1;
	state->rgdwPOV[3]   = -1;

	return true;
}
