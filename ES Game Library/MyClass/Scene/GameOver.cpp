#include"GameOver.h"

void GameOver::Initialize() {
	GO_bg = GraphicsDevice.CreateSpriteFromFile(_T("Sprite/game_over.png"));
}
void GameOver::Update() {
	if (Input::GetPadInputDown(0, 0)) {
		SceneManager::ChangeScene(SceneManager::MAIN);
	}
}

void GameOver::Draw3D() {

}

void GameOver::DrawAlpha3D() {

}

void GameOver::Draw2D() {
	SpriteBatch.Draw(*GO_bg, Vector3(0.f, 0.f, 0.f));
}