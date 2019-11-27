/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft Windows 7以降

 【コンパイラ】
     Microsoft VisualC++ 2013

 【プログラム】
	 Camera.cpp
				カメラクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Camera.hpp"

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CCamera::CCamera() : m_Position(0.0f, 0.0f, 0.0f),
                     m_Angle   (0.0f, 0.0f, 0.0f, 1.0f),
					 m_Update  (false)
{
	::D3DXMatrixIdentity(&m_View);
	::D3DXMatrixIdentity(&m_Projection);
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CCamera::~CCamera()
{
}

//------------------------------------------------------------------------------
//	ビュー変換行列設定
//------------------------------------------------------------------------------
void CCamera::SetView(const D3DXMATRIX& inView)
{
	m_View = inView;

	// 座標・角度保存
	D3DXMATRIX   inv_view;
	::D3DXMatrixInverse(&inv_view, NULL, &m_View);

	m_Position = *(D3DXVECTOR3*)inv_view.m[3];
	::D3DXQuaternionRotationMatrix(&m_Angle, &inv_view);

	m_Update = false;
}

//------------------------------------------------------------------------------
//	ビュー変換行列設定
//------------------------------------------------------------------------------
void CCamera::SetView(const D3DXVECTOR3& inPosition, const D3DXVECTOR3& inRotation)
{
	m_Position = inPosition;

	::D3DXQuaternionRotationYawPitchRoll(&m_Angle, D3DXToRadian(-inRotation.y),
												   D3DXToRadian( inRotation.x),
												   D3DXToRadian(-inRotation.z));

	// カメラ行列(ワールド変換行列)→ビュー行列
	::D3DXMatrixRotationQuaternion(&m_View, &m_Angle);
	*(D3DXVECTOR3*)&m_View.m[3] = m_Position;
	::D3DXMatrixInverse(&m_View, NULL, &m_View);

	m_Update = false;
}

//------------------------------------------------------------------------------
//	ビュー変換行列設定
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
//	ワールド変換行列→ビュー変換行列
//------------------------------------------------------------------------------
void CCamera::SetViewFromWorldMatrix(const D3DXMATRIX& inWorldMatrix)
{
	D3DXMATRIX   world = inWorldMatrix;

	// 軸ベクトル正規化
	::D3DXVec3Normalize((D3DXVECTOR3*)&world.m[0], (D3DXVECTOR3*)&world.m[0]);
	::D3DXVec3Normalize((D3DXVECTOR3*)&world.m[1], (D3DXVECTOR3*)&world.m[1]);
	::D3DXVec3Normalize((D3DXVECTOR3*)&world.m[2], (D3DXVECTOR3*)&world.m[2]);

	// 座標設定
	m_Position = *(D3DXVECTOR3*)&world.m[3];

	// 回転クォータニオン設定
	::D3DXQuaternionRotationMatrix(&m_Angle, &world);

	// ワールド変換行列→ビュー変換行列
	if(::D3DXMatrixInverse(&m_View, NULL, &world) == NULL)
		::D3DXMatrixIdentity(&m_View);

	m_Update = false;
}

//------------------------------------------------------------------------------
//	公転
//------------------------------------------------------------------------------
void CCamera::SetRevolution(const D3DXVECTOR3& inAt, const D3DXVECTOR3& inRotation)
{
	// ｚ軸設定
	D3DXVECTOR3   z_axis(inAt - m_Position);
	::D3DXVec3Normalize(&z_axis, &z_axis);

	// ｘ軸設定
	D3DXVECTOR3   x_axis;
	::D3DXVec3Cross(&x_axis, &D3DXVECTOR3(m_View._12, m_View._22, m_View._32), &z_axis);
	::D3DXVec3Normalize(&x_axis, &x_axis);

	// ｙ軸設定
	D3DXVECTOR3   y_axis;
	::D3DXVec3Cross(&y_axis, &z_axis, &x_axis);

	// 回転クォータニオン設定
	D3DXQUATERNION   qt_rot(0.0f, 0.0f, 0.0f, 1.0f), qt;
	if(inRotation.z != 0.0f) {
		// ロール
		::D3DXQuaternionRotationAxis(&qt, &z_axis, D3DXToRadian(-inRotation.z));
		qt_rot *= qt;
	}
	if(inRotation.y != 0.0f) {
		// ヨー
		::D3DXQuaternionRotationAxis(&qt, &y_axis, D3DXToRadian( inRotation.y));
		qt_rot *= qt;
	}
	if(inRotation.x != 0.0f) {
		// ピッチ
		::D3DXQuaternionRotationAxis(&qt, &x_axis, D3DXToRadian( inRotation.x));
		qt_rot *= qt;
	}

	// クォータニオン→回転行列
	D3DXMATRIX   rot;
	::D3DXMatrixRotationQuaternion(&rot, &qt_rot);

	// 視点回転(ワールド座標の中心を注視点にした視点座標)
	D3DXVECTOR3   eye = m_Position - inAt;
	::D3DXVec3TransformCoord(&eye, &eye, &rot);
	eye += inAt;

	// 上方ベクトル回転
	D3DXVECTOR3   up;
	::D3DXVec3TransformCoord(&up, &y_axis, &rot);

	// ビュー行列生成
	SetLookAt(eye, inAt, up);
}

//------------------------------------------------------------------------------
//	座標設定
//------------------------------------------------------------------------------
void CCamera::SetPosition(const float inX, const float inY, const float inZ)
{
	m_Position.x = inX;
	m_Position.y = inY;
	m_Position.z = inZ;

	m_Update = true;
}

//------------------------------------------------------------------------------
//	角度設定
//------------------------------------------------------------------------------
void CCamera::SetAngle(const float inX, const float inY, const float inZ)
{
	::D3DXQuaternionRotationYawPitchRoll(&m_Angle, D3DXToRadian(-inY),
												   D3DXToRadian( inX),
												   D3DXToRadian(-inZ));

	m_Update = true;
}

//------------------------------------------------------------------------------
//	角度設定
//------------------------------------------------------------------------------
void CCamera::SetAngle(const D3DXMATRIX& inAngle)
{
	::D3DXQuaternionRotationMatrix(&m_Angle, &inAngle);

	m_Update = true;
}

//------------------------------------------------------------------------------
//	移動
//------------------------------------------------------------------------------
void CCamera::Move(const float inRight, const float inUp, const float inFront)
{
	// カメラの軸ベクトルを取り出す
	D3DXVECTOR3   right(m_View._11, m_View._21, m_View._31);
	D3DXVECTOR3   up   (m_View._12, m_View._22, m_View._32);
	D3DXVECTOR3   front(m_View._13, m_View._23, m_View._33);

	// 移動
	right      *= inRight;
	up         *= inUp;
	front      *= inFront;
	m_Position += front + up + right;

	m_Update = true;
}

//------------------------------------------------------------------------------
//	回転
//------------------------------------------------------------------------------
void CCamera::Rotation(const float inX, const float inY, const float inZ)
{
	// クォータニオンから軸を求める
	D3DXMATRIX   axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Angle);

	D3DXQUATERNION   qt;

	// ヨー
	if(inY != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[1], D3DXToRadian( inY));
		m_Angle *= qt;
	}

	// ピッチ
	if(inX != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[0], D3DXToRadian(-inX));
		m_Angle *= qt;
	}

	// ロール
	if(inZ != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[2], D3DXToRadian(-inZ));
		m_Angle *= qt;
	}

	m_Update = true;
}

