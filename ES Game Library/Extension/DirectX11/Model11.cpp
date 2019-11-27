/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 Model11.cpp
				���f��(DirectX11)�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4005)
#include "Model11.hpp"
#pragma warning(pop)

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#if !(defined(_DEBUG) || defined(DEBUG))
	#pragma comment(lib, "DirectXTK.lib")
#else
	#pragma comment(lib, "DirectXTKd.lib")
#endif

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CModel11::CModel11(std::unique_ptr<DirectX::Model> upModel)
	: m_upModel(std::move(upModel)),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f, 1.0f)
{
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CModel11::~CModel11()
{
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CModel11::Draw(ID3D11DeviceContext* pContext, DirectX::CommonStates& inStates,
					DirectX::CXMMATRIX& inView, DirectX::CXMMATRIX& inProjection)
{
	DirectX::SimpleMath::Matrix   world = GetWorldMatrix();
	m_upModel->Draw(pContext, inStates, world, inView, inProjection);
}

//------------------------------------------------------------------------------
//	�p�x�ݒ�
//------------------------------------------------------------------------------
void CModel11::SetDirection(const float inX, const float inY, const float inZ)
{
	::D3DXQuaternionIdentity(&m_Rotation);

	// �p���̃N�H�[�^�j�I�������߂�
	D3DXQUATERNION   qt;

	// ���[��
	if(inZ != 0.0f) {
		qt.x = 0.0f;
		qt.y = 0.0f;
		qt.z = ::sinf(D3DXToRadian(inZ) / 2.0f);
		qt.w = ::cosf(D3DXToRadian(inZ) / 2.0f);
		m_Rotation *= qt;
	}

	// �s�b�`
	if(inX != 0.0f) {
		qt.x = ::sinf(D3DXToRadian(inX) / 2.0f);
		qt.y = 0.0f;
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inX) / 2.0f);
		m_Rotation *= qt;
	}

	// ���[
	if(inY != 0.0f) {
		qt.x = 0.0f;
		qt.y = ::sinf(D3DXToRadian(inY) / 2.0f);
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inY) / 2.0f);
		m_Rotation *= qt;
	}
}

//------------------------------------------------------------------------------
//	�ړ�
//------------------------------------------------------------------------------
void CModel11::Move(const float inRight, const float inUp, const float inFront)
{
	// ��]�s�񐶐�
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Rotation);

	// �����x�N�g�������o��
	D3DXVECTOR3*   right = (D3DXVECTOR3*)rotation.m[0];
	D3DXVECTOR3*   up    = (D3DXVECTOR3*)rotation.m[1];
	D3DXVECTOR3*   front = (D3DXVECTOR3*)rotation.m[2];

	// �ړ�
	*right     *= inRight;
	*up        *= inUp;
	*front     *= inFront;
	m_Position += *front + *up + *right;
}

//------------------------------------------------------------------------------
//	��]
//------------------------------------------------------------------------------
void CModel11::Rotation(const float inX, const float inY, const float inZ)
{
	// �N�H�[�^�j�I�����玲�����߂�
	D3DXMATRIX   axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXQUATERNION   qt;

	// ���[
	if (inY != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[1], D3DXToRadian(inY));
		m_Rotation *= qt;
	}

	// �s�b�`
	if (inX != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[0], D3DXToRadian(inX));
		m_Rotation *= qt;
	}

	// ���[��
	if (inZ != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[2], D3DXToRadian(inZ));
		m_Rotation *= qt;
	}
}

//------------------------------------------------------------------------------
//	���_�̐��ʂ������悤�ɉ�]
//------------------------------------------------------------------------------
void CModel11::SetRotationBillboard(const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle)
{
	m_Rotation = -inAngle;

	// ���_�̕����֌�����
	D3DXVECTOR3   dir = m_Position - inEye;
	float   angleY = ::atan2f(dir.z, dir.x);

	D3DXQUATERNION   qt;
	qt.x = 0.0f;
	qt.y = ::sinf(angleY / 2.0f);
	qt.z = 0.0f;
	qt.w = ::cosf(angleY / 2.0f);

	m_Rotation *= qt;
}

