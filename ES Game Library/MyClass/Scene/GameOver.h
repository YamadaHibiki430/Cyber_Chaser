#pragma once
#include"BaseScreen.h"
#include "../Input/Input.h"
#include "SceneManager.h"
class GameOver :public BaseScene {
private:
	SPRITE GO_bg;
public:
	void Initialize() ;
	void Update() ;
	void Draw3D() ;
	void Draw2D() ;
	void DrawAlpha3D();
	GameOver() {};
	virtual ~GameOver() {};
};