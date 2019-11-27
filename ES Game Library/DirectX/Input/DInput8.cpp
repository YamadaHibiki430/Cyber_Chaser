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
	 DInput8.cpp
				DirectInput8クラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DInput8.hpp"
#include "../Common/ComLibrary.hpp"

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CDInput8::CDInput8() : m_pDInput(NULL), m_pKeyboard(NULL), m_pMouse(NULL)
{
	CComLibrary::Initialize();		// COMライブラリ初期化
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CDInput8::~CDInput8()
{
	Release();
	CComLibrary::Uninitialize();	// COMライブラリ解放
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CDInput8::Initialize(const HWND hWnd)
{
	Release();

	// DirectInput8オブジェクトの生成とインタフェースの取得
	if(::CoCreateInstance(CLSID_DirectInput8, NULL, CLSCTX_INPROC_SERVER,
						  IID_IDirectInput8, (LPVOID*)&m_pDInput)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - DirectInput8オブジェクト生成失敗(CDInput8_Initialize)\n"));
		return false;
	}

	// DirectInput8オブジェクト初期化
	if(m_pDInput->Initialize((HINSTANCE)::GetWindowLong(hWnd, GWL_HINSTANCE),
							 DIRECTINPUT_VERSION)
	   != DI_OK) {
		::OutputDebugString(TEXT("*** Error - DirectInput8オブジェクト初期化失敗(CDInput8_Initialize)\n"));
		Release();
		return false;
	}

	m_pKeyboard = new CNullInputDevice(256);					// キーボードにNULLオブジェクトを設定
	m_pMouse    = new CNullInputDevice(sizeof(DIMOUSESTATE2));	// マウスにNULLオブジェクトを設定

	m_hWnd      = hWnd;

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CDInput8::Release()
{
	m_hWnd = NULL;

	ReleaseGamePad();

	delete m_pMouse;	m_pMouse    = NULL;		// マウス解放
	delete m_pKeyboard;	m_pKeyboard = NULL;		// キーボード解放

	// DirectInput8オブジェクト解放
	if(m_pDInput != NULL) {
		m_pDInput->Release();
		m_pDInput = NULL;
	}
}

//------------------------------------------------------------------------------
//	デバイス解放
//------------------------------------------------------------------------------
void CDInput8::ReleaseDevice(IInputDevice* pDevice)
{
	// pDeviceがNULLまたはNULLオブジェクトの場合は解放しない
	if(pDevice == NULL || pDevice->IsNull())
		return;

	// デバイスを解放し、NULLオブジェクトを設定
	delete pDevice;
	pDevice = new CNullInputDevice();
}

//------------------------------------------------------------------------------
//	キーボードデバイス生成
//------------------------------------------------------------------------------
bool CDInput8::CreateKeyboard()
{
	if(m_pDInput == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectInput未初期化(CDInput8_CreateKeyboard)\n"));
		return false;
	}

	// キーボードオブジェクトが生成済みの場合は、そのまま返す
	if(m_pKeyboard->IsNull() == false)
		return true;

	// デバイス生成
	IDirectInputDevice8*   pDIDevice = NULL;
	if(m_pDInput->CreateDevice(GUID_SysKeyboard, &pDIDevice, NULL) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - デバイス生成失敗(CDInput8_CreateKeyboard)\n"));
		return false;
	}

	// データフォーマット設定
	if(pDIDevice->SetDataFormat(&c_dfDIKeyboard) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - データフォーマット設定失敗(CDInput8_CreateKeyboard)\n"));
		pDIDevice->Release();
		return false;
	}

	// 協調レベル設定
	if(pDIDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - 協調レベル設定失敗(CDInput8_CreateKeyboard)\n"));
		pDIDevice->Release();
		return false;
	}

	// バッファサイズ設定
	DIPROPDWORD   dipd;
	::ZeroMemory(&dipd, sizeof(dipd));
	dipd.diph.dwSize       = sizeof(dipd);
	dipd.diph.dwHeaderSize = sizeof(dipd.diph);
	dipd.diph.dwObj        = 0;
	dipd.diph.dwHow        = DIPH_DEVICE;
	dipd.dwData            = DIDEV_BUFSIZE;
	if(pDIDevice->SetProperty(DIPROP_BUFFERSIZE, &dipd.diph) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - バッファサイズ設定失敗(CDInput8_CreateKeyboard)\n"));
		pDIDevice->Release();
		return false;
	}

	// アクセス権取得
	pDIDevice->Acquire();

	// キーボードオブジェクト生成
	delete m_pKeyboard;
	m_pKeyboard = new CKeyboard(pDIDevice);
	pDIDevice->Release();

	return true;
}

//------------------------------------------------------------------------------
//	マウスデバイス生成
//------------------------------------------------------------------------------
bool CDInput8::CreateMouse()
{
	if(m_pDInput == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectInput未初期化(CDInput8_CreateMouse)\n"));
		return false;
	}

	// マウスオブジェクトが生成済みの場合は、そのまま返す
	if(m_pMouse->IsNull() == false)
		return true;

	// デバイス生成
	IDirectInputDevice8*   pDIDevice = NULL;
	if(m_pDInput->CreateDevice(GUID_SysMouse, &pDIDevice, NULL) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - デバイス生成失敗(CDInput8_CreateMouse)\n"));
		return false;
	}

	// データフォーマット設定
	if(pDIDevice->SetDataFormat(&c_dfDIMouse2) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - データフォーマット設定失敗(CDInput8_CreateMouse)\n"));
		pDIDevice->Release();
		return false;
	}

	// 協調レベル設定
	if(pDIDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - 協調レベル設定失敗(CDInput8_CreateMouse)\n"));
		pDIDevice->Release();
		return false;
	}

	// バッファサイズ設定
	DIPROPDWORD   dipd;
	::ZeroMemory(&dipd, sizeof(dipd));
	dipd.diph.dwSize       = sizeof(dipd);
	dipd.diph.dwHeaderSize = sizeof(dipd.diph);
	dipd.diph.dwObj        = 0;
	dipd.diph.dwHow        = DIPH_DEVICE;
	dipd.dwData            = DIDEV_BUFSIZE;
	if(pDIDevice->SetProperty(DIPROP_BUFFERSIZE, &dipd.diph) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - バッファサイズ設定失敗(CDInput8_CreateMouse)\n"));
		pDIDevice->Release();
		return false;
	}

	// アクセス権取得
	pDIDevice->Acquire();

	// マウスオブジェクト生成
	delete m_pMouse;
	m_pMouse = new CMouse(pDIDevice);
	pDIDevice->Release();

	return true;
}

//------------------------------------------------------------------------------
//	ゲームパッドデバイス解放
//------------------------------------------------------------------------------
void CDInput8::ReleaseGamePad()
{
	for(unsigned int i = 0; i < m_GamePad.size(); i++)
		delete m_GamePad[i];
	m_GamePad.clear();
}

//------------------------------------------------------------------------------
//	ゲームパッドデバイス生成
//------------------------------------------------------------------------------
DWORD CDInput8::CreateGamePad(const DWORD inCount)
{
	if(m_pDInput == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectInput未初期化(CDInput8_CreateGamePad)\n"));
		return 0;
	}

	ReleaseGamePad();

	if(inCount == 0)
		return 0;

	m_GamePad.resize(inCount, NULL);

	// ゲームパッドデバイス生成
	m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL,
						   (LPDIENUMDEVICESCALLBACK)EnumGamePadProc, this, DIEDFL_ATTACHEDONLY);

	// 生成できたゲームパッドを数える
	unsigned int   i, Create = 0;
	for(i = 0; i < m_GamePad.size(); i++) {
		if(m_GamePad[i] == NULL)
			break;
		Create++;
	}

	// 足りない部分にNullGamePadを設定
	for(i = Create; i < inCount; i++)
		m_GamePad[i] = new CNullGamePad(sizeof(DIJOYSTATE));

	return Create;
}

//------------------------------------------------------------------------------
//	ゲームパッド列挙コールバック
//------------------------------------------------------------------------------
BOOL CALLBACK CDInput8::EnumGamePadProc(LPDIDEVICEINSTANCE pDevInst, LPVOID pRef)
{
	return ((CDInput8*)pRef)->InitGamePad(pDevInst, pRef);
}

//------------------------------------------------------------------------------
//	ゲームパッド初期化
//------------------------------------------------------------------------------
BOOL CDInput8::InitGamePad(const LPDIDEVICEINSTANCE pDevInst, LPVOID pRef)
{
#ifdef _DEBUG
	if(m_pDInput == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectInput未初期化(CDInput8_InitGamePad)\n"));
		return DIENUM_STOP;
	}
#endif

	// ゲームパッドオブジェクト作成
	IDirectInputDevice8*   pDIDevice;
	if(m_pDInput->CreateDevice(pDevInst->guidInstance, &pDIDevice, NULL) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - ゲームパッドオブジェクト作成失敗(CDInput8_InitGamePad)\n"));
		return DIENUM_CONTINUE;
	}

	// データフォーマット設定
	if(pDIDevice->SetDataFormat(&c_dfDIJoystick) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - データフォーマット設定失敗(CDInput8_InitGamePad)\n"));
		pDIDevice->Release();
		return DIENUM_CONTINUE;
	}

	// 協調レベル設定
	if(pDIDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE) != DI_OK) {
		::OutputDebugString(TEXT("*** Error - 協調レベル設定失敗(CDInput8_InitGamePad)\n"));
		pDIDevice->Release();
		return DIENUM_CONTINUE;
	}

	// バッファサイズ設定
	DIPROPDWORD   dipd;
	::ZeroMemory(&dipd, sizeof(dipd));
	dipd.diph.dwSize       = sizeof(dipd);
	dipd.diph.dwHeaderSize = sizeof(dipd.diph);
	dipd.diph.dwObj        = 0;
	dipd.diph.dwHow        = DIPH_DEVICE;
	dipd.dwData            = DIDEV_BUFSIZE;
	if(FAILED(pDIDevice->SetProperty(DIPROP_BUFFERSIZE, &dipd.diph))) {
		::OutputDebugString(TEXT("*** Error - バッファサイズ設定失敗(CDInput8_InitGamePad)\n"));
		pDIDevice->Release();
		return DIENUM_CONTINUE;
	}

	// 軸範囲設定
	DIENUMAXESARGS   Args = {this, pDIDevice};
	if(FAILED(pDIDevice->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACK)CDInput8::EnumGamePadAxesProc,
									 &Args, DIDFT_AXIS))) {
		pDIDevice->Release();
		return DIENUM_CONTINUE;
	}

	// アクセス権取得
	pDIDevice->Acquire();

	// ゲームパッド配列へ追加
	for(unsigned int i = 0; i < m_GamePad.size(); i++) {
		if(m_GamePad[i] == NULL) {
			m_GamePad[i] = new CGamePad(pDIDevice);
			break;
		}
	}
	pDIDevice->Release();

	// 列挙終了判定
	if(m_GamePad[m_GamePad.size() - 1] != NULL)
		return DIENUM_STOP;

	return DIENUM_CONTINUE;
}

