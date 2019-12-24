
#include"SceneManager.h"
BaseScene *SceneManager::m_pScene = nullptr;       //シーンを空にする/**
/*
*@fn シーンのチェンジ
* @brief 引数のシーンを生成する
* @param 生成するシーン
* @return 
*/
void SceneManager::ChangeScene(SCENE scene) {

	if (m_pScene != nullptr) {
		delete m_pScene;
	}
	int i = 0;
	switch (scene) {          
	case SCENE::TITLE:
		m_pScene = new TitleScene(); 
		break;
	case SCENE::MAIN:
		m_pScene = new MainScene();      
		break;
	case SCENE::RESULT:
		m_pScene = new ResultScene();     
		break;
	case SCENE::GAMEOVER:
		m_pScene = new GameOver();
		break;
	}
	m_pScene->Initialize();
	int a = 1;
}
/*
*@fn 現在のシーンの更新関数
* @brief 現在読み込んでいるシーンの描画関数を呼び出す
* @param 
* @return
*/
void SceneManager::Update() {
	m_pScene->Update();         //現在のシーンの更新関数
}
/*
*@fn 現在のシーンの3D描画関数
* @brief 現在読み込んでいるシーンの3D描画関数を呼び出す
* @param 
* @return 
*/
void SceneManager::Draw3D() {
	m_pScene->Draw3D();           //現在のシーンの描画関数
}
/*
*@fn 現在のシーンのアルファ3D描画関数
* @brief 現在読み込んでいるシーンの3D描画関数を呼び出す
* @param
* @return
*/

void SceneManager::DrawAlpha3D() {
	m_pScene->DrawAlpha3D();
}
/*
*@fn 現在のシーンの2D描画関数
* @brief 現在読み込んでいるシーンの2D描画関数を呼び出す
* @param
* @return
*/
void SceneManager::Draw2D() {
	m_pScene->Draw2D();
}
