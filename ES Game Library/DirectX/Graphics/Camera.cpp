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
	 Camera.cpp
				�J�����N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Camera.hpp"

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CCamera::CCamera() : m_Position(0.0f, 0.0f, 0.0f),
                     m_Angle   (0.0f, 0.0f, 0.0f, 1.0f),
					 m_Update  (false)
{
	::D3DXMatrixIdentity(&m_View);
	::D3DXMatrixIdentity(&m_Projection);
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CCamera::~CCamera()
{
}

//------------------------------------------------------------------------------
//	�r���[�ϊ��s��ݒ�
//------------------------------------------------------------------------------
void CCamera::SetView(const D3DXMATRIX& inView)
{
	m_View = inView;

	// ���W�E�p�x�ۑ�
	D3DXMATRIX   inv_view;
	::D3DXMatrixInverse(&inv_view, NULL, &m_View);

	m_Position = *(D3DXVECTOR3*)inv_view.m[3];
	::D3DXQuaternionRotationMatrix(&m_Angle, &inv_view);

	m_Update = false;
}

//------------------------------------------------------------------------------
//	�r���[�ϊ��s��ݒ�
//------------------------------------------------------------------------------
void CCamera::SetView(const D3DXVECTOR3& inPosition, const D3DXVECTOR3& inRotation)
{
	m_Position = inPosition;

	::D3DXQuaternionRotationYawPitchRoll(&m_Angle, D3DXToRadian(-inRotation.y),
												   D3DXToRadian( inRotation.x),
												   D3DXToRadian(-inRotation.z));

	// �J�����s��(���[���h�ϊ��s��)���r���[�s��
	::D3DXMatrixRotationQuaternion(&m_View, &m_Angle);
	*(D3DXVECTOR3*)&m_View.m[3] = m_Position;
	::D3DXMatrixInverse(&m_View, NULL, &m_View);

	m_Update = false;
}

//------------------------------------------------------------------------------
//	�r���[�ϊ��s��ݒ�
//------------------------------------------------------------------------------
void CCamera::SetLookAt(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inAt, const D3DXVECTOR3& inUp)
{
	::D3DXMatrixLookAtLH(&m_View, &inEye, &inAt, &inUp);

	m_Position = inEye;

	D3DXMATRIX   inv_view;
	::D3DXMatrixInverse(&inv_view, NULL, &m_View);
	::D3DXQuaternionRotationMatrix(&m_Angle, &inv_view);

	m_Update = false;
}

//------------------------------------------------------------------------------
//	���[���h�ϊ��s�񁨃r���[�ϊ��s��
//------------------------------------------------------------------------------
void CCamera::SetViewFromWorldMatrix(const D3DXMATRIX& inWorldMatrix)
{
	D3DXMATRIX   world = inWorldMatrix;

	// ���x�N�g�����K��
	::D3DXVec3Normalize((D3DXVECTOR3*)&world.m[0], (D3DXVECTOR3*)&world.m[0]);
	::D3DXVec3Normalize((D3DXVECTOR3*)&world.m[1], (D3DXVECTOR3*)&world.m[1]);
	::D3DXVec3Normalize((D3DXVECTOR3*)&world.m[2], (D3DXVECTOR3*)&world.m[2]);

	// ���W�ݒ�
	m_Position = *(D3DXVECTOR3*)&world.m[3];

	// ��]�N�H�[�^�j�I���ݒ�
	::D3DXQuaternionRotationMatrix(&m_Angle, &world);

	// ���[���h�ϊ��s�񁨃r���[�ϊ��s��
	if(::D3DXMatrixInverse(&m_View, NULL, &world) == NULL)
		::D3DXMatrixIdentity(&m_View);

	m_Update = false;
}

//------------------------------------------------------------------------------
//	���]
//------------------------------------------------------------------------------
void CCamera::SetRevolution(const D3DXVECTOR3& inAt, const D3DXVECTOR3& inRotation)
{
	// �����ݒ�
	D3DXVECTOR3   z_axis(inAt - m_Position);
	::D3DXVec3Normalize(&z_axis, &z_axis);

	// �����ݒ�
	D3DXVECTOR3   x_axis;
	::D3DXVec3Cross(&x_axis, &D3DXVECTOR3(m_View._12, m_View._22, m_View._32), &z_axis);
	::D3DXVec3Normalize(&x_axis, &x_axis);

	// �����ݒ�
	D3DXVECTOR3   y_axis;
	::D3DXVec3Cross(&y_axis, &z_axis, &x_axis);

	// ��]�N�H�[�^�j�I���ݒ�
	D3DXQUATERNION   qt_rot(0.0f, 0.0f, 0.0f, 1.0f), qt;
	if(inRotation.z != 0.0f) {
		// ���[��
		::D3DXQuaternionRotationAxis(&qt, &z_axis, D3DXToRadian(-inRotation.z));
		qt_rot *= qt;
	}
	if(inRotation.y != 0.0f) {
		// ���[
		::D3DXQuaternionRotationAxis(&qt, &y_axis, D3DXToRadian( inRotation.y));
		qt_rot *= qt;
	}
	if(inRotation.x != 0.0f) {
		// �s�b�`
		::D3DXQuaternionRotationAxis(&qt, &x_axis, D3DXToRadian( inRotation.x));
		qt_rot *= qt;
	}

	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX   rot;
	::D3DXMatrixRotationQuaternion(&rot, &qt_rot);

	// ���_��](���[���h���W�̒��S�𒍎��_�ɂ������_���W)
	D3DXVECTOR3   eye = m_Position - inAt;
	::D3DXVec3TransformCoord(&eye, &eye, &rot);
	eye += inAt;

	// ����x�N�g����]
	D3DXVECTOR3   up;
	::D3DXVec3TransformCoord(&up, &y_axis, &rot);

	// �r���[�s�񐶐�
	SetLookAt(eye, inAt, up);
}

//------------------------------------------------------------------------------
//	���W�ݒ�
//------------------------------------------------------------------------------
void CCamera::SetPosition(const float inX, const float inY, const float inZ)
{
	m_Position.x = inX;
	m_Position.y = inY;
	m_Position.z = inZ;

	m_Update = true;
}

//------------------------------------------------------------------------------
//	�p�x�ݒ�
//------------------------------------------------------------------------------
void CCamera::SetAngle(const float inX, const float inY, const float inZ)
{
	::D3DXQuaternionRotationYawPitchRoll(&m_Angle, D3DXToRadian(-inY),
												   D3DXToRadian( inX),
												   D3DXToRadian(-inZ));

	m_Update = true;
}

//------------------------------------------------------------------------------
//	�p�x�ݒ�
//------------------------------------------------------------------------------
void CCamera::SetAngle(const D3DXMATRIX& inAngle)
{
	::D3DXQuaternionRotationMatrix(&m_Angle, &inAngle);

	m_Update = true;
}

//------------------------------------------------------------------------------
//	�ړ�
//------------------------------------------------------------------------------
void CCamera::Move(const float inRight, const float inUp, const float inFront)
{
	// �J�����̎��x�N�g�������o��
	D3DXVECTOR3   right(m_View._11, m_View._21, m_View._31);
	D3DXVECTOR3   up   (m_View._12, m_View._22, m_View._32);
	D3DXVECTOR3   front(m_View._13, m_View._23, m_View._33);

	// �ړ�
	right      *= inRight;
	up         *= inUp;
	front      *= inFront;
	m_Position += front + up + right;

	m_Update = true;
}

//------------------------------------------------------------------------------
//	��]
//------------------------------------------------------------------------------
void CCamera::Rotation(const float inX, const float inY, const float inZ)
{
	// �N�H�[�^�j�I�����玲�����߂�
	D3DXMATRIX   axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Angle);

	D3DXQUATERNION   qt;

	// ���[
	if(inY != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[1], D3DXToRadian( inY));
		m_Angle *= qt;
	}

	// �s�b�`
	if(inX != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[0], D3DXToRadian(-inX));
		m_Angle *= qt;
	}

	// ���[��
	if(inZ != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[2], D3DXToRadian(-inZ));
		m_Angle *= qt;
	}

	m_Update = true;
}

