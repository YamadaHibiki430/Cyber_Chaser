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
	 GameScene.hpp
				ゲームシーンクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "../DirectX/Graphics/DXGraphics9.hpp"
#include "../DirectX/Input/DInput8.hpp"
#include "../DirectX/Audio/DXAudio8.hpp"
#include "../DirectX/Show/DShow.hpp"
#include "../Framework/FixTimer.hpp"

//------------------------------------------------------------------------------
//	ゲームシーン生成マクロ
//------------------------------------------------------------------------------
#define GAME_SCENE(scene) reinterpret_cast<int>(scene)

//------------------------------------------------------------------------------
// ゲームシーン基本クラス定義
//------------------------------------------------------------------------------
class CGameScene {
public:
	CGameScene() {}
	virtual ~CGameScene() {}

	virtual int ActiveProc()
	{
/*
		if(FPSTimer().IsSkip() == false) {
			Draw();
			DXGraphics().UpdateFrame();
		}
		FPSTimer().Wait();
*/
		Draw();
		DXGraphics().UpdateFrame();
		FixTimer().Wait();
		return Update(); 
	}

	virtual int NonActiveProc() 
	{ 
		::WaitMessage();
		return 0;
	}

public:
	virtual bool Initialize () { return true; }
	virtual void LoadContent() {}

	virtual int  Update() { return 0; }
	virtual void Draw  () {}

private:
	CGameScene(const CGameScene&);
	CGameScene& operator=(const CGameScene&);
};

//------------------------------------------------------------------------------
//	ゲームシーンインクルードファイル
//------------------------------------------------------------------------------
#include "../GameMain.h"