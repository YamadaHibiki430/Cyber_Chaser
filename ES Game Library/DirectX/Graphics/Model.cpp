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
	 Model.cpp
				モデルクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Model.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CModel::CModel(ID3DXMesh* pMesh, const DWORD inMeshes)
	: m_pBaseMesh(NULL), m_pMesh(pMesh), m_pPMesh(NULL), m_pD3DDevice(NULL),
	  m_Meshes(inMeshes), m_Material(inMeshes), m_Texture(inMeshes, NULL),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f, 1.0f)
{
	assert(m_pMesh != NULL);
	m_pMesh->AddRef();

	m_pMesh->GetDevice(&m_pD3DDevice);
	assert(m_pD3DDevice != NULL);

	for(DWORD i = 0; i < m_Meshes; i++)
		::ZeroMemory(&m_Material[i], sizeof(m_Material[i]));

	m_pBaseMesh = m_pMesh;
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CModel::~CModel()
{
	// エフェクトテクスチャ解放
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		SafeRelease((*it).pTexture);

	// テクスチャ解放
	for(unsigned int i = 0; i < m_Texture.size(); i++)
		SafeRelease(m_Texture[i]);

	m_pD3DDevice->Release();

	m_pBaseMesh = NULL;

	SafeRelease(m_pPMesh);
	SafeRelease(m_pMesh );
}

//------------------------------------------------------------------------------
//	メッシュ最適化
//------------------------------------------------------------------------------
bool CModel::Optimize()
{
	if(m_pMesh != NULL) {
		// 通常モード
		try {
			// 隣接面生成
			const DWORD    FACE = m_pMesh->GetNumFaces();
			DWORD*   pAdjacency = new DWORD[FACE * 3 * sizeof(DWORD)];
			if(m_pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency) != D3D_OK)
				throw TEXT("*** Error - 隣接面生成失敗(CModel_Optimize)\n");

			// 最適化
			HRESULT   hr = m_pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
													pAdjacency, NULL, NULL, NULL);
			delete[] pAdjacency;
			if(hr != D3D_OK)
				throw TEXT("*** Error - メッシュ最適化失敗(CModel_Optimize)\n");
		} catch(LPCTSTR ErrorString) {
			::OutputDebugString(ErrorString);
			return false;
		}
	} else {
		// プログレッシブモード
		m_pPMesh->OptimizeBaseLOD(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, NULL);
	}

	return true;
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CModel::Draw()
{
	// ワールド変換行列設定
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// 描画
	for(DWORD i = 0; i < m_Meshes; i++) {
		m_pD3DDevice->SetMaterial(  &m_Material[i]);	// マテリアル設定
		m_pD3DDevice->SetTexture (0, m_Texture [i]);	// テクスチャ設定
		m_pBaseMesh->DrawSubset(i);
	}
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CModel::DrawPure()
{
	// 描画
	for(DWORD i = 0; i < m_Meshes; i++)
		m_pBaseMesh->DrawSubset(i);
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CModel::Draw(IEffect*& pEffect)
{
	const UINT   pass = pEffect->Begin();
	for(UINT ps = 0; ps < pass; ps++) {
		pEffect->BeginPass(ps);

		// メッシュ描画
		for(DWORD i = 0; i < m_Meshes; i++) {
			pEffect->SetMaterial(m_Material[i], m_Texture[i]);
			m_pBaseMesh->DrawSubset(i);
		}

		pEffect->EndPass();
	}
	pEffect->End();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CModel::Draw(IEffect*& pEffect, const UINT inPass)
{
	pEffect->Begin();
	DrawPass(pEffect, inPass);
	pEffect->End();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CModel::DrawPass(IEffect*& pEffect, const UINT inPass)
{
	pEffect->BeginPass(inPass);

	for(DWORD i = 0; i < m_Meshes; i++) {
		pEffect->SetMaterial(m_Material[i], m_Texture[i]);
		m_pBaseMesh->DrawSubset(i);
	}

	pEffect->EndPass();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
/*
void CModel::Draw(IEffect* EffectArray[])
{
	// メッシュ描画
	for(DWORD i = 0; i < m_Meshes; i++) {
		if(EffectArray[i] == NULL) {
			// T&L固定機能で描画
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh->DrawSubset(i);
		} else {
			// エフェクトで描画
			const UINT   pass = EffectArray[i]->Begin();
			for(UINT ps = 0; ps < pass; ps++) {
				EffectArray[i]->BeginPass(ps);

				EffectArray[i]->SetMaterial(m_Material[i], m_Texture[i]);
				m_pBaseMesh->DrawSubset(i);

				EffectArray[i]->EndPass();
			}
			EffectArray[i]->End();
		}
	}
}
*/

//------------------------------------------------------------------------------
//	半透明描画
//------------------------------------------------------------------------------
void CModel::DrawAlpha(const float inAlpha, const DWORD inDraw)
{
	if(inAlpha <= FLT_EPSILON)
		return;

	m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// マテリアル設定
	std::vector<float>   Diffuse(m_Meshes);
	for(DWORD i = 0; i < m_Meshes; i++) {
		Diffuse[i] = m_Material[i].Diffuse.a;
		m_Material[i].Diffuse.a *= inAlpha;
	}

	// カラー出力保存
	DWORD   colorwrite_enable;
	m_pD3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite_enable);

	// 背面
	if((inDraw & DXGDRAWA_BACKFACE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		// ｚ設定
		if((inDraw & DXGDRAWA_BACKFACE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ｚ出力設定
			if((inDraw & (DXGDRAWA_BACKFACE_ZWRDEFER | DXGDRAWA_BACKFACE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		for(DWORD i = 0; i < m_Meshes; i++) {
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh ->DrawSubset (i);
		}

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_BACKFACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			for(DWORD i = 0; i < m_Meshes; i++)
				m_pBaseMesh->DrawSubset(i);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// 表面
	if((inDraw & DXGDRAWA_FACE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		// ｚ設定
		if((inDraw & DXGDRAWA_FACE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ｚ出力設定
			if((inDraw & (DXGDRAWA_FACE_ZWRDEFER | DXGDRAWA_FACE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		for(DWORD i = 0; i < m_Meshes; i++) {
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh ->DrawSubset (i);
		}

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_FACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			for(DWORD i = 0; i < m_Meshes; i++)
				m_pBaseMesh->DrawSubset(i);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// カリングなし
	if((inDraw & DXGDRAWA_CULLNONE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// ｚ設定
		if((inDraw & DXGDRAWA_CULLNONE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ｚ出力設定
			if((inDraw & (DXGDRAWA_CULLNONE_ZWRDEFER | DXGDRAWA_CULLNONE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		for(DWORD i = 0; i < m_Meshes; i++) {
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh ->DrawSubset (i);
		}

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_CULLNONE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			for(DWORD i = 0; i < m_Meshes; i++)
				m_pBaseMesh->DrawSubset(i);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// ｚ書込
	const DWORD   ZWRITE = inDraw & 0x3000;
	if(ZWRITE != 0) {
		if(ZWRITE == DXGDRAWA_ZWRITE_FACE)
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		// 表面
		else if(ZWRITE == DXGDRAWA_ZWRITE_CULLNONE)
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		// カリングなし
		else
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);		// 裏面

		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		for(DWORD i = 0; i < m_Meshes; i++)
			m_pBaseMesh->DrawSubset(i);

		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
	}

	// マテリアル復元
	for(DWORD i = 0; i < m_Meshes; i++)
		m_Material[i].Diffuse.a = Diffuse[i];
}

//------------------------------------------------------------------------------
//	影描画
//------------------------------------------------------------------------------
void CModel::DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth)
{
	// ワールド変換行列設定
	D3DXMATRIX   ShadowMatrix = GetWorldMatrix() * inShadowMatrix;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &ShadowMatrix);

	// マテリアル設定
	D3DMATERIAL9   material;
	::ZeroMemory(&material, sizeof(material));
	material.Diffuse.a = inDepth;

	m_pD3DDevice->SetMaterial(&material);
	m_pD3DDevice->SetTexture (0, NULL);

	// 描画
	for(DWORD i = 0; i < m_Meshes; i++)
		m_pBaseMesh->DrawSubset(i);
}

//------------------------------------------------------------------------------
//	マテリアル設定
//------------------------------------------------------------------------------
void CModel::SetMaterial(const D3DMATERIAL9& inMaterial, const DWORD inMeshIdx)
{
	m_Material[inMeshIdx] = inMaterial;
}

//------------------------------------------------------------------------------
//	マテリアル設定
//------------------------------------------------------------------------------
void CModel::SetMaterial(const D3DMATERIAL9& inMaterial)
{
	for(DWORD i = 0; i < m_Meshes; i++)
		m_Material[i] = inMaterial;
}

//------------------------------------------------------------------------------
//	テクスチャ設定
//------------------------------------------------------------------------------
void CModel::SetTexture(LPCTSTR inFileName, const DWORD inMeshIdx)
{
	SafeRelease(m_Texture[inMeshIdx]);

	// テクスチャ生成
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &pTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CModel_SetTexture)\n"));
		pTexture = NULL;
	}

	m_Texture[inMeshIdx] = pTexture;
}

//------------------------------------------------------------------------------
//	キューブテクスチャ設定
//------------------------------------------------------------------------------
void CModel::SetCubeTexture(LPCTSTR inFileName, const DWORD inMeshIdx)
{
	SafeRelease(m_Texture[inMeshIdx]);

	// テクスチャ生成
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CModel_SetTexture)\n"));
		pCubeTexture = NULL;
	}

	m_Texture[inMeshIdx] = pCubeTexture;
}

//------------------------------------------------------------------------------
//	テクスチャ設定
//------------------------------------------------------------------------------
void CModel::SetTexture(IDirect3DBaseTexture9* pTexture, const DWORD inMeshIdx)
{
	SafeRelease(m_Texture[inMeshIdx]);

	if(pTexture != NULL) {
		m_Texture[inMeshIdx] = pTexture;
		m_Texture[inMeshIdx]->AddRef();
	}
}

//------------------------------------------------------------------------------
//	角度設定
//------------------------------------------------------------------------------
void CModel::SetDirection(const float inX, const float inY, const float inZ)
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

/*
	// 回転クォータニオンを求める
	::D3DXQuaternionRotationYawPitchRoll(&m_Rotation, D3DXToRadian(inY),
													  D3DXToRadian(inX),
													  D3DXToRadian(inZ));
*/
}

//------------------------------------------------------------------------------
//	移動
//------------------------------------------------------------------------------
void CModel::Move(const float inRight, const float inUp, const float inFront)
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
void CModel::Rotation(const float inX, const float inY, const float inZ)
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
void CModel::SetRotationBillboard(const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle)
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
void CModel::SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront)
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
void CModel::AlignmentAxisY(const D3DXVECTOR3& inAxisY)
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
D3DXVECTOR3 CModel::GetDirection() const
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
D3DXVECTOR3 CModel::GetDirectionRadian() const
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
D3DXMATRIX CModel::GetWorldMatrix() const
{
	// ワールド行列設定
	D3DXMATRIX   world;

	// 回転行列生成
	::D3DXMatrixRotationQuaternion(&world, &m_Rotation);

	// スケーリング
	world._11 *= m_Scale.x;
	world._12 *= m_Scale.x;
	world._13 *= m_Scale.x;

	world._21 *= m_Scale.y;
	world._22 *= m_Scale.y;
	world._23 *= m_Scale.y;

	world._31 *= m_Scale.z;
	world._32 *= m_Scale.z;
	world._33 *= m_Scale.z;

	// 移動
	world._41  = m_Position.x;
	world._42  = m_Position.y;
	world._43  = m_Position.z;

	world._44  = 1.0f;

	return world;
}

//------------------------------------------------------------------------------
//	前方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetFrontVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[2];
}

//------------------------------------------------------------------------------
//	右方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetRightVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[0];
}

//------------------------------------------------------------------------------
//	上方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetUpVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[1];
}

//------------------------------------------------------------------------------
//	衝突判定
//------------------------------------------------------------------------------
BOOL CModel::IntersectRay(D3DXVECTOR3& inPos, D3DXVECTOR3& inDir,
						  float* pDist, D3DXVECTOR3* pNormal)
{
	// ワールド行列の逆行列を生成
	D3DXMATRIXA16   world_inv;
	::D3DXMatrixInverse(&world_inv, NULL, &GetWorldMatrix());

	// レイをモデルのローカルへ座標へ変換
	D3DXVECTOR3   pos;
	::D3DXVec3TransformCoord (&pos, &inPos, &world_inv);

	// 方向をモデルのローカル座標へ変換
	D3DXVECTOR3   dir;
	::D3DXVec3TransformNormal(&dir, &inDir, &world_inv);

	BOOL   hit = FALSE;
	if(pNormal == NULL) {
		// 法線不要
		::D3DXIntersect(m_pBaseMesh, &pos, &dir, &hit, NULL, NULL, NULL, pDist, NULL, NULL);
	} else {
		// 法線必要
		DWORD   face_index;
		::D3DXIntersect(m_pBaseMesh, &pos, &dir, &hit, &face_index, NULL, NULL, pDist, NULL, NULL);
		if(hit != FALSE) {
			// インデックスバッファロック
			WORD*   pIndices;
			if(m_pBaseMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pIndices) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - インデックスバッファロック失敗(CModel_IntersectRay)\n"));
				*pNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				return hit;
			}

			// 衝突面インデックス取得
			WORD   index[3];
			index[0] = pIndices[face_index * 3 + 0];
			index[1] = pIndices[face_index * 3 + 1];
			index[2] = pIndices[face_index * 3 + 2];

			// インデックスバッファアンロック
			m_pBaseMesh->UnlockIndexBuffer();

			// ストライド設定
			const DWORD   STRIDE = m_pBaseMesh->GetNumBytesPerVertex();

			// 頂点バッファロック
			BYTE*   pVertices;
			if(m_pBaseMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pVertices) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - 頂点バッファロック失敗(CModel_IntersectRay)\n"));
				*pNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				return hit;
			}

			// 衝突面頂点座標取得
			D3DXVECTOR3   face[3];
			::CopyMemory(&face[0], pVertices + index[0] * STRIDE, sizeof(D3DXVECTOR3));
			::CopyMemory(&face[1], pVertices + index[1] * STRIDE, sizeof(D3DXVECTOR3));
			::CopyMemory(&face[2], pVertices + index[2] * STRIDE, sizeof(D3DXVECTOR3));

			// 頂点バッファアンロック
			m_pBaseMesh->UnlockVertexBuffer();

			// 法線設定
			::D3DXVec3Cross(pNormal, &D3DXVECTOR3(face[1] - face[0]), &D3DXVECTOR3(face[2] - face[0]));
			::D3DXVec3Normalize(pNormal, pNormal);
		}
	}

	return hit;
}

//------------------------------------------------------------------------------
//	境界球生成
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CModel::GetBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere(GetBaseBoundingSphere());

	float   scale = m_Scale.x;
	if(scale < m_Scale.y)
		scale = m_Scale.y;
	if(scale < m_Scale.z)
		scale = m_Scale.z;

	shpere.Center += m_Position;
	shpere.Radius *= scale;

	return shpere;
}

//------------------------------------------------------------------------------
//	基本境界球生成
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CModel::GetBaseBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere;

	// 頂点バッファロック
	void*   pVertices;
	if(m_pBaseMesh->LockVertexBuffer(0, &pVertices) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 頂点バッファロック失敗(CModel_GetBaseBoundingSphere)\n"));
		return shpere;
	}

	// 境界球生成
	::D3DXComputeBoundingSphere((D3DXVECTOR3*)pVertices, m_pMesh->GetNumVertices(),
								::D3DXGetFVFVertexSize(m_pMesh->GetFVF()),
								&shpere.Center, &shpere.Radius);

	m_pBaseMesh->UnlockVertexBuffer();

	return shpere;
}

//------------------------------------------------------------------------------
//	OBB生成
//------------------------------------------------------------------------------
DXGOBB CModel::GetOBB()
{
	DXGOBB   obb(GetBaseOBB());

	// 中心座標設定
	obb.Center += m_Position;

	// 各軸の傾き設定
	D3DXMATRIX   rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);
	obb.Axis[0] = D3DXVECTOR3(rot._11, rot._12, rot._13);
	obb.Axis[1] = D3DXVECTOR3(rot._21, rot._22, rot._23);
	obb.Axis[2] = D3DXVECTOR3(rot._31, rot._32, rot._33);

	// 各軸の長さ設定
	obb.Radius.x *= m_Scale.x;
	obb.Radius.y *= m_Scale.y;
	obb.Radius.z *= m_Scale.z;

	return obb;
}

//------------------------------------------------------------------------------
//	基本OBB生成
//------------------------------------------------------------------------------
DXGOBB CModel::GetBaseOBB()
{
	DXGOBB   obb;

	// 頂点バッファロック
	BYTE*   pVertices;
	if(m_pBaseMesh->LockVertexBuffer(0, (LPVOID*)&pVertices) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 頂点バッファロック失敗(CModel_GetOBB)\n"));
		return obb;
	}

	// 各軸の最大、最小座標を求める
	D3DXVECTOR3   pos_max(FLT_MIN, FLT_MIN, FLT_MIN);
	D3DXVECTOR3   pos_min(FLT_MAX, FLT_MAX, FLT_MAX);

	const int   NumVertices = m_pMesh->GetNumVertices();
	const int   STRIDE      = ::D3DXGetFVFVertexSize(m_pMesh->GetFVF());
	for(int i = 0; i < NumVertices; i++) {
		D3DXVECTOR3   pos = *(D3DXVECTOR3*)pVertices;
		if(pos.x < pos_min.x)	pos_min.x = pos.x;
		if(pos.x > pos_max.x)	pos_max.x = pos.x;
		if(pos.y < pos_min.y)	pos_min.y = pos.y;
		if(pos.y > pos_max.y)	pos_max.y = pos.y;
		if(pos.z < pos_min.z)	pos_min.z = pos.z;
		if(pos.z > pos_max.z)	pos_max.z = pos.z;

		pVertices += STRIDE;
	}

	m_pBaseMesh->UnlockVertexBuffer();

	// 中心点設定
	obb.Center = (pos_max + pos_min) * 0.5f;

	// 各軸の長さの半分を設定
	obb.Radius.x = ::fabsf(pos_max.x - pos_min.x) * 0.5f;
	obb.Radius.y = ::fabsf(pos_max.y - pos_min.y) * 0.5f;
	obb.Radius.z = ::fabsf(pos_max.z - pos_min.z) * 0.5f;

	return obb;
}

//------------------------------------------------------------------------------
//	オクルージョンクエリ取得
//------------------------------------------------------------------------------
DWORD CModel::GetOcclusionQuery()
{
	IDirect3DQuery9*   pQuery;
	const HRESULT   hr = m_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - オクルージョンクエリ取得失敗(CModel_GetOcclusionQuery)\n"));
		return 0xffffffff;
	}

	pQuery->Issue(D3DISSUE_BEGIN);
	Draw();
	pQuery->Issue(D3DISSUE_END  );

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	DWORD   drawn = 0xffffffff;
	while(pQuery->GetData(&drawn, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE)
		;	// none

	pQuery->Release();

	return drawn;
}

//------------------------------------------------------------------------------
//	オクルージョンクエリ取得
//------------------------------------------------------------------------------
DWORD CModel::GetOcclusionQuery(IEffect*& pEffect)
{
	IDirect3DQuery9*   pQuery;
	const HRESULT   hr = m_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - オクルージョンクエリ取得失敗(CModel_GetOcclusionQuery)\n"));
		return 0xffffffff;
	}

	pQuery->Issue(D3DISSUE_BEGIN);
	Draw(pEffect);
	pQuery->Issue(D3DISSUE_END  );

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	DWORD   drawn = 0xffffffff;
	while(pQuery->GetData(&drawn, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE)
		;	// none

	pQuery->Release();

	return drawn;
}

//------------------------------------------------------------------------------
//	プログレッシブモード
//------------------------------------------------------------------------------
bool CModel::EnableProgressive()
{
	if(m_pPMesh != NULL) {
		::OutputDebugString(TEXT("*** Error - プログレッシブモード設定済み(CModel_EnableProgressive)\n"));
		return true;
	}

	// 隣接面生成
	const DWORD    FACE = m_pMesh->GetNumFaces();
	DWORD*   pAdjacency = new DWORD[FACE * 3 * sizeof(DWORD)];
	if(m_pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 隣接面生成失敗(CModel_EnableProgressive)\n"));
		delete[] pAdjacency;
		return false;
	}

	// メッシュクリーニング
	ID3DXMesh*   pCleanMesh;
	if(::D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION, m_pMesh, pAdjacency, &pCleanMesh, pAdjacency, NULL) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - メッシュクリーニング失敗(CModel_EnableProgressive)\n"));
		delete[] pAdjacency;
		return false;
	}
	SafeRelease(m_pMesh);
	m_pMesh = pCleanMesh;

	// メッシュ結合
//	::D3DXWeldVertices(m_pMesh, D3DXWELDEPSILONS_WELDALL, NULL, pAdjacency, pAdjacency, NULL, NULL);

	// プログレッシブメッシュ生成
	if(::D3DXGeneratePMesh(m_pMesh, pAdjacency, NULL, NULL, 1, D3DXMESHSIMP_FACE, &m_pPMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - プログレッシブメッシュ生成失敗(CModel_EnableProgressive)\n"));
		delete[] pAdjacency;
		return false;
	}

	// 最適化
	m_pPMesh->SetNumFaces(m_pPMesh->GetMaxFaces());
	m_pPMesh->OptimizeBaseLOD(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, NULL);

	m_pBaseMesh = m_pPMesh;

	delete[] pAdjacency;
	SafeRelease(m_pMesh);

	return true;
}

//------------------------------------------------------------------------------
//	詳細レベル設定
//------------------------------------------------------------------------------
void CModel::SetLOD(const float inLOD)
{
	if(m_pPMesh == NULL) {
		::OutputDebugString(TEXT("*** Error - プログレッシブメッシュ未生成(CModel_SetLOD)\n"));
		return;
	}

	const DWORD   MaxFaces = m_pPMesh->GetMaxFaces();
	const DWORD   MinFaces = m_pPMesh->GetMinFaces();

	// 詳細レベル設定
	m_pPMesh->SetNumFaces((DWORD)(MinFaces + ((MaxFaces - MinFaces) * inLOD + 0.5f)));
}

//------------------------------------------------------------------------------
//	エフェクトマップ読み込み
//------------------------------------------------------------------------------
void CModel::LoadEffectMap(const DWORD inStage, LPCTSTR inFileName)
{
	// キューブテクスチャ生成
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &pTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CModel_LoadEffectMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(EFFECTMAP(inStage, pTexture));
}

//------------------------------------------------------------------------------
//	キューブエフェクトマップ読み込み
//------------------------------------------------------------------------------
void CModel::LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName)
{
	// キューブテクスチャ生成
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - キューブテクスチャ生成失敗(CModel_LoadCubeEffectMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(EFFECTMAP(inStage, pCubeTexture));
}

//------------------------------------------------------------------------------
//	エフェクトマップ適用
//------------------------------------------------------------------------------
void CModel::ApplyEffectMap()
{
	// エフェクトテクスチャ設定
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		m_pD3DDevice->SetTexture((*it).Stage, (*it).pTexture);
}

//------------------------------------------------------------------------------
//	エフェクトマップ解除
//------------------------------------------------------------------------------
void CModel::RemoveEffectMap()
{
	// エフェクトテクスチャ解除
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		m_pD3DDevice ->SetTexture((*it).Stage, NULL);
}