//------------------------------------------------------------------------------
//	ゲームパッド軸列挙コールバック
//------------------------------------------------------------------------------
BOOL CALLBACK CDInput8::EnumGamePadAxesProc(LPDIDEVICEOBJECTINSTANCE pDevObjInst, LPVOID pRef)
{
	return ((DIENUMAXESARGS*)pRef)->pDInput->SetAxesProp(pDevObjInst, pRef);
}

//------------------------------------------------------------------------------
//	軸プロパティ設定
//------------------------------------------------------------------------------
BOOL CDInput8::SetAxesProp(const LPDIDEVICEOBJECTINSTANCE pDevObjInst, LPVOID pRef)
{
	IDirectInputDevice8*   pDIDevice = ((DIENUMAXESARGS*)pRef)->pDIDevice;

#ifdef _DEBUG
	if(pDIDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - ゲームパッド未初期化(CDInput8_SetAxesProp)\n"));
		return DIENUM_STOP;
	}
#endif

	// 軸範囲設定(絶対軸のみ)
	if((pDevObjInst->dwType & DIDFT_ABSAXIS) != 0) {
		DIPROPRANGE   dipr;
		dipr.diph.dwSize       = sizeof(dipr);
		dipr.diph.dwHeaderSize = sizeof(dipr.diph);
		dipr.diph.dwHow        = DIPH_BYID;
		dipr.diph.dwObj        = pDevObjInst->dwType;
		dipr.lMin              = DIDEV_AXIS_MIN;
		dipr.lMax              = DIDEV_AXIS_MAX;
		if(FAILED(pDIDevice->SetProperty(DIPROP_RANGE, &dipr.diph)))
			::OutputDebugString(TEXT("*** Error - 軸範囲設定失敗(CDInput8_SetAxesProp)\n"));
	}

	// デッドゾーン設定
	DIPROPDWORD   dipdw;
	dipdw.diph.dwSize       = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwHow        = DIPH_BYID;
	dipdw.diph.dwObj        = pDevObjInst->dwType;
	dipdw.dwData            = DIDEV_AXIS_DEADZONE;
	if(FAILED(pDIDevice->SetProperty(DIPROP_DEADZONE, &dipdw.diph)))
		::OutputDebugString(TEXT("*** Error - デッドゾーン設定失敗(CDInput8_SetAxesProp)\n"));

	return DIENUM_CONTINUE;
}

//------------------------------------------------------------------------------
//	デバイスアクセス権取得
//------------------------------------------------------------------------------
void CDInput8::Acquire()
{
	if(m_pDInput == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectInput未初期化(CDInput8_Acquire)\n"));
		return;
	}

	m_pKeyboard->Acquire();
	m_pMouse   ->Acquire();

	for(unsigned int i = 0; i < m_GamePad.size(); i++)
		m_GamePad[i]->Acquire();
}

//------------------------------------------------------------------------------
//	デバイスアクセス権解放
//------------------------------------------------------------------------------
void CDInput8::Unacquire()
{
	if(m_pDInput == NULL) {
//		::OutputDebugString(TEXT("*** Error - DirectInput未初期化(CDInput8_Unacquire)\n"));
		return;
	}

	m_pKeyboard->Unacquire();
	m_pMouse   ->Unacquire();

	for(unsigned int i = 0; i < m_GamePad.size(); i++)
		m_GamePad[i]->Unacquire();
}