//------------------------------------------------------------------------------
//	�ˉe�s��ݒ�
//------------------------------------------------------------------------------
void CCamera::SetProjection(const D3DXMATRIX& inProjection)
{
	m_Projection = inProjection;
}

//------------------------------------------------------------------------------
//	���ߎˉe�s��ݒ�
//------------------------------------------------------------------------------
void CCamera::SetPerspectiveFieldOfView(const float inFovY, const float inAspect,
										const float inZn,   const float inZf)
{
	::D3DXMatrixPerspectiveFovLH(&m_Projection, D3DXToRadian(inFovY), inAspect, inZn, inZf);
}

//------------------------------------------------------------------------------
//	���ˉe�s��ݒ�
//------------------------------------------------------------------------------
void CCamera::SetOrthographic(const float inWidth, const float inHeight, const float inZn, const float inZf)
{
	::D3DXMatrixOrthoLH(&m_Projection, inWidth, inHeight, inZn, inZf);
}

//------------------------------------------------------------------------------
//	���ˉe�s��ݒ�
//------------------------------------------------------------------------------
void CCamera::SetOrthographicOffCenter(const float inLeft,       const float inRight,
									   const float inBottom,     const float inTop,
									   const float inZNearPlane, const float inZFarPlane)
{
	::D3DXMatrixOrthoOffCenterLH(&m_Projection, inLeft, inRight, inBottom, inTop, inZNearPlane, inZFarPlane);
}

