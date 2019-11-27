#include "BulletEnemy.h"

BulletEnemy::BulletEnemy() {
	SetSimpleColor(Vector3(0.0f, 1.0f, 0.0f));
	SetSimpleModel(1.f, 1.f, 1.f);

	//ステート
	_stateprocessor.bulletenemy = this;
	_stateprocessor.ChangeState(new BulletEnemy::Idle(&_stateprocessor));

	enemy_scale = 7.f;
	enemy_position = Vector3(10.f, 20.0f, -250.0f);
	enemy_rotation = Vector3(0.0f, 0.0f, 0.0f);
}

void BulletEnemy::Update() {
	std::for_each(Bullet_List.begin(), Bullet_List.end(), [](BulletProduct* bullet) {bullet->Update(); });
	_stateprocessor.Update();
}

void BulletEnemy::Draw3D() {
	std::for_each(Bullet_List.begin(), Bullet_List.end(), [](BulletProduct* bullet) {bullet->Draw3D(); });
	SetSimplePosition(enemy_position);
	SetSimpleRotation(enemy_rotation);
	SetSimpleScale(enemy_scale);

	_model->Draw();
}

void BulletEnemy::SetTarget(Vector3 _target) {
	target = _target;
}

void BulletEnemy::Shot() {
	bulletfactory = new BulletStationeryFactory();
	BulletProduct* Nomal = bulletfactory->Create(NOMAL_TYPE);
	Nomal->SetPosition(enemy_position);
	Nomal->SetTarget(target);
	AddBulletList(Nomal);
}

void BulletEnemy::AddBulletList(BulletProduct* bullet) {
	Bullet_List.push_back(bullet);
}

void BulletEnemy::Idle::Update() {
	if (MathHelper_Random(0, 30) == 0) {
		_owner->bulletenemy->Shot();
	}
}