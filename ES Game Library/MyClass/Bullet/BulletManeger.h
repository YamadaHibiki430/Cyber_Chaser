#pragma once
#include "../../ESGLib.h"
#include "../HitBox/HitBox.h"

enum Bullet_Type {
	NOMAL_TYPE,
	LASER_TYPE,
	AIMING_TYPE
};



class BulletProduct {
public:
	virtual void Update() = 0;
	virtual void Draw3D() = 0;
	virtual void SetPosition(Vector3 pos) = 0;
	virtual void SetTarget(Vector3 _target) = 0;
};

class NomalBullet :public BulletProduct {
private:
	HitBox* hitbox;
	MODEL model;
	Vector3 position = Vector3(10.f, 20.0f, -250.0f);
	float speed = 5.5f;
	Vector3 target;
	Vector3 color = Vector3(1.0f, 0.0f, 1.0f);
public:
	NomalBullet();
	void Update();
	void Draw3D();
	void SetPosition(Vector3 pos);
	void SetTarget(Vector3 _target);
	~NomalBullet() {};
};

class BulletFactory {
private:
	virtual BulletProduct* CreateBP(Bullet_Type type) = 0;
public:
	BulletProduct* Create(Bullet_Type type) {
		BulletProduct* bp = CreateBP(type);
		return bp;
	}
};

class BulletStationeryFactory :public BulletFactory {

	BulletProduct* CreateBP(Bullet_Type type) {
		switch (type)
		{
		case NOMAL_TYPE:
			return new NomalBullet();
			break;
		default:
			return nullptr;
			break;
		}
	}
};
