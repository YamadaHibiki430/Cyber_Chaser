#include "HitBox.h"

//static
std::list<HitBox*> HitBox::_HitBox_list;
//デストラクタ
HitBox::~HitBox() {

}
void HitBox::Init() {
	//HitBox生成
	SetHitBox(1.f, 1.f, 1.f);
	//自分をリストに挿入
	_HitBox_list.push_back(this);

	//タグ設定
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
	//パラメータ設定
	_width = width;
	_height = height;
	_depth = depth ;
	//Model設定
	SimpleShape   shape;
	shape.Type = Shape_Box;
	shape.Width = _width;
	shape.Height = _height;
	shape.Depth = _depth;
	_model = GraphicsDevice.CreateModelFromSimpleShape(shape);
	//Material設定
	Material _mtrl;
	_mtrl.Emissive = Color(color);
	_model->SetMaterial(_mtrl);
}

void HitBox::SetColor(Vector3 colors) {
	Material _mtrl;
	_mtrl.Emissive = Color(colors);
	_model->SetMaterial(_mtrl);
}

// 消去時に呼ばれる処理
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
		//自分とは判定しない
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

