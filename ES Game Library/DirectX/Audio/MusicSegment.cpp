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
	 MusicSegment.cpp
				ミュージックセグメントクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "MusicSegment.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CMusicSegment::CMusicSegment(IDirectMusicLoader8*      pDMLoader,
							 IDirectMusicPerformance8* pDMPerformance,
							 IDirectMusicSegment8*     pDMSegment,
							 BYTE*                     pWaveData)
{
	assert(pDMLoader != NULL && pDMPerformance != NULL && pDMSegment != NULL);

	m_pDMLoader      = pDMLoader;		m_pDMLoader     ->AddRef();		// 参照カウンタインクリメント
	m_pDMPerformance = pDMPerformance;	m_pDMPerformance->AddRef();
	m_pDMSegment     = pDMSegment;		m_pDMSegment    ->AddRef();
	m_pWaveData      = pWaveData;
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CMusicSegment::~CMusicSegment()
{
	m_pDMPerformance->StopEx(m_pDMSegment, 0, 0);		// 停止
	m_pDMSegment    ->Unload(m_pDMPerformance);			// アンロード

	m_pDMLoader->ReleaseObjectByUnknown(m_pDMSegment);	// キャッシュ削除
	m_pDMLoader->CollectGarbage();						// クリーンアップ

	m_pDMSegment    ->Release();						// セグメント解放
	m_pDMPerformance->Release();						// パフォーマンス参照解除
	m_pDMLoader     ->Release();						// ローダ参照解除

	delete[] m_pWaveData;
}

//------------------------------------------------------------------------------
//	再生
//------------------------------------------------------------------------------
bool CMusicSegment::Play(const DWORD inFlag, const int inLoop)
{
	Stop();

	// ループ設定
	if(inLoop >= 0)
		m_pDMSegment->SetRepeats(inLoop);
	else
		m_pDMSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);

	// 再生
	if(m_pDMPerformance->PlaySegmentEx(m_pDMSegment, NULL, NULL, inFlag, 0, NULL, NULL, NULL) != S_OK) {
		::OutputDebugString(TEXT("*** Error - 再生失敗(CMusicSegment_Play)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	再生中か調べる
//------------------------------------------------------------------------------
bool CMusicSegment::IsPlaying()
{
	if(m_pDMPerformance->IsPlaying(m_pDMSegment, NULL) != S_OK)
		return false;

	return true;
}
