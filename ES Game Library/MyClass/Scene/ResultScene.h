#pragma once
#include"BaseScreen.h"
#include "../Input/Input.h"
#include "SceneManager.h"
class ResultScene :public BaseScene {
private:
	SPRITE GC_bg;
public:
	void Initialize() ;
	void Update() ;
	void Draw3D() ;
	void Draw2D() ;
	void DrawAlpha3D();
	ResultScene() {};
	virtual ~ResultScene() {};
};