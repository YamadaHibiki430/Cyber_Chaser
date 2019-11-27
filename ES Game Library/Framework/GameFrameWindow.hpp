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
	 GameFrameWindow.hpp
				ゲーム用フレームウィンドウクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <windows.h>

//------------------------------------------------------------------------------
// ゲーム用フレームウィンドウクラス定義
//------------------------------------------------------------------------------
class CGameFrameWindow {
public:
	CGameFrameWindow();
	virtual ~CGameFrameWindow() { Destroy(); }

	bool Create(const HINSTANCE hInstance, const WNDPROC inWndProc,
				const UINT inWidth, const UINT inHeight, const bool inWindowed);
	void Destroy();

	bool ChangeMode(const bool inWindowed);

	// アクセス関数
	inline HWND GetHWnd()   const { return m_hWnd;   }
	inline UINT GetWidth()  const { return m_Width;  }
	inline UINT GetHeight() const { return m_Height; }

private:
	// メンバー変数
	HWND   m_hWnd;
	UINT   m_Width;
	UINT   m_Height;

	CGameFrameWindow(const CGameFrameWindow&);
	CGameFrameWindow& operator=(const CGameFrameWindow&);
};
