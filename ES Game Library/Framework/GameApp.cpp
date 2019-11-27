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
	 GameApp.cpp
				ゲームアプリケーション

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "GameApp.hpp"
#include "..\..\Resource\resource.h"

//------------------------------------------------------------------------------
//	クラス変数
//------------------------------------------------------------------------------
CGameApp* CGameApp::m_pGameApp = NULL;

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CGameApp::CGameApp() : m_Windowed(true), m_Active(false),
					   m_SingleApp(TEXT("O&O_GameProgramming")), m_GameFrameWindow(), m_GameProc()
{
	m_pGameApp = this;
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CGameApp::Initialize(const HINSTANCE hInstance)
{
	// アプリ多重起動検出
	if(m_SingleApp.Initialize() == false)
		return false;

	// アプリケーション設定読み込み
	TCHAR   string_buffer[MAX_PATH];
	UINT    width = 1280, height = 720, fps = 60;

	// スクリーン幅
	if(::LoadString(hInstance, IDS_SCREENWIDTH, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		width = (UINT)::_tstoi(string_buffer);
	}

	// スクリーン高さ
	if(::LoadString(hInstance, IDS_SCREENHEIGHT, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		height = (UINT)::_tstoi(string_buffer);
	}

	// フレームレート
	if(::LoadString(hInstance, IDS_FRAMERATE, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		fps = (UINT)::_tstoi(string_buffer);
	}

	// スクリーンモード
	m_Windowed = true;
	if(::LoadString(hInstance, IDS_FULLSCREEN, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		if((UINT)::_tstoi(string_buffer))
			m_Windowed = false;
//		else
//			m_Windowed = true;
	}

	// マウスカーソル表示
	if(::LoadString(hInstance, IDS_MOUSECURSOR, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		if((UINT)::_tstoi(string_buffer) == 0) {
			// マウスカーソル消去
			while(::ShowCursor(FALSE) >= 0)
				;
		}
	}

	// コンテンツフォルダー
	if(::LoadString(hInstance, IDS_CONTENTFOLDER, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   == 0) {
		::_tcscpy_s(string_buffer, _T("Content"));
	}

	// ウィンドウ生成
	if(m_GameFrameWindow.Create(hInstance, CGameApp::WndProc, width, height, m_Windowed) == false)
		return false;

	// DirectX Graphics初期化
	if(DXGraphics().Initialize(m_GameFrameWindow.GetHWnd  (),
							   m_GameFrameWindow.GetWidth (),
							   m_GameFrameWindow.GetHeight(),
							   m_Windowed)
	   == false)
		return false;

	// DirectInput初期化
	if(DInput().Initialize(m_GameFrameWindow.GetHWnd()) == false)
		return false;

	// キーボード生成
	if(DInput().CreateKeyboard() == false)
		return false;

	// DirectXAudio初期化
	DXAudio().Initialize(m_GameFrameWindow.GetHWnd());

	// FPS設定
	FixTimer().SetFPS(fps);

	// デフォルトフォルダ設定
	::SetCurrentDirectory(string_buffer);

	// ゲームシーン設定
	if(m_GameProc.CreateScene(new GameMain()) == false)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CGameApp::Release()
{
	m_GameProc.ReleaseScene();		// ゲームシーン解放

	DShow()     .Release();			// DirectShow解放
	DXAudio()   .Release();			// DirectX Audio解放
	DInput()    .Release();			// DirectInput解放
	DXGraphics().Release();			// DirectX Graphics解放

	m_SingleApp.RemoveWindow();		// ウィンドウ関連づけ解除
	m_GameFrameWindow.Destroy();	// ウィンドウフレーム解放
}

//------------------------------------------------------------------------------
//	実行
//------------------------------------------------------------------------------
int CGameApp::Run()
{
	// アプリケーションメインループ
	while(true) {
		// メッセージ処理
		if(MessageLoop() == false)
			break;
		// ゲーム処理
		if(m_GameProc.MainLoop(m_Active) == false)
			break;
	}

	return 0;
}

//------------------------------------------------------------------------------
//	メッセージループ
//------------------------------------------------------------------------------
bool CGameApp::MessageLoop()
{
	MSG   msg;
	while(::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) != 0) {
		if(::GetMessage(&msg, NULL, 0, 0) <= 0)
			return false;
//		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return true;
}

//------------------------------------------------------------------------------
//	ウィンドウプロシージャ
//------------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameApp->WindowProc(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	メッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::WindowProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	switch(uMsg) {
	  case WM_PAINT:			return OnPaint        (hWnd, wParam, lParam);
	  case WM_KEYDOWN:			return OnKeyDown      (hWnd, wParam, lParam);
	  case WM_SYSKEYDOWN:		return OnSysKeyDown   (hWnd, wParam, lParam);
	  case WM_SYSCOMMAND:		return OnSysCommand   (hWnd, wParam, lParam);
	  case WM_ACTIVATEAPP:		return OnActivateApp  (hWnd, wParam, lParam);
	  case WM_CREATE:			return OnCreate       (hWnd, wParam, lParam);
	  case WM_CLOSE:			return OnClose        (hWnd, wParam, lParam);
	  case WM_DESTROY:			return OnDestroy      (hWnd, wParam, lParam);
//	  case WM_EXITSIZEMOVE:		return OnExitMouseMove(hWnd, wParam, lParam);
//	  case WM_NCLBUTTONDOWN:	return OnNCLButtonDown(hWnd, wParam, lParam);
//	  case WM_NCRBUTTONDOWN:	return OnNCRButtonDown(hWnd, wParam, lParam);
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_PAINTメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnPaint(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	PAINTSTRUCT   ps;
	::BeginPaint(hWnd, &ps);

	DXGraphics().OnPaint(ps);

	::EndPaint(hWnd, &ps);

	return 0;
}

//------------------------------------------------------------------------------
//	WM_KEYDOWNメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
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
LRESULT CGameApp::OnSysKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	switch(wParam) {
	  case VK_RETURN:
		// モード変更
		m_Windowed ^= true;

		if(DXGraphics().ChangeMode(m_Windowed) == false) {
			Release();
			return 0;
		}
		m_GameFrameWindow.ChangeMode(m_Windowed);

		return 0;

	  case VK_F4:
		Release();
		return 0;
	}

	return ::DefWindowProc(hWnd, WM_SYSKEYDOWN, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_SYSCOMMANDメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnSysCommand(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	// タイマーリセット
//	FPSTimer().Reset();

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
LRESULT CGameApp::OnActivateApp(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	if(wParam != 0) {
		m_Active = true;
		DShow()     .Resume();
		DXAudio()   .Resume();
		DXGraphics().ResetDevice();
		DInput()    .ResetDevice();
		FixTimer()  .Reset();
	} else {
		m_Active = false;
		DShow()     .Suspend();
		DXAudio()   .Suspend();
		DInput()    .SuspendDevice();
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_CREATEメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnCreate(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	m_SingleApp.OnCreate(hWnd);	// ウィンドウ関連づけ
	return 0;
}

//------------------------------------------------------------------------------
//	WM_CLOSEメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnClose(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	Release();
	return 0;
}

//------------------------------------------------------------------------------
//	WM_DESTROYメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnDestroy(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	::PostQuitMessage(0);
	return 0;
}

//------------------------------------------------------------------------------
//	WM_EXITMOUSEMOVEメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnExitMouseMove(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
//	FPSTimer().Reset();		// タイマーリセット
	FixTimer().Reset();		// タイマーリセット
	return 0;
}

//------------------------------------------------------------------------------
//	WM_NCLBUTTONUPメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnNCLButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
//	FPSTimer().Reset();		// タイマーリセット

	switch(wParam) {
	  case HTMINBUTTON:
	  case HTCLOSE:
		return ::DefWindowProc(hWnd, WM_NCLBUTTONDOWN, wParam, lParam);
	  case HTCAPTION:
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE, 0);
		return 0;
//		return ::SendMessage(hWnd, WM_SYSCOMMAND, SC_MOVE, lParam);
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_NCRBUTTONUPメッセージ処理
//------------------------------------------------------------------------------
LRESULT CGameApp::OnNCRButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	return 0;
}
