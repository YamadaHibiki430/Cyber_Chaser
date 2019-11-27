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
	 ESGLib.h
				ES Game Libraryヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <SS5Player.h>
// #include "DirectX\2D\Direct2D.hpp"
// #include "Extension\Effekseer\Effekseer.hpp"
// #include "Extension\Live2D\Live2D.hpp"
// #include "Extension\Squirrel\Squirrel.hpp"
// #include "Extension\Kinect\NUI.hpp"
// #include "Extension\OpenGL\OpenGL.hpp"
// #include <amp.h>

#include "DirectX\Graphics\DXGraphics9.hpp"
#include "DirectX\Input\DInput8.hpp"
#include "DirectX\Audio\DXAudio8.hpp"
#include "DirectX\Show\DShow.hpp"

#include "Framework\FixTimer.hpp"
#include "Framework\Random.hpp"
#include "Framework\Stream.hpp"
#include "Framework\Thread.hpp"
#include "Framework\Network.hpp"

#include "GameScene\SceneShared.hpp"

#include <math.h>
#include <tchar.h>

//------------------------------------------------------------------------------
//	foreachマクロ
//------------------------------------------------------------------------------
// #define foreach for each

//------------------------------------------------------------------------------
//	文字列型
//------------------------------------------------------------------------------
typedef std::basic_string<TCHAR> tstring;
typedef std::basic_string<char>  cstring;
typedef std::basic_string<WCHAR> wstring;

//------------------------------------------------------------------------------
//	乱数クラス
//------------------------------------------------------------------------------
typedef CRandom        Random;

//------------------------------------------------------------------------------
//	エフェクトパラメータ
//------------------------------------------------------------------------------
typedef D3DXHANDLE     EffectParameter;

//------------------------------------------------------------------------------
//	ベクトル構造体
//------------------------------------------------------------------------------
typedef D3DXVECTOR2    Vector2;
typedef D3DXVECTOR3    Vector3;
typedef D3DXVECTOR4    Vector4;

//------------------------------------------------------------------------------
//	行列構造体
//------------------------------------------------------------------------------
typedef D3DXMATRIXA16  Matrix;

//------------------------------------------------------------------------------
//	クォータニオン
//------------------------------------------------------------------------------
typedef D3DXQUATERNION Quaternion;

//------------------------------------------------------------------------------
//	平面構造体
//------------------------------------------------------------------------------
typedef D3DXPLANE      Plane;

//------------------------------------------------------------------------------
//	ビューポート構造体
//------------------------------------------------------------------------------
typedef D3DVIEWPORT9   Viewport;

//------------------------------------------------------------------------------
//	マテリアル構造体
//------------------------------------------------------------------------------
struct Material : public D3DMATERIAL9 {
	Material()
	{
		Diffuse  = D3DXCOLOR(DWORD(0));
		Ambient  = D3DXCOLOR(DWORD(0));
		Specular = D3DXCOLOR(DWORD(0));
		Emissive = D3DXCOLOR(DWORD(0));
		Power    = 0.0f;
	}

	Material(D3DMATERIAL9& mtrl)
	{
		Diffuse  = mtrl.Diffuse;
		Ambient  = mtrl.Ambient;
		Specular = mtrl.Specular;
		Emissive = mtrl.Emissive;
		Power    = mtrl.Power;
	}

	Material(IModel*& model)
	{
		*this = model->GetMaterial();
	}

	inline Material& operator =(const D3DMATERIAL9& mtrl)
	{
		Diffuse  = mtrl.Diffuse;
		Ambient  = mtrl.Ambient;
		Specular = mtrl.Specular;
		Emissive = mtrl.Emissive;
		Power    = mtrl.Power;
		return *this;
	}
};

//------------------------------------------------------------------------------
//	ライト
//------------------------------------------------------------------------------
typedef D3DLIGHT9         Light;

#define Light_Point       D3DLIGHT_POINT
#define Light_Spot        D3DLIGHT_SPOT
#define Light_Directional D3DLIGHT_DIRECTIONAL

//------------------------------------------------------------------------------
//	ベクトル定数
//------------------------------------------------------------------------------
#define Vector3_Zero      Vector3( 0.0f,  0.0f,  0.0f)
#define Vector3_One       Vector3( 1.0f,  1.0f,  1.0f)
#define Vector3_UnitX     Vector3( 1.0f,  0.0f,  0.0f)
#define Vector3_UnitY     Vector3( 0.0f,  1.0f,  0.0f)
#define Vector3_UnitZ     Vector3( 0.0f,  0.0f,  1.0f)
#define Vector3_Right     Vector3( 1.0f,  0.0f,  0.0f)
#define Vector3_Left      Vector3(-1.0f,  0.0f,  0.0f)
#define Vector3_Up        Vector3( 0.0f,  1.0f,  0.0f)
#define Vector3_Down      Vector3( 0.0f, -1.0f,  0.0f)
#define Vector3_Forward   Vector3( 0.0f,  0.0f,  1.0f)
#define Vector3_Backward  Vector3( 0.0f,  0.0f, -1.0f)

#define Vector2_Zero      Vector2(0.0f, 0.0f)
#define Vector2_One       Vector2(1.0f, 1.0f)
#define Vector2_UnitX     Vector2(1.0f, 0.0f)
#define Vector2_UnitY     Vector2(0.0f, 1.0f)

#define Vector4_Zero      Vector4(0.0f, 0.0f, 0.0f, 0.0f)
#define Vector4_One       Vector4(1.0f, 1.0f, 1.0f, 1.0f)
#define Vector4_UnitX     Vector4(1.0f, 0.0f, 0.0f, 0.0f)
#define Vector4_UnitY     Vector4(0.0f, 1.0f, 0.0f, 0.0f)
#define Vector4_UnitZ     Vector4(0.0f, 0.0f, 1.0f, 0.0f)
#define Vector4_UnitW     Vector4(0.0f, 0.0f, 0.0f, 1.0f)

//------------------------------------------------------------------------------
//	ベクトル関数
//------------------------------------------------------------------------------
#pragma region Vector3Func

// 加算
inline Vector3 Vector3_Add(const Vector3& v1, const Vector3& v2)
{
	Vector3   v;
	::D3DXVec3Add(&v, &v1, &v2);
	return v;
}

// 重心座標取得
inline Vector3 Vector3_Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3,
								   const float b2, const float b3)
{
	Vector3   v;
	::D3DXVec3BaryCentric(&v, &v1, &v2, &v3, b2, b3);
	return v;
}

// Catmull-Rom補間
inline Vector3 Vector3_CatmullRom(const Vector3& v1, const Vector3& v2,
								  const Vector3& v3, const Vector3& v4, const float s)
{
	Vector3   v;
	::D3DXVec3CatmullRom(&v, &v1, &v2, &v3, &v4, s);
	return v;
}

// 外積
inline Vector3 Vector3_Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3   v;
	::D3DXVec3Cross(&v, &v1, &v2);
	return v;
}

// ２点間の距離
inline float Vector3_Distance(const Vector3& v1, const Vector3& v2)
{
	Vector3   v = v2 - v1;
	return ::D3DXVec3Length(&v);
}

// ２点間の直交ベクトル間の距離
inline float Vector3_DistanceSquared(const Vector3& v1, const Vector3& v2)
{
	Vector3   v = v2 - v1;
	return ::D3DXVec3LengthSq(&v);
}

// スカラー除算
inline Vector3 Vector3_Divide(const Vector3& v1, const float d)
{
	Vector3   v;
	::D3DXVec3Scale(&v, &v1, 1.0f / d);
	return v;
}

// 内積
inline float Vector3_Dot(const Vector3& v1, const Vector3& v2)
{
	return ::D3DXVec3Dot(&v1, &v2);
}

// エルミートスプライン補間
inline Vector3 Vector3_Hermite(const Vector3& v1, const Vector3& t1,
							   const Vector3& v2, const Vector3& t2, const float s)
{
	Vector3   v;
	::D3DXVec3Hermite(&v, &v1, &t1, &v2, &t2, s);
	return v;
}

// 長さ
inline float Vector3_Length(const Vector3& v1)
{
	return ::D3DXVec3Length(&v1);
}

// 長さの２乗
inline float Vector3_LengthSquared(const Vector3& v1)
{
	return ::D3DXVec3LengthSq(&v1);
}

// 線形補間
inline Vector3 Vector3_Hermite(const Vector3& v1, const Vector3& v2, const float s)
{
	Vector3   v;
	::D3DXVec3Lerp(&v, &v1, &v2, s);
	return v;
}

// ベジエ曲線
inline Vector3 Vector3_Bezier(const Vector3& start,
							  const Vector3& control1, const Vector3& control2,
							  const Vector3& end,
							  const float t)
{
	const float   A1 = 1.0f - t;
	const float   A2 = A1 * A1;
	const float   A3 = A2 * A1;

	const float   B1 = t;
	const float   B2 = B1 * B1;
	const float   B3 = B2 * B1;

	Vector3   v;

	v.x = A3 * start.x + 3 * A2 * B1 * control1.x + 3 * A1 * B2 * control2.x + B3 * end.x;
	v.y = A3 * start.y + 3 * A2 * B1 * control1.y + 3 * A1 * B2 * control2.y + B3 * end.y;
	v.z = A3 * start.z + 3 * A2 * B1 * control1.z + 3 * A1 * B2 * control2.z + B3 * end.z;

	return v;
}

// 最大
inline Vector3 Vector3_Max(const Vector3& v1, const Vector3& v2)
{
	Vector3   v;
	::D3DXVec3Maximize(&v, &v1, &v2);
	return v;
}

// 最小
inline Vector3 Vector3_Min(const Vector3& v1, const Vector3& v2)
{
	Vector3   v;
	::D3DXVec3Minimize(&v, &v1, &v2);
	return v;
}

// スカラー乗算
inline Vector3 Vector3_Multiply(const Vector3& v1, const float s)
{
	Vector3   v;
	::D3DXVec3Scale(&v, &v1, s);
	return v;
}

// 成分乗算
inline Vector3 Vector3_Multiply(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

// 逆方向ベクトル
inline Vector3 Vector3_Negate(const Vector3& v1)
{
	return Vector3(v1.x * -1, v1.y * -1, v1.z * -1);
}

// 正規化
inline Vector3 Vector3_Normalize(const Vector3& v1)
{
	Vector3   v;
	::D3DXVec3Normalize(&v, &v1);
	return v;
}

// 減算
inline Vector3 Vector3_Subtract(const Vector3& v1, const Vector3& v2)
{
	Vector3   v;
	::D3DXVec3Subtract(&v, &v1, &v2);
	return v;
}

// トランスフォーム
inline Vector3 Vector3_Transform(const Vector3& v1, const Matrix& mat)
{
	Vector3   v;
	::D3DXVec3TransformCoord(&v, &v1, &mat);
	return v;
}

// 配列トランスフォーム
inline void Vector3_Transform(Vector3* srcvec, const int srcidx, const Matrix& trans,
							  Vector3* dstvec, const int dstidx, const int len)
{
	::D3DXVec3TransformCoordArray(dstvec + dstidx, sizeof(Vector3), srcvec + srcidx, sizeof(Vector3),
								  &trans, len);
}

// 法線トランスフォーム
inline Vector3 Vector3_TransformNormal(const Vector3& v1, const Matrix& mat)
{
	Vector3   v;
	::D3DXVec3TransformNormal(&v, &v1, &mat);
	return v;
}

// 配列法線トランスフォーム
inline void Vector3_TransformNormal(Vector3* srcvec, const int srcidx, const Matrix& trans,
									Vector3* dstvec, const int dstidx, const int len)
{
	::D3DXVec3TransformNormalArray(dstvec + dstidx, sizeof(Vector3), srcvec + srcidx, sizeof(Vector3),
								   &trans, len);
}

// なす角
inline float Vector3_Angle(const Vector3& v1, const Vector3& v2)
{
	D3DXVECTOR3   vec1, vec2;
	::D3DXVec3Normalize(&vec1, &v1);
	::D3DXVec3Normalize(&vec2, &v2);
	return ::acosf(::D3DXVec3Dot(&vec1, &vec2));
}

// 移動後座標取得
inline Vector3 Vector3_MovePosition(const Vector3& pos, const Matrix& rot, const Vector3& move)
{
	Vector3   p(pos), v;

	v.x  = rot.m[0][0];
	v.y  = rot.m[0][1];
	v.z  = rot.m[0][2];
	p   += v * move.x;

	v.x  = rot.m[1][0];
	v.y  = rot.m[1][1];
	v.z  = rot.m[1][2];
	p   += v * move.y;

	v.x  = rot.m[2][0];
	v.y  = rot.m[2][1];
	v.z  = rot.m[2][2];
	p   += v * move.z;

	return v;
}

// 移動後座標取得
inline Vector3 Vector3_MovePosition(const Vector3& pos, const Quaternion& rot, const Vector3& move)
{
	Matrix   r;
	::D3DXMatrixRotationQuaternion(&r, &rot);
	return Vector3_MovePosition(pos, r, move);
}

#pragma endregion

//------------------------------------------------------------------------------
//	クォータニオン定数
//------------------------------------------------------------------------------
#define Quaternion_Identity Quaternion(0.0f, 0.0f, 0.0f, 1.0f)

//------------------------------------------------------------------------------
//	クォータニオン関数
//------------------------------------------------------------------------------
#pragma region QuaternionFunc

// 加算
inline Quaternion Quaternion_Add(const Quaternion& q1, const Quaternion& q2)
{
	return q1 + q2;
}

// 連結
inline Quaternion Quaternion_Concatenate(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion   q;
	::D3DXQuaternionMultiply(&q, &q1, &q2);
	return q;
}

// 共役
inline Quaternion Quaternion_Concatenate(const Quaternion& q1)
{
	Quaternion   q;
	::D3DXQuaternionConjugate(&q, &q1);
	return q;
}

// 生成
inline Quaternion Quaternion_CreateFromAxisAngle(const Vector3& axis, const float angle)
{
	Quaternion   q;
	::D3DXQuaternionRotationAxis(&q, &axis, D3DXToRadian(angle));
	return q;
}

// 生成
inline Quaternion Quaternion_CreateFromRotationMatrix(const Matrix& mat)
{
	Quaternion   q;
	::D3DXQuaternionRotationMatrix(&q, &mat);
	return q;
}

// 生成
inline Quaternion Quaternion_CreateFromYawPitchRoll(const float y, const float x, const float z)
{
	Quaternion   q;
	::D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(y), D3DXToRadian(x), D3DXToRadian(z));
	return q;
}

// 生成
inline Quaternion Quaternion_CreateFromVector(const Vector3& v)
{
	// 回転行列生成
	D3DXMATRIX   rot;
	::D3DXMatrixIdentity(&rot);

	// 軸設定
	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)rot.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)rot.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)rot.m[2];

	// ｚ軸設定
	::D3DXVec3Normalize(z_axis, &v);

	// ｘ軸設定
	::D3DXVec3Cross(x_axis, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), z_axis);

	// ｙ軸設定
	::D3DXVec3Cross(y_axis, z_axis, x_axis);

	// 回転行列をクォータニオンへ変換
	Quaternion   q;
	::D3DXQuaternionRotationMatrix(&q, &rot);

	return q;
}

// 生成
inline Quaternion Quaternion_CreateFromOrientation(const Vector3& v1, const Vector3& v2)
{
	return Quaternion_CreateFromVector(v1 - v2);
}

// 除算
inline Quaternion Quaternion_Divide(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion   q;
	::D3DXQuaternionConjugate(&q, &q2);
	return q1 * q2;
}

// 内積
inline float Quaternion_Dot(const Quaternion& q1, const Quaternion& q2)
{
	return ::D3DXQuaternionDot(&q1, &q2);
}

// 逆クォータニオン
inline Quaternion Quaternion_Inverse(const Quaternion& q1)
{
	Quaternion   q;
	::D3DXQuaternionInverse(&q, &q1);
	return q;
}

// 長さ
inline float Quaternion_Length(const Quaternion& q1)
{
	return ::D3DXQuaternionLength(&q1);
}

// 長さの２乗
inline float Quaternion_LengthSquared(const Quaternion& q1)
{
	return ::D3DXQuaternionLengthSq(&q1);
}

// 線形補間
inline Quaternion Quaternion_Lerp(const Quaternion& q1, const Quaternion& q2)
{
	return (q1 + q2) / 2.0f;
}

// 乗算
inline Quaternion Quaternion_Multiply(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion   q;
	::D3DXQuaternionMultiply(&q, &q1, &q2);
	return q;
}

// 乗算
inline Quaternion Quaternion_Multiply(const Quaternion& q1, const float f)
{
	return q1 * f;
}

// 符号反転
inline Quaternion Quaternion_Negate(const Quaternion& q1)
{
	return -q1;
}

// 正規化
inline Quaternion Quaternion_Normalize(const Quaternion& q1)
{
	Quaternion   q;
	::D3DXQuaternionNormalize(&q, &q1);
	return q;
}

// 球状線形補間
inline Quaternion Quaternion_Slerp(const Quaternion& q1, const Quaternion& q2, const float t)
{
	Quaternion   q;
	::D3DXQuaternionSlerp(&q, &q1, &q2, t);
	return q;
}

// 減算
inline Quaternion Quaternion_Subtract(const Quaternion& q1, const Quaternion& q2)
{
	return q1 - q2;
}

#pragma endregion

//------------------------------------------------------------------------------
//	行列関数
//------------------------------------------------------------------------------
#pragma region MatrixFunc

// 単位行列
inline Matrix Matrix_Identity()
{
	Matrix   matT;
	::D3DXMatrixIdentity(&matT);
	return matT;
}

// 逆行列
inline Matrix Matrix_Invert(const Matrix& mat)
{
	Matrix   matT;
	if(::D3DXMatrixInverse(&matT, NULL, &mat) == NULL)
		::ZeroMemory(&matT, sizeof(matT));
	return matT;
}

// 転置行列
inline Matrix Matrix_Transpose(const Matrix& mat)
{
	Matrix   matT;
	::D3DXMatrixTranspose(&matT, &mat);
	return matT;
}

// x軸回転行列
inline Matrix Matrix_CreateRotationX(const float degrees)
{
	Matrix   matT;
	::D3DXMatrixRotationX(&matT, D3DXToRadian(degrees));
	return matT;
}

// y軸回転行列
inline Matrix Matrix_CreateRotationY(const float degrees)
{
	Matrix   matT;
	::D3DXMatrixRotationY(&matT, D3DXToRadian(degrees));
	return matT;
}

// z軸回転行列
inline Matrix Matrix_CreateRotationZ(const float degrees)
{
	Matrix   matT;
	::D3DXMatrixRotationZ(&matT, D3DXToRadian(degrees));
	return matT;
}

