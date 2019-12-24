#include"MainScene.h"
#include <iostream>

void MainScene::Initialize() {
	groundmaneger.Initialize();
	background = GraphicsDevice.CreateSpriteFromFile(_T("BackGround/BG_v01.png"));
	camera.Init();
	goaltime.Initialize();
	init_time = 0.f;
	SNDMANAGER.Initialize();
	SNDMANAGER.PlayBGM();

}
void MainScene::Update() {
	
	groundmaneger.Update();

	goaltime.Update();



	if (MathHelper_Random(0, 200) <= 10) {
		Character_Ref shortwall(new ShortWall());
		AddList(shortwall);
	}
	if (MathHelper_Random(0, 200) <= 0) {
		Character_Ref longwall(new LongWall());
		AddList(longwall);
	}
	if (MathHelper_Random(0, 200) <= 0) {
		Character_Ref widthwall(new WidthWall());
		AddList(widthwall);
	}
	


	std::for_each(Character_List.begin(), Character_List.end(), [](Character_Ref& chara) {chara->Update(); });
	player.Update();
	
	camera.SetLockAtPosCamera(player.GetAnimePosition(),player.GetPadID());
	player.SetCameraVector(camera.GetCameraVector());



	auto end = std::remove_if(Character_List.begin(), Character_List.end(), [](Character_Ref& chara) {return chara->IsRemove(); });

	Character_List.erase(end, Character_List.end());

	init_time += 1.f;
	if (goaltime.GetMyTime() > 60.f) {
		SceneManager::ChangeScene(SceneManager::RESULT);
		
		
	}else if (player.GetDamageCount() >= 10) {
		SceneManager::ChangeScene(SceneManager::GAMEOVER);
	}

}
void MainScene::Draw3D() {
	groundmaneger.Draw3D();
	std::for_each(Character_List.begin(), Character_List.end(), [](Character_Ref& chara) {chara->Draw3D(); });
	player.Draw3D();
	camera.Draw();
}
void MainScene::Draw2D() {
	player.Draw2D();
	goaltime.Draw2D();
	SpriteBatch.Draw(*background, Vector3(0.f, 0.f, 10000.f));
}
void MainScene::DrawAlpha3D() {
	GraphicsDevice.BeginAlphaBlend();
	std::for_each(Character_List.begin(), Character_List.end(), [](Character_Ref& chara) {chara->DrawAlph(); });
	GraphicsDevice.EndAlphaBlend();
}
void MainScene::AddList(Character_Ref& chara) {
	Character_List.push_back(chara);
}
