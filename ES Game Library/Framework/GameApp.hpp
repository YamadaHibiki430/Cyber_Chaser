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
	 GameApp.hpp
				ゲームアプリケーションクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SingleApp.hpp"
#include "GameFrameWindow.hpp"
#include "GameProc.hpp"

//------------------------------------------------------------------------------
// ゲームアプリケーションクラス定義
//------------------------------------------------------------------------------
class CGameApp {
public:
	virtual ~CGameApp() {}

	bool Initialize(const HINSTANCE hInstance);
	void Release();

	int Run();

	// アクセス関数
	inline HWND GetHWnd() const { return m_GameFrameWindow.GetHWnd(); }

	// シングルトンインスタンス取得
	static CGameApp& GetInstance()
	{
		static CGameApp   theGameApp;
		return theGameApp;
	}


private:
	CGameApp();

	// メッセージ処理
	bool MessageLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

	LRESULT OnPaint        (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnKeyDown      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysKeyDown   (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysCommand   (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnActivateApp  (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnCreate       (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnClose        (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnDestroy      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnExitMouseMove(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnNCLButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnNCRButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);

	// メンバ変数
	static CGameApp*   m_pGameApp;			// 自身へのポインタ(ウィンドウプロシージャ用)

	bool               m_Windowed;			// ウィンドウモードフラグ
	bool               m_Active;			// アクティブ状態フラグ

	CSingleApp         m_SingleApp;			// 多重起動検出
	CGameFrameWindow   m_GameFrameWindow;	// ゲーム用フレームウィンドウ
	CGameProc          m_GameProc;

	CGameApp(const CGameApp&);				// コピーコンストラクタを生成しない
	CGameApp& operator=(const CGameApp&);	// 代入演算子を生成しない
};

//------------------------------------------------------------------------------
//	短縮名定義
//------------------------------------------------------------------------------
inline CGameApp& GameApp() { return CGameApp::GetInstance(); }
