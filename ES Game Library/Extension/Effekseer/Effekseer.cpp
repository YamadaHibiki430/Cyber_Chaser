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
	 Effekseer.cpp
				Effekseerクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Effekseer.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#ifndef _DEBUG
	#pragma comment(lib, "Effekseer.lib")
	#pragma comment(lib, "EffekseerRendererDX9.lib")
#else
	#pragma comment(lib, "Effekseerd.lib")
	#pragma comment(lib, "EffekseerRendererDX9d.lib")
#endif

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CEffekseer::CEffekseer() : m_manager(NULL), m_renderer(NULL)
{
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CEffekseer::~CEffekseer()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CEffekseer::Initialize(IDirect3DDevice9* pD3DDevice, const int inMaxSprite)
{
	assert(pD3DDevice != NULL);
	Release();

	// 描画管理インスタンス生成
	m_renderer = EffekseerRendererDX9::Renderer::Create(pD3DDevice, inMaxSprite);

	// エフェクト管理インスタンス生成
	m_manager  = Effekseer::Manager::Create(inMaxSprite);

	// 描画方法設定
	m_manager->SetSpriteRenderer  (m_renderer->CreateSpriteRenderer());	// スプライト
	m_manager->SetRibbonRenderer  (m_renderer->CreateRibbonRenderer());	// リボン
	m_manager->SetRingRenderer    (m_renderer->CreateRingRenderer  ());	// リング
	m_manager->SetTrackRenderer   (m_renderer->CreateTrackRenderer ());	// 軌跡
	m_manager->SetModelRenderer   (m_renderer->CreateModelRenderer ());	// モデル

	m_manager->SetModelLoader     (m_renderer->CreateModelLoader   ());	// モデルローダ
	m_manager->SetTextureLoader   (m_renderer->CreateTextureLoader ());	// テクスチャローダ

	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH   );	// 座標系

	// カメラ設定
	D3DMATRIX             d3dmat;
	Effekseer::Matrix44   matrix;

	// ビュー
	pD3DDevice->GetTransform(D3DTS_VIEW, &d3dmat);
	::CopyMemory(&matrix.Values[0][0], &d3dmat.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetCameraMatrix(matrix);

	// プロジェクション
	pD3DDevice->GetTransform(D3DTS_PROJECTION, &d3dmat);
	::CopyMemory(&matrix.Values[0][0], &d3dmat.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetProjectionMatrix(matrix);

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CEffekseer::Release()
{
	m_protected_resource.clear();
	ReleaseAllEffects();

	if(m_manager != NULL) {
		m_manager->Destroy();
		m_manager = NULL;
	}

	if(m_renderer != NULL) {
		m_renderer->Destory();
		m_renderer = NULL;
	}
}

//------------------------------------------------------------------------------
//	エフェクト生成
//------------------------------------------------------------------------------
IEffekseerEffect* CEffekseer::CreateEffectFromFile(LPCWSTR inFileName)
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_CreateEffectFromFile)\n"));
		return NULL;
	}
#endif

	IEffekseerEffect*    pEffekseerEffect;
	Effekseer::Effect*   effect = Effekseer::Effect::Create(m_manager, (const EFK_CHAR*)inFileName);
	if(effect != NULL) {
		pEffekseerEffect = new CEffekseerEffect(m_manager, effect);
	} else {
		pEffekseerEffect = new CNullEffekseerEffect();
	}
	m_effect_list.push_back(pEffekseerEffect);

	return pEffekseerEffect;
}

//------------------------------------------------------------------------------
//	エフェクト解放
//------------------------------------------------------------------------------
void CEffekseer::ReleaseEffect(IEffekseerEffect*& pEffect)
{
	m_protected_resource.erase(pEffect);
	m_effect_list.remove(pEffect);

	delete pEffect;
	pEffect = NULL;
}

//------------------------------------------------------------------------------
//	全エフェクト解放
//------------------------------------------------------------------------------
void CEffekseer::ReleaseAllEffects()
{
	std::list<IEffekseerEffect*>::iterator   list_it = m_effect_list.begin();
	if(m_protected_resource.empty()) {
		while(list_it != m_effect_list.end()) {
			delete *list_it;
			list_it++;
		}
		m_effect_list.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_protected_resource.end();
		while(list_it != m_effect_list.end()) {
			set_it = m_protected_resource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_effect_list.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	リソースプロテクト設定
//------------------------------------------------------------------------------
void CEffekseer::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_protected_resource.insert(pResource);		// プロテクト設定
	} else {
		m_protected_resource.erase (pResource);		// プロテクト解除
	}
}

/*
//------------------------------------------------------------------------------
//	ビュー設定
//------------------------------------------------------------------------------
void CEffekseer::SetView(const Effekseer::Matrix44& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_SetView)\n"));
		return;
	}
#endif

	m_renderer->SetCameraMatrix(inMatrix);
}

//------------------------------------------------------------------------------
//	プロジェクション設定
//------------------------------------------------------------------------------
void CEffekseer::SetProjection(const Effekseer::Matrix44& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_SetProjection)\n"));
		return;
	}
#endif

	m_renderer->SetProjectionMatrix(inMatrix);
}
*/

//------------------------------------------------------------------------------
//	ビュー設定
//------------------------------------------------------------------------------
void CEffekseer::SetView(D3DMATRIX& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_SetView)\n"));
		return;
	}