// y-x-z軸回転行列
inline Matrix Matrix_CreateFromYawPitchRoll(const float y, const float x, const float z)
{
	Matrix   matT;
	::D3DXMatrixRotationYawPitchRoll(&matT, D3DXToRadian(y), D3DXToRadian(x), D3DXToRadian(z));
	return matT;
}

// 任意軸回転行列
inline Matrix Matrix_CreateFromAxisAngle(const Vector3& v, const float degrees)
{
	Matrix   matT;
	::D3DXMatrixRotationAxis(&matT, &v, D3DXToRadian(degrees));
	return matT;
}

// スケーリング行列
inline Matrix Matrix_CreateScale(const float s)
{
	Matrix   matT;
	::D3DXMatrixScaling(&matT, s, s, s);
	return matT;
}

// スケーリング行列
inline Matrix Matrix_CreateScale(const float x, const float y, const float z)
{
	Matrix   matT;
	::D3DXMatrixScaling(&matT, x, y, z);
	return matT;
}

// スケーリング行列
inline Matrix Matrix_CreateScale(const Vector3& v)
{
	Matrix   matT;
	::D3DXMatrixScaling(&matT, v.x, v.y, v.z);
	return matT;
}

// せん断
inline Matrix Matrix_CreateShear(const float xy_x, const float xy_y,
								 const float xz_x, const float xz_z,
								 const float yz_y, const float yz_z)
{
	Matrix   matT;

	matT._11 = 1;
	matT._12 = ::tanf(D3DXToRadian(xy_x));
	matT._13 = ::tanf(D3DXToRadian(xz_x));
	matT._14 = 0;

	matT._21 = ::tanf(D3DXToRadian(xy_y));
	matT._22 = 1;
	matT._23 = ::tanf(D3DXToRadian(yz_y));
	matT._24 = 0;

	matT._31 = ::tanf(D3DXToRadian(xz_z));
	matT._32 = ::tanf(D3DXToRadian(yz_z));
	matT._33 = 1;
	matT._34 = 0;

	matT._41 = 0;
	matT._42 = 0;
	matT._43 = 0;
	matT._44 = 1;

	return matT;
}

// 移動行列
inline Matrix Matrix_CreateTranslation(const float x, const float y, const float z)
{
	Matrix   matT;
	::D3DXMatrixTranslation(&matT, x, y, z);
	return matT;
}

// 移動行列
inline Matrix Matrix_CreateTranslation(const Vector3& v)
{
	Matrix   matT;
	::D3DXMatrixTranslation(&matT, v.x, v.y, v.z);
	return matT;
}

// 平面射影行列
inline Matrix Matrix_CreateShadow(const Vector4& l, const Plane& p)
{
	Matrix   matT;
	::D3DXMatrixShadow(&matT, &l, &p);
	return matT;
}

// 平面射影行列
inline Matrix Matrix_CreateShadow(const D3DLIGHTTYPE light_type, const Vector3& light, 
								  const Vector3& pos, const Vector3& shadow_offset,
								  const Vector3& normal)
{
	D3DXVECTOR4   Light;
	if(light_type == D3DLIGHT_DIRECTIONAL) {
		// ディレクショナルライト
		Light = D3DXVECTOR4(light, 0.0f);
		::D3DXVec4Normalize(&Light, &Light);
		::D3DXVec4Scale(&Light, &Light, -1.5e+11F);	// 1.5億km = 1AU
	} else {
		// ポイントライト・スポットライト
		Light = D3DXVECTOR4(light - pos, 1.0f);
	}

	D3DXPLANE     Plane;
	::D3DXPlaneFromPointNormal(&Plane, &(pos + shadow_offset), &normal);

	D3DXMATRIX   matT;
	::D3DXMatrixShadow(&matT, &Light, &Plane);

	return matT;
}

// 平面座標系反映行列
inline Matrix Matrix_CreateReflection(const Plane& p)
{
	Matrix   matT;
	::D3DXMatrixReflect(&matT, &p);
	return matT;
}

// 回転行列
inline Matrix Matrix_CreateFromQuaternion(const Quaternion& q)
{
	Matrix   matT;
	::D3DXMatrixRotationQuaternion(&matT, &q);
	return matT;
}

// 行列式
inline float Matrix_Determinant(const Matrix& mat)
{
	return ::D3DXMatrixDeterminant(&mat);
}

// LookAt
inline Matrix Matrix_CreateLookAt(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	Matrix   matT;
	::D3DXMatrixLookAtLH(&matT, &pos, &target, &up);
	return matT;
}

// View
inline Matrix Matrix_CreateView(const Vector3& pos, const Vector3& angle)
{
	Quaternion   qt;
	::D3DXQuaternionRotationYawPitchRoll(&qt, D3DXToRadian(-angle.y),
											  D3DXToRadian( angle.x),
											  D3DXToRadian(-angle.z));

	Matrix   matT;
	::D3DXMatrixRotationQuaternion(&matT, &qt);
	*(D3DXVECTOR3*)&matT.m[3] = pos;
	::D3DXMatrixInverse(&matT, NULL, &matT);

	return matT;
}

// 正射影
inline Matrix Matrix_CreateOrthographic(const float width, const float height,
										const float znear, const float zfar)
{
	Matrix   matT;
	::D3DXMatrixOrthoLH(&matT, width, height, znear, zfar);
	return matT;
}

// カスタマイズ正射影
inline Matrix Matrix_CreateOrthographicOffCenter(const float left,   const float right,
												 const float bottom, const float top,
												 const float znear,  const float zfar)
{
	Matrix   matT;
	::D3DXMatrixOrthoOffCenterLH(&matT, left, right, bottom, top, znear, zfar);
	return matT;
}

// パースペクティブ射影
inline Matrix Matrix_CreatePerspectiveFieldOfView(const float fov,   const float aspect,
												  const float znear, const float zfar)
{
	Matrix   matT;
	::D3DXMatrixPerspectiveFovLH(&matT, fov, aspect, znear, zfar);
	return matT;
}

// カスタマイズパースペクティブ射影
inline Matrix Matrix_CreatePerspectiveOffCenter(const float left,   const float right,
												const float bottom, const float top,
												const float znear,  const float zfar)
{
	Matrix   matT;
	::D3DXMatrixPerspectiveOffCenterLH(&matT, left, right, bottom, top, znear, zfar);
	return matT;
}

// 変換行列分解
inline void Matrix_Decompose(const Matrix& matT, Vector3& scale, Quaternion& rot, Vector3& trans)
{
	::D3DXMatrixDecompose(&scale, &rot, &trans, &matT);
}

#pragma endregion

//------------------------------------------------------------------------------
//	Math
//------------------------------------------------------------------------------
#pragma region math

#define Math_Abs(single)          ::fabsf(single)
#define Math_Acos(single)         ::acosf(single)
#define Math_Asin(single)         ::asinf(single)
#define Math_Atan(single)         ::atanf(single)
#define Math_Atan2(y,x)           ::atan2f(y, x)
#define Math_Ceiling(single)      ::ceilf(single)
#define Math_Cos(single)          ::cosf(single)
#define Math_Cosh(single)         ::coshf(single)
#define Math_Exp(single)          ::expf(single)
#define Math_Floor(single)        ::floorf(single)
#define Math_Log(single)          ::logf(single)
#define Math_Log10(single)        ::log10f(single)
#define Math_Max(a,b)             (((a) > (b)) ? (a) : (b))
#define Math_Min(a,b)             (((a) < (b)) ? (a) : (b))
#define Math_Pow(single)          ::powf(single)
#define Math_Sin(single)          ::sinf(single)
#define Math_Sinh(single)         ::sinfh(single)
#define Math_Sqrt(single)         ::sqrtf(single)
#define Math_Tan(single)          ::tanf(single)
#define Math_Tanh(single)         ::tanhf(single)

#define Math_E                    (2.718282f)
#define Math_PI                   (3.141592f)

#pragma endregion

//------------------------------------------------------------------------------
//	MathHelper
//------------------------------------------------------------------------------
#pragma region mathhelper

#define MathHelper_E              (2.718282f)
#define MathHelper_Log2E          (1.442695f)
#define MathHelper_Log10E         (0.4342945f)
#define MathHelper_Pi             ((float)3.141592654f)
#define MathHelper_TwoPi          ((float)6.283185307f)
#define MathHelper_PiOver2        ((float)1.570796327f)
#define MathHelper_PiOver4        ((float)0.785398163f)
#define MathHelper_OneDividePi    ((float)0.318309886f)
#define MathHelper_OneDivideTwoPi ((float)0.159154943f)

// 重心
inline float MathHelper_Barycentric(const float value1,  const float value2, const float value3,
									const float amount1, const float amount2)
{
	return ((1.0f - amount1 - amount2) * value1) + (amount1 * value2) + (amount2 * value3);
}

// クランプ
inline float MathHelper_Clamp(const float value, const float min, const float max)
{
	if(value > max)
		return max;
	if(value < min)
		return min;

	return value;
}

// 距離
inline float MathHelper_Distance(const float value1, const float value2)
{
	return ::fabsf(value2 - value1);
}

// 最大値
inline float MathHelper_Max(const float value1, const float value2)
{
	return value1 > value2 ? value1 : value2;
}

// 最小値
inline float MathHelper_Min(const float value1, const float value2)
{
	return value1 < value2 ? value1 : value2;
}

// 線形補間
inline float MathHelper_Lerp(const float value1, const float value2, const float amount)
{
	if(amount <= 0.0f)
		return value1;
	if(amount >= 1.0f)
		return value2;

	return value1 + (value2 - value1) * amount;
}

// 三次方程式補間
inline float MathHelper_SmoothStep(const float value1, const float value2, const float amount)
{
	float   t;

	t = (amount - value1) / (value2 - value1);
	t = t * t * (3.0f - 2.0f * t);

	return t;
}

// Catmull-Rom補間
inline float MathHelper_CatmullRom(const float value1, const float value2,
								   const float value3, const float value4,
								   const float amount)
{
	const float   v0 = (value3 - value1) / 2.0f;
	const float   v1 = (value4 - value2) / 2.0f;
	const float   t2 = amount * amount;
	const float   t3 = t2     * amount;

	return ( 2 * value2 - 2 * value3 +     v0 + v1) * t3 + 
		   (-3 * value2 + 3 * value3 - 2 * v0 - v1) * t2 + v0 * amount + value2;
}

// エルミートスプライン補間
inline float MathHelper_Hermite(const float value1, const float tangent1,
								const float value2, const float tangent2,
								const float amount)
{
	Vector2   v;
	::D3DXVec2Hermite(&v,
					  &Vector2(value1, 0.0f), &Vector2(tangent1, 0.0f),
					  &Vector2(value2, 0.0f), &Vector2(tangent2, 0.0f),
					  amount);

	return v.x;
}

// ラジアン→度変換
inline float MathHelper_ToDegrees(const float radians)
{
	return radians * (180.0f / MathHelper_Pi);
}

// 度→ラジアン変換
inline float MathHelper_ToRadians(const float degrees)
{
	return degrees * (MathHelper_Pi / 180.0f);
}

// 角度をπ～－πの間にする
inline float MathHelper_WrapAngle(float radians)
{
	while(radians < -MathHelper_Pi)
		radians += MathHelper_TwoPi;

	while(radians > MathHelper_Pi)
		radians -= MathHelper_TwoPi;

	return radians;
}

// ベジエ曲線
inline Vector2 MathHelper_Bezir(const Vector2& start,
								const Vector2& control1, const Vector2& control2,
								const Vector2& end,
								const float t)
{
	const float   A1 = 1.0f - t;
	const float   A2 = A1 * A1;
	const float   A3 = A2 * A1;

	const float   B1 = t;
	const float   B2 = B1 * B1;
	const float   B3 = B2 * B1;

	Vector2   p;

	p.x = A3 * start.x + 3 * A2 * B1 * control1.x + 3 * A1 * B2 * control2.x + B3 * end.x;
	p.y = A3 * start.y + 3 * A2 * B1 * control1.y + 3 * A1 * B2 * control2.y + B3 * end.y;

	return p;
}

// 三角関数(度数版)
#define MathHelper_Sin(degrees)    (::sinf((degrees) * (MathHelper_Pi / 180.0f)))
#define MathHelper_Cos(degrees)    (::cosf((degrees) * (MathHelper_Pi / 180.0f)))
#define MathHelper_Acos(degrees)  (::acosf((degrees) * (MathHelper_Pi / 180.0f)))
#define MathHelper_Asin(degrees)  (::asinf((degrees) * (MathHelper_Pi / 180.0f)))
#define MathHelper_Atan2(y,x)     (::atan2f(y, x)    * (180.0f / MathHelper_Pi))

// 乱数(閉区間)
inline int MathHelper_Random(const int max)
{
	return Math_RandomInt() % (max + 1);
}

inline int MathHelper_Random(const int min, const int max)
{
	return (Math_RandomInt() % ((max + 1) - min)) + min;
}

#pragma endregion

//------------------------------------------------------------------------------
//	レンダリングステート
//------------------------------------------------------------------------------
#pragma region renderstate

#define Lighting_Enable                        D3DRS_LIGHTING,TRUE
#define Lighting_Disable                       D3DRS_LIGHTING,FALSE

#define DepthBuffer_Enable                     D3DRS_ZENABLE,TRUE
#define DepthBuffer_Disable                    D3DRS_ZENABLE,FALSE

#define DepthBufferWrite_Enable                D3DRS_ZWRITEENABLE,TRUE
#define DepthBufferWrite_Disable               D3DRS_ZWRITEENABLE,FALSE

#define AlphaTest_Enable                       D3DRS_ALPHATESTENABLE,TRUE
#define AlphaTest_Disable                      D3DRS_ALPHATESTENABLE,FALSE

#define Dither_Enable                          D3DRS_DITHERENABLE,TRUE
#define Dither_Disable                         D3DRS_DITHERENABLE,FALSE

#define Specular_Enable                        D3DRS_SPECULARENABLE,TRUE
#define Specular_Disable                       D3DRS_SPECULARENABLE,FALSE

#define Stencil_Enable                         D3DRS_STENCILENABLE,TRUE
#define Stencil_Disable                        D3DRS_STENCILENABLE,FALSE

#define Clipping_Enable                        D3DRS_CLIPPING,TRUE
#define Clipping_Disable                       D3DRS_CLIPPING,FALSE

#define LocalViewer_Enable                     D3DRS_LOCALVIEWER,TRUE
#define LocalViewer_Disable                    D3DRS_LOCALVIEWER,FALSE

#define ScissorTest_Enable                     D3DRS_SCISSORTESTENABLE,TRUE
#define ScissorTest_Disable                    D3DRS_SCISSORTESTENABLE,FALSE

#define SeparateAlphaBlend_Enable              D3DRS_SEPARATEALPHABLENDENABLE,TRUE
#define SeparateAlphaBlend_Disable             D3DRS_SEPARATEALPHABLENDENABLE,FALSE

#define NormalizeNormals_Enable                D3DRS_NORMALIZENORMALS,TRUE
#define NormalizeNormals_Disable               D3DRS_NORMALIZENORMALS,FALSE

#define Fog_Enable                             D3DRS_FOGENABLE,TRUE
#define Fog_Disable                            D3DRS_FOGENABLE,FALSE

#define FogColor(r,g,b)                        D3DRS_FOGCOLOR,Color(r,g,b)

#define FogStart                               D3DRS_FOGSTART
#define FogEnd                                 D3DRS_FOGEND
#define FogDensity                             D3DRS_FOGDENSITY

#define RangeFog_Enable                        D3DRS_RANGEFOGENABLE,TRUE
#define RangeFog_Disable                       D3DRS_RANGEFOGENABLE,FALSE

#define FogVertexMode_None                     D3DRS_FOGVERTEXMODE,D3DFOG_NONE
#define FogVertexMode_Linear                   D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR
#define FogVertexMode_Exponent                 D3DRS_FOGVERTEXMODE,D3DFOG_EXP
#define FogVertexMode_ExponentSquared          D3DRS_FOGVERTEXMODE,D3DFOG_EXP2

#define FogPixelMode_None                      D3DRS_FOGTABLEMODE,D3DFOG_NONE
#define FogPixelMode_Linear                    D3DRS_FOGTABLEMODE,D3DFOG_LINEAR
#define FogPixelMode_Exponent                  D3DRS_FOGTABLEMODE,D3DFOG_EXP
#define FogPixelMode_ExponentSquared           D3DRS_FOGTABLEMODE,D3DFOG_EXP2

#define PointSize                              D3DRS_POINTSIZE
#define PointSizeMin                           D3DRS_POINTSIZE_MIN
#define PointSizeMax                           D3DRS_POINTSIZE_MAX

#define PointSprite_Enable                     D3DRS_POINTSPRITEENABLE,TRUE
#define PointSprite_Disable                    D3DRS_POINTSPRITEENABLE,FALSE

#define PointScale_Enable                      D3DRS_POINTSCALEENABLE,TRUE
#define PointScale_Disable                     D3DRS_POINTSCALEENABLE,FALSE

#define MultiSampleAntiAlias_Enable            D3DRS_MULTISAMPLEANTIALIAS,TRUE
#define MultiSampleAntiAlias_Disable           D3DRS_MULTISAMPLEANTIALIAS,FALSE

#define MultiSampleMask                        D3DRS_MULTISAMPLEMASK

#define FillMode_Point                         D3DRS_FILLMODE,D3DFILL_POINT
#define FillMode_WireFrame                     D3DRS_FILLMODE,D3DFILL_WIREFRAME
#define FillMode_Solid                         D3DRS_FILLMODE,D3DFILL_SOLID

#define ShadeMode_Flat                         D3DRS_SHADEMODE,D3DSHADE_FLAT
#define ShadeMode_Gouraud                      D3DRS_SHADEMODE,D3DSHADE_GOURAUD

#define CullMode_CullClockwiseFace             D3DRS_CULLMODE,D3DCULL_CW
#define CullMode_CullCounterClockwiseFace      D3DRS_CULLMODE,D3DCULL_CCW
#define CullMode_None                          D3DRS_CULLMODE,D3DCULL_NONE

#define AmbientLightColor(r,g,b)               D3DRS_AMBIENT,Color(r,g,b)

#define DepthBias                              D3DRS_DEPTHBIAS

#define AlphaBlend_Enable                      D3DRS_ALPHABLENDENABLE,TRUE
#define AlphaBlend_Disable                     D3DRS_ALPHABLENDENABLE,FALSE

#define BlendFactor(r,g,b)                     D3DRS_BLENDFACTOR,Color(r,g,b)

