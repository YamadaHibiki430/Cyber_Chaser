#include "TitleScene.h"

void TitleScene::Initialize() {
	T_bg = GraphicsDevice.CreateSpriteFromFile(_T("Sprite/titele.png"));
}

void TitleScene::Update() {
	if (Input::GetPadInputDown(0, 0)) {
		SceneManager::ChangeScene(SceneManager::MAIN);
	}
}

void TitleScene::Draw2D() {
	SpriteBatch.Draw(*T_bg, Vector3(0.f, 0.f, 0.f));
}