//------------------------------------------------------------------------------
//	射影行列設定
//------------------------------------------------------------------------------
void CCamera::SetProjection(const D3DXMATRIX& inProjection)
{
	m_Projection = inProjection;
}

//------------------------------------------------------------------------------
//	遠近射影行列設定
//------------------------------------------------------------------------------
void CCamera::SetPerspectiveFieldOfView(const float inFovY, const float inAspect,
										const float inZn,   const float inZf)
{
	::D3DXMatrixPerspectiveFovLH(&m_Projection, D3DXToRadian(inFovY), inAspect, inZn, inZf);
}

//------------------------------------------------------------------------------
//	正射影行列設定
//------------------------------------------------------------------------------
void CCamera::SetOrthographic(const float inWidth, const float inHeight, const float inZn, const float inZf)
{
	::D3DXMatrixOrthoLH(&m_Projection, inWidth, inHeight, inZn, inZf);
}

//------------------------------------------------------------------------------
//	正射影行列設定
//------------------------------------------------------------------------------
void CCamera::SetOrthographicOffCenter(const float inLeft,       const float inRight,
									   const float inBottom,     const float inTop,
									   const float inZNearPlane, const float inZFarPlane)
{
	::D3DXMatrixOrthoOffCenterLH(&m_Projection, inLeft, inRight, inBottom, inTop, inZNearPlane, inZFarPlane);
}

//------------------------------------------------------------------------------
//	Field of View-Y設定
//------------------------------------------------------------------------------
void CCamera::SetFieldOfViewY(const float inFovY)
{
	m_Projection._11 = m_Projection._22 / m_Projection._11;
	m_Projection._22 = 1.0f / ::tanf(D3DXToRadian(inFovY) / 2.0f);
	m_Projection._11 = m_Projection._22 / m_Projection._11;
}

//------------------------------------------------------------------------------
//	デバイスカメラ設定
//------------------------------------------------------------------------------
void CCamera::SetDevice(IDirect3DDevice9* pD3DDevice)
{
#ifdef _DEBUG
	if(pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CCamera_SetDevice)\n"));
		return;
	}
#endif

	// ビュー行列設定
	if(m_Update) {
		// 回転クォータニオン正規化(誤差補正)
		::D3DXQuaternionNormalize(&m_Angle, &m_Angle);

		// カメラ行列(ワールド変換行列)→ビュー行列
		::D3DXMatrixRotationQuaternion(&m_View, &m_Angle);
		*(D3DXVECTOR3*)m_View.m[3] = m_Position;
		::D3DXMatrixInverse(&m_View, NULL, &m_View);

		m_Update = false;
	}
	pD3DDevice->SetTransform(D3DTS_VIEW,       &m_View);

	// プロジェクション行列設定
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_Projection);
}

