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
	 FPSTimer.hpp
				FPS制御タイマー

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "FPSTimer.hpp"

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CFPSTimer::CFPSTimer() : m_FPS(0),       m_Frequency(0),   m_MaxWait(0),
						 m_LastTime(0),	 m_SurplusTime(0), m_PeriodMin(0),
						 m_DrawCount(0), m_SkipCount(0),
						 m_DrawFPS(0),   m_DrawSkip(0),    m_LastDraw(0)
{
	InitTimer();
	SetFPS(60);
	Reset();
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CFPSTimer::~CFPSTimer()
{
	if(m_PeriodMin != 0)
		::timeEndPeriod(m_PeriodMin);
}

//------------------------------------------------------------------------------
//	タイマー初期化
//------------------------------------------------------------------------------
bool CFPSTimer::InitTimer()
{
	// マルチメディアタイマ能力取得
	TIMECAPS   tc;
	::timeGetDevCaps(&tc, sizeof(tc));
	m_PeriodMin = tc.wPeriodMin;

	// マルチメディアタイマ分解能設定
	::timeBeginPeriod(m_PeriodMin);

	// パフォーマンスカウンタ周波数取得
	if(::QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	待機
//------------------------------------------------------------------------------
void CFPSTimer::Wait()
{
	// 現時間取得
	__int64   CurrentTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

	// 待機、超過時間設定
	__int64   WaitTime = m_MaxWait - (CurrentTime - m_LastTime);
	m_SurplusTime += WaitTime;			// 余剰時間の累積
	if(m_SurplusTime < 0) {
		WaitTime = 0;					// 余剰時間が負の場合は追いつくまでウェイトしない
		m_SkipCount++;
	} else {
		if(WaitTime != m_SurplusTime)
			WaitTime = m_SurplusTime;	// 追いついた場合のウェイト調整
		m_SurplusTime = 0;
		m_DrawCount++;
	}

	// ループで待機時間を消費
	do {
//		::Sleep(0);
		::Sleep(1);
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_LastTime);
	} while(WaitTime > m_LastTime - CurrentTime);
}

//------------------------------------------------------------------------------
//	現時間取得
//------------------------------------------------------------------------------
__int64 CFPSTimer::GetCount()
{
	__int64   CurrentTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	return CurrentTime / m_Frequency;
}

//------------------------------------------------------------------------------
//	フレームレート描画
//------------------------------------------------------------------------------
void CFPSTimer::DrawFPS(const HDC hDC)
{
	const DWORD   CurrentTime = ::timeGetTime();			// 現時間
	const DWORD   ElapsedTime = CurrentTime - m_LastDraw;	// 経過時間
	if(ElapsedTime >= 1000) {
		m_DrawFPS   = m_DrawCount * 1000 / ElapsedTime;
		m_DrawSkip  = m_SkipCount * 1000 / ElapsedTime;
		m_DrawCount = 0;
		m_SkipCount = 0;
		m_LastDraw  = CurrentTime;
	}

	// FPS描画
	TCHAR   FPSInfo[32];
	::wsprintf(FPSInfo, TEXT("FPS:%3d Skip:%3d"), m_DrawFPS, m_DrawSkip);
	::TextOut(hDC, 0, 0, FPSInfo, ::lstrlen(FPSInfo));
}
