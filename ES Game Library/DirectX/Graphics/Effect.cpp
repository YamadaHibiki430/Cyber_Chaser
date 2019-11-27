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
	 Effect.cpp
				エフェクトクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Effect.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CEffect::CEffect(ID3DXEffect* pD3DXEffect) : m_pD3DXEffect(NULL), m_pD3DDevice(NULL),
											 m_Material(), m_UseStage0(false)
{
	assert(pD3DXEffect != NULL);
	m_pD3DXEffect = pD3DXEffect;
	m_pD3DXEffect->AddRef();

	m_pD3DXEffect->GetDevice(&m_pD3DDevice);
	assert(m_pD3DDevice != NULL);
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CEffect::~CEffect()
{
	// エフェクトテクスチャ解放
	std::list<DXGEFFECTMAPINFO>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		SafeRelease((*it).pTexture);

	m_pD3DDevice ->Release();
	m_pD3DXEffect->Release();
}

//------------------------------------------------------------------------------
//	デバイス消失処理
//------------------------------------------------------------------------------
void CEffect::OnLostDevice()
{
	m_pD3DXEffect->OnLostDevice();
}

//------------------------------------------------------------------------------
//	デバイスリセット処理
//------------------------------------------------------------------------------
void CEffect::OnResetDevice()
{
	m_pD3DXEffect->OnResetDevice();
}

//------------------------------------------------------------------------------
//	エフェクト開始
//------------------------------------------------------------------------------
UINT CEffect::Begin()
{
	// エフェクトテクスチャ設定
	std::list<DXGEFFECTMAPINFO>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++) {
		if((*it).hParameter != NULL)
			m_pD3DXEffect->SetTexture((*it).hParameter,   (*it).pTexture);
		else
			m_pD3DDevice ->SetTexture((*it).TextureStage, (*it).pTexture);
	}

	UINT   pass;
	if(m_pD3DXEffect->Begin(&pass, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - エフェクト開始失敗(CEffect_Begin)\n"));
		return 0;
	}

	return pass;
}

//------------------------------------------------------------------------------
//	エフェクト終了
//------------------------------------------------------------------------------
void CEffect::End()
{
	m_pD3DXEffect->End();

	// エフェクトテクスチャ解除
	std::list<DXGEFFECTMAPINFO>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++) {
		if((*it).hParameter == NULL)
			m_pD3DDevice ->SetTexture((*it).TextureStage, NULL);
	}
}

