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
	 GameWindow.hpp
				ゲーム用ウィンドウクラスヘッダーファイル

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
// ゲーム用ウィンドウクラス定義
//------------------------------------------------------------------------------
class CGameWindow {
public:
	CGameWindow();
	virtual ~CGameWindow() { Destroy(); }

	bool Create(const HINSTANCE hInstance, const bool inWindowed);
	void Destroy();

	bool ChangeMode(const bool inWindowed);

	// アクセス関数
	HWND GetHWnd()   const { return m_hWnd;   }
	bool GetActive() const { return m_Active; }

private:
	bool Adjust(const bool inWindowed);

	// メッセージ処理
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProcedure(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

	LRESULT OnPaint      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnKeyDown    (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysKeyDown (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysCommand (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnActivateApp(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnCreate     (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnClose      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnDestroy    (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);

	// メンバー変数
	HWND   m_hWnd;
	bool   m_Windowed;						// ウィンドウモードフラグ 
	bool   m_Active;						// アクティブ状態フラグ

	static CGameWindow*   m_pGameWindow;	// 自身へのポインタ(ウィンドウプロシージャ用)

	CGameWindow(const CGameWindow&);
	CGameWindow& operator=(const CGameWindow&);
};
