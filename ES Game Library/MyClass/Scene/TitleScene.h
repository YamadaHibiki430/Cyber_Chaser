#pragma once
#include"BaseScreen.h"
#include "../Input/Input.h"
#include "SceneManager.h"

class TitleScene :public BaseScene {
private:
	SPRITE T_bg;
public:
	void Initialize() ;
	void Update() ;
	void Draw3D() {};
	void Draw2D() ;
	void DrawAlpha3D() {}
	TitleScene() {};
	virtual ~TitleScene() {};
};