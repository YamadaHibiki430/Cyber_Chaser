#pragma once
#include"BaseScreen.h"
#include "../Camera/CameraManager.h"
#include "../Camera/3DCamera.h"
#include "../Ground/Ground.h"
#include "../GroundManeger/GroundManeger.h"
//#include "../Enemy/BulletEnemy.h"
#include "../GoalTime/GoalTime.h"
#include "../Wall/LongWall.h"
#include "../Wall/ShortWall.h"
#include "../Wall/WidthWall.h"
#include "../SEManeger/SoundManager.h"
#include "SceneManager.h"


class MainScene:public BaseScene{
public:
	void Initialize();
	void Update();
	void Draw3D() ;
	void Draw2D() ;
	void DrawAlpha3D();
	MainScene() {};
	virtual ~MainScene(){};
private:
	std::vector<Character_Ref> Character_List;
	void AddList(Character_Ref& chara);
	GroundManeger groundmaneger;
	Player player;
	//Ground grou;
	TimeManeger goaltime;
	CameraManejar camera;
	Vector3 player_pos;
	SPRITE background;
	float init_time;
};