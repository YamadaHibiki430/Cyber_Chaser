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
	 Sprite3D.hpp
				Sprite3Dクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Sprite3D.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CSprite3D::CSprite3D() : m_pD3DDevice    (NULL), m_Begin(false),
						 m_pRenderSurface(NULL), m_pDepthStencil(NULL)
{
	::ZeroMemory(&m_View      , sizeof(m_View      ));
	::ZeroMemory(&m_Projection, sizeof(m_Projection));
	::ZeroMemory(&m_Viewport  , sizeof(m_Viewport  ));
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CSprite3D::~CSprite3D()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CSprite3D::Initialize(IDirect3DDevice9* pD3DDevice)
{
	assert(pD3DDevice != NULL);

	Release();

	m_pD3DDevice = pD3DDevice;
	m_pD3DDevice->AddRef();

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CSprite3D::Release()
{
	m_Begin = false;

	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	スプライト3D描画開始
//------------------------------------------------------------------------------
void CSprite3D::Begin()
{
	m_pD3DDevice->GetRenderTarget(0, &m_pRenderSurface);
	m_pD3DDevice->GetDepthStencilSurface(&m_pDepthStencil);

	m_pD3DDevice->GetTransform(D3DTS_VIEW      , &m_View);
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &m_Projection);
	m_pD3DDevice->GetViewport (&m_Viewport);

	m_Begin = true;
}

//------------------------------------------------------------------------------
//	スプライト3D描画終了
//------------------------------------------------------------------------------
void CSprite3D::End(const bool inRestoreState)
{
	if(m_Begin == false)
		return;

	if(m_pRenderSurface != NULL) {
		m_pD3DDevice->SetRenderTarget(0, m_pRenderSurface);
		m_pRenderSurface->Release();
		m_pRenderSurface = NULL;
	}

	if(m_pDepthStencil != NULL) {
		m_pD3DDevice->SetDepthStencilSurface(m_pDepthStencil);
		m_pDepthStencil->Release();
		m_pDepthStencil = NULL;
	}

	if(inRestoreState) {
		m_pD3DDevice->SetTransform(D3DTS_VIEW      , &m_View);
		m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_Projection);
		m_pD3DDevice->SetViewport (&m_Viewport);
	}

	m_Begin = false;
}

//------------------------------------------------------------------------------
//	Sprite3DLayer - コンストラクタ
//------------------------------------------------------------------------------
CSprite3DLayer::CSprite3DLayer(IDirect3DTexture9* pTexture,
							   IDirect3DSurface9* pDepthStencil,
							   const UINT inWidth, const UINT inHeight)
	: CRenderTarget(pTexture, pDepthStencil, inWidth, inHeight)
{
	::D3DXMatrixIdentity(&m_InitialView);
	::D3DXMatrixIdentity(&m_InitialProjection);
}

//------------------------------------------------------------------------------
//	Sprite3DLayer - 初期レンダーステート設定
//------------------------------------------------------------------------------
void CSprite3DLayer::SetInitialRenderState(const D3DRENDERSTATETYPE inState,
										   const DWORD inValue)
{
	STATE   state;
	state.Type    = inState;
	state.Value   = inValue;
	state.Restore = inValue;

	m_StateList.push_back(state);
}

//------------------------------------------------------------------------------
//	Sprite3DLayer - レンダリング開始
//------------------------------------------------------------------------------
void CSprite3DLayer::BeginRender(const bool inInitView, const bool inInitProj, 
								 const bool inSetState)
{
	CRenderTarget::Use();

	if(inInitView)
		m_pD3DDevice->SetTransform(D3DTS_VIEW      , &m_InitialView);
	if(inInitProj)
		m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_InitialProjection);

	if(inSetState) {
		std::list<STATE>::iterator   rit;

		// ステート保存
		for(rit = m_StateList.begin(); rit != m_StateList.end(); rit++)
			m_pD3DDevice->GetRenderState((*rit).Type, &((*rit).Restore));

		// ステート設定
		for(rit = m_StateList.begin(); rit != m_StateList.end(); rit++)
			m_pD3DDevice->SetRenderState((*rit).Type, (*rit).Value);
	}

}

//------------------------------------------------------------------------------
//	Sprite3DLayer - レンダリング終了
//------------------------------------------------------------------------------
void CSprite3DLayer::EndRender(const bool inRestoreState)
{
	if(inRestoreState == false)
		return;

	std::list<STATE>::iterator   rit;
	for(rit = m_StateList.begin(); rit != m_StateList.end(); rit++)
		m_pD3DDevice->SetRenderState((*rit).Type, (*rit).Restore);
}

//------------------------------------------------------------------------------
//	Sprite3DLayer - レンダリング領域設定
//------------------------------------------------------------------------------
void CSprite3DLayer::SetRenderRect(const RECT& inRenderRect)
{
	D3DVIEWPORT9   view;
	view.X      = inRenderRect.left;
	view.Y      = inRenderRect.top;
	view.Width  = inRenderRect.right  - inRenderRect.left;
	view.Height = inRenderRect.bottom - inRenderRect.top;
	view.MinZ   = 0.0f;
	view.MaxZ   = 1.0f;

	m_pD3DDevice->SetViewport(&view);
}