//------------------------------------------------------------------------------
//	パス開始
//------------------------------------------------------------------------------
bool CEffect::BeginPass(const UINT inPass)
{
	if(m_pD3DXEffect->BeginPass(inPass) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - パス開始失敗(CEffect_BeginPass)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	パス終了
//------------------------------------------------------------------------------
void CEffect::EndPass()
{
	m_pD3DXEffect->EndPass();
}

//------------------------------------------------------------------------------
//	テクニック設定
//------------------------------------------------------------------------------
void CEffect::SetTechnique(LPCSTR inTechName)
{
	m_pD3DXEffect->SetTechnique(inTechName);
}

//------------------------------------------------------------------------------
//	データ設定
//------------------------------------------------------------------------------
void CEffect::SetData(const D3DXHANDLE inParamName, LPCVOID lpData, const UINT inBytes)
{
	m_pD3DXEffect->SetValue(inParamName, lpData, inBytes);
}

//------------------------------------------------------------------------------
//	整数配列設定
//------------------------------------------------------------------------------
void CEffect::SetIntArray(const D3DXHANDLE inParamName, const int inIntArray[], UINT inCount)
{
	m_pD3DXEffect->SetIntArray(inParamName, inIntArray, inCount);
}

//------------------------------------------------------------------------------
//	単精度浮動小数配列設定
//------------------------------------------------------------------------------
void CEffect::SetFloatArray(const D3DXHANDLE inParamName, const float inFloatArray[], UINT inCount)
{
	m_pD3DXEffect->SetFloatArray(inParamName, inFloatArray, inCount);
}

//------------------------------------------------------------------------------
//	ベクトル４配列設定
//------------------------------------------------------------------------------
void CEffect::SetVector4Array(const D3DXHANDLE inParamName, const D3DXVECTOR4 inVec4Array[], UINT inCount)
{
	m_pD3DXEffect->SetVectorArray(inParamName, inVec4Array, inCount);
}

//------------------------------------------------------------------------------
//	行列配列設定
//------------------------------------------------------------------------------
void CEffect::SetMatrixArray(const D3DXHANDLE inParamName, const D3DXMATRIX inMatrixArray[], UINT inCount)
{
	m_pD3DXEffect->SetMatrixArray(inParamName, inMatrixArray, inCount);
}

//------------------------------------------------------------------------------
//	逆行列設定
//------------------------------------------------------------------------------
void CEffect::SetMatrixInverse(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
{
	// 逆行列生成
	D3DXMATRIX   inv;
	if(::D3DXMatrixInverse(&inv, NULL, &inMatrix) == NULL)
		::ZeroMemory(&inv, sizeof(inv));

	m_pD3DXEffect->SetMatrix(inParamName, &inv);
}

//------------------------------------------------------------------------------
//	転置行列設定
//------------------------------------------------------------------------------
void CEffect::SetMatrixTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
{
	m_pD3DXEffect->SetMatrixTranspose(inParamName, &inMatrix);
}

//------------------------------------------------------------------------------
//	逆転置行列設定
//------------------------------------------------------------------------------
void CEffect::SetMatrixInverseTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
{
	// 逆行列生成
	D3DXMATRIX   inv;
	if(::D3DXMatrixInverse(&inv, NULL, &inMatrix) == NULL)
		::ZeroMemory(&inv, sizeof(inv));

	m_pD3DXEffect->SetMatrixTranspose(inParamName, &inv);
}

//------------------------------------------------------------------------------
//	テクスチャ設定
//------------------------------------------------------------------------------
void CEffect::SetTexture(const D3DXHANDLE inParamName, IDirect3DBaseTexture9* pTexture)
{
	m_pD3DXEffect->SetTexture(inParamName, pTexture);
}

//------------------------------------------------------------------------------
//	テクスチャ設定
//------------------------------------------------------------------------------
void CEffect::SetTexture(const DWORD inTextureStage, IDirect3DBaseTexture9* pTexture)
{
	m_pD3DDevice->SetTexture(inTextureStage, pTexture);
}

//------------------------------------------------------------------------------
//	マテリアル名登録
//------------------------------------------------------------------------------
void CEffect::RetisterMaterialByName(LPCSTR inDiffuse,  LPCSTR inAmbient, LPCSTR inSpecular,
									 LPCSTR inEmissive, LPCSTR inPower,   LPCSTR inTexture)
{
	D3DXVECTOR4   v(0.0f, 0.0f, 0.0f, 0.0f);
	float         f = 0.0f;

	if(inDiffuse  != NULL) {
		m_Material.Diffuse  = m_pD3DXEffect->GetParameterByName(NULL, inDiffuse );
		m_pD3DXEffect->SetVector (m_Material.Diffuse,  &v);
	}
	if(inAmbient  != NULL) {
		m_Material.Ambient  = m_pD3DXEffect->GetParameterByName(NULL, inAmbient );
		m_pD3DXEffect->SetVector (m_Material.Ambient,  &v);
	}
	if(inSpecular != NULL) {
		m_Material.Specular = m_pD3DXEffect->GetParameterByName(NULL, inSpecular);
		m_pD3DXEffect->SetVector (m_Material.Specular, &v);
	}
	if(inEmissive != NULL) {
		m_Material.Emissive = m_pD3DXEffect->GetParameterByName(NULL, inEmissive);
		m_pD3DXEffect->SetVector (m_Material.Emissive, &v);
	}
	if(inPower    != NULL) {
		m_Material.Power    = m_pD3DXEffect->GetParameterByName(NULL, inPower   );
		m_pD3DXEffect->SetFloat  (m_Material.Power,     f);
	}
	if(inTexture  != NULL) {
		m_Material.Texture  = m_pD3DXEffect->GetParameterByName(NULL, inTexture );
		m_pD3DXEffect->SetTexture(m_Material.Texture,  NULL);
	}
}

//------------------------------------------------------------------------------
//	マテリアル設定
//------------------------------------------------------------------------------
bool CEffect::SetMaterial(D3DMATERIAL9& inMaterial, IDirect3DBaseTexture9* pTexture)
{
	bool   change = false;

	// マテリアル設定
	if(m_Material.Diffuse  != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Diffuse,  (D3DXVECTOR4*)&inMaterial.Diffuse );
		change = true;
	}
	if(m_Material.Ambient  != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Ambient,  (D3DXVECTOR4*)&inMaterial.Ambient );
		change = true;
	}
	if(m_Material.Specular != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Specular, (D3DXVECTOR4*)&inMaterial.Specular);
		change = true;
	}
	if(m_Material.Emissive != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Emissive, (D3DXVECTOR4*)&inMaterial.Emissive);
		change = true;
	}
	if(m_Material.Power    != NULL) {
		m_pD3DXEffect->SetFloat (m_Material.Power,    inMaterial.Power);
		change = true;
	}

	// テクスチャ設定
	if(m_UseStage0 == false) {
		if(m_Material.Texture != NULL) {
			m_pD3DXEffect->SetTexture(m_Material.Texture, pTexture);
			change = true;
		} else {
			m_pD3DDevice ->SetTexture(0, pTexture);
		}
	}

	if(change)
		m_pD3DXEffect->CommitChanges();

	return change;
}

