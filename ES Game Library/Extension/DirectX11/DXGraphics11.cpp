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
	 DXGraphics11.cpp
				DirectX Graphics11クラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <Effects.h>
#include "DXGraphics11.hpp"

#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "..\\..\\..\\SDK\\DirectX\\DirectX11_2\\Lib\\x86\\d3d11.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CDXGraphics11::CDXGraphics11()
	: m_pDevice(NULL), m_pContext(NULL),
	  m_pRenderTexture(NULL), m_pRenderView(NULL),
	  m_pDepthStenciTexture(NULL), m_pDepthStencilView(NULL),
	  m_pFrameBufferTexture(NULL), m_FrameBufferWidth(0), m_FrameBufferHeight(0),
	  m_pDevice9(NULL), m_pTexture9(NULL)
{
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CDXGraphics11::~CDXGraphics11()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
int CDXGraphics11::Initialize(const UINT inWidth, UINT inHeight)
{
	Release();

	D3D_FEATURE_LEVEL   level;
	try {
		HRESULT   result;

		// デバイス生成
		const D3D_FEATURE_LEVEL   feature_levels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};
		result = ::D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
									 feature_levels,
									 sizeof(feature_levels) / sizeof(feature_levels[0]),
									 D3D11_SDK_VERSION,
									 &m_pDevice, &level, &m_pContext);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - Direct3D11デバイス生成失敗(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		D3D11_TEXTURE2D_DESC   tex_desc;

		// レンダーターゲット生成
		// テクスチャ
		::ZeroMemory(&tex_desc, sizeof(tex_desc));
		tex_desc.Width              = inWidth;
		tex_desc.Height             = inHeight;
		tex_desc.MipLevels          = 1;
		tex_desc.ArraySize          = 1;
		tex_desc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
		tex_desc.SampleDesc.Count   = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage              = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags     = 0;
		tex_desc.MiscFlags          = 0;
		result = m_pDevice->CreateTexture2D(&tex_desc, NULL, &m_pRenderTexture);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - レンダーターゲットテクスチャ生成失敗(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// レンダーターゲットビュー
		result = m_pDevice->CreateRenderTargetView(m_pRenderTexture, NULL, &m_pRenderView);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - レンダーターゲットビュー生成失敗(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// デプスステンシル生成
		// テクスチャ
		::ZeroMemory(&tex_desc, sizeof(tex_desc));
		tex_desc.Width              = inWidth;
		tex_desc.Height             = inHeight;
		tex_desc.MipLevels          = 1;
		tex_desc.ArraySize          = 1;
		tex_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tex_desc.SampleDesc.Count   = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage              = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
		tex_desc.CPUAccessFlags     = 0;
		tex_desc.MiscFlags          = 0;
		result = m_pDevice->CreateTexture2D(&tex_desc, NULL, &m_pDepthStenciTexture);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - デプスステンシルテクスチャ生成失敗(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// デプスステンシルビュー
		D3D11_DEPTH_STENCIL_VIEW_DESC   dsv_desc;
		dsv_desc.Format		   = tex_desc.Format;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsv_desc.Flags         = 0;
		result = m_pDevice->CreateDepthStencilView(m_pDepthStenciTexture, &dsv_desc, &m_pDepthStencilView);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - デプスステンシルビュー生成失敗(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// フレームバッファテクスチャ生成
		::ZeroMemory(&tex_desc, sizeof(tex_desc));
		tex_desc.Width              = inWidth;
		tex_desc.Height             = inHeight;
		tex_desc.MipLevels          = 1;
		tex_desc.ArraySize          = 1;
		tex_desc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
		tex_desc.SampleDesc.Count   = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage              = D3D11_USAGE_STAGING;
		tex_desc.BindFlags          = 0;
		tex_desc.CPUAccessFlags     = D3D11_CPU_ACCESS_READ;
		tex_desc.MiscFlags          = 0;
		result = m_pDevice->CreateTexture2D(&tex_desc, NULL, &m_pFrameBufferTexture);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - フレームバッファテクスチャ生成失敗(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// テクスチャ情報取得
		m_pFrameBufferTexture->GetDesc(&tex_desc);
		m_FrameBufferWidth  = tex_desc.Width;
		m_FrameBufferHeight = tex_desc.Height;
	} catch(int err_code) {
		Release();
		return err_code;
	}

	// レンダーターゲット設定
	m_pContext->OMSetRenderTargets(1, &m_pRenderView, m_pDepthStencilView);

	// ビューポート設定
	D3D11_VIEWPORT   vp;
	vp.Width    = (float)inWidth;
	vp.Height   = (float)inHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pContext->RSSetViewports(1, &vp);

	// 戻り値
	switch(level) {
	case D3D_FEATURE_LEVEL_11_0:	return 110;	// 11.0
	case D3D_FEATURE_LEVEL_10_1:	return 101;	// 10.1
	case D3D_FEATURE_LEVEL_10_0:	return 100;	// 10.0
	}

	return 111;	// 11.1
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CDXGraphics11::Release()
{
	m_Projection = DirectX::SimpleMath::Matrix::Identity;
	m_View       = DirectX::SimpleMath::Matrix::Identity;

	m_upCommonStates.release();

	SafeRelease(m_pTexture9);
	SafeRelease(m_pDevice9);

	m_ProtectedResource.clear();
	ReleaseAllModels();

	m_FrameBufferHeight = 0;
	m_FrameBufferWidth  = 0;
	SafeRelease(m_pFrameBufferTexture);

	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStenciTexture);

	SafeRelease(m_pRenderView);
	SafeRelease(m_pRenderTexture);

	SafeRelease(m_pContext);
	SafeRelease(m_pDevice);
}

//------------------------------------------------------------------------------
//	画面クリア
//------------------------------------------------------------------------------
void CDXGraphics11::Clear(const float inR, const float inG, const float inB, const float inA)
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pContext == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectX Graphics11未初期化(CDXGraphics11_Clear)\n"));
		return;
	}
#endif
	const float  RGBA[] = {inR, inB, inG, inA};
	m_pContext->ClearRenderTargetView(m_pRenderView, RGBA);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//------------------------------------------------------------------------------
//	レンダーターゲット→フレームバッファ
//------------------------------------------------------------------------------
void CDXGraphics11::Present()
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pContext == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectX Graphics11未初期化(CDXGraphics11_Present)\n"));
		return;
	}
#endif
	m_pContext->CopyResource(m_pFrameBufferTexture, m_pRenderTexture);
}

//------------------------------------------------------------------------------
//	フレームバッファ取得
//------------------------------------------------------------------------------
void CDXGraphics11::GetFrameBuffer(void* pImage)
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pContext == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectX Graphics11未初期化(CDXGraphics11_GetFrameBuffer)\n"));
		return;
	}
#endif
//	m_pContext->CopyResource(m_pRenderCopyTexture, m_pRenderTexture);

	D3D11_MAPPED_SUBRESOURCE   map_subres;
#if !(defined(_DEBUG) || defined(DEBUG))
	m_pContext->Map(m_pFrameBufferTexture, 0, D3D11_MAP_READ, 0, &map_subres);
#else
	if(FAILED(m_pContext->Map(m_pFrameBufferTexture, 0, D3D11_MAP_READ, 0, &map_subres))) {
		::OutputDebugString(TEXT("*** Error - マップ失敗(CDXGraphics11_GetFrameBuffer)\n"));
		return;
	}
#endif
	::CopyMemory(pImage, map_subres.pData, map_subres.RowPitch * m_FrameBufferHeight);
	m_pContext->Unmap(m_pFrameBufferTexture, 0);
}

