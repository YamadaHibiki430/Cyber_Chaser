#pragma once
#include "../Character/Character.h"
#include "../State/State.h"
#include "../Bullet/BulletManeger.h"

class BulletEnemy;

class BulletEnemyStateProcessor :public StateProcessor {
public:
	BulletEnemy* bulletenemy;
};

class BulletEnemy :public Character {
private:
	BulletEnemyStateProcessor _stateprocessor;
	BulletFactory* bulletfactory;
	std::list<BulletProduct*> Bullet_List;
	void AddBulletList(BulletProduct* bullet);
	Vector3 target;
public:
	BulletEnemy();
	~BulletEnemy() {};

	void Update();
	void Draw3D();
	void EnemyRemove() {};
	void Shot();

	float enemy_scale;
	Vector3 enemy_position;
	Vector3 enemy_rotation;

	void SetTarget(Vector3 _target);

private:
	CHARACTER_TYPE GetType() { return BULLET_ENEMY_TYPE; }

	//-----STATE---------------

    //ë“ã@
	class Idle : public State
	{
	private:
		//èÛë‘ÇÃèäóLé“
		BulletEnemyStateProcessor* _owner;



		float speed = 5.f;
		float time = 0;
		bool is_remove = false;

	public:
		Idle(BulletEnemyStateProcessor* owner) : _owner(owner) {}
		virtual ~Idle() {}

		virtual void Update();
	};
};