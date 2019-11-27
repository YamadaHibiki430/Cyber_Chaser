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
	 Sprite.cpp
				スプライトクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Sprite.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CSprite::CSprite(IDirect3DTexture9* pD3DTexture)
	: m_pD3DTexture(pD3DTexture), m_pD3DSurface(NULL),
	  m_pPool(NULL), m_PoolMax(0), m_PoolCount(0), m_hDC(NULL)
{
	assert(m_pD3DTexture != NULL);
	m_pD3DTexture->AddRef();

	// サーフェス取得
	m_pD3DTexture->GetSurfaceLevel(0, &m_pD3DSurface);

	// サーフェス情報取得
	D3DSURFACE_DESC   desc;
	m_pD3DSurface->GetDesc(&desc);
	m_Width  = (float)desc.Width;
	m_Height = (float)desc.Height;
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CSprite::~CSprite()
{
	delete[] m_pPool;

	ReleaseDC();

	m_pD3DSurface->Release();
	m_pD3DTexture->Release();
}

//------------------------------------------------------------------------------
//	描画プール領域生成
//------------------------------------------------------------------------------
DWORD CSprite::CreatePool(const DWORD inPoolMax)
{
	delete m_pPool;

	m_PoolMax   = inPoolMax;
	m_PoolCount = 0;

	m_pPool = new DXGTLVERTEX[m_PoolMax * 6];	// TRIANGLE LISTのため"×６"
	if(m_pPool == NULL)
		m_PoolMax = 0;

	return m_PoolMax;
}

//------------------------------------------------------------------------------
//	描画プール領域解放
//------------------------------------------------------------------------------
void CSprite::ReleasePool()
{
	delete m_pPool;
	m_pPool = NULL;
}

//------------------------------------------------------------------------------
//	描画プール領域登録
//------------------------------------------------------------------------------
void CSprite::DrawPool(const D3DXVECTOR3& inPos, const RECT* pSrc, const D3DCOLOR inColor)
{
#ifdef _DEBUG
	if(m_pPool == NULL) {
		::OutputDebugString(TEXT("*** Error - 描画プール領域未生成(CDXGraphics9_DrawPool)\n"));
		return;
	}
#endif

	const DWORD   IDX = m_PoolCount * 6;

	const float   P_X = inPos.x - 0.5f;
	const float   P_Y = inPos.y - 0.5f;

	// 描画プール領域へ登録
	if(pSrc != NULL) {
		const float  U_LF  = pSrc->left   / m_Width;
		const float  U_RT  = pSrc->right  / m_Width;
		const float  V_TP  = pSrc->top    / m_Height;
		const float  V_BT  = pSrc->bottom / m_Height;

		const float  SRC_W = float(pSrc->right  - pSrc->left);
		const float  SRC_H = float(pSrc->bottom - pSrc->top );

		// 頂点０
		m_pPool[IDX + 0].x     = P_X;
		m_pPool[IDX + 0].y     = P_Y;
		m_pPool[IDX + 0].z     = inPos.z;
		m_pPool[IDX + 0].color = inColor;
		m_pPool[IDX + 0].rhw   = 1.0f;
		m_pPool[IDX + 0].tu    = U_LF;
		m_pPool[IDX + 0].tv    = V_TP;

		// 頂点１
		m_pPool[IDX + 1].x     = P_X + SRC_W;
		m_pPool[IDX + 1].y     = P_Y;
		m_pPool[IDX + 1].z     = inPos.z;
		m_pPool[IDX + 1].color = inColor;
		m_pPool[IDX + 1].rhw   = 1.0f;
		m_pPool[IDX + 1].tu    = U_RT;
		m_pPool[IDX + 1].tv    = V_TP;

		// 頂点２
		m_pPool[IDX + 2].x     = P_X;
		m_pPool[IDX + 2].y     = P_Y + SRC_H;
		m_pPool[IDX + 2].z     = inPos.z;
		m_pPool[IDX + 2].color = inColor;
		m_pPool[IDX + 2].rhw   = 1.0f;
		m_pPool[IDX + 2].tu    = U_LF;
		m_pPool[IDX + 2].tv    = V_BT;

		// 頂点３
		m_pPool[IDX + 3]       = m_pPool[IDX + 1];

		// 頂点４
		m_pPool[IDX + 4].x     = P_X + SRC_W;
		m_pPool[IDX + 4].y     = P_Y + SRC_H;
		m_pPool[IDX + 4].z     = inPos.z;
		m_pPool[IDX + 4].color = inColor;
		m_pPool[IDX + 4].rhw   = 1.0f;
		m_pPool[IDX + 4].tu    = U_RT;
		m_pPool[IDX + 4].tv    = V_BT;

		// 頂点５
		m_pPool[IDX + 5]       = m_pPool[IDX + 2];
	} else {
		// 頂点０
		m_pPool[IDX + 0].x     = P_X;
		m_pPool[IDX + 0].y     = P_Y;
		m_pPool[IDX + 0].z     = inPos.z;
		m_pPool[IDX + 0].color = inColor;
		m_pPool[IDX + 0].rhw   = 1.0f;
		m_pPool[IDX + 0].tu    = 0.0f;
		m_pPool[IDX + 0].tv    = 0.0f;

		// 頂点１
		m_pPool[IDX + 1].x     = P_X + m_Width;
		m_pPool[IDX + 1].y     = P_Y;
		m_pPool[IDX + 1].z     = inPos.z;
		m_pPool[IDX + 1].color = inColor;
		m_pPool[IDX + 1].rhw   = 1.0f;
		m_pPool[IDX + 1].tu    = 1.0f;
		m_pPool[IDX + 1].tv    = 0.0f;

		// 頂点２
		m_pPool[IDX + 2].x     = P_X;
		m_pPool[IDX + 2].y     = P_Y + m_Height;
		m_pPool[IDX + 2].z     = inPos.z;
		m_pPool[IDX + 2].color = inColor;
		m_pPool[IDX + 2].rhw   = 1.0f;
		m_pPool[IDX + 2].tu    = 0.0f;
		m_pPool[IDX + 2].tv    = 1.0f;

		// 頂点３
		m_pPool[IDX + 3]       = m_pPool[IDX + 1];

		// 頂点４
		m_pPool[IDX + 4].x     = P_X + m_Width;
		m_pPool[IDX + 4].y     = P_Y + m_Height;
		m_pPool[IDX + 4].z     = inPos.z;
		m_pPool[IDX + 4].color = inColor;
		m_pPool[IDX + 4].rhw   = 1.0f;
		m_pPool[IDX + 4].tu    = 1.0f;
		m_pPool[IDX + 4].tv    = 1.0f;

		// 頂点５
		m_pPool[IDX + 5]       = m_pPool[IDX + 2];
	}

	// カウンタ処理
	m_PoolCount++;
	if(m_PoolCount >= m_PoolMax) {
		FlushPool();
	}
}

//------------------------------------------------------------------------------
//	描画プール領域登録
//------------------------------------------------------------------------------
void  CSprite::DrawPool(const float lt_x, const float lt_y, const float rt_x, const float rt_y,
						const float rb_x, const float rb_y, const float lb_x, const float lb_y,
						const float z, const RECT* pSrc, const D3DCOLOR inColor)
{
#ifdef _DEBUG
	if(m_pPool == NULL) {
		::OutputDebugString(TEXT("*** Error - 描画プール領域未生成(CDXGraphics9_DrawPool)\n"));
		return;
	}
#endif

	// 描画プール領域へ登録
	const DWORD   IDX = m_PoolCount * 6;

	// 頂点０
	m_pPool[IDX + 0].x      = lt_x - 0.5f;
	m_pPool[IDX + 0].y      = lt_y - 0.5f;
	m_pPool[IDX + 0].z      = z;
	m_pPool[IDX + 0].color  = inColor;
	m_pPool[IDX + 0].rhw    = 1.0f;

	// 頂点１
	m_pPool[IDX + 1].x      = rt_x - 0.5f;
	m_pPool[IDX + 1].y      = rt_y - 0.5f;
	m_pPool[IDX + 1].z      = z;
	m_pPool[IDX + 1].color  = inColor;
	m_pPool[IDX + 1].rhw    = 1.0f;

	// 頂点２
	m_pPool[IDX + 2].x      = lb_x - 0.5f;
	m_pPool[IDX + 2].y      = lb_y - 0.5f;
	m_pPool[IDX + 2].z      = z;
	m_pPool[IDX + 2].color  = inColor;
	m_pPool[IDX + 2].rhw    = 1.0f;

	// 頂点４
	m_pPool[IDX + 4].x      = rb_x - 0.5f;
	m_pPool[IDX + 4].y      = rb_y - 0.5f;
	m_pPool[IDX + 4].z      = z;
	m_pPool[IDX + 4].color  = inColor;
	m_pPool[IDX + 4].rhw    = 1.0f;

	// UV座標設定
	if(pSrc != NULL) {
		const float  U_LF   = pSrc->left   / m_Width;
		const float  U_RT   = pSrc->right  / m_Width;
		const float  V_TP   = pSrc->top    / m_Height;
		const float  V_BT   = pSrc->bottom / m_Height;

		m_pPool[IDX + 0].tu = U_LF;
		m_pPool[IDX + 0].tv = V_TP;

		m_pPool[IDX + 1].tu = U_RT;
		m_pPool[IDX + 1].tv = V_TP;

		m_pPool[IDX + 2].tu = U_LF;
		m_pPool[IDX + 2].tv = V_BT;

		m_pPool[IDX + 4].tu = U_RT;
		m_pPool[IDX + 4].tv = V_BT;
	} else {
		m_pPool[IDX + 0].tu = 0.0f;
		m_pPool[IDX + 0].tv = 0.0f;

		m_pPool[IDX + 1].tu = 1.0f;
		m_pPool[IDX + 1].tv = 0.0f;

		m_pPool[IDX + 2].tu = 0.0f;
		m_pPool[IDX + 2].tv = 1.0f;

		m_pPool[IDX + 4].tu = 1.0f;
		m_pPool[IDX + 4].tv = 1.0f;

	}

	// 共通頂点設定(頂点３，５)
	m_pPool[IDX + 3] = m_pPool[IDX + 1];
	m_pPool[IDX + 5] = m_pPool[IDX + 2];

	// カウンタ処理
	m_PoolCount++;
	if(m_PoolCount >= m_PoolMax) {
		FlushPool();
	}
}

//------------------------------------------------------------------------------
//	描画プール領域描画
//------------------------------------------------------------------------------
void CSprite::FlushPool()
{
#ifdef _DEBUG
	if(m_pPool == NULL) {
		::OutputDebugString(TEXT("*** Error - 描画プール領域未生成(CDXGraphics9_FlushPool)\n"));
		return;
	}
#endif

	IDirect3DDevice9*   pD3DDevice;
	m_pD3DTexture->GetDevice(&pD3DDevice);

	pD3DDevice->SetTexture(0, m_pD3DTexture);

	pD3DDevice->SetFVF(DXGFVF_TLVERTEX);
	pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_PoolCount * 2, m_pPool, sizeof(DXGTLVERTEX));

	pD3DDevice->Release();

	m_PoolCount = 0;
}

