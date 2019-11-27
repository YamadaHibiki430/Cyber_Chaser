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
	 FixTimer.hpp
				固定タイマークラスヘッダーファイル

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
//	固定タイマークラス定義
//------------------------------------------------------------------------------
class CFixTimer {
public:
	virtual ~CFixTimer();

	void Wait();

	void Reset() { m_LastTime = ::timeGetTime();  }
	bool IsSkip() const { return false; }

	void SetFPS(const UINT inFPS) { m_FPS = inFPS; m_MaxWait = 1000 / inFPS; }
	UINT GetFPS() const { return m_FPS; }

	inline double GetElapsedSecond()      const { return m_MaxWait / 1000.0; }
	inline DWORD  GetElapsedMilliSecond() const { return m_MaxWait; }

	// シングルトンインスタンス取得
	static CFixTimer& GetInstance()
	{
		static CFixTimer   theFixTimer;
		return theFixTimer;
	}

private:
	CFixTimer();

	UINT    m_FPS;			// １秒あたりのフレーム数
	UINT    m_PeriodMin;	// マルチメディアタイマ最小分解能
	DWORD   m_MaxWait;		// 最大休止時間
	DWORD   m_LastTime;		// フレーム終了時間
};

//------------------------------------------------------------------------------
//	マクロ定義
//------------------------------------------------------------------------------
inline CFixTimer& FixTimer() { return CFixTimer::GetInstance(); }