#define BlendOperation_Add                     D3DRS_BLENDOP,D3DBLENDOP_ADD
#define BlendOperation_Max                     D3DRS_BLENDOP,D3DBLENDOP_MAX
#define BlendOperation_Min                     D3DRS_BLENDOP,D3DBLENDOP_MIN
#define BlendOperation_ReverseSubtract         D3DRS_BLENDOP,D3DBLENDOP_REVSUBTRACT
#define BlendOperation_Subtract                D3DRS_BLENDOP,D3DBLENDOP_SUBTRACT

#define SourceBlend_Default                      D3DRS_SRCBLEND,D3DBLEND_ONE
#define SourceBlend_Zero                         D3DRS_SRCBLEND,D3DBLEND_ZERO
#define SourceBlend_One                          D3DRS_SRCBLEND,D3DBLEND_ONE
#define SourceBlend_SourceColor                  D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR
#define SourceBlend_InverseSourceColor           D3DRS_SRCBLEND,D3DBLEND_INVSRCCOLOR
#define SourceBlend_SourceAlpha                  D3DRS_SRCBLEND,D3DBLEND_SRCALPHA
#define SourceBlend_InverseSourceAlpha           D3DRS_SRCBLEND,D3DBLEND_INVSRCALPHA
#define SourceBlend_DestinationAlpha             D3DRS_SRCBLEND,D3DBLEND_DESTALPHA
#define SourceBlend_InverseDestinationAlpha      D3DRS_SRCBLEND,D3DBLEND_INVDESTALPHA
#define SourceBlend_DestinationColor             D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR
#define SourceBlend_InverseDestinationColor      D3DRS_SRCBLEND,D3DBLEND_INVDESTCOLOR
#define SourceBlend_SourceAlphaSaturation        D3DRS_SRCBLEND,D3DBLEND_SRCALPHASAT
#define SourceBlend_BothSourceAlpha              D3DRS_SRCBLEND,D3DBLEND_BOTHSRCALPHA
#define SourceBlend_BothInverseSourceAlpha       D3DRS_SRCBLEND,D3DBLEND_BOTHINVSRCALPHA
#define SourceBlend_BlendFactor                  D3DRS_SRCBLEND,D3DBLEND_BLENDFACTOR
#define SourceBlend_InverseBlendFactor           D3DRS_SRCBLEND,D3DBLEND_INVBLENDFACTOR

#define DestinationBlend_Default                 D3DRS_DESTBLEND,D3DBLEND_ZERO
#define DestinationBlend_Zero                    D3DRS_DESTBLEND,D3DBLEND_ZERO
#define DestinationBlend_One                     D3DRS_DESTBLEND,D3DBLEND_ONE
#define DestinationBlend_SourceColor             D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR
#define DestinationBlend_InverseSourceColor      D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR
#define DestinationBlend_SourceAlpha             D3DRS_DESTBLEND,D3DBLEND_SRCALPHA
#define DestinationBlend_InverseSourceAlpha      D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA
#define DestinationBlend_DestinationAlpha        D3DRS_DESTBLEND,D3DBLEND_DESTALPHA
#define DestinationBlend_InverseDestinationAlpha D3DRS_DESTBLEND,D3DBLEND_INVDESTALPHA
#define DestinationBlend_DestinationColor        D3DRS_DESTBLEND,D3DBLEND_DESTCOLOR
#define DestinationBlend_InverseDestinationColor D3DRS_DESTBLEND,D3DBLEND_INVDESTCOLOR
#define DestinationBlend_SourceAlphaSaturation   D3DRS_DESTBLEND,D3DBLEND_SRCALPHASAT
#define DestinationBlend_BothSourceAlpha         D3DRS_DESTBLEND,D3DBLEND_BOTHSRCALPHA
#define DestinationBlend_BothInverseSourceAlpha  D3DRS_DESTBLEND,D3DBLEND_BOTHINVSRCALPHA
#define DestinationBlend_BlendFactor             D3DRS_DESTBLEND,D3DBLEND_BLENDFACTOR
#define DestinationBlend_InverseBlendFactor      D3DRS_DESTBLEND,D3DBLEND_INVBLENDFACTOR

#define AlphaSourceBlend_Default                 D3DRS_SRCBLENDALPHA,D3DBLEND_ONE
#define AlphaSourceBlend_Zero                    D3DRS_SRCBLENDALPHA,D3DBLEND_ZERO
#define AlphaSourceBlend_One                     D3DRS_SRCBLENDALPHA,D3DBLEND_ONE
#define AlphaSourceBlend_SourceColor             D3DRS_SRCBLENDALPHA,D3DBLEND_SRCCOLOR
#define AlphaSourceBlend_InverseSourceColor      D3DRS_SRCBLENDALPHA,D3DBLEND_INVSRCCOLOR
#define AlphaSourceBlend_SourceAlpha             D3DRS_SRCBLENDALPHA,D3DBLEND_SRCALPHA
#define AlphaSourceBlend_InverseSourceAlpha      D3DRS_SRCBLENDALPHA,D3DBLEND_INVSRCALPHA
#define AlphaSourceBlend_DestinationAlpha        D3DRS_SRCBLENDALPHA,D3DBLEND_DESTALPHA
#define AlphaSourceBlend_InverseDestinationAlpha D3DRS_SRCBLENDALPHA,D3DBLEND_INVDESTALPHA
#define AlphaSourceBlend_DestinationColor        D3DRS_SRCBLENDALPHA,D3DBLEND_DESTCOLOR
#define AlphaSourceBlend_InverseDestinationColor D3DRS_SRCBLENDALPHA,D3DBLEND_INVDESTCOLOR
#define AlphaSourceBlend_SourceAlphaSaturation   D3DRS_SRCBLENDALPHA,D3DBLEND_SRCALPHASAT
#define AlphaSourceBlend_BothSourceAlpha         D3DRS_SRCBLENDALPHA,D3DBLEND_BOTHSRCALPHA
#define AlphaSourceBlend_BothInverseSourceAlpha  D3DRS_SRCBLENDALPHA,D3DBLEND_BOTHINVSRCALPHA
#define AlphaSourceBlend_BlendFactor             D3DRS_SRCBLENDALPHA,D3DBLEND_BLENDFACTOR
#define AlphaSourceBlend_InverseBlendFactor      D3DRS_SRCBLENDALPHA,D3DBLEND_INVBLENDFACTOR

#define AlphaDestinationBlend_Default                 D3DRS_DESTBLENDALPHA,D3DBLEND_ZERO
#define AlphaDestinationBlend_Zero                    D3DRS_DESTBLENDALPHA,D3DBLEND_ZERO
#define AlphaDestinationBlend_One                     D3DRS_DESTBLENDALPHA,D3DBLEND_ONE
#define AlphaDestinationBlend_SourceColor             D3DRS_DESTBLENDALPHA,D3DBLEND_SRCCOLOR
#define AlphaDestinationBlend_InverseSourceColor      D3DRS_DESTBLENDALPHA,D3DBLEND_INVSRCCOLOR
#define AlphaDestinationBlend_SourceAlpha             D3DRS_DESTBLENDALPHA,D3DBLEND_SRCALPHA
#define AlphaDestinationBlend_InverseSourceAlpha      D3DRS_DESTBLENDALPHA,D3DBLEND_INVSRCALPHA
#define AlphaDestinationBlend_DestinationAlpha        D3DRS_DESTBLENDALPHA,D3DBLEND_DESTALPHA
#define AlphaDestinationBlend_InverseDestinationAlpha D3DRS_DESTBLENDALPHA,D3DBLEND_INVDESTALPHA
#define AlphaDestinationBlend_DestinationColor        D3DRS_DESTBLENDALPHA,D3DBLEND_DESTCOLOR
#define AlphaDestinationBlend_InverseDestinationColor D3DRS_DESTBLENDALPHA,D3DBLEND_INVDESTCOLOR
#define AlphaDestinationBlend_SourceAlphaSaturation   D3DRS_DESTBLENDALPHA,D3DBLEND_SRCALPHASAT
#define AlphaDestinationBlend_BothSourceAlpha         D3DRS_DESTBLENDALPHA,D3DBLEND_BOTHSRCALPHA
#define AlphaDestinationBlend_BothInverseSourceAlpha  D3DRS_DESTBLENDALPHA,D3DBLEND_BOTHINVSRCALPHA
#define AlphaDestinationBlend_BlendFactor             D3DRS_DESTBLENDALPHA,D3DBLEND_BLENDFACTOR
#define AlphaDestinationBlend_InverseBlendFactor      D3DRS_DESTBLENDALPHA,D3DBLEND_INVBLENDFACTOR

#define AlphaBlendOperation_Add                D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD
#define AlphaBlendOperation_Max                D3DRS_BLENDOPALPHA,D3DBLENDOP_MAX
#define AlphaBlendOperation_Min                D3DRS_BLENDOPALPHA,D3DBLENDOP_MIN
#define AlphaBlendOperation_ReverseSubtract    D3DRS_BLENDOPALPHA,D3DBLENDOP_REVSUBTRACT
#define AlphaBlendOperation_Subtract           D3DRS_BLENDOPALPHA,D3DBLENDOP_SUBTRACT

#define ReferenceAlpha                         D3DRS_ALPHAREF
#define ReferenceStencil                       D3DRS_STENCILREF

#define AlphaFunction_Default                  D3DRS_ALPHAFUNC,D3DCMP_ALWAYS
#define AlphaFunction_Never                    D3DRS_ALPHAFUNC,D3DCMP_NEVER
#define AlphaFunction_Always                   D3DRS_ALPHAFUNC,D3DCMP_ALWAYS
#define AlphaFunction_Equal                    D3DRS_ALPHAFUNC,D3DCMP_EQUAL
#define AlphaFunction_NotEqual                 D3DRS_ALPHAFUNC,D3DCMP_NOTEQUAL
#define AlphaFunction_Greater                  D3DRS_ALPHAFUNC,D3DCMP_GREATER
#define AlphaFunction_GreaterEqual             D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL
#define AlphaFunction_Less                     D3DRS_ALPHAFUNC,D3DCMP_LESS
#define AlphaFunction_LessEqual                D3DRS_ALPHAFUNC,D3DCMP_LESSEQUAL

#define DepthBufferFunction_Default            D3DRS_ZFUNC,D3DCMP_LESSEQUAL
#define DepthBufferFunction_Never              D3DRS_ZFUNC,D3DCMP_NEVER
#define DepthBufferFunction_Always             D3DRS_ZFUNC,D3DCMP_ALWAYS
#define DepthBufferFunction_Equal              D3DRS_ZFUNC,D3DCMP_EQUAL
#define DepthBufferFunction_NotEqual           D3DRS_ZFUNC,D3DCMP_NOTEQUAL
#define DepthBufferFunction_Greater            D3DRS_ZFUNC,D3DCMP_GREATER
#define DepthBufferFunction_GreaterEqual       D3DRS_ZFUNC,D3DCMP_GREATEREQUAL
#define DepthBufferFunction_Less               D3DRS_ZFUNC,D3DCMP_LESS
#define DepthBufferFunction_LessEqual          D3DRS_ZFUNC,D3DCMP_LESSEQUAL

#define StencilFunction_Default                D3DRS_STENCILFUNC,D3DCMP_ALWAYS
#define StencilFunction_Never                  D3DRS_STENCILFUNC,D3DCMP_NEVER
#define StencilFunction_Always                 D3DRS_STENCILFUNC,D3DCMP_ALWAYS
#define StencilFunction_Equal                  D3DRS_STENCILFUNC,D3DCMP_EQUAL
#define StencilFunction_NotEqual               D3DRS_STENCILFUNC,D3DCMP_NOTEQUAL
#define StencilFunction_Greater                D3DRS_STENCILFUNC,D3DCMP_GREATER
#define StencilFunction_GreaterEqual           D3DRS_STENCILFUNC,D3DCMP_GREATEREQUAL
#define StencilFunction_Less                   D3DRS_STENCILFUNC,D3DCMP_LESS
#define StencilFunction_LessEqual              D3DRS_STENCILFUNC,D3DCMP_LESSEQUAL

#define TwoSidedStencilMode_Enable             D3DRS_TWOSIDEDSTENCILMODE,TRUE
#define TwoSidedStencilMode_Disable            D3DRS_TWOSIDEDSTENCILMODE,FALSE

#define StencilFailOperation_Default                        D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP
#define StencilFailOperation_Keep                           D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP
#define StencilFailOperation_Zero                           D3DRS_STENCILFAIL,D3DSTENCILOP_ZERO
#define StencilFailOperation_Replace                        D3DRS_STENCILFAIL,D3DSTENCILOP_REPLACE
#define StencilFailOperation_IncrementSaturation            D3DRS_STENCILFAIL,D3DSTENCILOP_INCRSAT
#define StencilFailOperation_DecrementSaturation            D3DRS_STENCILFAIL,D3DSTENCILOP_DECRSAT
#define StencilFailOperation_Invert                         D3DRS_STENCILFAIL,D3DSTENCILOP_INVERT
#define StencilFailOperation_Increment                      D3DRS_STENCILFAIL,D3DSTENCILOP_INCR
#define StencilFailOperation_Decrement                      D3DRS_STENCILFAIL,D3DSTENCILOP_DECR

#define StencilDepthBufferFailOperation_Default             D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP
#define StencilDepthBufferFailOperation_Keep                D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP
#define StencilDepthBufferFailOperation_Zero                D3DRS_STENCILZFAIL,D3DSTENCILOP_ZERO
#define StencilDepthBufferFailOperation_Replace             D3DRS_STENCILZFAIL,D3DSTENCILOP_REPLACE
#define StencilDepthBufferFailOperation_IncrementSaturation D3DRS_STENCILZFAIL,D3DSTENCILOP_INCRSAT
#define StencilDepthBufferFailOperation_DecrementSaturation D3DRS_STENCILZFAIL,D3DSTENCILOP_DECRSAT
#define StencilDepthBufferFailOperation_Invert              D3DRS_STENCILZFAIL,D3DSTENCILOP_INVERT
#define StencilDepthBufferFailOperation_Increment           D3DRS_STENCILZFAIL,D3DSTENCILOP_INCR
#define StencilDepthBufferFailOperation_Decrement           D3DRS_STENCILZFAIL,D3DSTENCILOP_DECR

#define StencilPassOperation_Default                        D3DRS_STENCILPASS,D3DSTENCILOP_KEEP
#define StencilPassOperation_Keep                           D3DRS_STENCILPASS,D3DSTENCILOP_KEEP
#define StencilPassOperation_Zero                           D3DRS_STENCILPASS,D3DSTENCILOP_ZERO
#define StencilPassOperation_Replace                        D3DRS_STENCILPASS,D3DSTENCILOP_REPLACE
#define StencilPassOperation_IncrementSaturation            D3DRS_STENCILPASS,D3DSTENCILOP_INCRSAT
#define StencilPassOperation_DecrementSaturation            D3DRS_STENCILPASS,D3DSTENCILOP_DECRSAT
#define StencilPassOperation_Invert                         D3DRS_STENCILPASS,D3DSTENCILOP_INVERT
#define StencilPassOperation_Increment                      D3DRS_STENCILPASS,D3DSTENCILOP_INCR
#define StencilPassOperation_Decrement                      D3DRS_STENCILPASS,D3DSTENCILOP_DECR

#define StencilMask                            D3DRS_STENCILMASK
#define StencilWriteMask                       D3DRS_STENCILWRITEMASK

#define CounterClockwiseStencilFail            D3DRS_CCW_STENCILFAIL
#define CounterClockwiseStencilDepthBufferFail D3DRS_CCW_STENCILZFAIL
#define CounterClockwiseStencilPass            D3DRS_CCW_STENCILPASS
#define CounterClockwiseStencilFunction        D3DRS_CCW_STENCILFUNC

#define Wrap0                                  D3DRS_WRAP0
#define Wrap1                                  D3DRS_WRAP1
#define Wrap2                                  D3DRS_WRAP2
#define Wrap3                                  D3DRS_WRAP3
#define Wrap4                                  D3DRS_WRAP4
#define Wrap5                                  D3DRS_WRAP5
#define Wrap6                                  D3DRS_WRAP6
#define Wrap7                                  D3DRS_WRAP7
#define Wrap8                                  D3DRS_WRAP8
#define Wrap9                                  D3DRS_WRAP9
#define Wrap10                                 D3DRS_WRAP10
#define Wrap11                                 D3DRS_WRAP11
#define Wrap12                                 D3DRS_WRAP12
#define Wrap13                                 D3DRS_WRAP13
#define Wrap14                                 D3DRS_WRAP14
#define Wrap15                                 D3DRS_WRAP15

#define TextureWrapCoordinates_None            0
#define TextureWrapCoordinates_Zero            D3DWRAPCOORD_0
#define TextureWrapCoordinates_One             D3DWRAPCOORD_1
#define TextureWrapCoordinates_Two             D3DWRAPCOORD_2
#define TextureWrapCoordinates_Three           D3DWRAPCOORD_3

#define TextureWrap_None                       0
#define TextureWrap_U                          D3DWRAP_U
#define TextureWrap_V                          D3DWRAP_V
#define TextureWrap_W                          D3DWRAP_W

#define ColorWriteChannels                     D3DRS_COLORWRITEENABLE
#define ColorWriteChannels1                    D3DRS_COLORWRITEENABLE1
#define ColorWriteChannels2                    D3DRS_COLORWRITEENABLE2
#define ColorWriteChannels3                    D3DRS_COLORWRITEENABLE3
#define ColorWrite_None                        0
#define ColorWrite_Red                         D3DCOLORWRITEENABLE_RED
#define ColorWrite_Green                       D3DCOLORWRITEENABLE_GREEN
#define ColorWrite_Blue                        D3DCOLORWRITEENABLE_BLUE
#define ColorWrite_Alpha                       D3DCOLORWRITEENABLE_ALPHA
#define ColorWrite_All                        (D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA)

#pragma endregion

//------------------------------------------------------------------------------
//	ブレンドモード
//------------------------------------------------------------------------------
#define BlendMode_Default                      DXGBLEND_MODULATE
#define BlendMode_Modulate                     DXGBLEND_MODULATE
#define BlendMode_Add                          DXGBLEND_ADD
#define BlendMode_Subtract                     DXGBLEND_SUBTRACT
#define BlendMode_SourceColor                  DXGBLEND_SRCCOLOR
#define BlendMode_ZeroColor                    DXGBLEND_ZEROCOLOR