//------------------------------------------------------------------------------
//	Field of View-Y�ݒ�
//------------------------------------------------------------------------------
void CCamera::SetFieldOfViewY(const float inFovY)
{
	m_Projection._11 = m_Projection._22 / m_Projection._11;
	m_Projection._22 = 1.0f / ::tanf(D3DXToRadian(inFovY) / 2.0f);
	m_Projection._11 = m_Projection._22 / m_Projection._11;
}

//------------------------------------------------------------------------------
//	�f�o�C�X�J�����ݒ�
//------------------------------------------------------------------------------
void CCamera::SetDevice(IDirect3DDevice9* pD3DDevice)
{
#ifdef _DEBUG
	if(pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CCamera_SetDevice)\n"));
		return;
	}
#endif

	// �r���[�s��ݒ�
	if(m_Update) {
		// ��]�N�H�[�^�j�I�����K��(�덷�␳)
		::D3DXQuaternionNormalize(&m_Angle, &m_Angle);

		// �J�����s��(���[���h�ϊ��s��)���r���[�s��
		::D3DXMatrixRotationQuaternion(&m_View, &m_Angle);
		*(D3DXVECTOR3*)m_View.m[3] = m_Position;
		::D3DXMatrixInverse(&m_View, NULL, &m_View);

		m_Update = false;
	}
	pD3DDevice->SetTransform(D3DTS_VIEW,       &m_View);

	// �v���W�F�N�V�����s��ݒ�
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_Projection);
}

//------------------------------------------------------------------------------
//	�p�x�擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetAngle()
{
	D3DXVECTOR3   rotation = GetAngleRadian();

	rotation.x = D3DXToDegree(rotation.x);
	rotation.y = D3DXToDegree(rotation.y);
	rotation.z = D3DXToDegree(rotation.z);

	return rotation;
}

//------------------------------------------------------------------------------
//	�p�x�擾(���W�A��)
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetAngleRadian()
{
	D3DXVECTOR3   rotation;

	rotation.x = ::asinf(m_Angle.x) * 2.0f;
	rotation.y = ::asinf(m_Angle.y) * 2.0f;
	rotation.z = ::asinf(m_Angle.z) * 2.0f;

	return rotation;
/*
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    m;
	::D3DXMatrixRotationQuaternion(&m, &m_Angle);

	// y, z���擾
	D3DXVECTOR3   y_axis(m._12, m._22, m._32);
	D3DXVECTOR3   z_axis(m._13, m._23, m._33);

	// ��]�p�擾
	D3DXVECTOR3   rotation;

	// ����]�p
	rotation.y = ::atan2f(z_axis.x, z_axis.z);

	// ����]�p
	const float  cos_y = ::cosf(rotation.y);
	rotation.x = ::atan2f(cos_y * z_axis.y, ::fabs(z_axis.z));

	// ����]�p
	const float  sin_y = ::sinf(rotation.y);
	const float  cos_x = ::cosf(rotation.x);
	rotation.z = ::atan2(cos_x * (sin_y * y_axis.z - cos_y * y_axis.x), y_axis.y);

	return rotation;
*/
}

//------------------------------------------------------------------------------
//	��]�s��擾
//------------------------------------------------------------------------------
D3DXMATRIX CCamera::GetAngleMatrix()
{
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Angle);

	return rotation;
}

//------------------------------------------------------------------------------
//	���[���h�ϊ��s��擾
//------------------------------------------------------------------------------
D3DXMATRIX CCamera::GetWorldMatrix() const
{
	D3DXMATRIX   world;

	::D3DXMatrixRotationQuaternion(&world, &m_Angle);
	*(D3DXVECTOR3*)&world.m[3] = m_Position;

	return world;
}

//------------------------------------------------------------------------------
//	�O���x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetFrontVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);

	return *(D3DXVECTOR3*)rot.m[2];
}