#endif

	Effekseer::Matrix44   matrix;
	::CopyMemory(&matrix.Values[0][0], &inMatrix.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetCameraMatrix(matrix);
}

//------------------------------------------------------------------------------
//	プロジェクション設定
//------------------------------------------------------------------------------
void CEffekseer::SetProjection(D3DMATRIX& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_SetProjection)\n"));
		return;
	}
#endif

	Effekseer::Matrix44   matrix;
	::CopyMemory(&matrix.Values[0][0], &inMatrix.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetProjectionMatrix(matrix);
}

//------------------------------------------------------------------------------
//	カメラ設定
//------------------------------------------------------------------------------
void CEffekseer::SetCamera(CCamera& inCamera)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_SetCamera)\n"));
		return;
	}
#endif

	Effekseer::Matrix44   matrix;

	::CopyMemory(&matrix.Values[0][0], &inCamera.GetViewMatrix().m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetCameraMatrix(matrix);

	::CopyMemory(&matrix.Values[0][0], &inCamera.GetProjectionMatrix().m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetProjectionMatrix(matrix);
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void CEffekseer::Update()
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_Update)\n"));
		return;
	}
#endif

	m_manager->Update();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CEffekseer::Draw()
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_Draw)\n"));
		return;
	}
#endif

	m_renderer->BeginRendering();
	m_manager ->Draw();
	m_renderer->EndRendering();
}

//------------------------------------------------------------------------------
//	カリング領域生成
//------------------------------------------------------------------------------
void CEffekseer::CreateCullingWorld(const float inWidth, const float inHeight,
									const float inDepth, const int inLayerCount)
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_CreateCullingWorld)\n"));
		return;
	}
#endif

	m_manager->CreateCullingWorld(inWidth, inHeight, inDepth, inLayerCount);
}

//------------------------------------------------------------------------------
//	カリング演算
//------------------------------------------------------------------------------
void CEffekseer::CalculateCulling()
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer未初期化(CEffekseer_CalculateCulling)\n"));
		return;
	}
#endif

	m_manager->CalcCulling(m_renderer->GetCameraProjectionMatrix(), false);
}

//------------------------------------------------------------------------------
//	スクリーン座標→Effekseerワールド座標変換
//------------------------------------------------------------------------------
D3DXVECTOR3 CEffekseer::ScreenToWorld(const float inScreenX, const float inScreenY,
									  const float inEffectPosZ)
{
	// ビューポート逆行列
	D3DVIEWPORT9   vp;
	m_renderer->GetDevice()->GetViewport(&vp);

	D3DXMATRIX   viewport;
	::ZeroMemory(viewport.m, sizeof(float) * 4 * 4);
	viewport.m[0][0] =  float(vp.Width ) / 2.0f;
	viewport.m[1][1] = -float(vp.Height) / 2.0f;
	viewport.m[2][2] =  vp.MaxZ - vp.MinZ;
	viewport.m[3][0] =  float(vp.X + vp.Width ) / 2.0f;
	viewport.m[3][1] =  float(vp.Y + vp.Height) / 2.0f;
	viewport.m[3][2] =  vp.MinZ;
	viewport.m[3][3] =  1.0f;
	::D3DXMatrixInverse(&viewport, NULL, &viewport);

	Effekseer::Matrix44   matrix;

	// プロジェクション逆行列
	matrix = m_renderer->GetProjectionMatrix();
	D3DXMATRIX   proj;
	::CopyMemory(&proj.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);
	::D3DXMatrixInverse(&proj, NULL, &proj);

	// ビュー逆行列
	matrix = m_renderer->GetCameraMatrix();
	D3DXMATRIX   view;
	::CopyMemory(&view.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);
	::D3DXMatrixInverse(&view, NULL, &view);

	// スクリーン座標→ワールド座標変換
	D3DXVECTOR3   world(inScreenX, inScreenY, 0.0f);
	::D3DXVec3TransformCoord(&world, &world, &viewport);
	::D3DXVec3TransformCoord(&world, &world, &proj);
	::D3DXVec3TransformCoord(&world, &world, &view);

	// ワールド座標補正
	world.x = (world.x - view.m[3][0]) * (inEffectPosZ - world.z) + view.m[3][0];
	world.y = (world.y - view.m[3][1]) * (inEffectPosZ - world.z) + view.m[3][1];
	world.z = inEffectPosZ;

	return world;

/*
	D3DVIEWPORT9   viewport;
	m_renderer->GetDevice()->GetViewport(&viewport);

	Effekseer::Matrix44   matrix;

	matrix = m_renderer->GetProjectionMatrix();
	D3DXMATRIX   proj;
	::CopyMemory(&proj.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);

	matrix = m_renderer->GetCameraMatrix();
	D3DXMATRIX   view;
	::CopyMemory(&view.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);

	// ワールド変換行列生成(単位行列)
	D3DXMATRIX   world;
	::D3DXMatrixIdentity(&world);

	D3DXVECTOR3   screen(inScreenX, inScreenY, 0.0f);
	D3DXVECTOR3   effekseer_world;
	::D3DXVec3Unproject(&effekseer_world, &screen, &viewport, &proj, &view, &world);

	effekseer_world.x = (effekseer_world.x - inCameraX) * (inEffectPosZ - effekseer_world.z) + inCameraX;
	effekseer_world.y = (effekseer_world.y - inCameraY) * (inEffectPosZ - effekseer_world.z) + inCameraY;
	effekseer_world.z = inEffectPosZ;

	return effekseer_world;
*/
}