//------------------------------------------------------------------------------
//	色宣言
//------------------------------------------------------------------------------
enum Color_by_Name {
	Color_AliceBlue            = 0xFFF0F8FF,
	Color_AntiqueWhite         = 0xFFFAEBD7,
	Color_Aqua                 = 0xFF00FFFF,
	Color_Aquamarine           = 0xFF7FFFD4,
	Color_Azure                = 0xFFF0FFFF,
	Color_Beige                = 0xFFF5F5DC,
	Color_Bisque               = 0XFFFFE4C4,
	Color_Black                = 0xFF000000,
	Color_BlanchedAlmond       = 0xFFFFEBCD,
	Color_Blue                 = 0xFF0000FF,
	Color_BlueViolet           = 0xFF8A2BE2,
	Color_Brown                = 0xFFA52A2A,
	Color_BurlyWood            = 0xFFDEB887,
	Color_CadetBlue            = 0xFF5F9EA0,
	Color_Chartreuse           = 0xFF7FFF00,
	Color_Chocolate            = 0xFFD2691E,
	Color_Coral                = 0xFFFF7F50,
	Color_CornflowerBlue       = 0xFF6495ED,
	Color_Cornsilk             = 0xFFFFF8DC,
	Color_Crimson              = 0xFFDC143C,
	Color_Cyan                 = 0xFF00FFFF,
	Color_DarkBlue             = 0xFF00008B,
	Color_DarkCyan             = 0xFF008B8B,
	Color_DarkGoldenrod        = 0xFFB8860B,
	Color_DarkGray             = 0xFFA9A9A9,
	Color_DarkGrey             = 0xFFA9A9A9,
	Color_DarkGreen            = 0xFF006400,
	Color_DarkKhaki            = 0xFFBDB76B,
	Color_DarkMagenta          = 0xFF8B008B,
	Color_DarkOliveGreen       = 0xFF556B2F,
	Color_DarkOrange           = 0xFFFF8C00,
	Color_DarkOrchid           = 0xFF9932CC,
	Color_DarkRed              = 0xFF8B0000,
	Color_DarkSalmon           = 0xFFE9967A,
	Color_DarkSeaGreen         = 0xFF8FBC8F,
	Color_DarkSlateBlue        = 0xFF483D8B,
	Color_DarkSlateGray        = 0xFF2F4F4F,
	Color_DarkSlateGrey        = 0xFF2F4F4F,
	Color_DarkTurquoise        = 0xFF00CED1,
	Color_DarkViolet           = 0xFF9400D3,
	Color_DeepPink             = 0xFFFF1493,
	Color_DeepSkyBlue          = 0xFF00BFFF,
	Color_DimGray              = 0xFF696969,
	Color_DimGrey              = 0xFF696969,
	Color_DodgerBlue           = 0xFF1E90FF,
	Color_FireBrick            = 0xFFB22222,
	Color_FloralWhite          = 0xFFFFFAF0,
	Color_ForestGreen          = 0xFF228B22,
	Color_Fuchsia              = 0xFFFF00FF,
	Color_Gainsboro            = 0xFFDCDCDC,
	Color_GhostWhite           = 0xFFF8F8FF,
	Color_Gold                 = 0xFFFFD700,
	Color_Goldenrod            = 0xFFDAA520,
	Color_Gray                 = 0xFF808080,
	Color_Grey                 = 0xFF808080,
	Color_Green                = 0xFF008000,
	Color_GreenYellow          = 0xFFADFF2F,
	Color_Honeydew             = 0xFFF0FFF0,
	Color_HotPink              = 0xFFFF69B4,
	Color_IndianRed            = 0xFFCD5C5C,
	Color_Indigo               = 0xFF4B0082,
	Color_Ivory                = 0xFFFFFFF0,
	Color_Khaki                = 0xFFF0E68C,
	Color_Lavender             = 0xFFE6E6FA,
	Color_LavenderBlush        = 0xFFFFF0F5,
	Color_LawnGreen            = 0xFF7CFC00,
	Color_LemonChiffon         = 0xFFFFFACD,
	Color_LightBlue            = 0xFFADD8E6,
	Color_LightCoral           = 0xFFF08080,
	Color_LightCyan            = 0xFFE0FFFF,
	Color_LightGoldenrodYellow = 0xFFFAFAD2,
	Color_LightGreen           = 0xFF90EE90,
	Color_LightGray            = 0xFFD3D3D3,
	Color_LightGrey            = 0xFFD3D3D3,
	Color_LightPink            = 0xFFFFB6C1,
	Color_LightSalmon          = 0xFFFFA07A,
	Color_LightSeaGreen        = 0xFF20B2AA,
	Color_LightSkyBlue         = 0xFF87CEFA,
	Color_LightSlateGray       = 0xFF778899,
	Color_LightSlateGrey       = 0xFF778899,
	Color_LightSteelBlue       = 0xFFB0C4DE,
	Color_LightYellow          = 0xFFFFFFE0,
	Color_Lime                 = 0xFF00FF00,
	Color_LimeGreen            = 0xFF32CD32,
	Color_Linen                = 0xFFFAF0E6,
	Color_Magenta              = 0xFFFF00FF,
	Color_Maroon               = 0xFF800000,
	Color_MediumAquamarine     = 0xFF66CDAA,
	Color_MediumBlue           = 0xFF0000CD,
	Color_MediumOrchid         = 0xFFBA55D3,
	Color_MediumPurple         = 0xFF9370DB,
	Color_MediumSeaGreen       = 0xFF3CB371,
	Color_MediumSlateBlue      = 0xFF7B68EE,
	Color_MediumSpringGreen    = 0xFF00FA9A,
	Color_MediumTurquoise      = 0xFF48D1CC,
	Color_MediumVioletRed      = 0xFFC71585,
	Color_MidnightBlue         = 0xFF191970,
	Color_MintCream            = 0xFFF5FFFA,
	Color_MistyRose            = 0xFFFFE4E1,
	Color_Moccasin             = 0xFFFFE4B5,
	Color_NavajoWhite          = 0xFFFFDEAD,
	Color_Navy                 = 0xFF000080,
	Color_OldLace              = 0xFFFDF5E6,
	Color_Olive                = 0xFF808000,
	Color_OliveDrab            = 0xFF6B8E23,
	Color_Orange               = 0xFFFFA500,
	Color_OrangeRed            = 0xFFFF4500,
	Color_Orchid               = 0xFFDA70D6,
	Color_PaleGoldenrod        = 0xFFEEE8AA,
	Color_PaleGreen            = 0xFF98FB98,
	Color_PaleTurquoise        = 0xFFAFEEEE,
	Color_PaleVioletRed        = 0xFFDB7093,
	Color_PapayaWhip           = 0xFFFFEFD5,
	Color_PeachPuff            = 0xFFFFDAB9,
	Color_Peru                 = 0xFFCD853F,
	Color_Pink                 = 0xFFFFC0CB,
	Color_Plum                 = 0xFFDDA0DD,
	Color_PowderBlue           = 0xFFB0E0E6,
	Color_Purple               = 0xFF800080,
	Color_Red                  = 0xFFFF0000,
	Color_RosyBrown            = 0xFFBC8F8F,
	Color_RoyalBlue            = 0xFF4169E1,
	Color_SaddleBrown          = 0xFF8B4513,
	Color_Salmon               = 0xFFFA8072,
	Color_SandyBrown           = 0xFFF4A460,
	Color_SeaGreen             = 0xFF2E8B57,
	Color_Seashell             = 0xFFFFF5EE,
	Color_Sienna               = 0xFFA0522D,
	Color_Silver               = 0xFFC0C0C0,
	Color_SkyBlue              = 0xFF87CEEB,
	Color_SlateBlue            = 0xFF6A5ACD,
	Color_SlateGray            = 0xFF708090,
	Color_SlateGrey            = 0xFF708090,
	Color_Snow                 = 0xFFFFFAFA,
	Color_SpringGreen          = 0xFF00FF7F,
	Color_SteelBlue            = 0xFF4682B4,
	Color_Tan                  = 0xFFD2B48C,
	Color_Teal                 = 0xFF008080,
	Color_Thistle              = 0xFFD8BFD8,
	Color_Tomato               = 0xFFFF6347,
	Color_Turquoise            = 0xFF40E0D0,
	Color_Violet               = 0xFFEE82EE,
	Color_Wheat                = 0xFFF5DEB3,
	Color_White                = 0xFFFFFFFF,
	Color_WhiteSmoke           = 0xFFF5F5F5,
	Color_Yellow               = 0xFFFFFF00,
	Color_YellowGreen          = 0xFF9ACD32
};

//------------------------------------------------------------------------------
//	色クラス(Direct3D用)
//------------------------------------------------------------------------------
class Color {
public:
	inline Color() : argb(0)
	{
		value.r = 0.0f;
		value.g = 0.0f;
		value.b = 0.0f;
		value.a = 0.0f;
	}

	inline Color(const int R, const int G, const int B) : a(0xff), r(R), g(G), b(B)
	{
		const float   f = 1.0f / 255.0f;
		value.r = f * R;
		value.g = f * G;
		value.b = f * B;
		value.a = 1.0f;
	}

	inline Color(const int R, const int G, const int B, const int A) : a(A), r(R), g(G), b(B)
	{
		const float   f = 1.0f / 255.0f;
		value.r = f * R;
		value.g = f * G;
		value.b = f * B;
		value.a = f * A;
	}

	inline Color(const float R, const float G, const float B)
	{
		a = (BYTE)0xff;
		r = R >= 1.0f ? 0xff : R <= 0.0f ? 0x00 : static_cast<BYTE>(R * 255.0f + 0.5f);
		g = G >= 1.0f ? 0xff : G <= 0.0f ? 0x00 : static_cast<BYTE>(G * 255.0f + 0.5f);
		b = B >= 1.0f ? 0xff : B <= 0.0f ? 0x00 : static_cast<BYTE>(B * 255.0f + 0.5f);

		value.r = R;
		value.g = G;
		value.b = B;
		value.a = 1.0f;
	}

	inline Color(const float R, const float G, const float B, const float A)
	{
		a = A >= 1.0f ? 0xff : A <= 0.0f ? 0x00 : static_cast<BYTE>(A * 255.0f + 0.5f);
		r = R >= 1.0f ? 0xff : R <= 0.0f ? 0x00 : static_cast<BYTE>(R * 255.0f + 0.5f);
		g = G >= 1.0f ? 0xff : G <= 0.0f ? 0x00 : static_cast<BYTE>(G * 255.0f + 0.5f);
		b = B >= 1.0f ? 0xff : B <= 0.0f ? 0x00 : static_cast<BYTE>(B * 255.0f + 0.5f);

		value.r = R;
		value.g = G;
		value.b = B;
		value.a = A;
	}

	inline Color(const DWORD dw)
	{
		argb = dw;

		const float   f = 1.0f / 255.0f;
		value.r = f * r;
		value.g = f * g;
		value.b = f * b;
		value.a = f * a;
	}

	inline Color(const Color_by_Name c)
	{
		argb = c;

		const float   f = 1.0f / 255.0f;
		value.r = f * r;
		value.g = f * g;
		value.b = f * b;
		value.a = f * a;
	}

	inline Color(const Color_by_Name c, const BYTE alpha)
	{
		argb = c;
		a    = alpha;

		const float   f = 1.0f / 255.0f;
		value.r = f * r;
		value.g = f * g;
		value.b = f * b;
		value.a = f * a;
	}

	inline Color(const Color_by_Name c, const float alpha)
	{
		argb = c;
		a    = alpha >= 1.0f ? 0xff : alpha <= 0.0f ? 0x00 : static_cast<BYTE>(alpha * 255.0f + 0.5f);

		const float   f = 1.0f / 255.0f;
		value.r = f * r;
		value.g = f * g;
		value.b = f * b;
		value.a = alpha;
	}

	inline Color(const Vector3& v)
	{
		a = (BYTE)0xff;
		r = v.x >= 1.0f ? 0xff : v.x <= 0.0f ? 0x00 : static_cast<BYTE>(v.x * 255.0f + 0.5f);
		g = v.y >= 1.0f ? 0xff : v.y <= 0.0f ? 0x00 : static_cast<BYTE>(v.y * 255.0f + 0.5f);
		b = v.z >= 1.0f ? 0xff : v.z <= 0.0f ? 0x00 : static_cast<BYTE>(v.z * 255.0f + 0.5f);

		value.r = v.x;
		value.g = v.y;
		value.b = v.z;
		value.a = 1.0f;
	}

	inline Color(const Vector4& v)
	{
		a = v.w >= 1.0f ? 0xff : v.w <= 0.0f ? 0x00 : static_cast<BYTE>(v.w * 255.0f + 0.5f);
		r = v.x >= 1.0f ? 0xff : v.x <= 0.0f ? 0x00 : static_cast<BYTE>(v.x * 255.0f + 0.5f);
		g = v.y >= 1.0f ? 0xff : v.y <= 0.0f ? 0x00 : static_cast<BYTE>(v.y * 255.0f + 0.5f);
		b = v.z >= 1.0f ? 0xff : v.z <= 0.0f ? 0x00 : static_cast<BYTE>(v.z * 255.0f + 0.5f);

		value.r = v.x;
		value.g = v.y;
		value.b = v.z;
		value.a = v.w;
	}

	inline void R(const int n)
	{
		r       = static_cast<BYTE>(n);
		value.r = r / 255.0f;
	}

	inline void G(const int n)
	{
		g       = static_cast<BYTE>(n);
		value.g = g / 255.0f;
	}

	inline void B(const int n)
	{
		b       = static_cast<BYTE>(n);
		value.b = n / 255.0f;
	}

	inline void A(const int n)
	{
		a       = static_cast<BYTE>(n);
		value.a = a / 255.0f;
	}

	inline void R(const float f)
	{
		value.r = f;
		r       = f >= 1.0f ? 0xff : f <= 0.0f ? 0x00 : static_cast<BYTE>(f * 255.0f + 0.5f);
	}

	inline void G(const float f)
	{
		value.g = f;
		g       = f >= 1.0f ? 0xff : f <= 0.0f ? 0x00 : static_cast<BYTE>(f * 255.0f + 0.5f);
	}

	inline void B(const float f)
	{
		value.b = f;
		b       = f >= 1.0f ? 0xff : f <= 0.0f ? 0x00 : static_cast<BYTE>(f * 255.0f + 0.5f);
	}

	inline void A(const float f)
	{
		value.a = f;
		a       = f >= 1.0f ? 0xff : f <= 0.0f ? 0x00 : static_cast<BYTE>(f * 255.0f + 0.5f);
	}

	inline operator DWORD() const
	{
		return argb;
	}

	inline operator D3DCOLORVALUE() const
	{
		return value;
	}

	inline operator D3DXCOLOR() const
	{
		return D3DXCOLOR(value);
	}

	inline operator D3DXVECTOR3()
	{
		return *((D3DXVECTOR3*)&value);
	}

	inline operator D3DXVECTOR4()
	{
		return *((D3DXVECTOR4*)&value);
	}

	inline Color& operator =(const DWORD dw)
	{
		argb = dw;

		const float   f = 1.0f / 255.0f;
		value.r = f * r;
		value.g = f * g;
		value.b = f * b;
		value.a = f * a;

		return *this;
	}

	inline Color& operator =(const Color_by_Name c)
	{
		argb = c;

		const float   f = 1.0f / 255.0f;
		value.r = f * r;
		value.g = f * g;
		value.b = f * b;
		value.a = f * a;

		return *this;
	}

	inline Color& operator =(const D3DXCOLOR& c)
	{
		value = c;

		a = c.a >= 1.0f ? 0xff : c.a <= 0.0f ? 0x00 : static_cast<BYTE>(c.a * 255.0f + 0.5f);
		r = c.r >= 1.0f ? 0xff : c.r <= 0.0f ? 0x00 : static_cast<BYTE>(c.r * 255.0f + 0.5f);
		g = c.g >= 1.0f ? 0xff : c.g <= 0.0f ? 0x00 : static_cast<BYTE>(c.g * 255.0f + 0.5f);
		b = c.b >= 1.0f ? 0xff : c.b <= 0.0f ? 0x00 : static_cast<BYTE>(c.b * 255.0f + 0.5f);

		return *this;
	}

	inline Color operator +(const D3DXCOLOR& c)
	{
		value.a += c.a;
		value.r += c.r;
		value.g += c.g;
		value.b += c.b;

		a = value.a >= 1.0f ? 0xff : value.a <= 0.0f ? 0x00 : static_cast<BYTE>(value.a * 255.0f + 0.5f);
		r = value.r >= 1.0f ? 0xff : value.r <= 0.0f ? 0x00 : static_cast<BYTE>(value.r * 255.0f + 0.5f);
		g = value.g >= 1.0f ? 0xff : value.g <= 0.0f ? 0x00 : static_cast<BYTE>(value.g * 255.0f + 0.5f);
		b = value.b >= 1.0f ? 0xff : value.b <= 0.0f ? 0x00 : static_cast<BYTE>(value.b * 255.0f + 0.5f);

		return *this;
	}

	inline Color operator -(const D3DXCOLOR& c)
	{
		value.a -= c.a;
		value.r -= c.r;
		value.g -= c.g;
		value.b -= c.b;

		a = value.a >= 1.0f ? 0xff : value.a <= 0.0f ? 0x00 : static_cast<BYTE>(value.a * 255.0f + 0.5f);
		r = value.r >= 1.0f ? 0xff : value.r <= 0.0f ? 0x00 : static_cast<BYTE>(value.r * 255.0f + 0.5f);
		g = value.g >= 1.0f ? 0xff : value.g <= 0.0f ? 0x00 : static_cast<BYTE>(value.g * 255.0f + 0.5f);
		b = value.b >= 1.0f ? 0xff : value.b <= 0.0f ? 0x00 : static_cast<BYTE>(value.b * 255.0f + 0.5f);

		return *this;
	}

	inline Color operator *(const D3DXCOLOR& c)
	{
		value.a *= c.a;
		value.r *= c.r;
		value.g *= c.g;
		value.b *= c.b;

		a = value.a >= 1.0f ? 0xff : value.a <= 0.0f ? 0x00 : static_cast<BYTE>(value.a * 255.0f + 0.5f);
		r = value.r >= 1.0f ? 0xff : value.r <= 0.0f ? 0x00 : static_cast<BYTE>(value.r * 255.0f + 0.5f);
		g = value.g >= 1.0f ? 0xff : value.g <= 0.0f ? 0x00 : static_cast<BYTE>(value.g * 255.0f + 0.5f);
		b = value.b >= 1.0f ? 0xff : value.b <= 0.0f ? 0x00 : static_cast<BYTE>(value.b * 255.0f + 0.5f);

		return *this;
	}

	inline BOOL operator ==(const DWORD dw) const
	{
		return argb == dw;
	}

	inline BOOL operator !=(const DWORD dw) const
	{
		return argb != dw;
	}

	inline BOOL operator ==(const Color_by_Name c) const
	{
		return (0xff000000 | argb) == static_cast<D3DCOLOR>(c);
	}

	inline BOOL operator !=(const Color_by_Name c) const
	{
		return (0xff000000 | argb) != static_cast<D3DCOLOR>(c);
	}

	inline BOOL operator ==(const D3DXCOLOR& c) const
	{
		return value.r == c.r && value.g == c.g && value.b == c.b && value.a == c.a;
	}