//------------------------------------------------------------------------------
//	角度取得
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
//	角度取得(ラジアン)
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetAngleRadian()
{
	D3DXVECTOR3   rotation;

	rotation.x = ::asinf(m_Angle.x) * 2.0f;
	rotation.y = ::asinf(m_Angle.y) * 2.0f;
	rotation.z = ::asinf(m_Angle.z) * 2.0f;

	return rotation;
/*
	// クォータニオン→回転行列
	D3DXMATRIX    m;
	::D3DXMatrixRotationQuaternion(&m, &m_Angle);

	// y, z軸取得
	D3DXVECTOR3   y_axis(m._12, m._22, m._32);
	D3DXVECTOR3   z_axis(m._13, m._23, m._33);

	// 回転角取得
	D3DXVECTOR3   rotation;

	// ｙ回転角
	rotation.y = ::atan2f(z_axis.x, z_axis.z);

	// ｘ回転角
	const float  cos_y = ::cosf(rotation.y);
	rotation.x = ::atan2f(cos_y * z_axis.y, ::fabs(z_axis.z));

	// ｚ回転角
	const float  sin_y = ::sinf(rotation.y);
	const float  cos_x = ::cosf(rotation.x);
	rotation.z = ::atan2(cos_x * (sin_y * y_axis.z - cos_y * y_axis.x), y_axis.y);

	return rotation;
*/
}

//------------------------------------------------------------------------------
//	回転行列取得
//------------------------------------------------------------------------------
D3DXMATRIX CCamera::GetAngleMatrix()
{
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Angle);

	return rotation;
}

//------------------------------------------------------------------------------
//	ワールド変換行列取得
//------------------------------------------------------------------------------
D3DXMATRIX CCamera::GetWorldMatrix() const
{
	D3DXMATRIX   world;

	::D3DXMatrixRotationQuaternion(&world, &m_Angle);
	*(D3DXVECTOR3*)&world.m[3] = m_Position;

	return world;
}

//------------------------------------------------------------------------------
//	前方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetFrontVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);

	return *(D3DXVECTOR3*)rot.m[2];
}

//------------------------------------------------------------------------------
//	右方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetRightVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);

	return *(D3DXVECTOR3*)rot.m[0];
}

//------------------------------------------------------------------------------
//	上方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CCamera::GetUpVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);

	return *(D3DXVECTOR3*)rot.m[1];
}

//------------------------------------------------------------------------------
//	視錐台取得
//------------------------------------------------------------------------------
DXGVIEWFRUSTUM CCamera::GetViewFrustum() const
{
	// 視錐台生成
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

	// 視錐台平面生成
	DXGVIEWFRUSTUM   Frustum;
	::D3DXPlaneFromPoints(&Frustum.Left  , & far_bl, & far_tl, &near_tl);
	::D3DXPlaneFromPoints(&Frustum.Right , &near_tr, & far_tr, & far_br);
	::D3DXPlaneFromPoints(&Frustum.Top   , &near_tr, &near_tl, & far_tl);
	::D3DXPlaneFromPoints(&Frustum.Bottom, & far_bl, &near_bl, &near_br);
	::D3DXPlaneFromPoints(&Frustum.Near  , &near_bl, &near_tl, &near_tr);
	::D3DXPlaneFromPoints(&Frustum.Far   , & far_tr, & far_tl, & far_bl);

	// 正規化
	::D3DXPlaneNormalize(&Frustum.Left  , &Frustum.Left  );
	::D3DXPlaneNormalize(&Frustum.Right , &Frustum.Right );
	::D3DXPlaneNormalize(&Frustum.Top   , &Frustum.Top   );
	::D3DXPlaneNormalize(&Frustum.Bottom, &Frustum.Bottom);
	::D3DXPlaneNormalize(&Frustum.Near  , &Frustum.Near  );
	::D3DXPlaneNormalize(&Frustum.Far   , &Frustum.Far   );

	return Frustum;
}

//------------------------------------------------------------------------------
//	境界球取得
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CCamera::GetBoundingSphere(const float inRadius) const
{
   return DXGBOUNDINGSPHERE(m_Position, inRadius);
}

//------------------------------------------------------------------------------
//	OBB取得
//------------------------------------------------------------------------------
DXGOBB CCamera::GetOBB(const float inRadius) const
{
	DXGOBB   obb;

	// 中心座標設定
	obb.Center = m_Position;

	// 各軸の傾き設定
	D3DXMATRIX   rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Angle);
	obb.Axis[0] = D3DXVECTOR3(rot._11, rot._12, rot._13);
	obb.Axis[1] = D3DXVECTOR3(rot._21, rot._22, rot._23);
	obb.Axis[2] = D3DXVECTOR3(rot._31, rot._32, rot._33);

	// 各軸の長さ設定
	obb.Radius.x = inRadius;
	obb.Radius.y = inRadius;
	obb.Radius.z = inRadius;

	return obb;
}
