#include "BulletManeger.h"


Vector3 Vector3_Lerp(Vector3 A, Vector3 B, float t) {

	Vector3 Lerp = Vector3_Zero;

	Lerp.x = MathHelper_Lerp(A.x, B.x, t);
	Lerp.y = MathHelper_Lerp(A.y, B.y, t);
	Lerp.z = MathHelper_Lerp(A.z, B.z, t);

	return Lerp;
}

NomalBullet::NomalBullet() {
	//ModelÝ’è
	SimpleShape   shape;
	shape.Type = Shape_Box;
	shape.Width = 2.f;
	shape.Height = 2.f;
	shape.Depth = 2.f;

	hitbox = new HitBox;
	hitbox->Init();
	hitbox->Settags("enemy");
	hitbox->SetHitBoxScale(2.f);

	model = GraphicsDevice.CreateModelFromSimpleShape(shape);

	//MaterialÝ’è
	Material _mtrl;
	_mtrl.Emissive = Color(color);
	model->SetMaterial(_mtrl);

	model->SetScale(2.f);
	model->SetRotation(Vector3(0.0f, 10.0f, 0.0f));
}

void NomalBullet::Update() {
	position = Vector3_Lerp(position, target + Vector3(target.x *0.5f, 10.0f, 100.0f), 0.05f);
	//position.z += 10.f;
}

void NomalBullet::SetTarget(Vector3 _target) {
	target = _target;
}

void NomalBullet::Draw3D() {
	hitbox->SetHitBoxPosition(position);
	hitbox->Draw3D();
	model->SetPosition(position);
	model->Draw();
}

void NomalBullet::SetPosition(Vector3 pos) {
	position = pos;
}

//NomalBullet::~NomalBullet() {
//	delete this;
//}