	inline BOOL operator !=(const D3DXCOLOR& c) const
	{
		return value.r != c.r || value.g != c.g || value.b != c.b || value.a != c.a;
	}

	inline Color Negative()
	{
		value.r = 1.0f - value.r;
		value.g = 1.0f - value.g;
		value.b = 1.0f - value.b;

		r = value.r >= 1.0f ? 0xff : value.r <= 0.0f ? 0x00 : static_cast<BYTE>(value.r * 255.0f + 0.5f);
		g = value.g >= 1.0f ? 0xff : value.g <= 0.0f ? 0x00 : static_cast<BYTE>(value.g * 255.0f + 0.5f);
		b = value.b >= 1.0f ? 0xff : value.b <= 0.0f ? 0x00 : static_cast<BYTE>(value.b * 255.0f + 0.5f);

		return *this;
	}

	inline COLORREF ToCOLORREF()
	{
		return RGB(r, g, b);
	}

private:
	union {
		D3DCOLOR   argb;
		struct {
			BYTE   b;
			BYTE   g;
			BYTE   r;
			BYTE   a;
		};
	};

	D3DCOLORVALUE   value;
};

//------------------------------------------------------------------------------
//	境界球構造体
//------------------------------------------------------------------------------
struct BoundingSphere : public DXGBOUNDINGSPHERE {
	BoundingSphere() : DXGBOUNDINGSPHERE() {}
	BoundingSphere(const D3DXVECTOR3& center, const float radius)
		: DXGBOUNDINGSPHERE(center, radius) {}
	BoundingSphere(const DXGBOUNDINGSPHERE& shpere)
		: DXGBOUNDINGSPHERE(shpere.Center, shpere.Radius) {}

	inline BoundingSphere& operator =(const DXGBOUNDINGSPHERE& shpere)
	{
		Center = shpere.Center;
		Radius = shpere.Radius;
		return *this;
	}

	inline BOOL Intersects(const Vector3& pos, const float radius)
	{
		const float R = Radius + radius;
        if(Vector3_LengthSquared(pos - Center) > R * R)
			return FALSE;

		return TRUE;
	}

	inline BOOL Intersects(const DXGBOUNDINGSPHERE& shpere)
	{ return Intersects(shpere.Center, shpere.Radius); }
};

//------------------------------------------------------------------------------
//	OBB構造体(3D)
//------------------------------------------------------------------------------
struct OrientedBoundingBox : public DXGOBB {
	OrientedBoundingBox() : DXGOBB() {}
	OrientedBoundingBox(const DXGOBB& obb) : DXGOBB(obb) {}

	inline void SetAxis(const D3DXVECTOR3& angle)
	{
		D3DXMATRIX   rot;
		::D3DXMatrixRotationYawPitchRoll(&rot, D3DXToRadian(angle.y),
											   D3DXToRadian(angle.x),
											   D3DXToRadian(angle.z));
		Axis[0] = *(D3DXVECTOR3*)rot.m[0];
		Axis[1] = *(D3DXVECTOR3*)rot.m[1];
		Axis[2] = *(D3DXVECTOR3*)rot.m[2];
	}

	inline void SetAxis(const D3DXQUATERNION& qt)
	{
		D3DXMATRIX   rot;
		::D3DXMatrixRotationQuaternion(&rot, &qt);
		Axis[0] = *(D3DXVECTOR3*)rot.m[0];
		Axis[1] = *(D3DXVECTOR3*)rot.m[1];
		Axis[2] = *(D3DXVECTOR3*)rot.m[2];
	}

	inline void SetAxis(const D3DXMATRIX& rot)
	{
		Axis[0] = *(D3DXVECTOR3*)rot.m[0];
		Axis[1] = *(D3DXVECTOR3*)rot.m[1];
		Axis[2] = *(D3DXVECTOR3*)rot.m[2];
	}

	BOOL Intersects(DXGOBB& obb)
	{
		const float   EPSILON = 1.175494e-37F;

		float   R[3][3], AbsR[3][3];
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				R   [i][j] = ::D3DXVec3Dot(&Axis[i], &obb.Axis[j]);
				AbsR[i][j] = ::fabsf(R[i][j]) + EPSILON;
			}
		}

		D3DXVECTOR3   t = obb.Center - Center;
		t = D3DXVECTOR3(::D3DXVec3Dot(&t, &Axis[0]), ::D3DXVec3Dot(&t, &Axis[1]), ::D3DXVec3Dot(&t, &Axis[2]));

		// 軸L=A0, L=A1, L=A2判定
		float   ra, rb;
		for(int i = 0; i < 3; i++) {
			ra =     Radius[i];
			rb = obb.Radius[0] * AbsR[i][0] + obb.Radius[1] * AbsR[i][1] + obb.Radius[2] * AbsR[i][2];
			if(::fabsf(t[i]) > ra + rb)
				return FALSE;
		}

		// 軸L=B0, L=B1, L=B2判定
		for(int i = 0; i < 3; i++)
		{
			ra =     Radius[0] * AbsR[0][i] + Radius[1] * AbsR[1][i] + Radius[2] * AbsR[2][i];
			rb = obb.Radius[i];
			if(::fabsf(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)
				return FALSE;
		}

		// 軸L = A0 x B0判定
		ra =     Radius[1] * AbsR[2][0] +     Radius[2] * AbsR[1][0];
		rb = obb.Radius[1] * AbsR[0][2] + obb.Radius[2] * AbsR[0][1];
		if(::fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)
			return FALSE;

		// 軸L = A0 x B1判定
		ra =     Radius[1] * AbsR[2][1] +     Radius[2] * AbsR[1][1];
		rb = obb.Radius[0] * AbsR[0][2] + obb.Radius[2] * AbsR[0][0];
		if(::fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)
			return FALSE;

		//軸L = A0 x B2判定
		ra =     Radius[1] * AbsR[2][2] +     Radius[2] * AbsR[1][2];
		rb = obb.Radius[0] * AbsR[0][1] + obb.Radius[1] * AbsR[0][0];
		if(::fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)
			return FALSE;

		// 軸L = A1 x B0判定
		ra =     Radius[0] * AbsR[2][0] +     Radius[2] * AbsR[0][0];
		rb = obb.Radius[1] * AbsR[1][2] + obb.Radius[2] * AbsR[1][1];
		if(::fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)
			return FALSE;

		// 軸L = A1 x B1判定
		ra =     Radius[0] * AbsR[2][1] +     Radius[2] * AbsR[0][1];
		rb = obb.Radius[0] * AbsR[1][2] + obb.Radius[2] * AbsR[1][0];
		if(::fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)
			return FALSE;

		// 軸L = A1 x B2判定
		ra =     Radius[0] * AbsR[2][2] +     Radius[2] * AbsR[0][2];
		rb = obb.Radius[0] * AbsR[1][1] + obb.Radius[1] * AbsR[1][0];
		if(::fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)
			return FALSE;

		// 軸L = A2 x B0判定
		ra =     Radius[0] * AbsR[1][0] +     Radius[1] * AbsR[0][0];
		rb = obb.Radius[1] * AbsR[2][2] + obb.Radius[2] * AbsR[2][1];
		if(::fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)
			return FALSE;

		// 軸L = A2 x B1判定
		ra =     Radius[0] * AbsR[1][1] +     Radius[1] * AbsR[0][1];
		rb = obb.Radius[0] * AbsR[2][2] + obb.Radius[2] * AbsR[2][0];
		if(::fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)
			return FALSE;

		// 軸L = A2 x B2判定
		ra =     Radius[0] * AbsR[1][2] +     Radius[1] * AbsR[0][2];
		rb = obb.Radius[0] * AbsR[2][1] + obb.Radius[1] * AbsR[2][0];
		if(::fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)
			return FALSE;

		return TRUE;
	}

	BOOL Intersects(const DXGBOUNDINGSPHERE& shpere)
	{
		// 最接点を求める
		D3DXVECTOR3   pt = Center;
		D3DXVECTOR3   d  = shpere.Center - Center;
		float         dist;
		for(int i = 0; i < 3; i++) {
			dist = ::D3DXVec3Dot(&d, &Axis[i]);

			if(dist >  Radius[i])
				dist =  Radius[i];
			if(dist < -Radius[i])
				dist = -Radius[i];

			pt += dist * Axis[i];
		}

		// 球と衝突しているか調べる
		D3DXVECTOR3   v = pt - shpere.Center;
		if(::D3DXVec3Dot(&v, &v) > shpere.Radius * shpere.Radius)
			return FALSE;

		return TRUE;
	}
};

//------------------------------------------------------------------------------
//	OBB構造体(2D)
//------------------------------------------------------------------------------
struct OrientedBoundingBox2D {
	D3DXVECTOR2   Center;
	D3DXVECTOR2   Axis[2];
	D3DXVECTOR2   Radius;

	OrientedBoundingBox2D()
		: Center(0.0f, 0.0f), Radius(0.0f, 0.0f)
	{
		Axis[0] = D3DXVECTOR2(1.0f, 0.0f);
		Axis[1] = D3DXVECTOR2(0.0f, 1.0f);
	}

	OrientedBoundingBox2D(const OrientedBoundingBox2D& obb)
		: Center(obb.Center), Radius(obb.Radius)
	{
		Axis[0] = obb.Axis[0];
		Axis[1] = obb.Axis[1];
	}

	inline OrientedBoundingBox2D& operator =(const OrientedBoundingBox2D& obb)
	{
		Center  = obb.Center;

		Axis[0] = obb.Axis[0];
		Axis[1] = obb.Axis[1];

		Radius  = obb.Radius;

		return *this;
	}

	inline void SetAxis(float angle)
	{
		const float ANGLE = D3DXToRadian(angle);
		const float SIN   = ::sinf(ANGLE);
		const float COS   = ::cosf(ANGLE);
		Axis[0].x = COS;
		Axis[0].y = SIN;
		Axis[1].x =-SIN;
		Axis[1].y = COS;
	}

	BOOL Intersects(OrientedBoundingBox2D& obb)
	{
		// 方向ベクトル
		D3DXVECTOR2   ax = Axis[0] * Radius.x;
		D3DXVECTOR2   ay = Axis[1] * Radius.y;
		D3DXVECTOR2   bx = obb.Axis[0] * obb.Radius.x;
		D3DXVECTOR2   by = obb.Axis[1] * obb.Radius.y;

		// 距離
		D3DXVECTOR2   dis = Center - obb.Center;

		float   ra, rb, l;

		// 分離軸(自x軸)
		ra = ::D3DXVec2Length(&ax);
		rb = SeparateAxisProjectionLength(Axis[0], bx, by);
		l  = ::fabs(D3DXVec2Dot(&dis, &Axis[0]));
		if(l > ra + rb)
			return FALSE;

		// 分離軸(自y軸)
		ra = ::D3DXVec2Length(&ay);
		rb = SeparateAxisProjectionLength(Axis[1], bx, by);
		l  = ::fabs(D3DXVec2Dot(&dis, &Axis[1]));
		if(l > ra + rb)
			return FALSE;

		// 分離軸(相手x軸)
		ra = ::D3DXVec2Length(&bx);
		rb = SeparateAxisProjectionLength(obb.Axis[0], ax, ay);
		l  = ::fabs(D3DXVec2Dot(&dis, &obb.Axis[0]));
		if(l > ra + rb)
			return FALSE;


		// 分離軸(相手y軸)
		ra = ::D3DXVec2Length(&bx);
		rb = SeparateAxisProjectionLength(obb.Axis[1], ax, ay);
		l  = ::fabs(D3DXVec2Dot(&dis, &obb.Axis[1]));
		if(l > ra + rb)
			return FALSE;

		return TRUE;
	}

	BOOL Intersects(const D3DXVECTOR2& inCenter, const float inRadius)
	{
		// 最接点を求める
		D3DXVECTOR2   pt = Center;
		D3DXVECTOR2   d  = inCenter - Center;
		float         dist;

		dist = ::D3DXVec2Dot(&d, &Axis[0]);
		if(dist >  Radius[0])
			dist =  Radius[0];
		if(dist < -Radius[0])
			dist = -Radius[0];
		pt += dist * Axis[0];

		dist = ::D3DXVec2Dot(&d, &Axis[1]);
		if(dist >  Radius[1])
			dist =  Radius[1];
		if(dist < -Radius[1])
			dist = -Radius[1];
		pt += dist * Axis[1];

		// 円と衝突しているか調べる
		D3DXVECTOR2   v = pt - inCenter;
		if(::D3DXVec2Dot(&v, &v) > inRadius * inRadius)
			return FALSE;

		return TRUE;
	}

private:
	// 分離軸投影線分長
	float SeparateAxisProjectionLength(D3DXVECTOR2& sep, D3DXVECTOR2& v1, D3DXVECTOR2& v2)
	{
		const float   l1 = ::fabs(::D3DXVec2Dot(&sep, &v1));
		const float   l2 = ::fabs(::D3DXVec2Dot(&sep, &v2));

		return l1 + l2;
	}
};

//------------------------------------------------------------------------------
//	OBB別名
//------------------------------------------------------------------------------
typedef OrientedBoundingBox   OBB3D;
typedef OrientedBoundingBox2D OBB2D;

//------------------------------------------------------------------------------
//	レイ構造体
//------------------------------------------------------------------------------
struct Ray : public DXGRAY {
	Ray() : DXGRAY() {}
	Ray(const Vector3& pos, const Vector3& dir) : DXGRAY(pos, dir) {}
	Ray(const DXGRAY& ray) : DXGRAY(ray.Position, ray.Direction) {}

	inline Ray& operator =(const DXGRAY& ray)
	{
		Position  = ray.Position;
		Direction = ray.Direction;
		return *this;
	}

	inline operator DXGRAY()
	{
		return *((DXGRAY*)&(*this));
	}

	inline BOOL Intersects(const Vector3& p0, const Vector3& p1, const Vector3& p2, float* pDist = NULL)
	{
		return ::D3DXIntersectTri(&p0, &p1, &p2, &Position, &Direction, NULL, NULL, pDist);
	}

	inline BOOL Intersects(const Vector3 p[3], float* pDist = NULL)
	{
		return ::D3DXIntersectTri(&p[0], &p[1], &p[2], &Position, &Direction, NULL, NULL, pDist);
	}

	inline BOOL Intersects(const Vector3& center, const float radius)
	{
		return ::D3DXSphereBoundProbe(&center, radius, &Position, &Direction);
	}

	inline BOOL Intersects(const DXGBOUNDINGSPHERE& sphere)
	{ return Intersects(sphere.Center, sphere.Radius); }

	BOOL Intersects(DXGOBB& obb)
	{
		const float   EPSILON = 1.175494e-37F;

		D3DXVECTOR3   m = (Position + Direction) * 0.5f;
		D3DXVECTOR3   d = Direction - m;

		m = m - obb.Center;
		m = D3DXVECTOR3(::D3DXVec3Dot(&obb.Axis[0], &m), ::D3DXVec3Dot(&obb.Axis[1], &m), ::D3DXVec3Dot(&obb.Axis[2], &m));
		d = D3DXVECTOR3(::D3DXVec3Dot(&obb.Axis[0], &d), ::D3DXVec3Dot(&obb.Axis[1], &d), ::D3DXVec3Dot(&obb.Axis[2], &d));

		float   adx = ::fabsf(d.x);
		if(::fabsf(m.x) > obb.Radius.x + adx) return FALSE;

		float   ady = ::fabsf(d.y);
		if(::fabsf(m.y) > obb.Radius.y + ady) return FALSE;

		float   adz = ::fabsf(d.z);
		if(::fabsf(m.z) > obb.Radius.z + adz) return FALSE;

		adx += EPSILON; 
		ady += EPSILON; 
		adz += EPSILON;
		    
		if(::fabsf(m.y * d.z - m.z * d.y) > obb.Radius.y * adz + obb.Radius.z * ady ) return FALSE;
		if(::fabsf(m.z * d.x - m.x * d.z) > obb.Radius.x * adz + obb.Radius.z * adx ) return FALSE;
		if(::fabsf(m.x * d.y - m.y * d.x) > obb.Radius.x * ady + obb.Radius.y * adx ) return FALSE;

		return TRUE;
	}
};

//------------------------------------------------------------------------------
//	視錐台構造体
//------------------------------------------------------------------------------
struct ViewFrustum : public DXGVIEWFRUSTUM {
	ViewFrustum(DXGVIEWFRUSTUM& frustum)
	{
		Left   = frustum.Left;
		Right  = frustum.Right;
		Top    = frustum.Top;
		Bottom = frustum.Bottom;
		Near   = frustum.Near;
		Far    = frustum.Far;
	}

	inline ViewFrustum& operator =(const DXGVIEWFRUSTUM& frustum)
	{
		Left   = frustum.Left;
		Right  = frustum.Right;
		Top    = frustum.Top;
		Bottom = frustum.Bottom;
		Near   = frustum.Near;
		Far    = frustum.Far;
		return *this;
	}

	inline operator DXGVIEWFRUSTUM()
	{
		return *((DXGVIEWFRUSTUM*)&(*this));
	}

	// 視錐台と境界球
	inline BOOL Intersects(const Vector3& center, const float radius)
	{
		if(Left  .a * center.x + Left  .b * center.y + Left  .c * center.z + Left  .d > radius)
			return FALSE;
		if(Right .a * center.x + Right .b * center.y + Right .c * center.z + Right .d > radius)
			return FALSE;
		if(Top   .a * center.x + Top   .b * center.y + Top   .c * center.z + Top   .d > radius)
			return FALSE;
		if(Bottom.a * center.x + Bottom.b * center.y + Bottom.c * center.z + Bottom.d > radius)
			return FALSE;
		if(Near  .a * center.x + Near  .b * center.y + Near  .c * center.z + Near  .d > radius)
			return FALSE;
		if(Far   .a * center.x + Far   .b * center.y + Far   .c * center.z + Far   .d > radius)
			return FALSE;

		return TRUE;
/*
		// 内積版
		if(::D3DXPlaneDotCoord(&Left  , &center) > radius)
			return FALSE;
		if(::D3DXPlaneDotCoord(&Right , &center) > radius)
			return FALSE;
		if(::D3DXPlaneDotCoord(&Top   , &center) > radius)
			return FALSE;
		if(::D3DXPlaneDotCoord(&Bottom, &center) > radius)
			return FALSE;
		if(::D3DXPlaneDotCoord(&Near  , &center) > radius)
			return FALSE;
		if(::D3DXPlaneDotCoord(&Far   , &center) > radius)
			return FALSE;

		return TRUE;
*/
	}

	inline BOOL Intersects(const DXGBOUNDINGSPHERE& sphere)
	{ return  Intersects(sphere.Center, sphere.Radius); }

	inline BOOL IsCulling(const Vector3& center, const float radius)
	{ return !Intersects(center, radius); }

