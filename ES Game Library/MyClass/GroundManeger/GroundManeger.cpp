#include "GroundManeger.h"


void GroundManeger::Initialize() {
	Road_Ref straight(new RoadStraight());
	AddList(straight);
	Road_Ref back(new RoadBack());
	AddList(back);


	time = 0.0f;
	back_time = 0.f;
	//“¹˜H¶¬”
	create_count = 0;
}

void GroundManeger::Update() {
	time += 1.0f;
	back_time += 0.5f;
	if (time >= 22.5f) {
		Road_Ref straight(new RoadStraight());
		AddList(straight);
		time = 0;
		create_count += 1;
	}
	if (back_time >= 75.f) {
		Road_Ref back(new RoadBack());
		AddList(back);
		back_time = 0;
	}

	std::for_each(road_list.begin(), road_list.end(), [](Road_Ref& road) {road->Update(); });


	auto end = std::remove_if(road_list.begin(), road_list.end(), [](Road_Ref& chara) {return chara->IsRemove(); });

	road_list.erase(end, road_list.end());
}

void GroundManeger::JointRoteto() {

}

void GroundManeger::AddList(Road_Ref& road) {
	road_list.push_back(road);
}

void GroundManeger::Draw3D() {
	std::for_each(road_list.begin(), road_list.end(), [](Road_Ref& road) {road->Draw3D(); });
}

//------------------------------------------------------

MODEL Road::loaded_model[] = {};

Road::Road() {
	remove_flag = false;
}

void Road::SetModel(LPCTSTR file_name) {
	if (loaded_model[GetType()] == NULL) {
		loaded_model[GetType()] = GraphicsDevice.CreateModelFromFile(file_name);
	}
	_model = loaded_model[GetType()];
	Material mtrl2(_model);
	mtrl2.Emissive = Color(1.06f, 1.06f, 1.06f);
	_model->SetMaterial(mtrl2);
}

void Road::SetPosition(Vector3 road_pos) {
	_model->SetPosition(road_pos);
}

void Road::SetScale(float road_scale) {
	_model->SetScale(road_scale);
}

void Road::SetRotation(Vector3 road_rot) {
	_model->SetRotation(road_rot);
}

void Road::SetSpeed(float _speed) {
	move_speed = _speed;
}

void Road::Draw3D() {
	_model->Draw();
}

//------------------------------------------------------

RoadStraight::RoadStraight() {
	SetModel(_T("Model/Ground/ground.x"));
	rotation = Vector3(0.0f, 0.0f, 0.0f);
	position = Vector3(0.0f, 0.0f, -2000.0f);
	SetScale(2.f);
	time = 0;
}

RoadStraight::~RoadStraight() {

}

void RoadStraight::Update() {
	position += _model->GetFrontVector() * 10.0f ;
	time += 1.f;
	if (time >= 250.f) {
		remove();
		time = 0;
	}
}

void RoadStraight::Draw3D() {
	SetPosition(position);
	SetRotation(rotation);
	_model->Draw();
}

//-----------------------------------------------------

RoadBack::RoadBack() {
	SetModel(_T("Model/Back/bigbox_v02.X"));
	rotation = Vector3(0.0f, 0.0f, 0.0f);
	position = Vector3(0.0f, 0.0f, -6000.0f);
	time = 0;
}

RoadBack::~RoadBack() {

}

void RoadBack::Update() {
	position += _model->GetFrontVector() * 20.0f;
	time += 0.01f;
	if (time >= 400.f) {
		remove();
		time = 0;
	}
}

void RoadBack::Draw3D() {
	SetPosition(position);
	SetRotation(rotation);
	_model->Draw();
}

//----------------------------------------------------

RoadWall::RoadWall() {
	SetModel(_T("Model/Back/bigbox_v02.X"));
	rotation = Vector3(0.0f, 0.0f, 0.0f);
	position = Vector3(0.0f, 0.0f, -1000.0f);
	time = 0;
}

RoadWall::~RoadWall() {

}

void RoadWall::Update() {

}

void RoadWall::Draw3D() {

}