//------------------------------------------------------------------------------
//	塗りつぶし
//------------------------------------------------------------------------------
void CSprite::ColorFill(const RECT* pFill, const D3DCOLOR inColor)
{
	IDirect3DDevice9*    pD3DDevice   = NULL;
	IDirect3DSurface9*   pFillSurface = NULL;

	try {
		// Direct3Dデバイスオブジェクト取得
		if(m_pD3DTexture->GetDevice(&pD3DDevice) != D3D_OK)
			throw TEXT("デバイスオブジェクト取得失敗");

		// 塗りつぶし用のサーフェスを生成
		if(pD3DDevice->CreateOffscreenPlainSurface(16, 16, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pFillSurface, NULL)
		   != D3D_OK)
			throw TEXT("塗りつぶしサーフェス生成失敗");

		// 指定された色で塗りつぶす
		pD3DDevice->ColorFill(pFillSurface,  NULL, inColor);

		// 塗りつぶし用サーフェスをテクスチャのサーフェスへロード
		::D3DXLoadSurfaceFromSurface(m_pD3DSurface, NULL, pFill, pFillSurface, NULL, NULL, D3DX_FILTER_POINT, 0);
	} catch(LPCTSTR ErrorString) {
		// エラーメッセージ生成
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CSprite_ColorFill)\n"), ErrorString);
		::OutputDebugString(msg);
	}

	SafeRelease(pFillSurface);
	SafeRelease(pD3DDevice);
}

