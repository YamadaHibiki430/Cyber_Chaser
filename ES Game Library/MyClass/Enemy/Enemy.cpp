#include "Enemy.h"

Enemy::Enemy(){
	SetSimpleColor(Vector3(0.0f, 1.0f, 0.0f));
	SetSimpleModel(1.f, 1.f, 1.f);
	//ステート
	_stateprocessor.enemy = this;
	_stateprocessor.ChangeState(new Enemy::Idle(&_stateprocessor));

	hitbox = new HitBox;
	hitbox->Init();
	hitbox->Settags("enemy");
	hitbox->SetHitBoxScale(5.f);


	anime_speed = 0.016f;
	enemy_scale = 5.f;
	int ram = MathHelper_Random(0, 3);

	enemy_rotation = Vector3(0.0f, 0.0f, 0.0f);

	if (ram == 0) {
		enemy_position = Vector3(0.f, 10.0f, -240.0f);
	}
	else if(ram == 1)
	{
		enemy_position = Vector3(20.f, 10.0f, -240.0f);
	}
	else if (ram == 2)
	{
		enemy_position = Vector3(-20.f, 10.0f, -240.0f);
	}
	else if (ram == 3)
	{
		enemy_position = Vector3(40.f, 10.0f, -240.0f);
	}
	
	
}
Enemy::~Enemy() {
	hitbox->OnReMove();
}


void Enemy::Update() {
	_stateprocessor.Update();
}

void Enemy::Draw3D() {
	hitbox->SetHitBoxPosition(enemy_position + Vector3(0.f, 0.f, 0.f));
	hitbox->Draw3D();
	SetSimplePosition(enemy_position);
	SetSimpleRotation(enemy_rotation);
	SetSimpleScale(enemy_scale);

	_model->Draw();
}


void Enemy::EnemyRemove() {
	remove();
	hitbox->OnReMove();
}




void Enemy::Idle::Update() {
	time += 1.0f;
	_owner->enemy->enemy_position.z += speed;
	if (time > 60.f) {
		_owner->enemy->EnemyRemove();
		speed = speed * -1;
		time = 0;
	}

	if (_owner->enemy->IsHitObjects("just") ==true) {
		_owner->enemy->EnemyRemove();
	}
}



bool Enemy::IsHitObjects(std::string tags) {
	bool result = false;
	std::list<HitBox*> HitList = hitbox->HitHitBoxlist();
	for (auto&& other : HitList) {
		if (other->tag == tags) {
			result = true;
		}
	}
	return result;
}