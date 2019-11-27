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
	 Thread.cpp
				スレッドクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Thread.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CThread::CThread() : m_hThread(NULL)
{
	// クローズイベント生成
	m_hCloseEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CThread::~CThread()
{
	// イベントクローズ
	if(m_hCloseEvent != NULL) {
		::SetEvent(m_hCloseEvent);
		::CloseHandle(m_hCloseEvent);
		m_hCloseEvent = NULL;
	}

	// スレッド終了待ち
	Join(1);

	// スレッドハンドルクローズ
	if(m_hThread != NULL) {
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

//------------------------------------------------------------------------------
//	開始
//------------------------------------------------------------------------------
void CThread::Start(unsigned (__stdcall *FuncName)(void*), void* arg)
{
	// 以前のスレッドが実行中か調べる
	if(m_hThread != NULL) {
		DWORD   exit_code;
		::GetExitCodeThread(m_hThread, &exit_code);
		if(exit_code == STILL_ACTIVE)
			return;

		// スレッドハンドルクローズ
		::CloseHandle(m_hThread);
	}

	// スレッド生成
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, FuncName, arg, 0, NULL);
}

//------------------------------------------------------------------------------
//	終了待ち
//------------------------------------------------------------------------------
void CThread::Join(const DWORD inMSTimeout)
{
	if(m_hThread == NULL)
		return;
	::WaitForSingleObject(m_hThread, inMSTimeout);
}

//------------------------------------------------------------------------------
//	終了待ち
//------------------------------------------------------------------------------
bool CThread::IsAlive() const
{
	if(::WaitForSingleObject(m_hThread, 0) != WAIT_OBJECT_0)
		return false;
	return true;
}

//------------------------------------------------------------------------------
//	プロセッサーアフィニティ設定
//------------------------------------------------------------------------------
void CThread::SetProcessorAffinity(const DWORD inCpu)
{
	if(m_hThread == NULL)
		return;
	::SetThreadAffinityMask(m_hThread, 1 << inCpu);
}

//------------------------------------------------------------------------------
//	スレッドクローズイベントが起きているか
//------------------------------------------------------------------------------
bool CThread::IsClosed()
{
	if(m_hCloseEvent == NULL)
		return true;

	if(::WaitForSingleObject(m_hCloseEvent, 0) == WAIT_TIMEOUT)
		return false;

	return true;
}
