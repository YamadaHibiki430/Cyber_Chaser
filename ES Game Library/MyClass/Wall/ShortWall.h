#pragma once
#include "../Character/Character.h"
#include "../HitBox/HitBox.h"

class ShortWall :public Character {
private:
	HitBox* hitbox;

	float time;
	float _scale;
	Vector3 _position;
	Vector3 _rotation;
public:
	ShortWall();
	~ShortWall();
	void Update();
	void Draw3D();
	void DrawAlph();

	CHARACTER_TYPE GetType() { return SHORT_WALL_TYPE; }

	void SetTarget(Vector3 _target) {}
};