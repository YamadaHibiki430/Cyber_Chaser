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
	 GameProc.hpp
				ゲーム処理クラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "../GameScene/GameScene.hpp"

//------------------------------------------------------------------------------
// ゲーム処理クラス定義
//------------------------------------------------------------------------------
class CGameProc {
public:
	CGameProc() : m_pGameScene(NULL) {}
	virtual ~CGameProc() {}

	bool MainLoop(const bool inActive);

	bool CreateScene(const CGameScene* pScene);
	void ReleaseScene() { delete m_pGameScene; m_pGameScene = NULL; }

private:
	CGameScene*   m_pGameScene;

	CGameProc(const CGameProc&);
	CGameProc& operator=(const CGameProc&);
};
