#include "Ground.h"

void Ground::Init() {
	ground = GraphicsDevice.CreateModelFromFile(_T("Model/Ground/ground.x"));
	Material _mtrl;
	_mtrl.Emissive = Color(color);
	ground->SetMaterial(_mtrl);
	ground->SetPosition(Vector3_Zero);
	time = 0.f;
	time_count = 0;
}

void Ground::Update() {
	time += 1.f;
	if (time / 60.f >= 60.f && time_count == 0) {
		SetColor(Vector3(1.f,1.f,0.f));
		time_count = 1;
		time = 0.f;
	}
	if (time / 60.f >= 60.f && time_count == 1) {
		SetColor(Vector3(0.f, 1.f, 0.f));
		time_count = 2;
		time = 0.f;
	}
	if (time / 60.f >= 60.f && time_count == 2) {
		SetColor(Vector3(0.f, 0.f, 1.f));
		time_count = 0;
		time = 0.f;
	}

}

void Ground::Draw3D() {
	ground->Draw();
}

void Ground::SetColor(Vector3 colors) {
	Material _mtrl;
	_mtrl.Emissive = Color(colors);
	ground->SetMaterial(_mtrl);
}