//------------------------------------------------------------------------------
//	エフェクトマッピング設定
//------------------------------------------------------------------------------
void CEffect::SetEffectMap(LPCSTR inParamName, LPCTSTR inFileName, const D3DFORMAT inFormat)
{
	// 有効なパラメータ名か調べる
	D3DXHANDLE   hParameter = m_pD3DXEffect->GetParameterByName(NULL, inParamName);
	if(hParameter == NULL) {
		::OutputDebugString(TEXT("*** Error - パラメータ名未定義(CEffect_SetEffectMap)\n"));
		return;
	}

	// テクスチャ生成
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFileEx(m_pD3DDevice, inFileName, 0, 0, 0, 0,
									 inFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
									 0, NULL, NULL, &pTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CEffect_SetEffectMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(DXGEFFECTMAPINFO(hParameter, 0, pTexture));
}

//------------------------------------------------------------------------------
//	エフェクトマッピング設定
//------------------------------------------------------------------------------
void CEffect::SetEffectMap(const DWORD inTextureStage, LPCTSTR inFileName, const D3DFORMAT inFormat)
{
	// テクスチャ生成
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFileEx(m_pD3DDevice, inFileName, 0, 0, 0, 0,
									 inFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
									 0, NULL, NULL, &pTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CEffect_SetEffectMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(DXGEFFECTMAPINFO(NULL, inTextureStage, pTexture));

	// ステージ０使用設定
	if(inTextureStage == 0)
		m_UseStage0 = true;
}

//------------------------------------------------------------------------------
//	キューブマッピング設定
//------------------------------------------------------------------------------
void CEffect::SetCubeMap(LPCSTR inParamName, LPCTSTR inFileName)
{
	// 有効なパラメータ名か調べる
	D3DXHANDLE   hParameter = m_pD3DXEffect->GetParameterByName(NULL, inParamName);
	if(hParameter == NULL) {
		::OutputDebugString(TEXT("*** Error - パラメータ名異常(CEffect_SetEffectMap)\n"));
		return;
	}

	// キューブテクスチャ生成
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - キューブテクスチャ生成失敗(CEffect_SetCubeMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(DXGEFFECTMAPINFO(hParameter, 0, pCubeTexture));
}

//------------------------------------------------------------------------------
//	キューブマッピング設定
//------------------------------------------------------------------------------
void CEffect::SetCubeMap(const DWORD inTextureStage, LPCTSTR inFileName)
{
	// キューブテクスチャ生成
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - キューブテクスチャ生成失敗(CEffect_SetCubeMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(DXGEFFECTMAPINFO(NULL, inTextureStage, pCubeTexture));

	// ステージ０使用設定
	if(inTextureStage == 0)
		m_UseStage0 = true;
}
