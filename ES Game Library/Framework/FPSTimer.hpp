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
				FPS制御タイマークラスヘッダーファイル

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
//	FPS制御タイマークラス定義
//------------------------------------------------------------------------------
class CFPSTimer {
public:
	virtual ~CFPSTimer();

	void Wait();
	void Reset() { ::QueryPerformanceCounter((LARGE_INTEGER*)&m_LastTime); m_SurplusTime = 0; }

	bool IsSkip() const { return (m_SurplusTime < 0) ? true : false; }

	void SetFPS(const UINT inFPS) {	m_FPS = inFPS; m_MaxWait = m_Frequency / inFPS; }
	UINT GetFPS() const { return m_FPS; }
	__int64 GetCount();

	void DrawFPS(const HDC hDC);

	// シングルトンインスタンスの取得
	static CFPSTimer& GetInstance()
	{
		static CFPSTimer   theFPSTimer;
		return theFPSTimer;
	}

private:
	CFPSTimer();
	bool InitTimer();

	UINT      m_FPS;			// １秒あたりのフレーム数
	__int64   m_Frequency;		// パフォーマンスカウンタ周波数
	__int64   m_MaxWait;		// 最大休止時間

	__int64   m_LastTime;		// 前フレーム終了時間
	__int64   m_SurplusTime;	// 余剰時間

	UINT      m_PeriodMin;		// マルチメディアタイマ最小分解能

	// フレームレート計測用変数
	int       m_DrawCount;		// 描画数
	int       m_SkipCount;		// スキップ数
	int       m_DrawFPS;		// フレームレート描画用
	int       m_DrawSkip;		// スキップ数描画用
	DWORD     m_LastDraw;		// 前描画時間
};

//------------------------------------------------------------------------------
//	短縮名定義
//------------------------------------------------------------------------------
inline CFPSTimer& FPSTimer() { return CFPSTimer::GetInstance(); }
