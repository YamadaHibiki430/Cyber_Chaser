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
	 Random.hpp
				乱数クラスヘッダーファイル

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
//	乱数クラス定義
//------------------------------------------------------------------------------
class CRandom {
public:
	CRandom() { SetSeed(::GetTickCount()); }
	CRandom(const unsigned long inSeed) { SetSeed(inSeed); }
	virtual ~CRandom() {}

	void SetSeed(const unsigned long inSeed);

	inline int Next() { return static_cast<int>(Sample() >> 1); }
	inline int Next(const int inMax) { return Next() % inMax; }
	inline int Next(const int inMin, const int inMax)
	{ return static_cast<int>((Sample() % (inMax - inMin)) + inMin); }

	inline unsigned long NextULong() { return Sample(); }

	float NextFloat()
	{ return static_cast<float>(Sample() * (1.0 / 4294967296.0)); }

	double NextDouble()
	{
	    const unsigned long u = Sample() >> 5, d = Sample() >> 6;
	    return (u * 67108864.0 + d) * (1.0 / 9007199254740992.0);
	}

	inline int NextInt()                                 { return Next(); }
	inline int NextInt(const int inMax)                  { return Next(inMax); }
	inline int NextInt(const int inMin, const int inMax) { return Next(inMin, inMax); }

private:
	unsigned long Sample();
	void NextState();

	unsigned long MixBits(const unsigned long u, const unsigned long l) const
	{ return (u & UPPER_MASK) | (l & LOWER_MASK); }

	unsigned long Twist  (const unsigned long u, const unsigned long v) const
	{ return (MixBits(u, v) >> 1) ^ (v & 0x01 ? MATRIX_A : 0); }

	// 定数
	enum {
		N = 624,
		M = 397,
		MATRIX_A     = 0x9908b0df,
		UPPER_MASK   = 0x80000000,
		LOWER_MASK   = 0x7fffffff,
	};
	
	unsigned long    m_State[N];
	unsigned long*   m_pNext;
	int              m_Left;
};

//------------------------------------------------------------------------------
//	ヘルパー関数
//------------------------------------------------------------------------------
float Math_Random();
int   Math_RandomInt();
