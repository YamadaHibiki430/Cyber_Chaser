#include "ShortWall.h"

ShortWall::ShortWall() {
	SetSimpleColor(Vector3(0.0f, 1.0f, 0.0f));
	SetSimpleModel(1.f, 1.f, 1.f);

	hitbox = new HitBox;
	hitbox->Init();
	hitbox->Settags("enemy");
	hitbox->SetHitBoxScale(5.f);

	_scale = 6.f;

	int ram = MathHelper_Random(-2, 4);

	_rotation = Vector3(0.0f, 0.0f, 0.0f);
	_position = Vector3(10.f * ram, 10.0f, -1000.0f);
	time = 0;
}

ShortWall::~ShortWall() {
	hitbox->OnReMove();
}

void ShortWall::Update() {
	_position += _model->GetFrontVector() * 10.0f;
	time += 1.f;
	if (time >= 200.f) {
		remove();
		time = 0;
	}
}

void ShortWall::Draw3D() {
	hitbox->SetHitBoxPosition(_position + Vector3(0.f, 0.f, 0.f));
	hitbox->Draw3D();
	SetSimplePosition(_position);
	SetSimpleRotation(_rotation);
	SetSimpleScale(_scale);

	_model->Draw();
}