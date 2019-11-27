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
	 SpriteBatch.cpp
				SpriteBatchクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SpriteBatch.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CSpriteBatch::CSpriteBatch() : m_pD3DDevice(NULL), m_pD3DXSprite(NULL)
{
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CSpriteBatch::~CSpriteBatch()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CSpriteBatch::Initialize(ID3DXSprite*& pD3DXSprite)
{
	assert(pD3DXSprite != NULL);

	Release();

	m_pD3DXSprite = pD3DXSprite;
	m_pD3DXSprite->AddRef();

	m_pD3DXSprite->GetDevice(&m_pD3DDevice);

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CSpriteBatch::Release()
{
	SafeRelease(m_pD3DXSprite);
	SafeRelease(m_pD3DDevice );
}

//------------------------------------------------------------------------------
//	スプライト描画開始
//------------------------------------------------------------------------------
void CSpriteBatch::Begin()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);

	// プロジェクション行列設定
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	スプライト描画開始
//------------------------------------------------------------------------------
void CSpriteBatch::Begin(const DWORD inBlendMode, const DWORD inSortMode)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	if(inBlendMode == 0) {
		m_pD3DXSprite->Begin(inSortMode);
	} else {
		m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | inSortMode);
		m_pD3DDevice ->SetTextureStageState(0, D3DTSS_COLOROP, inBlendMode);
	}

	// プロジェクション行列設定
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	スプライト描画開始
//------------------------------------------------------------------------------
void CSpriteBatch::BeginFast()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT);

	// プロジェクション行列設定
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	スプライト描画開始(右手系)
//------------------------------------------------------------------------------
void CSpriteBatch::BeginRH()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_BeginRH)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT);

	// ビュー行列設定
	D3DXMATRIX   view;
	::D3DXMatrixIdentity(&view);
	view._33 = -view._33;
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

	// プロジェクション行列設定
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	スプライト描画開始(右手系)
//------------------------------------------------------------------------------
void CSpriteBatch::BeginRH(const DWORD inBlendMode, const DWORD inSortMode)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_BeginRH)\n"));
		return;
	}
#endif

	if(inBlendMode == 0) {
		m_pD3DXSprite->Begin(inSortMode);
	} else {
		m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | inSortMode);
		m_pD3DDevice ->SetTextureStageState(0, D3DTSS_COLOROP, inBlendMode);
	}

	// ビュー行列設定
	D3DXMATRIX   view;
	::D3DXMatrixIdentity(&view);
	view._33 = -view._33;
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

	// プロジェクション行列設定
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	スプライト描画開始(右手系)
//------------------------------------------------------------------------------
void CSpriteBatch::BeginFastRH()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);

	// ビュー行列設定
	D3DXMATRIX   view;
	::D3DXMatrixIdentity(&view);
	view._33 = -view._33;
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

	// プロジェクション行列設定
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	スプライト描画終了
//------------------------------------------------------------------------------
void CSpriteBatch::End()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_End)\n"));
		return;
	}
#endif

	m_pD3DXSprite->End();
}

//------------------------------------------------------------------------------
//	スプライト変換行列初期化
//------------------------------------------------------------------------------
void CSpriteBatch::InitTransform()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_InitTransform)\n"));
		return;
	}
#endif

	D3DXMATRIX   mat;
	::D3DXMatrixIdentity(&mat);
	m_pD3DXSprite->SetTransform(&mat);
}

//------------------------------------------------------------------------------
//	スプライト変換行列設定
//------------------------------------------------------------------------------
void CSpriteBatch::SetTransform(const D3DXMATRIX& inTransform)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_SetTransform)\n"));
		return;
	}
#endif

	m_pD3DXSprite->SetTransform(&inTransform);
}

//------------------------------------------------------------------------------
//	スプライト描画フィルター設定
//------------------------------------------------------------------------------
void CSpriteBatch::SetFilter(const D3DTEXTUREFILTERTYPE inFilterType)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_SetFilter)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, inFilterType);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, inFilterType);
}

//------------------------------------------------------------------------------
//	単純描画
//------------------------------------------------------------------------------
void CSpriteBatch::DrawSimple(IDirect3DTexture9* pTexture,
							  const RECT* pSrc, const D3DXVECTOR3& inPos,
							  const D3DCOLOR inColorMask)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_DrawSimple)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Draw(pTexture, pSrc, NULL, &inPos, inColorMask);
}

//------------------------------------------------------------------------------
//	多機能描画
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(IDirect3DTexture9* pTexture,
						const RECT* pSrc, const D3DXVECTOR3& inPos,
						const D3DCOLOR inColorMask)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	// 変換行列初期化
	D3DXMATRIX   mat;
	m_pD3DXSprite->SetTransform(::D3DXMatrixIdentity(&mat));

	m_pD3DXSprite->Draw(pTexture, pSrc, NULL, &inPos, inColorMask);
}

