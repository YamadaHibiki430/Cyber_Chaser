#include"LongWall.h"

LongWall::LongWall() {
	SetModel(_T("Model/Ground/Portait.X"));

	hitbox = new HitBox;
	hitbox->Init();
	hitbox->Settags("enemy");
	hitbox->SetHitBoxScale(15.f);

	_scale = 1.5f;

	int ram = MathHelper_Random(-2, 5);

	_rotation = Vector3(0.0f, 0.0f, 0.0f);
	_position = Vector3(10.f * ram, 0.0f, -1000.0f);
	time = 0;
}

LongWall::~LongWall() {
	hitbox->OnReMove();
}

void LongWall::Update() {
	_position += _model->GetFrontVector() * 10.0f;
	time += 1.f;
	if (time >= 200.f) {
		remove();
		time = 0;
	}
}

void LongWall::Draw3D() {

}

void LongWall::DrawAlph() {
	hitbox->SetHitBoxPosition(_position + Vector3(-30.f, 10.f, 50.f));
	hitbox->Draw3D();
	SetSimplePosition(_position);
	SetSimpleRotation(_rotation);
	SetSimpleScale(_scale);

	_model->DrawAlpha(1.0f);

}