//------------------------------------------------------------------------------
//	ファイル読み込み
//------------------------------------------------------------------------------
void CSprite::LoadFromFile(LPCTSTR inFileName, const RECT* pSrcRect,
						   const RECT* pDestRect, const DWORD inFilter, const D3DCOLOR inColorKey)
{
	::D3DXLoadSurfaceFromFile(m_pD3DSurface, NULL, pDestRect, inFileName, pSrcRect, inFilter, inColorKey, NULL);
}

//------------------------------------------------------------------------------
//	デバイスコンテキスト取得
//------------------------------------------------------------------------------
HDC CSprite::GetDC()
{
	if(m_hDC == NULL)
		m_pD3DSurface->GetDC(&m_hDC);

	return m_hDC;
}

//------------------------------------------------------------------------------
//	デバイスコンテキスト解放
//------------------------------------------------------------------------------
void CSprite::ReleaseDC()
{
	if(m_hDC == NULL)
		return;

	m_pD3DSurface->ReleaseDC(m_hDC);
	m_hDC = NULL;
}

//------------------------------------------------------------------------------
//	ロック
//------------------------------------------------------------------------------
DXGLOCKED_DESC CSprite::Lock()
{
	// スプライト記述取得
	D3DSURFACE_DESC   surface;
	m_pD3DTexture->GetLevelDesc(0, &surface);

	// ロック
	D3DLOCKED_RECT   lock;
	m_pD3DTexture->LockRect(0, &lock, NULL, 0);

	// スプライト情報設定
	DXGLOCKED_DESC   desc;
	desc.Format = surface.Format;
	desc.Width  = surface.Width;
	desc.Height = surface.Height;
	desc.Pitch  = lock   .Pitch;
	desc.pBits  = lock   .pBits;

	return desc;
}

//------------------------------------------------------------------------------
//	ロック解除
//------------------------------------------------------------------------------
void CSprite::Unlock()
{
	m_pD3DTexture->UnlockRect(0);
}

//------------------------------------------------------------------------------
//	スプライト情報取得
//------------------------------------------------------------------------------
D3DSURFACE_DESC CSprite::GetDesc()
{
	// テクスチャ情報取得
	D3DSURFACE_DESC   desc;
	m_pD3DTexture->GetLevelDesc(0, &desc);

	return desc;
}

//------------------------------------------------------------------------------
//	ロック
//------------------------------------------------------------------------------
DXGLOCKED_DESC CNullSprite::Lock()
{
	DXGLOCKED_DESC   desc;
	::ZeroMemory(&desc, sizeof(desc));

	return desc;
}

//------------------------------------------------------------------------------
//	ロック解除
//------------------------------------------------------------------------------
void CNullSprite::Unlock()
{
}

//------------------------------------------------------------------------------
//	スプライト情報取得
//------------------------------------------------------------------------------
D3DSURFACE_DESC CNullSprite::GetDesc()
{
	D3DSURFACE_DESC   desc;
	::ZeroMemory(&desc, sizeof(desc));

	return desc;
}
