#pragma once
#include "../../ESGLib.h"
#include "../Input/Input.h"

class CameraManejar
{
public:
	void Init();
	void Move();

	void SetLockAtPosCamera(Vector3 atpos, int id);
	void Draw();
	Vector3 GetCameraVector();
private:
	CAMERA camera;
	MODEL target ;
	Vector3 targetpos;
	Vector3 targetrote;

};

