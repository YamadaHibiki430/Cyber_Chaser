

#pragma once
#include"BaseScreen.h"
#include"MainScene.h"
#include"TitleScene.h"
#include"ResultScene.h"
#include"GameOver.h"

/*
*シーン管理クラス
*/
class SceneManager {
public:
	enum SCENE {
		TITLE,
		MAIN,
		GAMEOVER,
		RESULT
	};
	static void ChangeScene(SCENE scene);//シーンを引数によって変える関数
	static void Update();//現在のシーンの更新関数
	static void Draw3D();//現在のシーンの描画関数
	static void Draw2D();//現在のシーンの描画関数
	static void DrawAlpha3D();//現在のシーンの描画関数

	static BaseScene *m_pScene;           //現在のシーン

private:

};

