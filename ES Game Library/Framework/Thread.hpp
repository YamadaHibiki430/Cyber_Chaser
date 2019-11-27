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
	 Thread.hpp
				スレッドクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <windows.h>
#include <process.h>

//------------------------------------------------------------------------------
//	スレッドクラス定義
//------------------------------------------------------------------------------
class CThread {
public:
	CThread();
	virtual ~CThread();

	void Start(unsigned (__stdcall *FuncName)(void*), void* arg = NULL);
	void Join(const DWORD inMSTimeout = INFINITE);
	bool IsAlive() const;

	void SetProcessorAffinity(const DWORD inCpu);

	inline HANDLE GetCloseEventHandle() const { return m_hCloseEvent; }
	bool IsClosed();

private:
	HANDLE   m_hThread;
	HANDLE   m_hCloseEvent;

	CThread(const CThread&);
	CThread& operator=(const CThread&);
};
