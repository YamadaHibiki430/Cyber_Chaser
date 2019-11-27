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
	 GameFrameWindow.cpp
				ゲーム用フレームウィンドウ

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "GameFrameWindow.hpp"
#include "../../Resource/resource.h"
#include <imm.h>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "imm32.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CGameFrameWindow::CGameFrameWindow() : m_hWnd(NULL)
{
}

//------------------------------------------------------------------------------
//	ウィンドウ生成
//------------------------------------------------------------------------------
bool CGameFrameWindow::Create(const HINSTANCE hInstance, const WNDPROC inWndProc,
							  const UINT inWidth, const UINT inHeight, const bool inWindowed)
{
	Destroy();

	LPCTSTR   WindowClassName = TEXT("OOGAMEPROG_WINDOWCLASS");	// ウィンドウクラス名

	WNDCLASSEX   wcx;
	// ウィンドウクラスが登録済みか調べる
	if(::GetClassInfoEx(hInstance, WindowClassName, &wcx) == 0) {
		// ウィンドウクラス登録
		::ZeroMemory(&wcx, sizeof(wcx));
		wcx.cbSize        = sizeof(wcx);
		wcx.style         = 0;
		wcx.lpfnWndProc   = inWndProc;
		wcx.cbClsExtra    = 0;
		wcx.cbWndExtra    = 0;
		wcx.hInstance     = hInstance;
		wcx.hIcon         = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MENUICON));
		wcx.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
		wcx.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
		wcx.lpszClassName = WindowClassName;
		wcx.lpszMenuName  = NULL;
		wcx.hIconSm       = NULL;

		if(::RegisterClassEx(&wcx) == 0) {
			::OutputDebugString(TEXT("*** Error - ウィンドウ登録失敗(CGameWindow_Create)\n"));
			return false;
		}
	}

	// ウィンドウ生成
	m_hWnd = ::CreateWindowEx(0, WindowClassName, TEXT("ES Game Library - 初期化中"), 0,
							  0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if(m_hWnd == NULL) {
		::OutputDebugString(TEXT("*** Error - ウィンドウ生成失敗(CGameWindow_Create)\n"));
		return false;
	}

	// ウィンドウ設定
	m_Width  = inWidth;
	m_Height = inHeight;
	ChangeMode(inWindowed);

	::ImmAssociateContext(m_hWnd, NULL);	// IME消去
	::ShowWindow(m_hWnd, SW_SHOW);			// ウィンドウ表示
	::SetFocus(m_hWnd);						// フォーカス設定

	return true;
}

//------------------------------------------------------------------------------
//	ウィンドウ生成
//------------------------------------------------------------------------------
void CGameFrameWindow::Destroy()
{
	// ウィンドウ破棄
	if(m_hWnd != NULL) {
		::ShowWindow(m_hWnd, SW_HIDE);
		::DestroyWindow(m_hWnd);

		m_hWnd   = NULL;
		m_Width  = 0;
		m_Height = 0;
	}
}

//------------------------------------------------------------------------------
//	ウィンドウモード変更
//------------------------------------------------------------------------------
bool CGameFrameWindow::ChangeMode(const bool inWindowed)
{
#ifdef _DEBUG
	if(m_hWnd == NULL) {
		::OutputDebugString(TEXT("*** Error - ウィンドウ未初期化(CGameWindow_ChangeMode)\n"));
		return false;
	}
#endif

	// ディスプレイサイズ取得
	const int   ScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
	const int   ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	DWORD   WndExStyle = 0, WndStyle = 0;
	RECT    WndRect;
	HWND    hZOder = HWND_TOP;
	if(inWindowed == false) {
		// フルスクリーンモード
#ifdef NDEBUG
		WndExStyle = WS_EX_TOPMOST;
		hZOder     = HWND_TOPMOST;
#endif
		WndStyle   = WS_POPUP | WS_SYSMENU;
		::SetRect(&WndRect, 0, 0, ScreenWidth, ScreenHeight);

		// カーソル消去
		while(::ShowCursor(FALSE) >= 0)
			;
	} else {
		// ウィンドウモード
		WndStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
		::SetRect(&WndRect, 0, 0, m_Width, m_Height);

		// カーソル表示
		while(::ShowCursor(TRUE) < 0)
			;
	}

	// ウィンドウスタイル設定
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, WndExStyle);
	::SetWindowLong(m_hWnd, GWL_STYLE,   WndStyle);

	// ウィンドウサイズ設定
	::AdjustWindowRectEx(&WndRect, WndStyle, FALSE, WndExStyle);
	const long   WndWidth  = WndRect.right  - WndRect.left;			// ウィンドウ幅
	const long   WndHeight = WndRect.bottom - WndRect.top;			// ウィンドウ高さ

	// ウィンドウ位置設定
	const int    WndPosX   = (ScreenWidth  - WndWidth ) / 2;
	const int    WndPosY   = (ScreenHeight - WndHeight) / 2;

	// ウィンドウ変更
	::SetWindowPos(m_hWnd, hZOder, WndPosX, WndPosY, WndWidth, WndHeight, SWP_FRAMECHANGED | SWP_DRAWFRAME | SWP_SHOWWINDOW);

	// オーナーウィンドウ再描画
	HWND   hOwnerWnd = ::GetWindow(m_hWnd, GW_OWNER);
	::InvalidateRect(hOwnerWnd, NULL, TRUE);
	::UpdateWindow(hOwnerWnd);

	return true;
}
