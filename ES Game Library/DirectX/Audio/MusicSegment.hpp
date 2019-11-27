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
	 MusicSegment.hpp
				ミュージックセグメントクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <dmusici.h>

//------------------------------------------------------------------------------
//	ミュージックセグメント演奏フラグ
//------------------------------------------------------------------------------
enum DXAMUSICSEGMENT_PLAYFLAGS {
	DXAMS_PRIMARY   = 0,						// プライマリ
	DXAMS_SECONDARY = DMUS_SEGF_SECONDARY,		// セカンダリ
	DXAMS_QUEUE     = DMUS_SEGF_QUEUE			// 演奏キューに登録
};

//------------------------------------------------------------------------------
//	ミュージックセグメントインタフェース定義
//------------------------------------------------------------------------------
class IMusicSegment {
public:
	virtual ~IMusicSegment() {}

	virtual bool Play(const DWORD inFlag = 0, const int inLoop = -1) = 0;
	virtual bool PlayOnce() = 0;
	virtual void Stop() = 0;

	virtual bool IsPlaying() = 0;
};

//------------------------------------------------------------------------------
//	ミュージックセグメントクラス定義
//------------------------------------------------------------------------------
class CMusicSegment : public IMusicSegment {
public:
	CMusicSegment(IDirectMusicLoader8*      pDMLoader,
				  IDirectMusicPerformance8* pDMPerformance,
				  IDirectMusicSegment8*     pDMSegment,
				  BYTE*                     pWaveData);
	virtual ~CMusicSegment();

	virtual bool Play(const DWORD inFlag, const int inLoop);
	virtual bool PlayOnce() { return Play(DXAMS_SECONDARY, 0); }
	virtual void Stop() { m_pDMPerformance->StopEx(m_pDMSegment, 0, 0); }

	virtual bool IsPlaying();

private:
	IDirectMusicLoader8*        m_pDMLoader;
	IDirectMusicPerformance8*   m_pDMPerformance;
	IDirectMusicSegment8*       m_pDMSegment;
	BYTE*                       m_pWaveData;
};

//------------------------------------------------------------------------------
//	NULLミュージックセグメントクラス定義
//------------------------------------------------------------------------------
class CNullMusicSegment : public IMusicSegment {
public:
	CNullMusicSegment() {}
	virtual ~CNullMusicSegment() {}

	virtual bool Play(const DWORD inFlag, const int inLoop) { return false; }
	virtual bool PlayOnce() { return Play(DXAMS_SECONDARY, 0); }
	virtual void Stop() {}

	virtual bool IsPlaying() { return false; }
};
