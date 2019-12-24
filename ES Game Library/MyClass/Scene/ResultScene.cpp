#include "ResultScene.h"

void ResultScene::Initialize() {
	GC_bg = GraphicsDevice.CreateSpriteFromFile(_T("Sprite/thank.png"));
}

void ResultScene::Update() {
	if (Input::GetPadInputDown(0, 0)) {
		SceneManager::ChangeScene(SceneManager::MAIN);
	}
}

void ResultScene::Draw3D() {

}

void ResultScene::DrawAlpha3D() {

}

void ResultScene::Draw2D() {
	SpriteBatch.Draw(*GC_bg, Vector3(0.f, 0.f, 0.f));

}