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
	Vector3 Vector3_Lerp(Vector3 A, Vector3 B, float t);
	CAMERA camera;
	MODEL target ;
	Vector3 targetpos;
	Vector3 targetrote;
	int camera_state = 0;
	float rote_powoer = 0;
};