	inline BOOL IsCulling(const DXGBOUNDINGSPHERE& sphere)
	{ return !Intersects(sphere.Center, sphere.Radius); }

	// 視錐台と点
	inline BOOL Intersects(const Vector3& pos)
	{ return  Intersects(pos, 0.0f); }
};

//------------------------------------------------------------------------------
//	頂点バッファ構造体
//------------------------------------------------------------------------------
typedef DXGTLVERTEX VertexTransformPositionColorTexture;
typedef DXGLVERTEX  VertexPositionColorTexture;
typedef DXGVERTEX   VertexPositionNormalTexture;
typedef DXGTLVERTEX ScreenVertex;

//------------------------------------------------------------------------------
//	プリミティブタイプ
//------------------------------------------------------------------------------
#define PrimitiveType_PointList       D3DPT_POINTLIST
#define PrimitiveType_LineList        D3DPT_LINELIST
#define PrimitiveType_LineStrip       D3DPT_LINESTRIP
#define PrimitiveType_TriangleList    D3DPT_TRIANGLELIST
#define PrimitiveType_TriangleStrip   D3DPT_TRIANGLESTRIP
#define PrimitiveType_TriangleFan     D3DPT_TRIANGLEFAN

//------------------------------------------------------------------------------
//	シンプルシェイプ形状
//------------------------------------------------------------------------------
#define Shape_Box                     DXGSHAPE_BOX
#define Shape_Cylinder                DXGSHAPE_CYLINDER
#define Shape_Polygon                 DXGSHAPE_POLYGON
#define Shape_Sphere                  DXGSHAPE_SPHERE
#define Shape_Torus                   DXGSHAPE_TORUS
#define Shape_Teapot                  DXGSHAPE_TEAPOT

//------------------------------------------------------------------------------
//	シンプルシェイプ構造体
//------------------------------------------------------------------------------
typedef DXGSIMPLESHAPE SimpleShape;

//------------------------------------------------------------------------------
//	アルファブレンド描画モード
//------------------------------------------------------------------------------
#define DrawAlpha_Face                DXGDRAWA_FACE
#define DrawAlpha_BackFace            DXGDRAWA_BACKFACE
#define DrawAlpha_CullNone            DXGDRAWA_CULLNONE
#define DrawAlpha_BothFaces           DXGDRAWA_BOTHFACES

#define DrawAlpha_Face2               DXGDRAWA_FACE2
#define DrawAlpha_BackFace2           DXGDRAWA_BACKFACE2
#define DrawAlpha_CullNone2           DXGDRAWA_CULLNONE2
#define DrawAlpha_BothFaces2          DXGDRAWA_BOTHFACES2

#define DrawAlpha_Face3               DXGDRAWA_FACE3
#define DrawAlpha_BackFace3           DXGDRAWA_BACKFACE3
#define DrawAlpha_CullNone3           DXGDRAWA_CULLNONE3
#define DrawAlpha_BothFaces3          DXGDRAWA_BOTHFACES3

#define DrawAlpha_AllFaces            DXGDRAWA_ALLFACES

//------------------------------------------------------------------------------
//	法線計算
//------------------------------------------------------------------------------
#define Compute_DoNot_InPlace         DXGCOMPUTE_DONOT_INPLACE
#define Compute_None                  DXGCOMPUTE_NONE
#define Compute_Normal                DXGCOMPUTE_NORMAL
#define Compute_Tangent               DXGCOMPUTE_TANGENT
#define Compute_Binormal              DXGCOMPUTE_BINORMAL
#define Compute_NormalTangent         DXGCOMPUTE_NORMALTANGENT
#define Compute_NormalBinormal        DXGCOMPUTE_NORMALBINORMAL
#define Compute_TangentBinormal       DXGCOMPUTE_TANGENTBINORMAL
#define Compute_NormalTangentBinormal DXGCOMPUTE_NORMALTANGENTBINORMAL
#define Compute_All                   DXGCOMPUTE_ALL

//------------------------------------------------------------------------------
//	スキニング方法
//------------------------------------------------------------------------------
#define SkinningMethod_NonIndexed     DXGSKMTD_NONINDEXED
#define SkinningMethod_Indexed        DXGSKMTD_INDEXED
#define SkinningMethod_IndexedHLSL    DXGSKMTD_INDEXEDHLSL
#define SkinningMethod_Software       DXGSKMTD_SOFTWARE
#define SkinningMethod_Default        DXGSKMTD_DEFAULT

//------------------------------------------------------------------------------
//	アニメーションループモード方法
//------------------------------------------------------------------------------
#define AnimationLoopMode_Loop        D3DXPLAY_LOOP
#define AnimationLoopMode_Once        D3DXPLAY_ONCE
#define AnimationLoopMode_PingPong    D3DXPLAY_PINGPONG

//------------------------------------------------------------------------------
//	アニメーション切り換え補間方法
//------------------------------------------------------------------------------
#define TrasitionType_Liner           D3DXTRANSITION_LINEAR
#define TrasitionType_EaseInEaseOut   D3DXTRANSITION_EASEINEASEOUT

//------------------------------------------------------------------------------
//	点構造体
//------------------------------------------------------------------------------
struct Point : public POINT {
	Point()
	{ 
		x = 0; 
		y = 0;
	}

	Point(const int X, const int Y)
	{
		x = X;
		y = Y;
	}

	Point(const POINT& p)
	{
		x = p.x;
		y = p.y;
	}

	inline Point& operator =(const Point& p)
	{
		x = p.x;
		y = p.y;
		return *this;
	}
};

//------------------------------------------------------------------------------
//	矩形構造体
//------------------------------------------------------------------------------
struct Rect : public RECT {
	Rect()
	{
		left   = 0;
		top    = 0;
		right  = 0;
		bottom = 0;
	}

	Rect(const int l, const int t, const int r, const int b)
	{
		left   = l;
		top    = t;
		right  = r;
		bottom = b;
	}

	Rect(const RECT& rect)
	{
		left   = rect.left;
		top    = rect.top;
		right  = rect.right;
		bottom = rect.bottom;
	}

	inline Rect& operator =(const RECT& rect)
	{
		left   = rect.left;
		top    = rect.top;
		right  = rect.right;
		bottom = rect.bottom;
		return *this;
	}

	inline bool Intersect(const RECT& rect)
	{
		if(right < rect.left   || left   > rect.right ||
		   top   > rect.bottom || bottom < rect.top)
			return false;			
		return true;
	}

	inline bool Intersect(const POINT& pt)
	{
		if(left > pt.x || top > pt.y || right < pt.x || bottom < pt.y)
			return false;
		return true;
	}
};

struct RectWH : public Rect {
	RectWH() : Rect()
	{
	}

	RectWH(const int l, const int t, const int w, const int h)
	{
		left   = l;
		top    = t;
		right  = left + w;
		bottom = top  + h;
	}
};

//------------------------------------------------------------------------------
//	ピクセルフォーマット
//------------------------------------------------------------------------------
#define PixelFormat_Unknown             D3DFMT_UNKNOWN

#define PixelFormat_RGBX8888            D3DFMT_X8R8G8B8
#define PixelFormat_RGBA8888            D3DFMT_A8R8G8B8
#define PixelFormat_RGBA1010102         D3DFMT_A2R10G10B10
#define PixelFormat_RGBA64              D3DFMT_A16B16G16R16

#define PixelFormat_RGB565              D3DFMT_R5G6B5
#define PixelFormat_RGBX5551            D3DFMT_R5G5B5X1
#define PixelFormat_RGBA5551            D3DFMT_A1R5G5B5
#define PixelFormat_RGBX4444            D3DFMT_X4R4G4B4
#define PixelFormat_RGBA4444            D3DFMT_A4R4G4B4
#define PixelFormat_Alpha8              D3DFMT_A8

#define PixelFormat_HDRBlendable        D3DFMT_A16B16G16R16F
#define PixelFormat_HalfSingle          D3DFMT_R16F
#define PixelFormat_HalfVector2         D3DFMT_G16R16F
#define PixelFormat_HalfVector4         D3DFMT_A16B16G16R16F
#define PixelFormat_Single              D3DFMT_R32F
#define PixelFormat_Vector2             D3DFMT_G32R32F
#define PixelFormat_Vector4             D3DFMT_A32B32G32R32F

#define PixelFormat_NormalizedByte2     D3DFMT_V8U8
#define PixelFormat_NormalizedByte4     D3DFMT_Q8W8V8U8

#define PixelFormat_DXT1                D3DFMT_DXT1
#define PixelFormat_DXT3                D3DFMT_DXT3
#define PixelFormat_DXT5                D3DFMT_DXT5

#define DepthFormat_Unknown               D3DFMT_UNKNOWN
#define DepthFormat_Depth16               D3DFMT_D16
#define DepthFormat_Depth24               D3DFMT_D24X8
#define DepthFormat_Depth32               D3DFMT_D32
#define DepthFormat_Depth15Stencil1       D3DFMT_D15S1
#define DepthFormat_Depth24Stencil8       D3DFMT_D24S8
#define DepthFormat_Depth24Stencil8Single D3DFMT_D24FS8

//------------------------------------------------------------------------------
//	スプライトブレンドモード
//------------------------------------------------------------------------------
#define SpriteBlendMode_None              0
#define SpriteBlendMode_AlphaBlend        D3DTOP_MODULATE
#define SpriteBlendMode_Modulate2x        D3DTOP_MODULATE2X
#define SpriteBlendMode_Modulate4x        D3DTOP_MODULATE4X
#define SpriteBlendMode_Additive          D3DTOP_ADD
#define SpriteBlendMode_AddSigned         D3DTOP_ADDSIGNED
#define SpriteBlendMode_AddSigned2x       D3DTOP_ADDSIGNED2X
#define SpriteBlendMode_AddSmooting       D3DTOP_ADDSMOOTH
#define SpriteBlendMode_Subtractive       D3DTOP_SUBTRACT

//------------------------------------------------------------------------------
//	スプライトソートモード
//------------------------------------------------------------------------------
#define SpriteSortMode_Deferred           0
#define SpriteSortMode_FrontToBack        D3DXSPRITE_SORT_DEPTH_BACKTOFRONT
#define SpriteSortMode_BackToFront        D3DXSPRITE_SORT_DEPTH_FRONTTOBACK
#define SpriteSortMode_Sprite             D3DXSPRITE_SORT_TEXTURE

//------------------------------------------------------------------------------
//	スプライトステート
//------------------------------------------------------------------------------
#define SaveStateMode_None                D3DXSPRITE_DONOTSAVESTATE
#define SaveStateMode_SaveState           0

//------------------------------------------------------------------------------
//	スプライト描画範囲
//------------------------------------------------------------------------------
#define SpriteBatch_TopMost               DXGSPRBAT_TOPMOST
#define SpriteBatch_BottomMost            DXGSPRBAT_BOTTOMMOST

//------------------------------------------------------------------------------
//	キューブマップ面指定
//------------------------------------------------------------------------------
#define CubeMapFace_PositiveX             D3DCUBEMAP_FACE_POSITIVE_X
#define CubeMapFace_NegativeX             D3DCUBEMAP_FACE_NEGATIVE_X
#define CubeMapFace_PositiveY             D3DCUBEMAP_FACE_POSITIVE_Y
#define CubeMapFace_NegativeY             D3DCUBEMAP_FACE_NEGATIVE_Y
#define CubeMapFace_PositiveZ             D3DCUBEMAP_FACE_POSITIVE_Z
#define CubeMapFace_NegativeZ             D3DCUBEMAP_FACE_NEGATIVE_Z

//------------------------------------------------------------------------------
//	サーフェスロード時カラー変換フィルター
//------------------------------------------------------------------------------
#define LoadSurfaceFilter_Default         D3DX_DEFAULT
#define LoadSurfaceFilter_None            D3DX_FILTER_NONE
#define LoadSurfaceFilter_Point           D3DX_FILTER_POINT
#define LoadSurfaceFilter_Linear          D3DX_FILTER_LINEAR
#define LoadSurfaceFilter_Dither          D3DX_FILTER_DITHER
#define LoadSurfaceFilter_TrianglePoint  (D3DX_FILTER_POINT  | D3DX_FILTER_TRIANGLE)
#define LoadSurfaceFilter_TriangleLinear (D3DX_FILTER_LINEAR | D3DX_FILTER_TRIANGLE)
#define LoadSurfaceFilter_TriangleDither (D3DX_FILTER_DITHER | D3DX_FILTER_TRIANGLE)

//------------------------------------------------------------------------------
//	キャンバス描画スタイル
//------------------------------------------------------------------------------
enum PaintStyle {
	PaintStyle_STROKE,			// 線のみ
	PaintStyle_FILL,			// 塗りつぶし
	PaintStyle_FILL_AND_STROKE	// 線＋塗りつぶし
};

//------------------------------------------------------------------------------
//	ペイント
//------------------------------------------------------------------------------
class Paint {
public:
	Paint() : line(0), width(1), paint(0) {}

	inline void SetColor(const D3DCOLOR color)
	{
		line  = RGB(color >> 16, color >> 8, color); 
		paint = RGB(color >> 16, color >> 8, color);
	}

	inline void SetLine(const D3DCOLOR color, const int w)
	{ line  = RGB(color >> 16, color >> 8, color); width = w; }

	inline void SetLineColor(const D3DCOLOR color)
	{ line  = RGB(color >> 16, color >> 8, color); }

	inline void SetLineWidth(const int w) 
	{ width = w; }

	inline void SetPaintColor(const D3DCOLOR color)
	{ paint = RGB(color >> 16, color >> 8, color); }

	inline COLORREF GetLineColor()  const { return line;  }
	inline int      GetLineWidth()  const { return width; }
	inline COLORREF GetPaintColor() const { return paint; }

private:
	COLORREF   line;
	int        width;
	COLORREF   paint;
};

//------------------------------------------------------------------------------
//	キャンバス
//------------------------------------------------------------------------------
class Canvas {
public:
	Canvas(const HDC inDC) : hDC(inDC) {}

	// 点描画
	inline void DrawPixel(const int x, const int y, const D3DCOLOR color)
	{
		::SetPixelV(hDC, x, y, RGB(color >> 16, color >> 8, color));
	}

	// 点描画
	inline void DrawPixel(const POINT pt, const D3DCOLOR color)
	{
		::SetPixelV(hDC, pt.x, pt.y, RGB(color >> 16, color >> 8, color));
	}

	// 点描画
	void DrawPoint(const int x, const int y, const Paint& p)
	{
		// ペン生成
		HPEN      hPen = ::CreatePen(PS_SOLID, p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOld = ::SelectObject(hDC, hPen);

		::MoveToEx(hDC, x, y, NULL);
		::LineTo  (hDC, x, y);

		// ペン削除
		::SelectObject(hDC, hOld);
		::DeleteObject(hPen);
	}

	// 点描画
	inline void DrawPoint(const POINT& pt, const Paint& p)
	{
		DrawPoint(pt.x, pt.y, p);
	}

	// 線描画
	void DrawLine(const int startX, const int startY,
				  const int stopX,  const int stopY,  const Paint& p)
	{
		// ペン生成
		HPEN      hPen = ::CreatePen(PS_SOLID, p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOld = ::SelectObject(hDC, hPen);

		::MoveToEx(hDC, startX, startY, NULL);
		::LineTo  (hDC, stopX,  stopY);

		// ペン削除
		::SelectObject(hDC, hOld);
		::DeleteObject(hPen);
	}

	// パス描画
	void DrawPath(const POINT pt[], const int count, const Paint& p)
	{
		// ペン生成
		HPEN      hPen = ::CreatePen(PS_SOLID, p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOld = ::SelectObject(hDC, hPen);

		::Polyline(hDC, pt, count);

		// ペン削除
		::SelectObject(hDC, hOld);
		::DeleteObject(hPen);
	}

	// ベジェ曲線描画
	void DrawBezier(const POINT pt[], const int count, const Paint& p)
	{
		// ペン生成
		HPEN      hPen = ::CreatePen(PS_SOLID, p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOld = ::SelectObject(hDC, hPen);

		::PolyBezier(hDC, pt, count);

		// ペン削除
		::SelectObject(hDC, hOld);
		::DeleteObject(hPen);
	}

	// 四角形描画
	void DrawRect(const RECT& rect, const Paint& p,
				  const PaintStyle style = PaintStyle_FILL_AND_STROKE)
	{
		// ペン生成
		HPEN      hPen    = ::CreatePen(style != PaintStyle_FILL ? PS_INSIDEFRAME : PS_NULL,
										p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOldPen = ::SelectObject(hDC, hPen);

		if(style == PaintStyle_STROKE) {

			// ブラシ生成
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));

			::Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
		} else {
			// ブラシ生成
			HBRUSH    hBrush    = ::CreateSolidBrush(p.GetPaintColor());
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, hBrush);

			::Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
			::DeleteObject(hBrush);
		}

		// ペン削除
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
	}

	// 角丸四角形描画
	void DrawRoundRect(const RECT& rect,  const int rx, const int ry,
					   const Paint& p, const PaintStyle style = PaintStyle_FILL_AND_STROKE)
	{
		// ペン生成
		HPEN      hPen    = ::CreatePen(style != PaintStyle_FILL ? PS_INSIDEFRAME : PS_NULL,
										p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOldPen = ::SelectObject(hDC, hPen);

		if(style == PaintStyle_STROKE) {
			// ブラシ生成
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));

			::RoundRect(hDC, rect.left, rect.top, rect.right, rect.bottom, rx, ry);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
		} else {
			// ブラシ生成
			HBRUSH    hBrush    = ::CreateSolidBrush(p.GetPaintColor());
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, hBrush);

			::RoundRect(hDC, rect.left, rect.top, rect.right, rect.bottom, rx, ry);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
			::DeleteObject(hBrush);
		}

		// ペン削除
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
	}

	// 多角形描画
	void DrawPolygon(const POINT pt[], const int count, const Paint& p,
					 const PaintStyle style = PaintStyle_FILL_AND_STROKE)
	{
		// ペン生成
		HPEN      hPen    = ::CreatePen(style != PaintStyle_FILL ? PS_INSIDEFRAME : PS_NULL,
										p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOldPen = ::SelectObject(hDC, hPen);

		if(style == PaintStyle_STROKE) {
			// ブラシ生成
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));

			::Polygon(hDC, pt, count);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
		} else {
			// ブラシ生成
			HBRUSH    hBrush    = ::CreateSolidBrush(p.GetPaintColor());
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, hBrush);

