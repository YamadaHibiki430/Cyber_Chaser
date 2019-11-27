#include "ShortWall.h"

ShortWall::ShortWall() {
	SetModel(_T("Model/douro/kari_bluebox.X"));

	hitbox = new HitBox;
	hitbox->Init();
	hitbox->Settags("enemy");
	hitbox->SetHitBoxScale(10.f);

	_scale = 1.f;

	int ram = MathHelper_Random(-2, 4);

	_rotation = Vector3(0.0f, 0.0f, 0.0f);
	_position = Vector3(10.f * ram, 0.0f, -1000.0f);
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
	hitbox->SetHitBoxPosition(_position + Vector3(0.f, 5.f, 0.f));
	hitbox->Draw3D();
	SetSimplePosition(_position);
	SetSimpleRotation(_rotation);
	SetSimpleScale(_scale);

	_model->Draw();
}