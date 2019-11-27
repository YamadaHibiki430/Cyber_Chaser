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
	 SingleApp.hpp
				シングルアプリケーションクラスヘッダーファイル

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
//	シングルアプリケーションクラス定義
//------------------------------------------------------------------------------
class CSingleApp {
public:
	CSingleApp();
	CSingleApp(LPCTSTR inMutexName);
	virtual ~CSingleApp() { Release(); }

	bool Initialize();
	void Release();
	
	bool SetWindow(const HWND hWnd);
	void RemoveWindow();

	bool OnCreate(const HWND hWnd) { return SetWindow(hWnd); }
	void OnClose() { RemoveWindow(); }

private:
	HANDLE   m_hMutex;
	TCHAR    m_MutexName[MAX_PATH + 1];
	HWND     m_hWnd;

	CSingleApp(const CSingleApp&);
	CSingleApp& operator=(const CSingleApp&);
};
