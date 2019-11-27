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
	 GameWindow.cpp
				ゲーム用ウィンドウ

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "GameWindow.hpp"
#include "../../Resource/resource.h"
#include <imm.h>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "imm32.lib")

//------------------------------------------------------------------------------
//	クラス変数
//------------------------------------------------------------------------------
CGameWindow*   CGameWindow::m_pGameWindow = NULL;

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CGameWindow::CGameWindow() : m_hWnd(NULL), m_Windowed(true), m_Active(false)
{
	m_pGameWindow = this;
}

//------------------------------------------------------------------------------
//	ウィンドウ生成
//------------------------------------------------------------------------------
bool CGameWindow::Create(const HINSTANCE hInstance, const bool inWindowed)
{
	Destroy();

	LPCTSTR   WindowClassName = L"OOGAMEPROG_WINDOWCLASS";	// ウィンドウクラス名

	WNDCLASSEX   wcx;
	// ウィンドウクラスが登録済みか調べる
	if(::GetClassInfoEx(hInstance, WindowClassName, &wcx) == 0) {
		// ウィンドウクラス登録
		::ZeroMemory(&wcx, sizeof(wcx));
		wcx.cbSize        = sizeof(wcx);
		wcx.style         = 0;
		wcx.lpfnWndProc   = CGameWindow::WndProc;
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
			::OutputDebugString(L"*** Error - ウィンドウ登録失敗(CGameWindow_Create)\n");
			return false;
		}
	}

	// ウィンドウ生成
	m_hWnd = ::CreateWindowEx(0, WindowClassName, L"ES Game Library", 0,
							  0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if(m_hWnd == NULL) {
		::OutputDebugString(L"*** Error - ウィンドウ生成失敗(CGameWindow_Create)\n");
		return false;
	}

	Adjust(inWindowed);						// ウィンドウ調整
	::ImmAssociateContext(m_hWnd, NULL);	// IME消去
	::ShowWindow(m_hWnd, SW_SHOW);			// ウィンドウ表示
	::SetFocus(m_hWnd);						// フォーカス設定

	return true;
}

//------------------------------------------------------------------------------
//	ウィンドウ生成
//------------------------------------------------------------------------------
void CGameWindow::Destroy()
{
	// ウィンドウ破棄
	if(m_hWnd != NULL) {
		::ShowWindow(m_hWnd, SW_HIDE);
		::DestroyWindow(m_hWnd);

		m_Active   = false;
		m_Windowed = true;
		m_hWnd     = NULL;
	}
}

//------------------------------------------------------------------------------
//	ウィンドウモード変更
//------------------------------------------------------------------------------
bool CGameWindow::ChangeMode(const bool inWindowed)
{
	return Adjust(inWindowed);
}

//------------------------------------------------------------------------------
//	ウィンドウ形状・位置設定
//------------------------------------------------------------------------------
bool CGameWindow::Adjust(const bool inWindowed)
{
	m_Windowed = inWindowed;

#ifdef _DEBUG
	if(m_hWnd == NULL) {
		::OutputDebugString(L"*** Error - ウィンドウ未初期化(CGameWindow_Adjust)\n");
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
		::SetRect(&WndRect, 0, 0, 1280, 720);

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

//------------------------------------------------------------------------------
//	ウィンドウプロシージャ
//------------------------------------------------------------------------------
LRESULT CALLBACK CGameWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameWindow->WindowProcedure(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	メッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::WindowProcedure(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	switch(uMsg) {
	  case WM_PAINT:		return OnPaint      (hWnd, wParam, lParam);
	  case WM_KEYDOWN:		return OnKeyDown    (hWnd, wParam, lParam);
	  case WM_SYSKEYDOWN:	return OnSysKeyDown (hWnd, wParam, lParam);
	  case WM_SYSCOMMAND:	return OnSysCommand (hWnd, wParam, lParam);
	  case WM_ACTIVATEAPP:	return OnActivateApp(hWnd, wParam, lParam);
	  case WM_CREATE:		return OnCreate     (hWnd, wParam, lParam);
	  case WM_CLOSE:		return OnClose      (hWnd, wParam, lParam);
	  case WM_DESTROY:		return OnDestroy    (hWnd, wParam, lParam);
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_PAINTメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnPaint(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	PAINTSTRUCT   ps;
	::BeginPaint(hWnd, &ps);

	::EndPaint(hWnd, &ps);

	return 0;
}

//------------------------------------------------------------------------------
//	WM_KEYDOWNメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	switch(wParam) {
	  case VK_ESCAPE:
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_SYSKEYDOWNメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnSysKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	switch(wParam) {
	  case VK_RETURN:
		// ウィンドウモード変更
		ChangeMode(m_Windowed ^ true);
		return 0;

	  case VK_F4:
		Destroy();
		return 0;
	}

	return ::DefWindowProc(hWnd, WM_SYSKEYDOWN, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_SYSCOMMANDメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnSysCommand(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	// スクリーンセーバ起動防止
	switch(wParam) {
	  case SC_SCREENSAVE:
		return 1;
	}

	return ::DefWindowProc(hWnd, WM_SYSCOMMAND, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_ACTIVATEAPPメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnActivateApp(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	if(wParam != 0) {
		m_Active = true;
	} else {
		m_Active = false;
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_CREATEメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnCreate(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	return 0;
}

//------------------------------------------------------------------------------
//	WM_CLOSEメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnClose(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	Destroy();
	return 0;
}

//------------------------------------------------------------------------------
//	WM_DESTROYメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnDestroy(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	::PostQuitMessage(0);
	return 0;
}
