#include "HitBox.h"

//static
std::list<HitBox*> HitBox::_HitBox_list;
//�f�X�g���N�^
HitBox::~HitBox() {

}
void HitBox::Init() {
	//HitBox����
	SetHitBox(1.f, 1.f, 1.f);
	//���������X�g�ɑ}��
	_HitBox_list.push_back(this);

	//�^�O�ݒ�
	tag = "HitBox";
};

void HitBox::Draw3D() {
	_model->SetPosition(position);
	_model->SetScale(scale);
#if _DEBUG
	_model->Draw();
#endif
}
void HitBox::DrawAlpha3D() {
	//
}

void HitBox::Settags(string tags) {
	tag = tags;
}

void HitBox::SetHitBox(float width, float height, float depth) {
	//�p�����[�^�ݒ�
	_width = width;
	_height = height;
	_depth = depth ;
	//Model�ݒ�
	SimpleShape   shape;
	shape.Type = Shape_Box;
	shape.Width = _width;
	shape.Height = _height;
	shape.Depth = _depth;
	_model = GraphicsDevice.CreateModelFromSimpleShape(shape);
	//Material�ݒ�
	Material _mtrl;
	_mtrl.Emissive = Color(color);
	_model->SetMaterial(_mtrl);
}

void HitBox::SetColor(Vector3 colors) {
	Material _mtrl;
	_mtrl.Emissive = Color(colors);
	_model->SetMaterial(_mtrl);
}

// �������ɌĂ΂�鏈��
void HitBox::OnReMove() {
	auto it = _HitBox_list.begin();
	while (it != _HitBox_list.end()) {
		if ((*it) != this) {
			it++;
		}
		else {
			it = _HitBox_list.erase(it);
			return;
		}
	}
}

bool HitBox::IsHit(HitBox* other) {
	OrientedBoundingBox model_obb1 = this->_model->GetOBB();
	OrientedBoundingBox model_obb2 = other->_model->GetOBB();
	return model_obb1.Intersects(model_obb2);
}

std::list<HitBox*> HitBox::HitHitBoxlist() {
	std::list<HitBox*>  result;
	for (auto&& h : _HitBox_list) {
		//�����Ƃ͔��肵�Ȃ�
		if (h->GetThisHitBox() == this) continue;
		if (h->IsHit(this)) result.push_back(h->GetThisHitBox());
	}
	return result;
};

void HitBox::SetHitBoxPosition(Vector3 pos) {
	position = pos;
}

void HitBox::SetHitBoxScale(float sca) {
	scale = sca;
}

