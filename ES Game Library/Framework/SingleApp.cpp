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
	 SingleApp.cpp
				シングルアプリケーション

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SingleApp.hpp"
#include <tchar.h>

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CSingleApp::CSingleApp() : m_hMutex(NULL), m_hWnd(NULL)
{
	::ZeroMemory(m_MutexName, sizeof(m_MutexName));

	// ミューテックス名設定
	::GetModuleFileName(NULL, m_MutexName, sizeof(m_MutexName));

	// '\'を'/'に変更
	for(int i = 0; i < lstrlen(m_MutexName); i++) {
		if(m_MutexName[i] == '\\')
			m_MutexName[i] = '/';
	}
}

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CSingleApp::CSingleApp(LPCTSTR inMutexName) : m_hMutex(NULL), m_hWnd(NULL)
{
	// ミューテックス名設定
	::_tcsncpy_s(m_MutexName, inMutexName, MAX_PATH);
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CSingleApp::Initialize()
{
	CSingleApp::Release();

	// ミューテックス生成
	m_hMutex = ::CreateMutex(NULL, 0, m_MutexName);

	// ミューテックスがすでに生成されていた場合、
	// ミューテックスを所有するウィンドウを探して復元する
	if(::GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND   hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while(hWnd != NULL) {
			// ミューテックス名を持つウィンドウか調べる
			if(::GetProp(hWnd, m_MutexName) != NULL) {
				// 最小化されている場合は復元
				if(::IsIconic(hWnd) != 0)
					::ShowWindow(hWnd, SW_RESTORE);

				::SetForegroundWindow(hWnd);						// フォアグラウンドに設定
				::SetForegroundWindow(::GetLastActivePopup(hWnd));	// 子ウィンドウにフォーカスを設定

				break;
			}	// if(GetProp)

			// プロパティが一致しない場合は次のウィンドウへ
			hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
		}	// while(hWnd)

		// ミューテックス解放
		::CloseHandle(m_hMutex);
		m_hMutex = NULL;

		return false;
	}	// if(GetLastError())

	// ミューテックス所有権取得
	const DWORD   Ret = ::WaitForSingleObject(m_hMutex, 0);
	if(Ret != WAIT_OBJECT_0 && Ret != WAIT_ABANDONED)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CSingleApp::Release()
{
	// プロパティ解除
	RemoveWindow();

	// ミューテックス解放
	if(m_hMutex != NULL) {
		::ReleaseMutex(m_hMutex);	// 所有権解放
		::CloseHandle(m_hMutex);	// ミューテックス解放
		m_hMutex = NULL;
	}
}

//------------------------------------------------------------------------------
//	ウィンドウ関連づけ
//------------------------------------------------------------------------------
bool CSingleApp::SetWindow(const HWND hWnd)
{
#ifdef _DEBUG
	if(m_hMutex == NULL || hWnd == NULL)
		return false;
#endif
	if(m_hWnd == hWnd)
		return true;

	RemoveWindow();

	// ミューテックス名をウィンドウのプロパティに設定する
	if(::SetProp(hWnd, m_MutexName, m_hMutex) == 0) {
		::OutputDebugString(TEXT("*** Error - プロパティ設定失敗(CSingleApp_SetWindow)\n"));
		return false;
	}
	m_hWnd = hWnd;	// ウィンドウハンドルの保存

	return true;
}

//------------------------------------------------------------------------------
//	ウィンドウ関連づけ解除
//------------------------------------------------------------------------------
void CSingleApp::RemoveWindow()
{
	if(m_hWnd == NULL)
		return;

	// ウィンドウのプロパティに設定したミューテックス名を解除する
	::RemoveProp(m_hWnd, m_MutexName);
	m_hWnd = NULL;
}