//------------------------------------------------------------------------------
//	���_�̐��ʂ������悤��Y������]������
//------------------------------------------------------------------------------
void CModel11::SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	D3DXVECTOR3   dir;

	// �����������_�x�N�g���ƈ�v������
	// �����ݒ�
	dir = -inFront;
	::D3DXVec3Normalize(z_axis, &dir);

	// �����ݒ�
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// ��]�N�H�[�^�j�I���ɕϊ�
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);

	// ���_�̕����֌�����
	dir = m_Position - inEye;
	float   angleY = ::atan2f(dir.z, dir.x);

	D3DXQUATERNION   qt;
	qt.x = 0.0f;
	qt.y = ::sinf(angleY / 2.0f);
	qt.z = 0.0f;
	qt.w = ::cosf(angleY / 2.0f);

	m_Rotation *= qt;
}

//------------------------------------------------------------------------------
//	�������w�肳�ꂽ���̌X���ɍ��킹��
//------------------------------------------------------------------------------
void CModel11::AlignmentAxisY(const D3DXVECTOR3& inAxisY)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	// �����ݒ�
	::D3DXVec3Normalize(y_axis, &inAxisY);

	// �����ݒ�
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// ������]�N�H�[�^�j�I���ɕϊ�
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);
}

//------------------------------------------------------------------------------
//	��]�p�擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetDirection() const
{
	D3DXVECTOR3   rotation = GetDirectionRadian();

	rotation.x = D3DXToDegree(rotation.x);
	rotation.y = D3DXToDegree(rotation.y);
	rotation.z = D3DXToDegree(rotation.z);

	return rotation;
}

//------------------------------------------------------------------------------
//	��]�p�擾(���W�A��)
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetDirectionRadian() const
{
	D3DXVECTOR3   rotation;

	rotation.x = ::asinf(m_Rotation.x) * 2.0f;
	rotation.y = ::asinf(m_Rotation.y) * 2.0f;
	rotation.z = ::asinf(m_Rotation.z) * 2.0f;

	return rotation;
/*
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    m;
	::D3DXMatrixRotationQuaternion(&m, &m_Rotation);

	// ��, y���擾
	D3DXVECTOR3   x_axis(m._11, m._21, m._31);
	D3DXVECTOR3   y_axis(m._12, m._22, m._32);

	// ��]�p�擾
	D3DXVECTOR3   rotation;

	// ����]�p
	rotation.z = ::atan2f(-y_axis.x, y_axis.y);

	// ����]�p
	const float  cos_z = ::cosf(rotation.z);
	rotation.x = ::atan2f(cos_z * -y_axis.z, ::fabs(y_axis.y));

	// ����]�p
	const float  sin_z = ::sinf(rotation.z);
	const float  cos_x = ::cosf(rotation.x);
	rotation.y = ::atan2(cos_x * -(sin_z * y_axis.y - cos_x * y_axis.x), y_axis.z);

	return rotation;
*/
}

//------------------------------------------------------------------------------
//	���[���h�ϊ��s�񐶐�
//------------------------------------------------------------------------------
DirectX::SimpleMath::Matrix CModel11::GetWorldMatrix() const
{
	// ���[���h�s��ݒ�
	DirectX::SimpleMath::Matrix   world;

	// ��]�s�񐶐�
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Rotation);

	// �X�P�[�����O
	world._11 = rotation._11 * m_Scale.x;
	world._12 = rotation._12 * m_Scale.x;
	world._13 = rotation._13 * m_Scale.x;

	world._21 = rotation._21 * m_Scale.y;
	world._22 = rotation._22 * m_Scale.y;
	world._23 = rotation._23 * m_Scale.y;

	world._31 = rotation._31 * m_Scale.z;
	world._32 = rotation._32 * m_Scale.z;
	world._33 = rotation._33 * m_Scale.z;

	// �ړ�
	world._41 = m_Position.x;
	world._42 = m_Position.y;
	world._43 = m_Position.z;

	world._44 = 1.0f;

	return world;
}

//------------------------------------------------------------------------------
//	�O���x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetFrontVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[2];
}

//------------------------------------------------------------------------------
//	�E���x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetRightVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[0];
}

//------------------------------------------------------------------------------
//	����x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetUpVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[1];
}
