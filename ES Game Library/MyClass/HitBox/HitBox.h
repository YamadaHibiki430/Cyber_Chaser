#pragma once
#include "../../ESGLib.h"

class HitBox {
public:
	//�f�X�g���N�^
	virtual HitBox::~HitBox();

	//������
	virtual void HitBox::Init();

	//�`��
	virtual void HitBox::Draw3D();
	virtual void HitBox::DrawAlpha3D();

	//�p�����[�^��ݒ�
	virtual void HitBox::SetHitBox(float width, float height, float depth);

	//�������ɌĂ΂�鏈��
	void HitBox::OnReMove();

	void SetHitBoxScale(float sca);

	void SetHitBoxPosition(Vector3 pos);

	//���g��Ԃ�
	HitBox* HitBox::GetThisHitBox() { return this; };

	//�Փ˂��Ă���HitBox�̃��X�g���擾
	std::list<HitBox*> HitBox::HitHitBoxlist();

	void Settags(string tags);

	void SetColor(Vector3 colors);

	string tag;
private:
	//�S�Ă�HitBox���i�[���Ă������X�g
	static std::list<HitBox*> _HitBox_list;

	//����p�̃��f��
	MODEL _model = nullptr;

	//�p�����[�^
	float _width = 1.0f;
	float _height = 1.0f;
	float _depth = 1.0f;

	Vector3 position;
	float   scale;

	Vector3 color = Vector3(1.0f, 0.0f, 0.0f);

	//�Փ˔���֐�
	bool HitBox::IsHit(HitBox* other);
	float HitBox::RayRange(HitBox* other, Vector3 angle);

protected:
	
};