//------------------------------------------------------------------------------
//	�E���x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetRightVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);

	return *(D3DXVECTOR3*)rot.m[0];
}

//------------------------------------------------------------------------------
//	����x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetUpVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);

	return *(D3DXVECTOR3*)rot.m[1];
}

//------------------------------------------------------------------------------
//	������擾
//------------------------------------------------------------------------------
DXGVIEWFRUSTUM CCamera::GetViewFrustum() const
{
	// �����䐶��
	D3DXVECTOR3   near_tl(-1.0f,  1.0f, 0.0f);
	D3DXVECTOR3   near_tr( 1.0f,  1.0f, 0.0f);
	D3DXVECTOR3   near_bl(-1.0f, -1.0f, 0.0f);
	D3DXVECTOR3   near_br( 1.0f, -1.0f, 0.0f);
	D3DXVECTOR3    far_tl(-1.0f,  1.0f, 1.0f);
	D3DXVECTOR3    far_tr( 1.0f,  1.0f, 1.0f);
	D3DXVECTOR3    far_bl(-1.0f, -1.0f, 1.0f);
	D3DXVECTOR3    far_br( 1.0f, -1.0f, 1.0f);

	D3DXMATRIX   InvViewProj;
	::D3DXMatrixInverse(&InvViewProj, NULL, &GetViewProjectionMatrix());
	::D3DXVec3TransformCoord(&near_tl, &near_tl, &InvViewProj);
	::D3DXVec3TransformCoord(&near_tr, &near_tr, &InvViewProj);
	::D3DXVec3TransformCoord(&near_bl, &near_bl, &InvViewProj);
	::D3DXVec3TransformCoord(&near_br, &near_br, &InvViewProj);
	::D3DXVec3TransformCoord(& far_tl, & far_tl, &InvViewProj);
	::D3DXVec3TransformCoord(& far_tr, & far_tr, &InvViewProj);
	::D3DXVec3TransformCoord(& far_bl, & far_bl, &InvViewProj);
	::D3DXVec3TransformCoord(& far_br, & far_br, &InvViewProj);

	// �����䕽�ʐ���
	DXGVIEWFRUSTUM   Frustum;
	::D3DXPlaneFromPoints(&Frustum.Left  , & far_bl, & far_tl, &near_tl);
	::D3DXPlaneFromPoints(&Frustum.Right , &near_tr, & far_tr, & far_br);
	::D3DXPlaneFromPoints(&Frustum.Top   , &near_tr, &near_tl, & far_tl);
	::D3DXPlaneFromPoints(&Frustum.Bottom, & far_bl, &near_bl, &near_br);
	::D3DXPlaneFromPoints(&Frustum.Near  , &near_bl, &near_tl, &near_tr);
	::D3DXPlaneFromPoints(&Frustum.Far   , & far_tr, & far_tl, & far_bl);

	// ���K��
	::D3DXPlaneNormalize(&Frustum.Left  , &Frustum.Left  );
	::D3DXPlaneNormalize(&Frustum.Right , &Frustum.Right );
	::D3DXPlaneNormalize(&Frustum.Top   , &Frustum.Top   );
	::D3DXPlaneNormalize(&Frustum.Bottom, &Frustum.Bottom);
	::D3DXPlaneNormalize(&Frustum.Near  , &Frustum.Near  );
	::D3DXPlaneNormalize(&Frustum.Far   , &Frustum.Far   );

	return Frustum;
}

//------------------------------------------------------------------------------
//	���E���擾
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CCamera::GetBoundingSphere(const float inRadius) const
{
   return DXGBOUNDINGSPHERE(m_Position, inRadius);
}

//------------------------------------------------------------------------------
//	OBB�擾
//------------------------------------------------------------------------------
DXGOBB CCamera::GetOBB(const float inRadius) const
{
	DXGOBB   obb;

	// ���S���W�ݒ�
	obb.Center = m_Position;

	// �e���̌X���ݒ�
	D3DXMATRIX   rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);
	obb.Axis[0] = D3DXVECTOR3(rot._11, rot._12, rot._13);
	obb.Axis[1] = D3DXVECTOR3(rot._21, rot._22, rot._23);
	obb.Axis[2] = D3DXVECTOR3(rot._31, rot._32, rot._33);

	// �e���̒����ݒ�
	obb.Radius.x = inRadius;
	obb.Radius.y = inRadius;
	obb.Radius.z = inRadius;

	return obb;
}
