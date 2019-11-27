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
	 Model11.cpp
				モデル(DirectX11)クラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4005)
#include "Model11.hpp"
#pragma warning(pop)

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#if !(defined(_DEBUG) || defined(DEBUG))
	#pragma comment(lib, "DirectXTK.lib")
#else
	#pragma comment(lib, "DirectXTKd.lib")
#endif

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CModel11::CModel11(std::unique_ptr<DirectX::Model> upModel)
	: m_upModel(std::move(upModel)),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f, 1.0f)
{
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CModel11::~CModel11()
{
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CModel11::Draw(ID3D11DeviceContext* pContext, DirectX::CommonStates& inStates,
					DirectX::CXMMATRIX& inView, DirectX::CXMMATRIX& inProjection)
{
	DirectX::SimpleMath::Matrix   world = GetWorldMatrix();
	m_upModel->Draw(pContext, inStates, world, inView, inProjection);
}

//------------------------------------------------------------------------------
//	角度設定
//------------------------------------------------------------------------------
void CModel11::SetDirection(const float inX, const float inY, const float inZ)
{
	::D3DXQuaternionIdentity(&m_Rotation);

	// 角軸のクォータニオンを求める
	D3DXQUATERNION   qt;

	// ロール
	if(inZ != 0.0f) {
		qt.x = 0.0f;
		qt.y = 0.0f;
		qt.z = ::sinf(D3DXToRadian(inZ) / 2.0f);
		qt.w = ::cosf(D3DXToRadian(inZ) / 2.0f);
		m_Rotation *= qt;
	}

	// ピッチ
	if(inX != 0.0f) {
		qt.x = ::sinf(D3DXToRadian(inX) / 2.0f);
		qt.y = 0.0f;
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inX) / 2.0f);
		m_Rotation *= qt;
	}

	// ヨー
	if(inY != 0.0f) {
		qt.x = 0.0f;
		qt.y = ::sinf(D3DXToRadian(inY) / 2.0f);
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inY) / 2.0f);
		m_Rotation *= qt;
	}
}

//------------------------------------------------------------------------------
//	移動
//------------------------------------------------------------------------------
void CModel11::Move(const float inRight, const float inUp, const float inFront)
{
	// 回転行列生成
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Rotation);

	// 方向ベクトルを取り出す
	D3DXVECTOR3*   right = (D3DXVECTOR3*)rotation.m[0];
	D3DXVECTOR3*   up    = (D3DXVECTOR3*)rotation.m[1];
	D3DXVECTOR3*   front = (D3DXVECTOR3*)rotation.m[2];

	// 移動
	*right     *= inRight;
	*up        *= inUp;
	*front     *= inFront;
	m_Position += *front + *up + *right;
}

//------------------------------------------------------------------------------
//	回転
//------------------------------------------------------------------------------
void CModel11::Rotation(const float inX, const float inY, const float inZ)
{
	// クォータニオンから軸を求める
	D3DXMATRIX   axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXQUATERNION   qt;

	// ヨー
	if (inY != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[1], D3DXToRadian(inY));
		m_Rotation *= qt;
	}

	// ピッチ
	if (inX != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[0], D3DXToRadian(inX));
		m_Rotation *= qt;
	}

	// ロール
	if (inZ != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[2], D3DXToRadian(inZ));
		m_Rotation *= qt;
	}
}

//------------------------------------------------------------------------------
//	視点の正面を向くように回転
//------------------------------------------------------------------------------
void CModel11::SetRotationBillboard(const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle)
{
	m_Rotation = -inAngle;

	// 視点の方向へ向ける
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
//	視点の正面を向くようにY軸を回転させる
//------------------------------------------------------------------------------
void CModel11::SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	D3DXVECTOR3   dir;

	// ｚ方向を視点ベクトルと一致させる
	// ｚ軸設定
	dir = -inFront;
	::D3DXVec3Normalize(z_axis, &dir);

	// ｘ軸設定
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// 回転クォータニオンに変換
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);

	// 視点の方向へ向ける
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
//	ｙ軸を指定された軸の傾きに合わせる
//------------------------------------------------------------------------------
void CModel11::AlignmentAxisY(const D3DXVECTOR3& inAxisY)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	// ｙ軸設定
	::D3DXVec3Normalize(y_axis, &inAxisY);

	// ｘ軸設定
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// 軸を回転クォータニオンに変換
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);
}

//------------------------------------------------------------------------------
//	回転角取得
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
//	回転角取得(ラジアン)
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetDirectionRadian() const
{
	D3DXVECTOR3   rotation;

	rotation.x = ::asinf(m_Rotation.x) * 2.0f;
	rotation.y = ::asinf(m_Rotation.y) * 2.0f;
	rotation.z = ::asinf(m_Rotation.z) * 2.0f;

	return rotation;
/*
	// クォータニオン→回転行列
	D3DXMATRIX    m;
	::D3DXMatrixRotationQuaternion(&m, &m_Rotation);

	// ｘ, y軸取得
	D3DXVECTOR3   x_axis(m._11, m._21, m._31);
	D3DXVECTOR3   y_axis(m._12, m._22, m._32);

	// 回転角取得
	D3DXVECTOR3   rotation;

	// ｚ回転角
	rotation.z = ::atan2f(-y_axis.x, y_axis.y);

	// ｘ回転角
	const float  cos_z = ::cosf(rotation.z);
	rotation.x = ::atan2f(cos_z * -y_axis.z, ::fabs(y_axis.y));

	// ｙ回転角
	const float  sin_z = ::sinf(rotation.z);
	const float  cos_x = ::cosf(rotation.x);
	rotation.y = ::atan2(cos_x * -(sin_z * y_axis.y - cos_x * y_axis.x), y_axis.z);

	return rotation;
*/
}

//------------------------------------------------------------------------------
//	ワールド変換行列生成
//------------------------------------------------------------------------------
DirectX::SimpleMath::Matrix CModel11::GetWorldMatrix() const
{
	// ワールド行列設定
	DirectX::SimpleMath::Matrix   world;

	// 回転行列生成
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Rotation);

	// スケーリング
	world._11 = rotation._11 * m_Scale.x;
	world._12 = rotation._12 * m_Scale.x;
	world._13 = rotation._13 * m_Scale.x;

	world._21 = rotation._21 * m_Scale.y;
	world._22 = rotation._22 * m_Scale.y;
	world._23 = rotation._23 * m_Scale.y;

	world._31 = rotation._31 * m_Scale.z;
	world._32 = rotation._32 * m_Scale.z;
	world._33 = rotation._33 * m_Scale.z;

	// 移動
	world._41 = m_Position.x;
	world._42 = m_Position.y;
	world._43 = m_Position.z;

	world._44 = 1.0f;

	return world;
}

//------------------------------------------------------------------------------
//	前方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetFrontVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[2];
}

//------------------------------------------------------------------------------
//	右方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetRightVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[0];
}

//------------------------------------------------------------------------------
//	上方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel11::GetUpVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[1];
}
