#include"MainScene.h"
#include <iostream>

void MainScene::Initialize() {
	groundmaneger.Initialize();
	background = GraphicsDevice.CreateSpriteFromFile(_T("BackGround/BG_v01.png"));
	camera.Init();
	goaltime.Initialize();
	init_time = 0.f;
}
void MainScene::Update() {
	
	groundmaneger.Update();

	goaltime.Update();

	if (goaltime.GetMyTime() > 70.f) {
		goaltime.IsGoal();
	}

	if (MathHelper_Random(0, 100) <= 10) {
		Character_Ref shortwall(new ShortWall());
		AddList(shortwall);
	}
	if (MathHelper_Random(0, 100) <= 0) {
		Character_Ref longwall(new LongWall());
		AddList(longwall);
	}
	


	std::for_each(Character_List.begin(), Character_List.end(), [](Character_Ref& chara) {chara->Update(); });
	player.Update();
	
	if (player.GetIsJust() == true) {
		goaltime.AddMoveTime();
	}

	if (player.GetIsDamage() == true) {
		goaltime.LowerMoveTime();
	}

	camera.SetLockAtPosCamera(player.GetAnimePosition(),player.GetPadID());
	player.SetCameraVector(camera.GetCameraVector());



	auto end = std::remove_if(Character_List.begin(), Character_List.end(), [](Character_Ref& chara) {return chara->IsRemove(); });

	Character_List.erase(end, Character_List.end());

	init_time += 1.f;
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
}
void MainScene::AddList(Character_Ref& chara) {
	Character_List.push_back(chara);
}