//------------------------------------------------------------------------------
//	モデル生成
//------------------------------------------------------------------------------
IModel11* CDXGraphics11::CreateModelFromFile(LPCWSTR inFileName)
{
	IModel11*   pModel = NULL;

	try {
		DirectX::EffectFactory   fx(m_pDevice);
		auto   model = DirectX::Model::CreateFromCMO(m_pDevice, inFileName, fx, false);
		pModel = new CModel11(std::move(model));
	} catch(...) {
		::OutputDebugString(TEXT("*** Error - モデル生成失敗(CDXGraphics11_CreateModelFromFile)\n"));
		pModel = new CNullModel11();
	}
	m_ModelList.push_back(pModel);

	return pModel;
}

//------------------------------------------------------------------------------
//	モデル解放
//------------------------------------------------------------------------------
void CDXGraphics11::ReleaseModel(IModel11*& pModel)
{
	m_ProtectedResource.erase(pModel);		// プロテクト解除

	m_ModelList.remove(pModel);
	delete pModel;
	pModel = NULL;
}

//------------------------------------------------------------------------------
//	全モデル解放
//------------------------------------------------------------------------------
void CDXGraphics11::ReleaseAllModels()
{
	std::list<IModel11*>::iterator  list_it = m_ModelList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_ModelList.end()) {
			delete *list_it;
			list_it++;
		}
		m_ModelList.clear();
	}else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_ModelList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_ModelList.erase(list_it);
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
void CDXGraphics11::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}