			::Polygon(hDC, pt, count);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
			::DeleteObject(hBrush);
		}

		// ペン削除
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
	}

	// 円描画
	void DrawCircle(const int cx, const int cy, const int radius,
					const Paint& p, const PaintStyle style = PaintStyle_FILL_AND_STROKE)
	{
		const int   x = cx - radius;
		const int   y = cy - radius;

		// ペン生成
		HPEN      hPen    = ::CreatePen(style != PaintStyle_FILL ? PS_INSIDEFRAME : PS_NULL,
										p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOldPen = ::SelectObject(hDC, hPen);

		if(style == PaintStyle_STROKE) {
			// ブラシ生成
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));

			::Ellipse(hDC, x - 1, y - 1, x + radius * 2 + 1, y + radius * 2 + 1);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
		} else {
			// ブラシ生成
			HBRUSH    hBrush    = ::CreateSolidBrush(p.GetPaintColor());
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, hBrush);

			::Ellipse(hDC, x, y, x + radius * 2, y + radius * 2);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
			::DeleteObject(hBrush);
		}

		// ペン削除
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
	}

	// 楕円描画
	void DrawOval(const RECT& rect, const Paint& p,
				  const PaintStyle style = PaintStyle_FILL_AND_STROKE)
	{
		// ペン生成
		HPEN      hPen    = ::CreatePen(style != PaintStyle_FILL ? PS_INSIDEFRAME : PS_NULL,
										p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOldPen = ::SelectObject(hDC, hPen);

		if(style == PaintStyle_STROKE) {
			// ブラシ生成
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));

			::Ellipse(hDC, rect.left - 1, rect.top - 1, rect.right + 1, rect.bottom + 1);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
		} else {
			// ブラシ生成
			HBRUSH    hBrush    = ::CreateSolidBrush(p.GetPaintColor());
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, hBrush);

			::Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
			::DeleteObject(hBrush);
		}

		// ペン削除
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
	}

	// 弧描画
	void DrawArc(const RECT& rect,
				 float startAngle, float sweepAngle, const bool useCenter,
				 const Paint& p, const PaintStyle style = PaintStyle_FILL_AND_STROKE)
	{
		// 半径
		const float   rx  = (rect.right  - rect.left) / 2.0f;
		const float   ry  = (rect.bottom - rect.top ) / 2.0f;

		// 中心点
		const float   cx  = rect.left + rx;
		const float   cy  = rect.top  + ry;

		// 開始点
		const int     rx2 = (int)(cx + rx * cosf((startAngle * 3.141592f) / 180.0f) + 0.5f);
		const int     ry2 = (int)(cy + ry * sinf((startAngle * 3.141592f) / 180.0f) + 0.5f);

		// 終了点
		const int     rx1 = (int)(cx + rx * cosf(((startAngle + sweepAngle) * 3.141592f) / 180.0f) + 0.5f);
		const int     ry1 = (int)(cy + ry * sinf(((startAngle + sweepAngle) * 3.141592f) / 180.0f) + 0.5f);

		// ペン生成
		HPEN      hPen    = ::CreatePen(style != PaintStyle_FILL ? PS_INSIDEFRAME : PS_NULL,
										p.GetLineWidth(), p.GetLineColor());
		HGDIOBJ   hOldPen = ::SelectObject(hDC, hPen);

		if(style == PaintStyle_STROKE) {
			if(useCenter) {
				// ブラシ生成
				HGDIOBJ   hOldBrush = ::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));

				::Pie(hDC, rect.left, rect.top, rect.right, rect.bottom,
					  rx1, ry1, rx2, ry2);

				// ブラシ削除
				::SelectObject(hDC, hOldBrush);
			} else {
				if(p.GetLineWidth() <= 1)
					::MoveToEx(hDC, rx2 - 1, ry2 - 1, NULL);
				else
					::MoveToEx(hDC, rx2 + 1, ry2 + 1, NULL);
				::ArcTo(hDC, rect.left, rect.top, rect.right, rect.bottom,
						rx1, ry1, rx2, ry2);
			}
		} else {
			// 位置補正
			const int   offset = p.GetLineWidth() < 2 ? 0 : -1;

			// ブラシ生成
			HBRUSH    hBrush    = ::CreateSolidBrush(p.GetPaintColor());
			HGDIOBJ   hOldBrush = ::SelectObject(hDC, hBrush);

			if(useCenter) {
				::Pie  (hDC, rect.left + offset, rect.top + offset, rect.right, rect.bottom,
						rx1, ry1, rx2, ry2);
			} else {
				::Chord(hDC, rect.left + offset, rect.top + offset, rect.right, rect.bottom,
						rx1, ry1, rx2, ry2);
			}

			// ブラシ削除
			::SelectObject(hDC, hOldBrush);
			::DeleteObject(hBrush);
		}

		// ペン削除
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
	}

private:
	HDC   hDC;
};

//------------------------------------------------------------------------------
//	キャンバス用マクロ
//------------------------------------------------------------------------------
#define LockCanvas   GetDC
#define UnlockCanvas ReleaseDC

//------------------------------------------------------------------------------
//	キー
//------------------------------------------------------------------------------
enum Keys {
	Keys_A                  = DIK_A,
	Keys_Add                = DIK_ADD,
	Kyes_Apostrophe         = DIK_APOSTROPHE,
	Keys_Apps               = DIK_APPS,
	Keys_At                 = DIK_AT,
	Keys_B                  = DIK_B,
	Keys_Back               = DIK_BACK,
	Keys_BackSlash          = DIK_BACKSLASH,
	Keys_BrowserBack        = DIK_WEBBACK,
	Keys_BrowserFavorites   = DIK_WEBFAVORITES,
	Keys_BrowserForward     = DIK_WEBFORWARD,
	Keys_BrowserHome        = DIK_WEBHOME,
	Keys_BrowserRefresh     = DIK_WEBREFRESH,
	Keys_BrowserSearch      = DIK_WEBSEARCH,
	Keys_BrowserStop        = DIK_WEBSTOP,
	Keys_C                  = DIK_C,
	Keys_CapsLock           = DIK_CAPITAL,
	Keys_Comma              = DIK_COMMA,
	Keys_Colon              = DIK_COLON,
	Keys_CircumFlex         = DIK_CIRCUMFLEX,
	Keys_CloseBrackets      = DIK_RBRACKET,
	Keys_D                  = DIK_D,
	Keys_D0                 = DIK_0,
	Keys_D1                 = DIK_1,
	Keys_D2                 = DIK_2,
	Keys_D3                 = DIK_3,
	Keys_D4                 = DIK_4,
	Keys_D5                 = DIK_5,
	Keys_D6                 = DIK_6,
	Keys_D7                 = DIK_7,
	Keys_D8                 = DIK_8,
	Keys_D9                 = DIK_9,
	Keys_Decimal            = DIK_DECIMAL,
	Keys_Delete             = DIK_DELETE,
	Keys_Divide             = DIK_DIVIDE,
	Keys_Down               = DIK_DOWN,
	Keys_E                  = DIK_E,
	Keys_End                = DIK_END,
	Keys_Enter              = DIK_RETURN,
	Kyes_Equals             = DIK_EQUALS,
	Keys_Escape             = DIK_ESCAPE,
	Keys_F                  = DIK_F,
	Keys_F1                 = DIK_F1,
	Keys_F10                = DIK_F10,
	Keys_F11                = DIK_F11,
	Keys_F12                = DIK_F12,
	Keys_F13                = DIK_F13,
	Keys_F14                = DIK_F14,
	Keys_F15                = DIK_F15,
	Keys_F2                 = DIK_F2,
	Keys_F3                 = DIK_F3,
	Keys_F4                 = DIK_F4,
	Keys_F5                 = DIK_F5,
	Keys_F6                 = DIK_F6,
	Keys_F7                 = DIK_F7,
	Keys_F8                 = DIK_F8,
	Keys_F9                 = DIK_F9,
	Keys_G                  = DIK_G,
	Keys_Grave              = DIK_GRAVE,
	Keys_H                  = DIK_H,
	Keys_Home               = DIK_HOME,
	Keys_I                  = DIK_I,
	Keys_ImeConvert         = DIK_CONVERT,
	Keys_ImeNoConvert       = DIK_NOCONVERT,
	Keys_Insert             = DIK_INSERT,
	Keys_J                  = DIK_J,
	Keys_K                  = DIK_K,
	Keys_Kana               = DIK_KANA,
	Keys_Kanji              = DIK_KANJI,
	Keys_L                  = DIK_L,
	Keys_LaunchMail         = DIK_MAIL,
	Keys_Left               = DIK_LEFT,
	Keys_LeftAlt            = DIK_LMENU,
	Keys_LeftControl        = DIK_LCONTROL,
	Keys_LeftShift          = DIK_LSHIFT,
	Keys_LeftWindows        = DIK_LWIN,
	Keys_M                  = DIK_M,
	Keys_MediaNextTrack     = DIK_NEXTTRACK,
	Keys_MediaPlayPause     = DIK_PLAYPAUSE,
	Keys_MediaPreviousTrack = DIK_PREVTRACK,
	Keys_MediaStop          = DIK_MEDIASTOP,
	Keys_Minus              = DIK_MINUS,
	Keys_Multiply           = DIK_MULTIPLY,
	Keys_N                  = DIK_N,
	Keys_NumLock            = DIK_NUMLOCK,
	Keys_NumPad0            = DIK_NUMPAD0,
	Keys_NumPad1            = DIK_NUMPAD1,
	Keys_NumPad2            = DIK_NUMPAD2,
	Keys_NumPad3            = DIK_NUMPAD3,
	Keys_NumPad4            = DIK_NUMPAD4,
	Keys_NumPad5            = DIK_NUMPAD5,
	Keys_NumPad6            = DIK_NUMPAD6,
	Keys_NumPad7            = DIK_NUMPAD7,
	Keys_NumPad8            = DIK_NUMPAD8,
	Keys_NumPad9            = DIK_NUMPAD9,
	Keys_NumPadEnter        = DIK_NUMPADENTER,
	Keys_NumPadMinus        = DIK_NUMPADMINUS,
	Keys_NumPadPeriod       = DIK_NUMPADPERIOD,
	Keys_NumPadPlus         = DIK_NUMPADPLUS,
	Keys_NumPadSlash        = DIK_NUMPADSLASH,
	Keys_NumPadStar         = DIK_NUMPADSTAR,
	Keys_O                  = DIK_O,
	Keys_OpenBrackets       = DIK_LBRACKET,
	Keys_P                  = DIK_P,
	Keys_PageDown           = DIK_NEXT,
	Keys_PageUp             = DIK_PRIOR,
	Keys_Pause              = DIK_PAUSE,
	Keys_Period             = DIK_PERIOD,
	Keys_PrintScreen        = DIK_SYSRQ,
	Keys_Q                  = DIK_Q,
	Keys_Quotes             = DIK_PREVTRACK,
	Keys_R                  = DIK_R,
	Keys_Return             = DIK_RETURN,
	Keys_Right              = DIK_RIGHT,
	Keys_RightAlt           = DIK_RMENU,
	Keys_RightControl       = DIK_RCONTROL,
	Keys_RightShift         = DIK_RSHIFT,
	Keys_RightWindows       = DIK_RWIN,
	Keys_S                  = DIK_S,
	Keys_Scroll             = DIK_SCROLL,
	Keys_SelectMedia        = DIK_MEDIASELECT,
	Keys_Semicolon          = DIK_SEMICOLON,
	Keys_Slash              = DIK_SLASH,
	Keys_Sleep              = DIK_SLEEP,
	Keys_Space              = DIK_SPACE,
	Keys_Subtract           = DIK_SUBTRACT,
	Keys_T                  = DIK_T,
	Keys_Tab                = DIK_TAB,
	Keys_U                  = DIK_U,
	Keys_Underline          = DIK_UNDERLINE,
	Keys_Up                 = DIK_UP,
	Keys_V                  = DIK_V,
	Keys_VolumeDown         = DIK_VOLUMEDOWN,
	Keys_VolumeMute         = DIK_MUTE,
	Keys_VolumeUp           = DIK_VOLUMEUP,
	Keys_W                  = DIK_W,
	Keys_X                  = DIK_X,
	Keys_Y                  = DIK_Y,
	Keys_Yen                = DIK_YEN,
	Keys_Z                  = DIK_Z
};

//------------------------------------------------------------------------------
//	マウス
//------------------------------------------------------------------------------
#pragma region Mouse

#define Mouse_Button1     DIMOFS_BUTTON0
#define Mouse_Button2     DIMOFS_BUTTON1
#define Mouse_Button3     DIMOFS_BUTTON2
#define Mouse_Button4     DIMOFS_BUTTON3
#define Mouse_Button5     DIMOFS_BUTTON4
#define Mouse_Button6     DIMOFS_BUTTON5
#define Mouse_Button7     DIMOFS_BUTTON6
#define Mouse_Button8     DIMOFS_BUTTON7
#define Mouse_X           DIMOFS_X
#define Mouse_Y           DIMOFS_Y
#define Mouse_ScrollWheel DIMOFS_Z

#pragma endregion

//------------------------------------------------------------------------------
//	ゲームパッド
//------------------------------------------------------------------------------
#pragma region GamePad

#define GamePad_Button1  DIJOFS_BUTTON0
#define GamePad_Button2  DIJOFS_BUTTON1
#define GamePad_Button3  DIJOFS_BUTTON2
#define GamePad_Button4  DIJOFS_BUTTON3
#define GamePad_Button5  DIJOFS_BUTTON4
#define GamePad_Button6  DIJOFS_BUTTON5
#define GamePad_Button7  DIJOFS_BUTTON6
#define GamePad_Button8  DIJOFS_BUTTON7
#define GamePad_Button9  DIJOFS_BUTTON8
#define GamePad_Button10 DIJOFS_BUTTON9
#define GamePad_Button11 DIJOFS_BUTTON10
#define GamePad_Button12 DIJOFS_BUTTON11
#define GamePad_Button13 DIJOFS_BUTTON12
#define GamePad_Button14 DIJOFS_BUTTON13
#define GamePad_Button15 DIJOFS_BUTTON14
#define GamePad_Button16 DIJOFS_BUTTON15
#define GamePad_Button17 DIJOFS_BUTTON16
#define GamePad_Button18 DIJOFS_BUTTON17
#define GamePad_Button19 DIJOFS_BUTTON18
#define GamePad_Button20 DIJOFS_BUTTON19
#define GamePad_Button21 DIJOFS_BUTTON21
#define GamePad_Button22 DIJOFS_BUTTON22
#define GamePad_Button23 DIJOFS_BUTTON23
#define GamePad_Button24 DIJOFS_BUTTON24
#define GamePad_Button25 DIJOFS_BUTTON25
#define GamePad_Button26 DIJOFS_BUTTON26
#define GamePad_Button27 DIJOFS_BUTTON27
#define GamePad_Button28 DIJOFS_BUTTON28
#define GamePad_Button29 DIJOFS_BUTTON29
#define GamePad_Button30 DIJOFS_BUTTON30
#define GamePad_Button31 DIJOFS_BUTTON31
#define GamePad_Button32 DIJOFS_BUTTON32
#define GamePad_POV1     DIJOFS_POV(0)
#define GamePad_POV2     DIJOFS_POV(1)
#define GamePad_POV3     DIJOFS_POV(2)
#define GamePad_POV4     DIJOFS_POV(3)
#define GamePad_X        DIJOFS_X
#define GamePad_Y        DIJOFS_Y
#define GamePad_X2       DIJOFS_Z
#define GamePad_Y2       DIJOFS_RX
#define GamePad_X3       DIJOFS_RY
#define GamePad_Y3       DIJOFS_RZ
#define GamePad_Slider1  DIJOFS_SLIDER(0)
#define GamePad_Slider2  DIJOFS_SLIDER(1)

#pragma endregion

//------------------------------------------------------------------------------
//	キー状態
//------------------------------------------------------------------------------
enum KeyState {
	KeyState_Released = 0x00,
	KeyState_Pressed  = 0x80
};

//------------------------------------------------------------------------------
//	ボタン状態
//------------------------------------------------------------------------------
enum ButtonState {
	ButtonState_Released = 0x00,
	ButtonState_Pressed  = 0x80
};

//------------------------------------------------------------------------------
//	軸範囲
//------------------------------------------------------------------------------
enum AxisRange {
	Axis_Min    = DIDEV_AXIS_MIN,	// ゲームパッド範囲
	Axis_Max    = DIDEV_AXIS_MAX,
	Axis_Center = DIDEV_AXIS_CENTER
};

//------------------------------------------------------------------------------
//	キーボード状態クラス
//------------------------------------------------------------------------------
class KeyboardState {
public:
	KeyboardState() { ::ZeroMemory(m_KeyboardState, 256); }
	KeyboardState(IInputDevice& rhs) { rhs.GetState(m_KeyboardState); }

	inline BOOL IsKeyDown(const Keys inKey) { return  (m_KeyboardState[inKey] & 0x80); }
	inline BOOL IsKeyUp  (const Keys inKey) { return !(m_KeyboardState[inKey] & 0x80); }

	inline BOOL IsKeyDown(const UINT inKey) { return  (m_KeyboardState[inKey] & 0x80); }
	inline BOOL IsKeyUp  (const UINT inKey) { return !(m_KeyboardState[inKey] & 0x80); }

	inline KeyboardState& operator =(IInputDevice& rhs)
	{
		rhs.GetState(m_KeyboardState);
		return *this;
	}

	inline KeyboardState& operator =(const KeyboardState& rhs)
	{
		::CopyMemory(m_KeyboardState, rhs.m_KeyboardState, 256);
		return *this;
	}
/*
	inline operator LPVOID()
	{
		return m_KeyboardState;
	}

	inline BYTE* operator &()
	{
		return static_cast<BYTE*>(m_KeyboardState);
	}
*/
	inline BYTE  operator [](UINT idx) const
	{
		return (m_KeyboardState[idx] & 0x80);
	}

private:
	BYTE   m_KeyboardState[256];
};

//------------------------------------------------------------------------------
//	マウス状態
//------------------------------------------------------------------------------
struct MouseState {
public:
	MouseState() { ::ZeroMemory(&State, sizeof(State)); }
	MouseState(IInputDevice& rhs)
	{ 
		rhs.GetState(&State);
		DWORD*   buttons = (DWORD*)Buttons;
		*(buttons + 0) &= 0x80808080;
		*(buttons + 1) &= 0x80808080;
//		for(int i = 0; i < 8; i++)
//			Buttons[i] &= 0x80;
	}

	POINT PointerPosition()
	{
		POINT   pt;
		::GetCursorPos(&pt);
		::ScreenToClient(DInput().GetHWnd(), &pt);
		return pt;
	}

	inline MouseState& operator =(IInputDevice& rhs)
	{
		rhs.GetState(&State);
		return *this;
	}

	inline MouseState& operator =(MouseState& rhs)
	{
		::CopyMemory(&State, &rhs.State, sizeof(State));
		return *this;
	}

	inline operator DIMOUSESTATE2()
	{
		return State;
	}

