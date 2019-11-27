/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft Windows 7以降

 【コンパイラ】
     Microsoft VisualC++ 2013

 【プログラム】
	 InputDevice.cpp
				インプットデバイスクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "InputDevice.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CInputDevice::CInputDevice(IDirectInputDevice8* pDIDevice) : m_pEffect(new CNullInputEffect())
{
	assert(pDIDevice != NULL);
	m_pDIDevice = pDIDevice;
	m_pDIDevice->AddRef();
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CInputDevice::~CInputDevice()
{
	delete m_pEffect;	// エフェクト解放

	m_pDIDevice->Unacquire();
	m_pDIDevice->Release();
}

//------------------------------------------------------------------------------
//	状態取得
//------------------------------------------------------------------------------
bool CInputDevice::GetState(const DWORD inSize, LPVOID pState)
{
	const HRESULT   hr = m_pDIDevice->GetDeviceState(inSize, pState);
	if(hr == DI_OK)
		return true;

	::OutputDebugString(TEXT("*** Error - 状態取得失敗(CInputDevice_GetState)\n"));
	::ZeroMemory(pState, inSize);

	// アクセス権再取得
	if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		m_pDIDevice->Acquire();

	return false;
}

//------------------------------------------------------------------------------
//	バッファ取得
//------------------------------------------------------------------------------
DWORD CInputDevice::GetBuffer(DIDEVICEOBJECTDATA outBufData[], const DWORD inElements)
{
	DWORD   Items = inElements;
	const HRESULT   hr = m_pDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), outBufData, &Items, 0);
	if(SUCCEEDED(hr))
		return Items;

	::OutputDebugString(TEXT("*** Error - バッファ取得失敗(CInputDevice_GetBuffer)\n"));
	::ZeroMemory(outBufData, sizeof(DIDEVICEOBJECTDATA) * inElements);

	// アクセス権再取得
	if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		m_pDIDevice->Acquire();

	return 0;
}

//------------------------------------------------------------------------------
//	バッファ消去
//------------------------------------------------------------------------------
void CInputDevice::FlushBuffer()
{
	DWORD   Items = INFINITE;
	const HRESULT   hr = m_pDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &Items, 0);

	// アクセス権再取得
	if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		m_pDIDevice->Acquire();
}

//------------------------------------------------------------------------------
//	振動エフェクト生成
//------------------------------------------------------------------------------
bool CInputDevice::CreateMagnitudeEffect(REFGUID inGUID,
										 const DWORD inMagnitude, const double inPeriod)
{
	// エフェクト周期設定
	DIPERIODIC   diprd;
	::ZeroMemory(&diprd, sizeof(diprd));
	diprd.dwMagnitude = inMagnitude;
	diprd.lOffset     = 0;
	diprd.dwPhase     = 0;
	diprd.dwPeriod    = (DWORD)(inPeriod * DI_SECONDS);
/*
	// エンベロープ設定
	DIENVELOPE   dienv;
	::ZeroMemory(&dienv, sizeof(dienv));
	dienv.dwSize        = sizeof(dienv);
	dienv.dwAttackLevel = 0;
	dienv.dwAttackTime  = (DWORD)(2.5 * DISECONDS);
	dienv.dwFadeLevel   = 0;
	dienv.dwFadeTime    = (DWORD)(2.5 * DISECONDS);
*/
	// エフェクト設定
	DWORD      Axes[]      = {DIJOFS_X, DIJOFS_Y};		// エフェクト軸
	LONG       Direction[] = {0, 0};					// エフェクト方向

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

	// エフェクト生成
	IDirectInputEffect*   pDIEffect;
	if(m_pDIDevice->CreateEffect(inGUID, &Effect, &pDIEffect, NULL) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - エフェクト生成失敗(CInputDevice_CreateMagnitudeEffect)\n"));
		ReleaseEffect();
		return false;
	}
	pDIEffect->AddRef();

	// エフェクトオブジェクト生成
	delete m_pEffect;
	m_pEffect = new CInputEffect(pDIEffect);
	pDIEffect->Release();

	return true;
}

//------------------------------------------------------------------------------
//	エフェクト解放
//------------------------------------------------------------------------------
void CInputDevice::ReleaseEffect()
{
	// pDeviceがNULLオブジェクトの場合は解放しない
	if(m_pEffect->IsNull())
		return;

	// エフェクトを解放し、NULLオブジェクトを設定
	delete m_pEffect;
	m_pEffect = new CNullInputEffect();
}

//------------------------------------------------------------------------------
//	NULLゲームパッド状態取得
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
