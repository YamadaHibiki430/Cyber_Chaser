#pragma once
#include "../Character/Character.h"
#include "../State/State.h"
#include "../HitBox/HitBox.h"

class Enemy;


class EnemyStateProcessor :public StateProcessor {
public:
	Enemy* enemy;
};


class Enemy :public Character {
private:
	EnemyStateProcessor _stateprocessor;
	HitBox* hitbox;
	float anime_speed;
	float enemy_scale;
	Vector3 enemy_position;
	Vector3 enemy_rotation;
public:


	Enemy();
	~Enemy();
	void Update();
	void Draw3D();
	void EnemyRemove();



	bool IsHitObjects(std::string tags);
	void SetTarget(Vector3 _target) {}
private:

	CHARACTER_TYPE GetType() { return ENEMY_TYPE; }

	//-----STATE---------------

	//ë“ã@
	class Idle : public State
	{
	private:
		//èÛë‘ÇÃèäóLé“
		EnemyStateProcessor* _owner;



		float speed =5.f;
		float time =0;

	public:
		Idle(EnemyStateProcessor* owner) : _owner(owner) {}
		virtual ~Idle() {}

		virtual void Update();
	};
};