//------------------------------------------------------------------------------
//	多機能描画
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(IDirect3DTexture9* pTexture, const RECT* pSrc,
						const D3DXVECTOR3& inPos,
						const D3DXVECTOR2& inScale,
						const D3DXVECTOR3& inRotation, const D3DXVECTOR3* pCenter,
						const D3DCOLOR     inColorMask)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	// 変換行列設定
	D3DXMATRIX   transform;

	// 回転
	::D3DXMatrixRotationYawPitchRoll(&transform,
									 D3DXToRadian(inRotation.y),
									 D3DXToRadian(inRotation.x),
									 D3DXToRadian(inRotation.z));
/*
	D3DXQUATERNION   rot;
	::D3DXQuaternionRotationYawPitchRoll(&rot,
										 D3DXToRadian(inRotation.y),
										 D3DXToRadian(inRotation.x),
										 D3DXToRadian(inRotation.z));
	::D3DXMatrixRotationQuaternion(&transform, &rot);
*/

	// スケーリング
	transform._11 *= inScale.x;
	transform._12 *= inScale.x;
	transform._13 *= inScale.x;

	transform._21 *= inScale.y;
	transform._22 *= inScale.y;
	transform._23 *= inScale.y;

	// 移動
	transform._41  = inPos.x;
	transform._42  = inPos.y;
	transform._43  = inPos.z;

	transform._44  = 1.0f;

	// 描画
	m_pD3DXSprite->SetTransform(&transform);
	m_pD3DXSprite->Draw(pTexture, pSrc, pCenter, NULL, inColorMask);
}

//------------------------------------------------------------------------------
//	SpriteElement描画
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(const SpriteElement& inSpriteElement)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	D3DXMATRIX   transform;

	// 回転
	::D3DXMatrixRotationYawPitchRoll(&transform,
									 D3DXToRadian(inSpriteElement.rotation.y),
									 D3DXToRadian(inSpriteElement.rotation.x),
									 D3DXToRadian(inSpriteElement.rotation.z));

	// スケーリング
	transform._11 *= inSpriteElement.scale.x;
	transform._12 *= inSpriteElement.scale.x;
	transform._13 *= inSpriteElement.scale.x;

	transform._21 *= inSpriteElement.scale.y;
	transform._22 *= inSpriteElement.scale.y;
	transform._23 *= inSpriteElement.scale.y;

	// 移動
	transform._41  = inSpriteElement.position.x;
	transform._42  = inSpriteElement.position.y;
	transform._43  = inSpriteElement.position.z;

	transform._44  = 1.0f;

	// 透明度
	const DWORD   COLOR_MASK = ((DWORD)(inSpriteElement.alpha * 255.0f) << 24) | 0x00ffffff;

	m_pD3DXSprite->SetTransform(&transform);
	m_pD3DXSprite->Draw(inSpriteElement.pTexture,    &inSpriteElement.srcRect,
					   &inSpriteElement.anchorPoint, NULL, COLOR_MASK);
}

//------------------------------------------------------------------------------
//	Box2D拡張
//------------------------------------------------------------------------------
#ifdef BOX2D_H

// スタティックリンクライブラリ
#ifndef _DEBUG
	#pragma comment(lib, "Box2D.lib")
#else
	#pragma comment(lib, "Box2Dd.lib")
#endif

// 静的メンバ
float PhysicsSprite::PTMRatio = 32;

//------------------------------------------------------------------------------
//	PhysicsSprite描画
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(const PhysicsSprite& inPhysicsSprite)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Draw)\n"));
		return;
	}
#endif
#ifdef _DEBUG
	if(inPhysicsSprite.pBody == NULL) {
		::OutputDebugString(TEXT("*** Error - Body未初期化(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	D3DXMATRIX   transform;
	b2Vec2       vec2;

	// 回転
	const float   z_angle = inPhysicsSprite.pBody->GetAngle();
	::D3DXMatrixRotationZ(&transform, D3DXToRadian(z_angle));

	// スケーリング


	// 移動
	vec2           = inPhysicsSprite.pBody->GetPosition();
	transform._41  = vec2.x * PhysicsSprite::PTMRatio;
	transform._42  = vec2.y * PhysicsSprite::PTMRatio;
	transform._43  = inPhysicsSprite.z;

	transform._44  = 1.0f;

	// 回転軸
	D3DXVECTOR3   anchor;
	vec2 = inPhysicsSprite.pBody->GetLocalCenter();
	anchor.x = vec2.x * PhysicsSprite::PTMRatio + inPhysicsSprite.anchorPoint.x;
	anchor.y = vec2.y * PhysicsSprite::PTMRatio + inPhysicsSprite.anchorPoint.y;
	anchor.z = 0.0f;

	// 透明度
	const DWORD   COLOR_MASK = ((DWORD)(inPhysicsSprite.alpha * 255.0f) << 24) | 0x00ffffff;

	m_pD3DXSprite->SetTransform(&transform);
	m_pD3DXSprite->Draw(inPhysicsSprite.pTexture, &inPhysicsSprite.srcRect,
					   &anchor, NULL, COLOR_MASK);
}

#endif

