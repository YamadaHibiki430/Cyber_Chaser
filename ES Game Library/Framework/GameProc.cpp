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
	 GameProc.cpp
				ゲーム処理

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "GameProc.hpp"

//------------------------------------------------------------------------------
//	メインループ
//------------------------------------------------------------------------------
bool CGameProc::MainLoop(const bool inActive)
{
	int   ret;

	if(inActive) {
		ret = m_pGameScene->ActiveProc();		// アクティブ時の処理
	} else {
		ret = m_pGameScene->NonActiveProc();	// 非アクティブ時の処理
	}

	if(ret ==  0)
		return true;							// シーン継続
	if(ret == -1)
		return false;							// ゲーム終了

	return CreateScene((CGameScene*)ret);		// シーン変更
}

//------------------------------------------------------------------------------
//	シーン変更
//------------------------------------------------------------------------------
bool CGameProc::CreateScene(const CGameScene* pScene)
{
	ReleaseScene();

	m_pGameScene = (CGameScene*)pScene;

	if(m_pGameScene->Initialize() == false)
		return false;
	m_pGameScene->LoadContent();

	return true;
}