//------------------------------------------------------------------------------
//	DirectX9拡張 - 初期化
//------------------------------------------------------------------------------
int CDXGraphics11::Initialize(IDirect3DDevice9* pDevice9)
{
	assert(pDevice9 != NULL);

	// バックバッファ取得
	IDirect3DSurface9*   pSurface;
	if(pDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - バックバッファ取得失敗(CDXGraphics11_Initialize)\n"));
		return 0;
	}

	D3DSURFACE_DESC   desc;
	pSurface->GetDesc(&desc);

	// DirectX11初期化
	const int   DXVER = Initialize(desc.Width, desc.Height);
	pSurface->Release();
	if(DXVER == 0)
		return 0;

	// DirectX9テクスチャ生成
	if(::D3DXCreateTexture(pDevice9, m_FrameBufferWidth, m_FrameBufferHeight, 1, 0,
						   D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture9)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - DirectX9テクスチャ生成失敗(CDXGraphics11_Initialize)\n"));
		Release();
		return 0;
	}

	m_pDevice9 = pDevice9;
	m_pDevice9->AddRef();

	m_upCommonStates.reset(new DirectX::CommonStates(m_pDevice));

	return DXVER;
}

//------------------------------------------------------------------------------
//	DirectX9拡張 - シーン開始
//------------------------------------------------------------------------------
void CDXGraphics11::BeginScene()
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pDevice9 == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3D9未初期化(CDXGraphics11_BeginScene)\n"));
		return;
	}
#endif

	D3DXMATRIX   matrix;

	// ビュー行列設定
	m_pDevice9->GetTransform(D3DTS_VIEW, &matrix);
	::CopyMemory(m_View.m, matrix.m, sizeof(float) * 4 * 4);

	// プロジェクション行列設定
	m_pDevice9->GetTransform(D3DTS_PROJECTION, &matrix);
	::CopyMemory(m_Projection.m, matrix.m, sizeof(float) * 4 * 4);
}

//------------------------------------------------------------------------------
//	DirectX9拡張 - シーン終了
//------------------------------------------------------------------------------
void CDXGraphics11::EndScene()
{
}

//------------------------------------------------------------------------------
//	DirectX9拡張 - 描画
//------------------------------------------------------------------------------
void CDXGraphics11::Draw(IModel11*& pModel)
{
	pModel->Draw(m_pContext, *m_upCommonStates, m_View, m_Projection); 
}

//------------------------------------------------------------------------------
//	DirectX9拡張 - DirectX11テクスチャ→DirectX9テクスチャ
//------------------------------------------------------------------------------
void CDXGraphics11::MapTexture()
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pTexture9 == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3D9未初期化(CDXGraphics11_Initialize)\n"));
		return;
	}
#endif
	D3DLOCKED_RECT   lock;
	m_pTexture9->LockRect(0, &lock, NULL, 0);

	D3D11_MAPPED_SUBRESOURCE   map_subres;
	m_pContext ->Map(m_pFrameBufferTexture, 0, D3D11_MAP_READ, 0, &map_subres);

	::CopyMemory(lock.pBits, map_subres.pData, map_subres.RowPitch * m_FrameBufferHeight);

	m_pContext ->Unmap(m_pFrameBufferTexture, 0);
	m_pTexture9->UnlockRect(0);
}

//------------------------------------------------------------------------------
//	DirectX9拡張 - 演算子IDirect3DTexture9*
//------------------------------------------------------------------------------
CDXGraphics11::operator IDirect3DTexture9*() const
{
	D3DLOCKED_RECT   lock;
	m_pTexture9->LockRect(0, &lock, NULL, 0);

	D3D11_MAPPED_SUBRESOURCE   map_subres;
	m_pContext ->Map(m_pFrameBufferTexture, 0, D3D11_MAP_READ, 0, &map_subres);

	::CopyMemory(lock.pBits, map_subres.pData, map_subres.RowPitch * m_FrameBufferHeight);

	m_pContext ->Unmap(m_pFrameBufferTexture, 0);
	m_pTexture9->UnlockRect(0);

	return m_pTexture9;
}