	inline operator POINT()
	{
		return PointerPosition();
	}

	union {
		DIMOUSESTATE2   State;
		struct {
			LONG X;
			LONG Y;
			LONG ScrollWheelValue;
			union {
				BYTE Buttons[8];
				struct {
					BYTE LeftButton;
					BYTE RightButton;
					BYTE MiddleButton;
					BYTE XButton1;
					BYTE XButton2;
					BYTE XButton3;
					BYTE XButton4;
					BYTE XButton5;
				};
			};
		};
	};
};

//------------------------------------------------------------------------------
//	ゲームパッド状態
//------------------------------------------------------------------------------
struct GamePadState {
public:
	GamePadState() { ::ZeroMemory(&State, sizeof(State)); }
	GamePadState(IInputDevice& rhs)
	{
		rhs.Polling();
		rhs.GetState(&State);
		DWORD*   buttons = (DWORD*)Buttons;
		for(int i = 0; i < sizeof(Buttons) / sizeof(DWORD); i++)
			*(buttons + i) &= 0x80808080;
	}

	inline GamePadState& operator =(IInputDevice& rhs)
	{
		rhs.Polling();
		rhs.GetState(&State);
		return *this;
	}

	inline GamePadState& operator =(GamePadState& rhs)
	{
		::CopyMemory(&State, &rhs.State, sizeof(State));
		return *this;
	}

	inline operator DIJOYSTATE()
	{
		return State;
	}

	union {
		DIJOYSTATE   State;
		struct {
			LONG    X;
			LONG    Y;
			LONG    X2;
			LONG    Y2;
			LONG    X3;
			LONG    Y3;
			LONG    Slider[2];
			DWORD   POV[4];
			BYTE    Buttons[32];
		};
	};
};

//------------------------------------------------------------------------------
//	デバイスバッファ
//------------------------------------------------------------------------------
struct DeviceBuffer {
public:
	union DEVICEBUFFER {
		DIDEVICEOBJECTDATA   Data;
		struct {
			DWORD      DeviceType;
			DWORD      State;
			DWORD      TickCount;
			DWORD      No;
			UINT_PTR   Action;
		};
	} Buffer[DIDEV_BUFSIZE];

	DeviceBuffer() : m_Items(0) { ::ZeroMemory(Buffer, sizeof(Buffer)); }
	DeviceBuffer(IInputDevice& rhs) { m_Items = rhs.GetBuffer((DIDEVICEOBJECTDATA*)Buffer); }

	BOOL IsPressed(const DWORD inDeviceType)
	{
		for(DWORD i = 0; i < m_Items; i++) {
			if(Buffer[i].DeviceType == inDeviceType)
				if((Buffer[i].State & 0x80) != 0)
					return TRUE;
		}
		return FALSE;
	}

	BOOL IsReleased(const DWORD inDeviceType)
	{
		for(DWORD i = 0; i < m_Items; i++) {
			if(Buffer[i].DeviceType == inDeviceType)
				if((Buffer[i].State & 0x80) == 0)
					return TRUE;
		}
		return FALSE;
	}

	DWORD GetData(const DWORD inDeviceType)
	{
		for(DWORD i = 0; i < m_Items; i++) {
			if(Buffer[i].DeviceType == inDeviceType)
				return Buffer[i].State;
		}
		return 0xffffffff;
	}

	inline DeviceBuffer& operator =(IInputDevice& rhs)
	{
		m_Items = rhs.GetBuffer((DIDEVICEOBJECTDATA*)Buffer);
		return *this;
	}
/*
	inline DeviceBuffer& operator =(const DeviceBuffer& rhs)
	{
		::CopyMemory(Buffer, rhs.Buffer, sizeof(Buffer));
		return *this;
	}

	inline DEVICEBUFFER operator [](UINT idx) const
	{
		return Buffer[idx];
	}
*/
private:
	DWORD   m_Items;
};

//------------------------------------------------------------------------------
//	カレントディレクトリ設定
//------------------------------------------------------------------------------
inline void ContentRootDirectory(LPCTSTR inDir)
{
/*
	TCHAR   full_path[MAX_PATH];
	::GetModuleFileName(NULL, full_path, MAX_PATH);

	TCHAR*  file_name = ::_tcsrchr(full_path, _T('\\'));	// 最後の'\'を検出
	if(file_name != NULL) {
		// ファイル名削除
		file_name = ::_tcsinc(file_name);
	   *file_name = _T('\0');
	}

	::SetCurrentDirectory(full_path);
*/
	::SetCurrentDirectory(inDir);
}

//------------------------------------------------------------------------------
//	ウィンドウタイトル設定
//------------------------------------------------------------------------------
inline void WindowTitle(LPCTSTR inTitle)
{
	::SetWindowText(CDXGraphics9::GetInstance().GetHWnd(), inTitle);
/*
	const DWORD   CurrentID = ::GetCurrentProcessId();

	HWND   hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
	while(hWnd != NULL) {
		DWORD   id;
		::GetWindowThreadProcessId(hWnd, &id);
		if(CurrentID == id) {
			TCHAR   name[32 + 1];
			::GetClassName(hWnd, name, 32);
			if(::lstrcmp(TEXT("OOGAMEPROG_WINDOWCLASS"), name) == 0) {
				::SetWindowText(hWnd, inTitle);
				break;
			}
		}
		hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
	}
*/
}

//------------------------------------------------------------------------------
//	スレッド
//------------------------------------------------------------------------------
// クラス前方参照
class GameMain;

// ゲームスレッドクラス
class GameThread : public CThread {
public:
	static unsigned __stdcall GameThreadFunc(void* arg)
	{
		GameThreadFuncInfo*       info = (GameThreadFuncInfo*)arg;
		void (GameMain::*ThreadFunc)() = info->ThreadFunc;

		(info->GameMainPtr->*ThreadFunc)();

		return 0;
	}

	void Start( void (GameMain::*inThreadFunc)(), GameMain* inGameMainPtr )
	{
		info.GameMainPtr = inGameMainPtr;
		info.ThreadFunc  = inThreadFunc;
		CThread::Start(GameThreadFunc, &info);
	}

private:
	// ゲームスレッド関数情報
	struct GameThreadFuncInfo {
		GameMain*   GameMainPtr;
		void (GameMain::*ThreadFunc)();
	} info;
};

// ロックオブジェクト
struct LockObject {
	LockObject()
	{
		::InitializeCriticalSection(&cs);
	}

	virtual ~LockObject()
	{
		::DeleteCriticalSection(&cs);
	}

	CRITICAL_SECTION   cs;
};

// lockステートメント
inline void lock(LockObject& lock_obj)
{
	::EnterCriticalSection(&lock_obj.cs);
}

// unlockステートメント
inline void unlock(LockObject& lock_obj)
{
	::LeaveCriticalSection(&lock_obj.cs);
}

// イベントウェイト
class EventWaitHandle {
public:
	EventWaitHandle()
	{
		m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	virtual ~EventWaitHandle()
	{
		if(m_hEvent != NULL) {
			::CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}

	void Set()
	{
		if(m_hEvent != NULL)
			::SetEvent(m_hEvent);
	}

	void Reset()
	{
		if(m_hEvent != NULL)
			::ResetEvent(m_hEvent);
	}

	bool Wait(const DWORD inMSTimeout = INFINITE)
	{
		if(m_hEvent == NULL)
			return false;

		if(::WaitForSingleObject(m_hEvent, inMSTimeout) != WAIT_OBJECT_0)
			return false;

		return true;
	}

	bool Wait(CThread& inThread, const DWORD inMSTimeout = INFINITE)
	{
		if(m_hEvent == NULL)
			return false;

		// イベントハンドル設定
		HANDLE   hEvents[2];
		hEvents[0] = m_hEvent;
		hEvents[1] = inThread.GetCloseEventHandle();

		// イベントシグナル、スレッドキャンセル、タイムアウトのいずれかまで待機
		if(::WaitForMultipleObjects(2, hEvents, FALSE, inMSTimeout) != WAIT_OBJECT_0)
			return false;

		return true;
	}

	bool IsSignaled()
	{
		if(m_hEvent == NULL)
			return false;

		if(::WaitForSingleObject(m_hEvent, 0) != WAIT_OBJECT_0)
			return false;

		return true;
	}

private:
	HANDLE   m_hEvent;
};

// ゲームスレッド関数指定用マクロ
#define GameThreadFunction(FuncName) &FuncName,this

//------------------------------------------------------------------------------
//	ストリーム
//------------------------------------------------------------------------------
typedef CStreamReader StreamReader;
typedef CStreamWriter StreamWriter;
typedef CBinaryReader BinaryReader;
typedef CBinaryWriter BinaryWriter;
typedef CMemoryStream MemoryStream;

//------------------------------------------------------------------------------
//	ネットワーク
//------------------------------------------------------------------------------
typedef CNetworkSession NetworkSession;

//------------------------------------------------------------------------------
//	入力デバイスバッファ
//------------------------------------------------------------------------------
typedef DeviceBuffer KeyboardBuffer;
typedef DeviceBuffer MouseBuffer;
typedef DeviceBuffer GamePadBuffer;

//------------------------------------------------------------------------------
//	ミュージック演奏フラグ
//------------------------------------------------------------------------------
#define MusicPlay_Primary   DXAMS_PRIMARY
#define MusicPlay_Secondary DXAMS_SECONDARY
#define MusicPlay_Queue     DXAMS_QUEUE

//------------------------------------------------------------------------------
//	サウンド状態
//------------------------------------------------------------------------------
#define SoundState_Playing  DSBSTATUS_PLAYING
#define SoundState_Looping  DSBSTATUS_LOOPING

//------------------------------------------------------------------------------
//	サウンドボリューム
//------------------------------------------------------------------------------
#define SoundVolume_Max     DSBVOLUME_MAX
#define SoundVolume_Min     DSBVOLUME_MIN

//------------------------------------------------------------------------------
//	サウンドパン
//------------------------------------------------------------------------------
#define SoundPan_Center     DSBPAN_CENTER
#define SoundPan_Left       DSBPAN_LEFT
#define SoundPan_Right      DSBPAN_RIGHT

//------------------------------------------------------------------------------
//	サウンドエフェクト
//------------------------------------------------------------------------------
#pragma region SoundEffect

#define SoundEffect_Chorus            DXAFX_CHORUS
#define SoundEffect_Compressor        DXAFX_COMPRESSOR
#define SoundEffect_Distortion        DXAFX_DISTORTION
#define SoundEffect_Echo              DXAFX_ECHO
#define SoundEffect_Flanger           DXAFX_FLANGER
#define SoundEffect_Gargle            DXAFX_GARGLE
#define SoundEffect_EnvironmentReverb DXAFX_ENVREVERB
#define SoundEffect_ParametricEQ      DXAFX_PARAMEQ
#define SoundEffect_WavesReverb       DXAFX_WAVESREVERB

#pragma endregion

//------------------------------------------------------------------------------
//	メディア状態
//------------------------------------------------------------------------------
#define MediaState_Plyaing State_Running
#define MediaState_Stopped State_Stopped
#define MediaState_Paused  State_Paused

//------------------------------------------------------------------------------
//	タイムフォーマット
//------------------------------------------------------------------------------
#define SecondsToMediaTime(n)        ((LONGLONG)(n * 10 * 1000) * 1000)

//------------------------------------------------------------------------------
//	マクロ定義
//------------------------------------------------------------------------------
#define GameTimer        CFixTimer::GetInstance()
#define SharedData       CSceneShared::GetInstance()

#define GraphicsDevice   CDXGraphics9::GetInstance()
#define SpriteBatch      CSpriteBatch::GetInstance()
#define Sprite3D         CSprite3D::GetInstance()
#define Direct2D         CDirect2D::GetInstance()
#define GraphicsDevice2D CDirect2D::GetInstance()

#define InputDevice      CDInput8::GetInstance()
#define Keyboard         DInput().GetKeyboard()
#define Mouse            DInput().GetMouse()
#define GamePad(i)       DInput().GetGamePad(i)
#define GamePadEffect(i) DInput().GetGamePad(i)->GetEffect()

#define SoundDevice      CDXAudio8::GetInstance()
#define SoundListener    DXAudio().GetInstance()
#define Sound3D(sb)      sb->GetSound3D()

#define MediaManager     DShow().GetInstance()

#define Squirrel         CSquirrel::GetInstance()

//------------------------------------------------------------------------------
//	型定義
//------------------------------------------------------------------------------
typedef IEffect          *EFFECT;
typedef IEffect          *SHADER;

typedef IVertexBuffer    *VERTEXBUFFER;
typedef IModel           *MODEL;
typedef IAnimationModel  *ANIMATIONMODEL;

typedef CCamera          Camera;
typedef CCamera          CAMERA;

typedef ISprite          *SPRITE;
typedef IDXGFont         *FONT;

typedef IStateBlock      *STATEBLOCK;
typedef IRenderTarget    *RENDERTARGET;

typedef ISprite3DLayer   *SPRITE3DLAYER;

typedef IMusicSegment    *MUSIC;
typedef ISoundBuffer     *SOUND;
typedef ISound3DBuffer   *SOUND3D;
typedef ISoundListener   *SOUNDLISTENER;

typedef IMedia           *MEDIA;

typedef std::vector<SpriteElement> SpriteArray;
typedef std::list  <SpriteElement> SpriteList;
typedef std::set   <SpriteElement> SpriteSet;

typedef void (GameMain::*GameMainFuncPtr_void)();
typedef int  (GameMain::*GameMainFuncPtr_int )();
#define CallGameMainFunction(func) ( this->* func )()

//------------------------------------------------------------------------------
//	Direct2D拡張
//------------------------------------------------------------------------------
#ifdef _INC_DIRECT2D
//------------------------------------------------------------------------------
//	Direct2D型定義
//------------------------------------------------------------------------------
typedef ID2D1Bitmap                   *Bitmap;

typedef ID2D1StrokeStyle              *StrokeStyle;
typedef ID2D1GradientStopCollection   *GradientStopCollection;

typedef ID2D1Brush                    *Brush;
typedef ID2D1SolidColorBrush          *SolidColorBrush;
typedef ID2D1BitmapBrush              *BitmapBrush;
typedef ID2D1LinearGradientBrush      *LinearGradientBrush;
typedef ID2D1RadialGradientBrush      *RadialGradientBrush;

typedef ID2D1Geometry                 *Geometry;
typedef ID2D1RectangleGeometry        *RectangleGeometry;
typedef ID2D1RoundedRectangleGeometry *RoundedRectangleGeometry;
typedef ID2D1EllipseGeometry          *EllipseGeometry;
typedef ID2D1PathGeometry             *PathGeometry;
typedef ID2D1TransformedGeometry      *TransformedGeometry;
typedef ID2D1GeometryGroup            *GeometryGroup;

typedef ID2D1GeometrySink             *GeometrySink;

typedef ID2D1Layer                    *Layer;

typedef IDWriteTextFormat             *TextFormat;
typedef IDWriteTextLayout             *TextLayout;
typedef IDWriteTypography             *Typography;

typedef D2D_MATRIX_3X2_F               Matrix3x2;

// 矩形構造体
struct RectF : public D2D_RECT_F {
	RectF()
	{
		left   = 0.0f;
		top    = 0.0f;
		right  = 0.0f;
		bottom = 0.0f;
	}

	RectF(const float l, const float t, const float r, const float b)
	{
		left   = l;
		top    = t;
		right  = r;
		bottom = b;
	}

	RectF(const D2D_RECT_F& rect)
	{
		left   = rect.left;
		top    = rect.top;
		right  = rect.right;
		bottom = rect.bottom;
	}

	inline RectF& operator =(const D2D_RECT_F& rect)
	{
		left   = rect.left;
		top    = rect.top;
		right  = rect.right;
		bottom = rect.bottom;
		return *this;
	}
};

struct RectFWH : public RectF {
	RectFWH() : RectF()
	{
	}

	RectFWH(const float l, const float t, const float w, const float h)
	{
		left   = l;
		top    = t;
		right  = left + w;
		bottom = top  + h;
	}
};

// 座標構造体
struct Point2F : public D2D_POINT_2F {
	Point2F()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Point2F(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	Point2F(const D2D_POINT_2F& pt)
	{
		x = pt.x;
		y = pt.y;
	}

	inline Point2F& operator =(const D2D_POINT_2F& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
};
#endif

//------------------------------------------------------------------------------
//	SpriteStudio拡張
//------------------------------------------------------------------------------
#ifdef SS5Player_h

#if !(defined(_DEBUG) || defined(DEBUG))
	#pragma comment(lib, "SS5Player.lib")
#else
	#pragma comment(lib, "SS5Playerd.lib")
#endif

#endif

//------------------------------------------------------------------------------
//	Effekseer拡張
//------------------------------------------------------------------------------
#ifdef _INC_EFFEKSEER

#define Effekseer CEffekseer::GetInstance()

typedef IEffekseerEffect *EFFEKSEER;
typedef int               EFFEKSEERID;

#endif

//------------------------------------------------------------------------------
//	Live2D拡張
//------------------------------------------------------------------------------
#ifdef _INC_LIVE2D

#define  Live2D       CLive2D::GetInstance()
typedef  Live2DFile   LIVE2DFILE;
typedef  Live2DModel* LIVE2DMODEL;

#include <util/UtSystem.h>

#endif

//------------------------------------------------------------------------------
//	Squirrel拡張
//------------------------------------------------------------------------------
#ifdef _INC_SQUIRREL

typedef IScript          *SCRIPT;
typedef SquirrelObject   ScriptObject;
typedef SqPlus::SquirrelFunction<void >  ScriptFunction_void;
typedef SqPlus::SquirrelFunction<int  >  ScriptFunction_int;
typedef SqPlus::SquirrelFunction<float>  ScriptFunction_float;
#define CallScriptFunction_void(scfunc)  SqPlus::SquirrelFunction<void >( scfunc )()
#define CallScriptFunction_int(scfunc)   SqPlus::SquirrelFunction<int  >( scfunc )()
#define CallScriptFunction_float(scfunc) SqPlus::SquirrelFunction<float>( scfunc )()

#endif

//------------------------------------------------------------------------------
//	Kinect拡張
//------------------------------------------------------------------------------
#ifdef _INC_NUI

#define NUI     CNUI::GetInstance()
typedef IKinect *KINECT;

#endif

//------------------------------------------------------------------------------
//	OpenGL拡張
//------------------------------------------------------------------------------
#ifdef _INC_OPENGL

#define OpenGL COpenGL::GetInstance()

#endif

//------------------------------------------------------------------------------
//	DirectX11拡張
//------------------------------------------------------------------------------
class   IModel11;
typedef IModel11 *Model11;

#ifdef _INC_DXGRAPHICS11
#define GraphicsDevice11 CDXGraphics11::GetInstance()
#endif
