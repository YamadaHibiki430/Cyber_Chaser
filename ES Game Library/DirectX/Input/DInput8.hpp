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
	 DInput8.hpp
				DirectInput8クラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	バージョン宣言
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "InputDevice.hpp"
#include <vector>

//------------------------------------------------------------------------------
//	DirectInput8クラス定義
//------------------------------------------------------------------------------
class CDInput8 {
public:
	virtual ~CDInput8();

	// シングルトンインスタンスの取得
	static CDInput8& GetInstance() { static CDInput8 theDInput; return theDInput; }

	bool Initialize(const HWND hWnd);
	void Release();

	bool CreateKeyboard();
	bool CreateMouse();

	void ReleaseKeyboard() { ReleaseDevice(m_pKeyboard); }
	void ReleaseMouse()    { ReleaseDevice(m_pMouse);    }

	IInputDevice* GetKeyboard() const { return m_pKeyboard; }
	IInputDevice* GetMouse()    const { return m_pMouse;    }

	DWORD CreateGamePad(const DWORD inCount);
	void ReleaseGamePad();
	IInputDevice* GetGamePad(int i) const { return m_GamePad.at(i); }

	void Acquire();
	void Unacquire();

	inline void ResetDevice()   { Acquire();   }
	inline void SuspendDevice() { Unacquire(); }

	inline HWND GetHWnd() const { return m_hWnd; }

private:
	CDInput8();

	void ReleaseDevice(IInputDevice* pDevice);
	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}

	// ゲームパッド列挙コールバック
	static BOOL CALLBACK EnumGamePadProc(LPDIDEVICEINSTANCE pDevInst, LPVOID pRef);
	// ゲームパッド初期化
	BOOL InitGamePad(const LPDIDEVICEINSTANCE pDevInst, LPVOID pRef);

	// ゲームパッド軸列挙コールバック
	static BOOL CALLBACK EnumGamePadAxesProc(LPDIDEVICEOBJECTINSTANCE pDevObjInst, LPVOID pRef);
	// ゲームパッド軸設定
	BOOL SetAxesProp(const LPDIDEVICEOBJECTINSTANCE pDevObjInst, LPVOID pRef);

	// メンバ変数
	IDirectInput8*               m_pDInput;			// DirectInput8インタフェース
	HWND                         m_hWnd;			// メインウィンドウハンドル

	IInputDevice*                m_pKeyboard;		// キーボード
	IInputDevice*                m_pMouse;			// マウス

	std::vector<IInputDevice*>   m_GamePad;			// ゲームパッド

	// ゲームパッド軸列挙引数構造体
	struct DIENUMAXESARGS {
		CDInput8*              pDInput;
		IDirectInputDevice8*   pDIDevice;
	};

	CDInput8(const CDInput8&);
	CDInput8& operator=(const CDInput8&);
};

//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
inline CDInput8& DInput() { return CDInput8::GetInstance(); }

inline IInputDevice* DIKeyboard()     { return DInput().GetKeyboard(); }
inline IInputDevice* DIMouse()        { return DInput().GetMouse();    }
inline IInputDevice* DIGamePad(int i) { return DInput().GetGamePad(i); }
