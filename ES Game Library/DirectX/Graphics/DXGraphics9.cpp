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
	 DXGraphics9.cpp
				DirectX Graphics9クラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DXGraphics9.hpp"

#pragma warning(push)
#pragma warning(disable:4005)
#include <D2D1.h>
#include <wincodec.h>
#pragma warning(pop)

#include <tchar.h>
#include <vector>

// #include <olectl.h>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "windowscodecs.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CDXGraphics9::CDXGraphics9() : m_pD3D(NULL),			    m_pD3DDevice(NULL),
							   m_pD3DXSprite(NULL),         m_pD3DXEffect(NULL),
							   m_Width(0),                  m_Height(0),
							   m_hBackBufferDC(NULL),
							   m_pBackBufferSurface(NULL),  m_pDepthStencilSurface(NULL),
							   m_pRenderTargetVertex(NULL),
							   m_RenderTargetWidth(0),      m_RenderTargetHeight(0)
{
	::ZeroMemory(&m_PresentParams, sizeof(m_PresentParams));
	::ZeroMemory(&m_DisplayMode,   sizeof(m_DisplayMode  ));
	::ZeroMemory(&m_AlphaState,    sizeof(m_AlphaState   ));

	m_pPresentRect[0] = NULL;
	m_pPresentRect[1] = NULL;
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CDXGraphics9::~CDXGraphics9()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CDXGraphics9::Initialize(const HWND hWnd, const UINT inWidth, UINT inHeight,
							  const bool inWindowed)
{
	Release();

	// Direct3D9オブジェクト生成
	m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
	if(m_pD3D == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3D9オブジェクト生成失敗(CDXGraphics9_Initialize)\n")); 
		return false;
	}

	m_Width  = inWidth;
	m_Height = inHeight;

	// 解像度検索
	D3DDISPLAYMODE   dpm;
	D3DFORMAT        format[3] = {D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8};

	m_DisplayMode.Format = D3DFMT_UNKNOWN;
	for(UINT fmt = 0; fmt < 3; fmt++) {
		const UINT   AdapterModeCount = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, format[fmt]);
		for(UINT amc = 0; amc < AdapterModeCount; amc++) {
			if(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, format[fmt], amc, &dpm) != D3D_OK)
				continue;
			if(m_Width <= dpm.Width && m_Height <= dpm.Height) {
				m_DisplayMode.Width  = dpm.Width;
				m_DisplayMode.Height = dpm.Height;
				m_DisplayMode.Format = dpm.Format;
				break;
			}	// if
		}	// for(amc)
	}	// for(fmt)

	if(m_DisplayMode.Format == D3DFMT_UNKNOWN) {
		::OutputDebugString(TEXT("*** Error - ディスプレイ未対応(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// プレゼンテーションパラメータ － 共通部設定
	m_PresentParams.BackBufferCount            = 1;										// バックバッファ数
	m_PresentParams.MultiSampleType            = D3DMULTISAMPLE_NONE;					// マルチサンプルタイプ
	m_PresentParams.MultiSampleQuality         = 0;										// マルチサンプル品質
	m_PresentParams.hDeviceWindow              = hWnd;									// ターゲットウィンドウ
	m_PresentParams.EnableAutoDepthStencil     = TRUE;									// 深度バッファ
	m_PresentParams.AutoDepthStencilFormat     = D3DFMT_D24S8;							// 深度バッファフォーマット
	m_PresentParams.Flags                      = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// フラグ
	m_PresentParams.FullScreen_RefreshRateInHz = 0;										// リフレッシュレート

	// プレゼンテーションパラメータ － 非共通部設定
	SetPresentParams(inWindowed);

	// Direct3DDevice9オブジェクト生成
	const DWORD   Behavior[3] = {D3DCREATE_MIXED_VERTEXPROCESSING    | D3DCREATE_MULTITHREADED,
								 D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
								 D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED};
	HRESULT       hr;
	for(int i = 0; i < 3; i++) {
		hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
								  Behavior[i], &m_PresentParams, &m_pD3DDevice);
		if(hr == D3D_OK)
			break;
	}
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9オブジェクト生成失敗(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// 画面消去
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

	// スプライトインタフェース生成
	if(::D3DXCreateSprite(m_pD3DDevice, &m_pD3DXSprite) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - スプライトインタフェース生成失敗(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// スプライトバッチ初期化
	if(DXGSpriteBatch().Initialize(m_pD3DXSprite) == false) {
//		::OutputDebugString(TEXT("*** Error - スプライトバッチ初期化失敗(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// バックバッファサーフェス取得
	if(m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - バックバッファサーフェス取得失敗(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// デプスステンシルサーフェス取得
	if(m_pD3DDevice->GetDepthStencilSurface(&m_pDepthStencilSurface) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - デプスステンシルサーフェス取得失敗(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CDXGraphics9::Release()
{
	m_ProtectedResource.clear();

	ReleaseAllRenderTargets();				// 全レンダリングターゲット解放
	ReleaseAllStateBlocks();				// 全ステートブロック解放
	ReleaseAllFonts();						// 全フォント解放
	ReleaseAllSprites();					// 全スプライト解放
	ReleaseAllAnimationModels();			// 全アニメーションモデル解放
	ReleaseAllModels();						// 全モデル解放
	ReleaseAllVertexBuffers();				// 全バーテックスバッファ解放
	ReleaseAllEffects();					// 全エフェクト解放

	ReleaseDC();

	SafeRelease(m_pDepthStencilSurface);
	SafeRelease(m_pBackBufferSurface);

	DXGSpriteBatch().Release();

	SafeRelease(m_pD3DXEffect);
	SafeRelease(m_pD3DXSprite);
	SafeRelease(m_pD3DDevice);
	SafeRelease(m_pD3D);

	delete m_pPresentRect[1];
	m_pPresentRect[1] = NULL;

	delete m_pPresentRect[0];
	m_pPresentRect[0] = NULL;

	::ZeroMemory(&m_DisplayMode,   sizeof(m_DisplayMode  ));
	::ZeroMemory(&m_PresentParams, sizeof(m_PresentParams));

	m_RenderTargetHeight = 0;
	m_RenderTargetWidth  = 0;

	m_Height = 0;
	m_Width  = 0;
}

//------------------------------------------------------------------------------
//	デバイスリセット
//------------------------------------------------------------------------------
bool CDXGraphics9::ResetDevice(const bool inTestDevice)
{
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_ResetDevice)\n"));
		return false;
	}

	// デバイスがロストしているか？
	if(inTestDevice) {
		if(m_pD3DDevice->TestCooperativeLevel() == D3D_OK)
			return true;
	}

	// ライト保存
	UINT   idx = 0;
	std::vector<DXGLIGHT>       LightData(m_LightIndex.size());
	std::set<DWORD>::iterator   light_it;
	for(light_it = m_LightIndex.begin(); light_it != m_LightIndex.end(); light_it++) {
		LightData[idx].Index = *light_it;
		m_pD3DDevice->GetLight      (*light_it, &LightData[idx].Light);
		m_pD3DDevice->GetLightEnable(*light_it, &LightData[idx].Enable);
	}

	// レンダーターゲットリセット前処理
	std::list<IRenderTarget*>::iterator     render_it;
	for(render_it = m_RenderTarget.begin(); render_it != m_RenderTarget.end(); render_it++)
		(*render_it)->OnLostDevice();

	// フォントリセット前処理
	std::list<IDXGFont*>::iterator     font_it;
	for(font_it = m_Font.begin(); font_it != m_Font.end(); font_it++)
		(*font_it)->OnLostDevice();

	// エフェクトリセット前処理
	std::list<IEffect*>::iterator   effect_it;
	for(effect_it = m_Effect.begin(); effect_it != m_Effect.end(); effect_it++)
		(*effect_it)->OnLostDevice();

	// スプライトバッチリセット前処理
	m_pD3DXSprite->OnLostDevice();

	// バックバッファリセット前処理
	SafeRelease(m_pDepthStencilSurface);
	SafeRelease(m_pBackBufferSurface  );

	// デバイスリセット
	if(m_pD3DDevice->Reset(&m_PresentParams) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - デバイスリセット失敗(CDXGraphics9_ResetDevice)\n"));
		return false;
	}
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

	// バックバッファリセット後処理
	m_pD3DDevice ->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface);
	m_pD3DDevice ->GetDepthStencilSurface(&m_pDepthStencilSurface);

	// スプライトバッチリセット後処理
	m_pD3DXSprite->OnResetDevice();

	// エフェクトリセット後処理
	for(effect_it = m_Effect.begin(); effect_it != m_Effect.end(); effect_it++)
		(*effect_it)->OnResetDevice();

	// フォントリセット後処理
	for(font_it = m_Font.begin(); font_it != m_Font.end(); font_it++)
		(*font_it)->OnResetDevice();

	// レンダーターゲットリセット後処理
	for(render_it = m_RenderTarget.begin(); render_it != m_RenderTarget.end(); render_it++)
		(*render_it)->OnResetDevice();

	// レンダリングステート復元
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator   state_it;
	for(state_it = m_RenderState.begin(); state_it != m_RenderState.end(); state_it++) {
		if(state_it->second.Value != state_it->second.DefValue)
			m_pD3DDevice->SetRenderState(state_it->first, state_it->second.Value);
	}

	// ライト復元
	for(idx = 0; idx < LightData.size(); idx++){
		m_pD3DDevice->SetLight   (LightData[idx].Index,	&LightData[idx].Light);
		m_pD3DDevice->LightEnable(LightData[idx].Index,	 LightData[idx].Enable);
	}

	return true;
}

//------------------------------------------------------------------------------
//	モード変更
//------------------------------------------------------------------------------
bool CDXGraphics9::ChangeMode(const bool inWindowed)
{
	if(m_PresentParams.Windowed == static_cast<BOOL>(inWindowed))
		return true;

	m_PresentParams.Windowed = inWindowed;
	SetPresentParams(m_PresentParams.Windowed);		// 新しいパラメータを設定する
	return ResetDevice(false);						// 新しいパラメータでデバイスをリセットする
}

//------------------------------------------------------------------------------
//	WM_PAINTメッセージ処理
//------------------------------------------------------------------------------
void CDXGraphics9::OnPaint(const PAINTSTRUCT& inPaint)
{
	if(m_pD3DDevice == NULL)
		return;

	HDC   hSrcDC = GetDC();

	// 再描画しなければならない領域をクライアント領域に転送する
	::BitBlt(inPaint.hdc, inPaint.rcPaint.left, inPaint.rcPaint.top,
			 inPaint.rcPaint.right - inPaint.rcPaint.left, inPaint.rcPaint.bottom - inPaint.rcPaint.top,
			 hSrcDC, inPaint.rcPaint.left, inPaint.rcPaint.top, SRCCOPY);

	ReleaseDC();
}

//------------------------------------------------------------------------------
//	プレゼンテーションパラメータ設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetPresentParams(const BOOL inWindowed)
{
	if(inWindowed != FALSE) {
		// ウィンドウモード
		m_PresentParams.Windowed             = TRUE;								// 動作モード
		m_PresentParams.BackBufferWidth      = 0;									// バックバッファ幅
		m_PresentParams.BackBufferHeight     = 0;									// バックバッファ高さ
		m_PresentParams.BackBufferFormat     = D3DFMT_UNKNOWN;						// バックバッファフォーマット
		m_PresentParams.SwapEffect           = D3DSWAPEFFECT_DISCARD;				// スワップエフェクト
//		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;		// スワップインターバル
		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;				// スワップインターバル

		delete m_pPresentRect[1];
		m_pPresentRect[1] = NULL;

		delete m_pPresentRect[0];
		m_pPresentRect[0] = NULL;
	} else {
		// フルスクリーンモード
		m_PresentParams.Windowed             = FALSE;								// 動作モード
		m_PresentParams.BackBufferWidth      = m_DisplayMode.Width;					// バックバッファ幅
		m_PresentParams.BackBufferHeight     = m_DisplayMode.Height;				// バックバッファ高さ
		m_PresentParams.BackBufferFormat     = m_DisplayMode.Format;				// バックバッファフォーマット
		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;				// スワップインターバル

		// スワップエフェクト
		if(m_DisplayMode.Width == m_Width && m_DisplayMode.Height == m_Height) {
			m_PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		} else {
			m_PresentParams.SwapEffect = D3DSWAPEFFECT_COPY;

			// 描画領域転送元設定
			m_pPresentRect[0] = new RECT;
			m_pPresentRect[0]->left   = 0;
			m_pPresentRect[0]->top    = 0;
			m_pPresentRect[0]->right  = m_pPresentRect[0]->left + m_Width;
			m_pPresentRect[0]->bottom = m_pPresentRect[0]->top  + m_Height;

			// 描画領域転送先設定
			m_pPresentRect[1] = new RECT;
			m_pPresentRect[1]->left   = (m_DisplayMode.Width  - m_Width ) / 2;
			m_pPresentRect[1]->top    = (m_DisplayMode.Height - m_Height) / 2;
			m_pPresentRect[1]->right  = m_pPresentRect[1]->left + m_Width;
			m_pPresentRect[1]->bottom = m_pPresentRect[1]->top  + m_Height;
		}
	}
}

//------------------------------------------------------------------------------
//	ビューポートクリア
//------------------------------------------------------------------------------
void CDXGraphics9::Clear(const D3DCOLOR inColor, const DWORD inFlags,
						 const D3DRECT* inRect , const DWORD inRectCount)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_Clear)\n"));
		return;
	}
#endif

	m_pD3DDevice->Clear(inRectCount, inRect, inFlags, inColor, 1.0f, 0);
}

//------------------------------------------------------------------------------
//	フレーム更新
//------------------------------------------------------------------------------
//void CDXGraphics9::UpdateFrame(const bool inWaitVSync)
void CDXGraphics9::UpdateFrame()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_UpdateFrame)\n"));
		return;
	}
#endif
/*
	// 垂直帰線を待つ
	if(inWaitVSync == true) {
		D3DRASTER_STATUS   rs;
		if(m_PresentParams.Windowed == TRUE) {
			// ウィンドウモード
			// クライアント領域を取得し、スクリーン座標へ変換
			RECT    window;
			::GetClientRect(m_PresentParams.hDeviceWindow, &window);
			POINT   pt = {0, 0};
			::ClientToScreen(m_PresentParams.hDeviceWindow, &pt);
			::OffsetRect(&window, pt.x, pt.y);

			while(true) {
				if(m_pD3DDevice->GetRasterStatus(0, &rs) != D3D_OK)
					break;
				if(rs.InVBlank == TRUE)
					break;			// 垂直帰線中
				if(m_PresentParams.Windowed == TRUE) {
					if(static_cast<int>(rs.ScanLine) < window.top || static_cast<int>(rs.ScanLine) >= window.bottom)
						break;		// 走査線の位置的にテアリングしない
				}
				::Sleep(0);
			}	// while(true)
		} else {
			// フルスクリーンモード
			while(true) {
				if(m_pD3DDevice->GetRasterStatus(0, &rs) != D3D_OK)
					break;
				if(rs.InVBlank == TRUE)
					break;			// 垂直帰線中
				::Sleep(0);
			}	// while(true)
		}	// if(m_PresentParams.Windowed)
	}	// if(inWaitVSync)
*/
	if(m_pD3DDevice->Present(m_pPresentRect[0], m_pPresentRect[1], NULL, NULL) != D3D_OK) {
		if(m_pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice(false);
	}
}

//------------------------------------------------------------------------------
//	ビューポートリセット
//------------------------------------------------------------------------------
void CDXGraphics9::ResetViewport()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_ResetViewport)\n"));
		return;
	}
#endif

	D3DVIEWPORT9   view;
	view.X      = 0;
	view.Y      = 0;
	view.Width  = m_Width;
	view.Height = m_Height;
	view.MinZ   = 0.0f;
	view.MaxZ   = 1.0f;

	if(m_pD3DDevice->SetViewport(&view) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - ビューポート設定失敗(CDXGraphics9_ResetViewport)\n"));
}

//------------------------------------------------------------------------------
//	ビューポート設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetViewport(const D3DVIEWPORT9& view)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetViewport)\n"));
		return;
	}
#endif

	if(m_pD3DDevice->SetViewport(&view) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - ビューポート設定失敗(CDXGraphics9_SetViewport)\n"));
}

//------------------------------------------------------------------------------
//	ビューポート情報取得
//------------------------------------------------------------------------------
D3DVIEWPORT9 CDXGraphics9::GetViewport()
{
	D3DVIEWPORT9   view;
	::ZeroMemory(&view, sizeof(view));

#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_GetViewport)\n"));
		return view;
	}
#endif

	if(m_pD3DDevice->GetViewport(&view) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - ビューポート設定失敗(CDXGraphics9_GetViewport)\n"));

	return view;
}

//------------------------------------------------------------------------------
//	ビューポート変換行列取得
//------------------------------------------------------------------------------
D3DXMATRIX CDXGraphics9::GetViewportMatrix()
{
	D3DVIEWPORT9   viewport = GetViewport();

	D3DXMATRIX     mat;
	::ZeroMemory(&mat, sizeof(mat));

	mat._11 = viewport.Width / 2.0f;
	mat._22 = -static_cast<float>(viewport.Height) / 2.0f;
	mat._33 = viewport.MaxZ - viewport.MinZ;
	mat._41 = viewport.X + viewport.Width  / 2.0f;
	mat._42 = viewport.Y + viewport.Height / 2.0f;
	mat._43 = viewport.MinZ;
	mat._44 = 1.0f;

	return mat;
}

//------------------------------------------------------------------------------
//	変換行列取得
//------------------------------------------------------------------------------
D3DXMATRIX CDXGraphics9::GetTransform(const D3DTRANSFORMSTATETYPE inTransType)
{
	D3DXMATRIX   transform;

#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_GetTransform)\n"));
		::ZeroMemory(&transform, sizeof(transform));
		return transform;
	}
#endif

	if(m_pD3DDevice->GetTransform(inTransType, &transform) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 変換行列取得失敗(CDXGraphics9_GetTransform)\n"));
		::D3DXMatrixIdentity(&transform);
	}

	return transform;
}

//------------------------------------------------------------------------------
//	ワールド変換行列設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetTransform(const D3DTRANSFORMSTATETYPE inTransType,
								const D3DXMATRIX& inTransform)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetTransform)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetTransform(inTransType, &inTransform);
}

//------------------------------------------------------------------------------
//	ワールド座標→スクリーン座標
//------------------------------------------------------------------------------
D3DXVECTOR3 CDXGraphics9::WorldToScreen(const D3DXVECTOR3& inWorld)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_WorldToScreen)\n"));
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
#endif

	// ビュー行列取得
	D3DXMATRIX   view;
	m_pD3DDevice->GetTransform(D3DTS_VIEW, &view);

	// プロジェクション行列取得
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);

	// ワールド座標→ビューポート(スクリーン)座標
	D3DXVECTOR3   screen = inWorld;
	::D3DXVec3TransformCoord(&screen, &screen, &view);
	::D3DXVec3TransformCoord(&screen, &screen, &proj);
	::D3DXVec3TransformCoord(&screen, &screen, &GetViewportMatrix());

	return screen;
}

//------------------------------------------------------------------------------
//	スクリーン座標→ワールド座標
//------------------------------------------------------------------------------
D3DXVECTOR3 CDXGraphics9::ScreenToWorld(const float inScreenX, const float inScreenY,
										const float inWorldPosZ)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_ScreenToWorld)\n"));
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
#endif

	// ビューポート逆行列
	D3DVIEWPORT9   vp;
	m_pD3DDevice->GetViewport(&vp);

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

	// プロジェクション逆行列
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	::D3DXMatrixInverse(&proj, NULL, &proj);

	// ビュー逆行列
	D3DXMATRIX   view;
	m_pD3DDevice->GetTransform(D3DTS_VIEW, &view);
	::D3DXMatrixInverse(&view, NULL, &view);

	// スクリーン座標→ワールド座標変換
	D3DXVECTOR3   world(inScreenX, inScreenY, 0.0f);
	::D3DXVec3TransformCoord(&world, &world, &viewport);
	::D3DXVec3TransformCoord(&world, &world, &proj);
	::D3DXVec3TransformCoord(&world, &world, &view);

	// ワールド座標補正
	world.x = (world.x - view.m[3][0]) * (inWorldPosZ - world.z) + view.m[3][0];
	world.y = (world.y - view.m[3][1]) * (inWorldPosZ - world.z) + view.m[3][1];
	world.z = inWorldPosZ;

	return world;
}

//------------------------------------------------------------------------------
//	シーン開始
//------------------------------------------------------------------------------
bool CDXGraphics9::BeginScene()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_BeginScene)\n"));
		return false;
	}
	if(m_pD3DDevice->BeginScene() != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - シーン開始失敗(CDXGraphics9_BeginScene)\n"));
		return false;
	}
#else
	m_pD3DDevice->BeginScene();
#endif

	return true;
}

//------------------------------------------------------------------------------
//	シーン終了
//------------------------------------------------------------------------------
void CDXGraphics9::EndScene()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_EndScene)\n"));
		return;
	}
#endif

	m_pD3DDevice->EndScene();
}

//------------------------------------------------------------------------------
//	プリミティブ描画
//------------------------------------------------------------------------------
void CDXGraphics9::DrawUserPrimitives(const D3DPRIMITIVETYPE inType, const void* pData,
									  const UINT inCount, const DWORD inFVF, const UINT inStride)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_DrawPrimitive)\n"));
		return;
	}
#endif

	const UINT   STRIDE = inStride == 0 ? ::D3DXGetFVFVertexSize(inFVF) : inStride;

	m_pD3DDevice->SetFVF(inFVF);
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, pData, STRIDE);
}

//------------------------------------------------------------------------------
//	ライト設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetLight(const D3DLIGHT9& inLight, const DWORD inIndex, const BOOL inEnable)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetLight)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetLight   (inIndex, &inLight);
	m_pD3DDevice->LightEnable(inIndex, inEnable);

	m_LightIndex.insert(inIndex);
}

//------------------------------------------------------------------------------
//	ライト設定
//------------------------------------------------------------------------------
void CDXGraphics9::EnableLight(const DWORD inIndex, const BOOL inEnable)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_EnableLight)\n"));
		return;
	}
#endif

	m_pD3DDevice->LightEnable(inIndex, inEnable);
}

//------------------------------------------------------------------------------
//	ライトクリア
//------------------------------------------------------------------------------
void CDXGraphics9::ClearLight()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL)
		return;
#endif

	std::set<DWORD>::iterator   it;
	for(it = m_LightIndex.begin(); it != m_LightIndex.end(); it++)
		m_pD3DDevice->LightEnable(*it, FALSE);

	m_LightIndex.clear();
}

//------------------------------------------------------------------------------
//	ライト取得
//------------------------------------------------------------------------------
D3DLIGHT9 CDXGraphics9::GetLight(const DWORD inIndex)
{
	D3DLIGHT9   light;
	::ZeroMemory(&light, sizeof(light));

	m_pD3DDevice->GetLight(inIndex, &light);

	return light;
}

//------------------------------------------------------------------------------
//	レンダリングステート取得
//------------------------------------------------------------------------------
DWORD CDXGraphics9::GetRenderState(const D3DRENDERSTATETYPE inState)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_GetRenderState)\n"));
		return 0;
	}
#endif

	DWORD   value;
	if(m_pD3DDevice->GetRenderState(inState, &value) != D3D_OK)
		return 0;

	return value;
}

//------------------------------------------------------------------------------
//	レンダリングステート取得
//------------------------------------------------------------------------------
float CDXGraphics9::GetRenderStateF(const D3DRENDERSTATETYPE inState)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_GetRenderState)\n"));
		return 0;
	}
#endif

	const DWORD   dwValue = GetRenderState(inState);

	float   fValue;
	::CopyMemory(&fValue, &dwValue, 4);

	return fValue;
}

/*
//------------------------------------------------------------------------------
//	レンダリングステート設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetRenderState)\n"));
		return;
	}
#endif

	// ステートがマップに登録されているか調べる
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator it;
	it = m_RenderState.find(inState);
	if(it == m_RenderState.end()) {
		// 登録されていない場合、デフォルト値を保存
		DXGSAVESTATE	SaveState;
		SaveState.Value = inValue;
		m_pD3DDevice->GetRenderState(inState, &SaveState.DefValue);
		if(inValue == SaveState.DefValue)
			return;							// 設定値がデフォルト値の場合は終了

		// マップに登録する
		m_RenderState.insert(std::pair<D3DRENDERSTATETYPE, DXGSAVESTATE>(inState, SaveState));
	} else{
		// 登録済みの場合
		if(it->second.DefValue != inValue) {
			if(it->second.Value == inValue)
				return;						// デフォルト値でなく、登録済みと同じ値の場合は何もしない
			it->second.Value = inValue;		// 設定値の更新
		} else {
			m_RenderState.erase(it);		// デフォルト値に戻る場合は、マップから削除
		}
	}

	//レンダリングステート設定
	m_pD3DDevice->SetRenderState(inState, inValue);
}
*/

//------------------------------------------------------------------------------
//	レンダリングステート設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetRenderState)\n"));
		return;
	}
#endif

	// ステートがマップに登録されているか調べる
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator it;
	it = m_RenderState.find(inState);
	if(it == m_RenderState.end()) {
		// 登録されていない場合、デフォルト値を保存
		DXGSAVESTATE	SaveState;
		SaveState.Value = inValue;
		m_pD3DDevice->GetRenderState(inState, &SaveState.DefValue);

		// マップに登録する
		m_RenderState.insert(std::pair<D3DRENDERSTATETYPE, DXGSAVESTATE>(inState, SaveState));

		// デフォルト値の場合は、ステートに変更がないので終了
		if(inValue == SaveState.DefValue)
			return;
	} else{
		// 登録済みの場合
		if(it->second.Value == inValue)
			return;						// 登録済みの値と同じ値の場合は何もしない

		it->second.Value = inValue;		// 設定値の更新
	}

	//レンダリングステート設定
	m_pD3DDevice->SetRenderState(inState, inValue);
}

//------------------------------------------------------------------------------
//	レンダリングステートリセット
//------------------------------------------------------------------------------
void CDXGraphics9::ResetRenderState()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_ResetRenderState)\n"));
		return;
	}
#endif

	// レンダリングステートをデフォルトに戻す
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator   it;
	for(it = m_RenderState.begin(); it != m_RenderState.end(); it++)
		m_pD3DDevice->SetRenderState(it->first, it->second.DefValue);

	// マップ削除
	m_RenderState.clear();
}

//------------------------------------------------------------------------------
//	マテリアル設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetMaterial(const D3DMATERIAL9& inMaterial)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetMaterial)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetMaterial(&inMaterial);
}

//------------------------------------------------------------------------------
//	テクスチャ設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetTexture(const DWORD inStage, IDirect3DBaseTexture9* pTexture)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetTexture)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetTexture(inStage, pTexture);
}

//------------------------------------------------------------------------------
//	テクスチャステージステート取得
//------------------------------------------------------------------------------
DWORD CDXGraphics9::GetTextureStageState(const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_GetTextureStageState)\n"));
		return 0;
	}
#endif

	DWORD   value;
	if(m_pD3DDevice->GetTextureStageState(inStage, inType, &value) != D3D_OK)
		return 0;

	return value;
}

//------------------------------------------------------------------------------
//	テクスチャステージステート設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetTextureStageState(const DWORD inStage,
										const D3DTEXTURESTAGESTATETYPE inType, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetTextureStageState)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetTextureStageState(inStage, inType, inValue);
}

//------------------------------------------------------------------------------
//	テクスチャサンプラーステート取得
//------------------------------------------------------------------------------
DWORD CDXGraphics9::GetSamplerState(const DWORD inSampler, const D3DSAMPLERSTATETYPE inType)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_GetSamplerState)\n"));
		return 0;
	}
#endif

	DWORD   value;
	if(m_pD3DDevice->GetSamplerState(inSampler, inType, &value) != D3D_OK)
		return 0;

	return value;
}

//------------------------------------------------------------------------------
//	テクスチャサンプラーステート設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetSamplerState(const DWORD inSampler, const D3DSAMPLERSTATETYPE inType, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetSamplerState)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetSamplerState(inSampler, inType, inValue);
}

//------------------------------------------------------------------------------
//	エフェクト生成
//------------------------------------------------------------------------------
IEffect* CDXGraphics9::CreateEffectFromFile(LPCTSTR inFileName)
{
	IEffect*   pEffect;

	try {
		if(m_pD3DDevice == NULL)
			throw TEXT("DirectX Graphics未初期化");

		// エフェクト読み込み
		ID3DXEffect*   pD3DXEffect;
//		DWORD   flag = inCompiled ? 0 : D3DXSHADER_SKIPVALIDATION;
		if(::D3DXCreateEffectFromFile(m_pD3DDevice, inFileName, NULL, NULL, D3DXFX_NOT_CLONEABLE, 
									  NULL, &pD3DXEffect, NULL)
		   != D3D_OK)
			   throw TEXT("エフェクト読み込み失敗");

		pEffect = new CEffect(pD3DXEffect);
		pD3DXEffect->Release();
	} catch(LPCTSTR ErrorString) {
		// エラーメッセージ生成
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateEffectFromFile)\n"), ErrorString);
		::OutputDebugString(msg);

		pEffect = new CNullEffect();
	}

	m_Effect.push_back(pEffect);

	return pEffect;
}

//------------------------------------------------------------------------------
//	エフェクト解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseEffect(IEffect*& pEffect)
{
	m_ProtectedResource.erase(pEffect);		// プロテクト解除

	m_Effect.remove(pEffect);
	delete pEffect;
	pEffect = NULL;
}

//------------------------------------------------------------------------------
//	全エフェクト解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllEffects()
{
	std::list<IEffect*>::iterator   list_it = m_Effect.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Effect.end()) {
			delete *list_it;
			list_it++;
		}
		m_Effect.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_Effect.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_Effect.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	頂点バッファ生成
//------------------------------------------------------------------------------
IVertexBuffer* CDXGraphics9::CreateVertexBuffer(const UINT inSize, const DWORD inFVF,
												const UINT inStride, const bool inWriteOnly)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateVertexBuffer)\n"));
		return NULL;
	}
#endif

	// ストライド設定
	const UINT   STRIDE = inStride == 0 ? ::D3DXGetFVFVertexSize(inFVF) : inStride;

	// 頂点バッファ生成
	IVertexBuffer*            pVertex;
	IDirect3DVertexBuffer9*   pBuffer;
	if(m_pD3DDevice->CreateVertexBuffer(inSize, 0, inFVF, D3DPOOL_MANAGED, &pBuffer, NULL) == D3D_OK) {
		pVertex = new CVertexBuffer(pBuffer, inFVF, STRIDE, inWriteOnly);
	} else {
		::OutputDebugString(TEXT("*** Error - 頂点バッファ生成失敗(CDXGraphics9_CreateVertexBuffer)\n"));
		pVertex = new CNullVertexBuffer();
	}

	m_VertexBuffer.push_back(pVertex);
	SafeRelease(pBuffer);

	return pVertex;
}

//------------------------------------------------------------------------------
//	頂点バッファ生成
//------------------------------------------------------------------------------
IVertexBuffer* CDXGraphics9::CreateVertexBufferUP(const UINT inSize, const DWORD inFVF,
												  const UINT inStride)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateVertexBufferUP)\n"));
		return NULL;
	}
#endif

	// ストライド設定
	const UINT   STRIDE = inStride == 0 ? ::D3DXGetFVFVertexSize(inFVF) : inStride;

	// 頂点バッファ生成
	IVertexBuffer*   pVertex;
	void*            pBuffer = new BYTE[inSize];
	if(pBuffer != NULL) {
		pVertex = new CVertexBufferUP(m_pD3DDevice, pBuffer, inSize, inFVF, STRIDE);
	} else {
		::OutputDebugString(TEXT("*** Error - 頂点バッファ生成失敗(CDXGraphics9_CreateVertexBufferUP)\n"));
		pVertex = new CNullVertexBuffer();
	}

	m_VertexBuffer.push_back(pVertex);

	return pVertex;
}

//------------------------------------------------------------------------------
//	頂点バッファ解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseVertexBuffer(IVertexBuffer*& pVertex)
{
	m_ProtectedResource.erase(pVertex);		// プロテクト解除

	m_VertexBuffer.remove(pVertex);
	delete pVertex;
	pVertex = NULL;
}

//------------------------------------------------------------------------------
//	全頂点バッファ解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllVertexBuffers()
{
	std::list<IVertexBuffer*>::iterator   list_it = m_VertexBuffer.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_VertexBuffer.end()) {
			delete *list_it;
			list_it++;
		}
		m_VertexBuffer.clear();
	} else {
		std::set<void*>   ::iterator    set_it;
		std::set<void*>   ::iterator    set_end = m_ProtectedResource.end();
		while(list_it != m_VertexBuffer.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_VertexBuffer.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	シンプルシェイプ生成
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromSimpleShape(const DXGSIMPLESHAPE&  inShape,
												 const DXGCOMPUTENORMAL inCompute)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL){
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateSimpleShape)\n"));
		return NULL;
	}
#endif

	// シンプルシェイプ生成
	ID3DXMesh*	pMesh = NULL;
	HRESULT		hr	  = D3DXERR_INVALIDDATA;
	switch(inShape.Type){
		case DXGSHAPE_BOX:
			hr = ::D3DXCreateBox(m_pD3DDevice, inShape.Width, inShape.Height, inShape.Depth,
								 &pMesh, NULL);
			break;

		case DXGSHAPE_CYLINDER:
			hr = ::D3DXCreateCylinder(m_pD3DDevice, inShape.Radius1, inShape.Radius2, inShape.Length,
									  inShape.Slices, inShape.Stacks, &pMesh, NULL);
			break;

		case DXGSHAPE_POLYGON:
			hr = ::D3DXCreatePolygon(m_pD3DDevice, inShape.Length, inShape.Sides, &pMesh, NULL);
			break;

		case DXGSHAPE_SPHERE:
			hr = ::D3DXCreateSphere(m_pD3DDevice, inShape.Radius, inShape.Slices, inShape.Stacks,
									&pMesh, NULL);
			break;

		case DXGSHAPE_TORUS:
			hr = ::D3DXCreateTorus(m_pD3DDevice, inShape.InnerRadius, inShape.OuterRadius,
								   inShape.Sides, inShape.Rings, &pMesh, NULL);
			break;

		case DXGSHAPE_TEAPOT:
			hr = ::D3DXCreateTeapot(m_pD3DDevice, &pMesh, NULL);
			break;
	};

	// メッシュオブジェクト生成
	IModel*   pShape;
	if(hr == D3D_OK){
		// メッシュ成形
		ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
		pMesh->Release();

		// メッシュオブジェクト生成
		pShape = new CModel(pNewMesh, 1);
		pShape->Optimize();

		pNewMesh->Release();
	} else{
		// NULLメッシュオブジェクト生成
		::OutputDebugString(TEXT("*** Error - シンプルシェイプ生成失敗(CDXGraphics9_CreateModelFromSimpleShape)\n"));
		pShape = new CNullModel();
	}

	m_Model.push_back(pShape);		// メッシュリストへ追加

	return pShape;
}

//------------------------------------------------------------------------------
//	モデル生成
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromFile(LPCTSTR inFileName,
										  const DXGCOMPUTENORMAL inCompute,
										  const bool inUseSysMem)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateModelFromFile)\n"));
		return NULL;
	}
#endif

	// ｘファイル読み込み
	IModel*        pModel;
	ID3DXMesh*     pMesh;
	ID3DXBuffer*   pMatBuf;
	DWORD          SubsetCount;
	const DWORD    FLAG = inUseSysMem == false ? D3DXMESH_MANAGED : D3DXMESH_SYSTEMMEM;
	if(::D3DXLoadMeshFromX(inFileName, FLAG, m_pD3DDevice, NULL, &pMatBuf, NULL, &SubsetCount, &pMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ｘファイル読み込み失敗(CDXGraphics9_CreateModelFromFile)\n"));
		pModel = new CNullModel();
		m_Model.push_back(pModel);
		return pModel;
	}

	// メッシュ成形
	ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
	pMesh->Release();

	pModel = new CModel(pNewMesh, SubsetCount);
	pModel->Optimize();				// 最適化
	m_Model.push_back(pModel);		// メッシュリストへ追加
	pNewMesh->Release();

	// フルパス取得
	TCHAR   FullPath[MAX_PATH + 1];
	::_tfullpath(FullPath, inFileName, MAX_PATH + 1);

	// ファイルパス取得
	TCHAR   drive[_MAX_DRIVE + 1];
	TCHAR   dir  [_MAX_DIR   + 1];
	::_tsplitpath_s(FullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);

	D3DXMATERIAL*   pMaterial = (D3DXMATERIAL*)pMatBuf->GetBufferPointer();
	for(DWORD i = 0; i < SubsetCount; i++) {
		// マテリアル設定
		pModel->SetMaterial((pMaterial + i)->MatD3D, i);

		// テクスチャ設定
		if((pMaterial + i)->pTextureFilename != NULL) {
			TCHAR   FileName[MAX_PATH + 1];
			::MultiByteToWideChar(CP_ACP, 0, (pMaterial + i)->pTextureFilename, -1, FileName, MAX_PATH + 1);
			::wsprintf(FullPath, TEXT("%s%s%s"), drive, dir, FileName);
			pModel->SetTexture(FullPath, i);
		}
	}

	pMatBuf->Release();

	return pModel;
}

//------------------------------------------------------------------------------
//	モデル生成
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromText(LPCTSTR inText,
										  const LOGFONT& inLogFont, const float inExtrusion,
										  const DXGCOMPUTENORMAL inCompute)
{
	// 互換デバイスコンテキスト生成
	HDC     hdc      = ::CreateCompatibleDC(NULL);

	// フォント生成
	HFONT   hFont    = ::CreateFontIndirect(&inLogFont);
	HFONT   hOldFont = (HFONT)::SelectObject(hdc, hFont);

	// メッシュオブジェクト生成
	ID3DXMesh*	 pMesh = NULL;
	const HRESULT   hr = ::D3DXCreateText(m_pD3DDevice, hdc, inText, 0.0f, inExtrusion, &pMesh, NULL, NULL);

	// リソース解放
    ::SelectObject(hdc, hOldFont);
    ::DeleteObject(hFont);
    ::DeleteDC(hdc);

	IModel*   pModel;
	if(hr == D3D_OK){
		// メッシュ成形
		ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
		pMesh->Release();

		// メッシュオブジェクト生成
		pModel = new CModel(pNewMesh, 1);
		pModel->Optimize();

		pNewMesh->Release();
	} else{
		// NULLメッシュオブジェクト生成
		::OutputDebugString(TEXT("*** Error - テキストも出る生成失敗(CDXGraphics9_CreateModelFromText)\n"));
		pModel = new CNullModel();
	}

	m_Model.push_back(pModel);		// メッシュリストへ追加

	return pModel;
}

//------------------------------------------------------------------------------
//	モデル生成
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromText(LPCTSTR inText,
										  LPCTSTR inFontName, const int inSize,
										  const float inExtrusion,
										  const DXGCOMPUTENORMAL inCompute)
{
	LOGFONT   lf;
	::ZeroMemory(&lf, sizeof(lf));

	lf.lfHeight         = inSize;
	lf.lfWidth          = 0;
	lf.lfWeight         = 0;
	lf.lfEscapement     = 0;
	lf.lfOrientation    = 0;
	lf.lfItalic         = 0;
	lf.lfUnderline      = 0;
	lf.lfStrikeOut      = 0;
#ifdef UNICODE
	lf.lfCharSet        = DEFAULT_CHARSET;
#else
	lf.lfCharSet        = SHIFTJIS_CHARSET;
#endif
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality        = PROOF_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	return CreateModelFromText(inText, lf, inExtrusion, inCompute);
}

//------------------------------------------------------------------------------
//	モデル成形
//------------------------------------------------------------------------------
ID3DXMesh* CDXGraphics9::ModelingMesh(ID3DXMesh* pBaseMesh, const DXGCOMPUTENORMAL inCompute)
{
	if(inCompute == DXGCOMPUTE_DONOT_INPLACE) {
		pBaseMesh->AddRef();
		return pBaseMesh;
	}

	// 頂点データレイアウト設定
	D3DVERTEXELEMENT9   VertexDecl[6] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END()
	};

	UINT    decl_idx =  1;	// 座標設定済みのため
	WORD    offset   = 12;	// float x 3 = 12byte

	// 法線
	DWORD   NormalSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_NORMAL) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_NORMAL;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		NormalSemantic = D3DDECLUSAGE_NORMAL;
	}

	// 接線
	DWORD   TangentSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_TANGENT) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_TANGENT;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		TangentSemantic = D3DDECLUSAGE_TANGENT;
	}

	// 従法線
	DWORD   BinormalSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_BINORMAL) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_BINORMAL;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		BinormalSemantic = D3DDECLUSAGE_BINORMAL;
	}

	// テクスチャ座標
	DWORD   TextureSemantic = D3DX_DEFAULT;
	if((pBaseMesh->GetFVF() & D3DFVF_TEX1) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT2;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_TEXCOORD;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 8;	// float x 2 = 8byte

		TextureSemantic = D3DDECLUSAGE_TEXCOORD;
	}

	// メッシュ生成
	ID3DXMesh*   pNewMesh;
	if(pBaseMesh->CloneMesh(pBaseMesh->GetOptions(), VertexDecl, m_pD3DDevice, &pNewMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - メッシュ生成失敗(CDXGraphics9_ModelingMesh)\n"));
		pBaseMesh->AddRef();
		return pBaseMesh;
	}

	// 法線・接線・従法線計算不要
	if(inCompute == DXGCOMPUTE_NONE   ||
	   inCompute == DXGCOMPUTE_NORMAL && (pBaseMesh->GetFVF() & D3DFVF_NORMAL) != 0)
		return pNewMesh;

	// 法線・接線・従法線計算
	if(::D3DXComputeTangentFrameEx(pNewMesh,
								   TextureSemantic , 0,
								   TangentSemantic , 0,
								   BinormalSemantic, 0,
								   NormalSemantic  , 0,
								   D3DXTANGENT_GENERATE_IN_PLACE, NULL,
								   -1.0f, 0.0f, -1.0f,
								   NULL, NULL)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 法線・接線・従法線計算失敗(CDXGraphics9_ModelingMesh)\n"));
	}

	return pNewMesh;
}

//------------------------------------------------------------------------------
//	共有モデル生成
//------------------------------------------------------------------------------
bool CDXGraphics9::CreateSharedModelFromFile(LPTSTR inFileName,
											 IModel* pModelArray[],
											 const DWORD inArrayCount,
											 const DXGCOMPUTENORMAL inCompute)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateSharedModelFromFile)\n"));
		return false;
	}
#endif

	// ｘファイル読み込み
	ID3DXMesh*     pMesh;
	ID3DXBuffer*   pMatBuf;
	DWORD          SubsetCount;
	if(::D3DXLoadMeshFromX(inFileName, D3DXMESH_MANAGED, m_pD3DDevice, NULL, &pMatBuf, NULL, &SubsetCount, &pMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ｘファイル読み込み失敗(CDXGraphics9_CreateSharedModelFromFile)\n"));
		// NULLモデル生成
		for(DWORD i = 0; i < inArrayCount; i++) {
			pModelArray[i] = new CNullModel();
			m_Model.push_back(pModelArray[i]);
		}

		return false;
	}

	// メッシュ成形
	ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
	pMesh->Release();

	// メッシュ最適化
	// 隣接面生成
	const DWORD    FACE = pNewMesh->GetNumFaces();
	DWORD*   pAdjacency = new DWORD[FACE * 3 * sizeof(DWORD)];
	if(pNewMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency) == D3D_OK) {
		// 最適化
		if(pNewMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
									pAdjacency, NULL, NULL, NULL) != D3D_OK) {
			delete[] pAdjacency;
			::OutputDebugString(TEXT("*** Error - メッシュ最適化失敗(CDXGraphics9_CreateSharedModelFromFile)\n"));
		}
	}

	// マテリアルポインター取得
	D3DXMATERIAL*   pMaterial = (D3DXMATERIAL*)pMatBuf->GetBufferPointer();

	// テクスチャ読み込み設定
	// フルパス取得
	TCHAR   FullPath[MAX_PATH + 1];
	::_tfullpath(FullPath, inFileName, MAX_PATH + 1);

	// ファイルパス取得
	TCHAR   drive[_MAX_DRIVE + 1];
	TCHAR   dir  [_MAX_DIR   + 1];
	::_tsplitpath_s(FullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);

	// テクスチャ読み込み
	std::vector<IDirect3DTexture9*>   textures(SubsetCount, NULL);
	for(DWORD i = 0; i < SubsetCount; i++) {
		if((pMaterial + i)->pTextureFilename != NULL) {
			if(::D3DXCreateTextureFromFileA(m_pD3DDevice, (pMaterial + i)->pTextureFilename, &textures[i])
			   != D3D_OK)
				::OutputDebugString(TEXT("*** Error - テクスチャ読み込み失敗(CDXGraphics9_CreateSharedModelFromFile)\n"));
		}
	}

	// モデル複製
	for(DWORD moi = 0; moi < inArrayCount; moi++) {
		pModelArray[moi] = new CModel(pNewMesh, SubsetCount);
		m_Model.push_back(pModelArray[moi]);

		// マテリアル設定
		for(DWORD mai = 0; mai < SubsetCount; mai++) {
			pModelArray[moi]->SetMaterial((pMaterial + mai)->MatD3D, mai);
			pModelArray[moi]->SetTexture (textures[mai], mai);
		}
	}

	pNewMesh->Release();
	pMatBuf ->Release();

	return true;
}

//------------------------------------------------------------------------------
//	モデル解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseModel(IModel*& pModel)
{
	m_ProtectedResource.erase(pModel);		// プロテクト解除

	m_Model.remove(pModel);
	delete pModel;
	pModel = NULL;
}

//------------------------------------------------------------------------------
//	全モデル解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllModels()
{
	std::list<IModel*>::iterator  list_it = m_Model.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Model.end()) {
			delete *list_it;
			list_it++;
		}
		m_Model.clear();
	}else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_Model.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_Model.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	アニメーションモデル生成
//------------------------------------------------------------------------------
IAnimationModel* CDXGraphics9::CreateAnimationModelFromFile(LPCTSTR inFileName,
															const DXGCOMPUTENORMAL   inCompute,
															const DXGSKINNING_METHOD inSkinMethod)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateAnimationModelFromX)\n"));
		return NULL;
	}
#endif

	CAnimationModel*   pAnimeModel = new CAnimationModel(m_pD3DDevice);
	if(pAnimeModel == NULL) {
		::OutputDebugString(TEXT("*** Error - アニメーションモデル生成失敗(CDXGraphics9_CreateAnimationModelFromX)\n"));
		return NULL;
	}

	// ｘファイル読み込み
	IAnimationModel*   pModel = pAnimeModel;
	if(pAnimeModel->LoadFromX(inFileName, inCompute, inSkinMethod) == false) {
		::OutputDebugString(TEXT("*** Error - ｘファイル読み込み失敗(CDXGraphics9_CreateAnimationModelFromX)\n"));
		delete pAnimeModel;
		pModel = new CNullAnimationModel();
	}
	m_AnimationModel.push_back(pModel);

	return pModel;
}

//------------------------------------------------------------------------------
//	アニメーションモデル解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAnimationModel(IAnimationModel*& pAnimeModel)
{
	m_ProtectedResource.erase(pAnimeModel);		// プロテクト解除

	m_AnimationModel.remove(pAnimeModel);
	delete pAnimeModel;
	pAnimeModel = NULL;
}

//------------------------------------------------------------------------------
//	全アニメーションモデル解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllAnimationModels()
{
	std::list<IAnimationModel*>::iterator   list_it = m_AnimationModel.begin();
	if(m_AnimationModel.empty()) {
		while(list_it != m_AnimationModel.end()) {
			delete *list_it;
			list_it++;
		}
		m_AnimationModel.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_AnimationModel.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_AnimationModel.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	スプライト生成
//------------------------------------------------------------------------------
ISprite* CDXGraphics9::CreateSprite(const UINT inWidth, const UINT inHeight, const D3DFORMAT inFormat)
{
	ISprite*   pSprite;

	try {
#ifdef _DEBUG
		if(m_pD3DDevice == NULL)
			throw TEXT("DirectX Graphics未初期化");
#endif

		// テクスチャ生成
		IDirect3DTexture9*   pTexture = NULL;
		if(::D3DXCreateTexture(m_pD3DDevice, inWidth, inHeight, 1, 0, inFormat, D3DPOOL_MANAGED, &pTexture) != D3D_OK)
			throw TEXT("テクスチャ生成失敗");

		pSprite = new CSprite(pTexture);
		pTexture->Release();
	} catch(LPCTSTR ErrorString) {
		// エラーメッセージ生成
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateSrpite)\n"), ErrorString);
		::OutputDebugString(msg);

		// 例外が発生した場合は、NULLオブジェクトを生成
		pSprite = new CNullSprite();
	}

	m_Sprite.push_back(pSprite);	// スプライトリストへ追加

	return pSprite;
}

//------------------------------------------------------------------------------
//	スプライト生成
//------------------------------------------------------------------------------
ISprite* CDXGraphics9::CreateSpriteFromFile(LPCTSTR inFileName,
											const D3DFORMAT inFormat,
											const D3DCOLOR  inColorKey)
{
	ISprite*   pSprite;

	try {
#ifdef _DEBUG
		if(m_pD3DDevice == NULL)
			throw TEXT("DirectX Graphics未初期化");
#endif

		// テクスチャ生成
		IDirect3DTexture9*   pTexture;
		if(::D3DXCreateTextureFromFileEx(m_pD3DDevice, inFileName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, inFormat,
//										 D3DPOOL_MANAGED, D3DX_DEFAULT,     D3DX_DEFAULT, inColorKey, NULL, NULL, &pTexture)
										 D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, inColorKey, NULL, NULL, &pTexture)
		   != D3D_OK) {
			// WICによるファイル読み込み
			pTexture = CreateTextureFromWIC(inFileName, inFormat, inColorKey);
			// OLEによるファイル読み込み
//			pTexture = CreateTextureFromPicture(inFileName, inFormat, inColorKey);
			if(pTexture == NULL)
				throw TEXT("ファイル読み込み失敗");
		}

		pSprite = new CSprite(pTexture);
		pTexture->Release();
	} catch(LPCTSTR ErrorString) {
		// エラーメッセージ生成
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateSrpiteFromFile)\n"), ErrorString);
		::OutputDebugString(msg);

		// 例外が発生した場合は、NULLオブジェクトを生成
		pSprite = new CNullSprite();
	}

	m_Sprite.push_back(pSprite);	// スプライトリストへ追加

	return pSprite;
}

//------------------------------------------------------------------------------
//	テクスチャ生成
//------------------------------------------------------------------------------
IDirect3DTexture9* CDXGraphics9::CreateTextureFromWIC(LPCTSTR inFileName,
													  const D3DFORMAT inFormat,
													  const D3DCOLOR  inColorKey)
{
	IDirect3DTexture9*       pTexture      = NULL;

	IWICImagingFactory*      pWICFactory   = NULL;
	IWICBitmapDecoder*       pWICDecoder   = NULL;
	IWICBitmapFrameDecode*   pWICFrameDec  = NULL;
	IWICFormatConverter*     pWICConverter = NULL;

	IDirect3DTexture9*       pMemTexture   = NULL;

	try {
		// WICファクトリー生成
		if(::CoCreateInstance(CLSID_WICImagingFactory, NULL,
							  CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
							  (LPVOID*)&pWICFactory)
		   != S_OK)
			throw TEXT("WICファクトリー生成失敗");

		// WICデコーダー生成
		if(pWICFactory->CreateDecoderFromFilename(inFileName, NULL, GENERIC_READ,
												  WICDecodeMetadataCacheOnLoad, &pWICDecoder)
		   != S_OK)
			throw TEXT("WICデコーダー生成失敗");

		// WICフレームデコーダー生成
		if(pWICDecoder->GetFrame(0, &pWICFrameDec) != S_OK)
			throw TEXT("WICフレームデコーダー生成失敗");

		// WICコンバーター生成
		if(pWICFactory->CreateFormatConverter(&pWICConverter) != S_OK)
			throw TEXT("WICコンバーター生成失敗");

		// WICコンバーター初期化
		if(pWICConverter->Initialize(pWICFrameDec, GUID_WICPixelFormat32bppPBGRA,
									 WICBitmapDitherTypeNone, NULL, 0.0f,
									 WICBitmapPaletteTypeMedianCut)
		   != S_OK)
			throw TEXT("WICコンバーター初期化失敗");

		// サイズ取得
		UINT   wic_width, wic_height;
		pWICConverter->GetSize(&wic_width, &wic_height);

		// システムメモリテクスチャ生成
		if(::D3DXCreateTexture(m_pD3DDevice, wic_width, wic_height, 1, 0, D3DFMT_A8R8G8B8,
							   D3DPOOL_SYSTEMMEM, &pMemTexture)
		   != D3D_OK)
			throw TEXT("システムメモリテクスチャ生成失敗");

		// システムメモリテクスチャ情報取得
		D3DSURFACE_DESC   desc;
		pMemTexture->GetLevelDesc(0, &desc);

		// コピー領域設定
		WICRect   copy_rect;
		copy_rect.X = 0;
		copy_rect.Y = 0;

		if(wic_width <= desc.Width)
			copy_rect.Width = wic_width;
		else
			copy_rect.Width = desc.Width;

		if(wic_width <= desc.Height)
			copy_rect.Height = wic_height;
		else
			copy_rect.Height = desc.Height;

		// ピクセル取得
		D3DLOCKED_RECT   lock_rect;
		if(pMemTexture->LockRect(0, &lock_rect, NULL, 0) != D3D_OK)
			throw TEXT("システムメモリテクスチャロック失敗");

		const HRESULT   hr = pWICConverter->CopyPixels(&copy_rect, lock_rect.Pitch,
													   lock_rect.Pitch * desc.Height,
													   (BYTE*)lock_rect.pBits);

		pMemTexture->UnlockRect(0);

		if(hr != S_OK)
			throw TEXT("ピクセル取得失敗");

		pWICConverter->Release();
		pWICFrameDec ->Release();
		pWICDecoder  ->Release();
		pWICFactory  ->Release();

		// テクスチャ生成
		if(::D3DXCreateTexture(m_pD3DDevice, wic_width, wic_height, 1, 0,
							   inFormat, D3DPOOL_MANAGED, &pTexture)
		   != D3D_OK)
			throw  TEXT("テクスチャ生成失敗");

		IDirect3DSurface9*   pMemSurface;
		pMemTexture->GetSurfaceLevel(0, &pMemSurface);

		IDirect3DSurface9*   pSurface;
		pTexture->GetSurfaceLevel(0, &pSurface);

		// システムメモリテクスチャ転送
		::D3DXLoadSurfaceFromSurface(pSurface, NULL, NULL, pMemSurface, NULL, NULL, D3DX_FILTER_POINT, inColorKey);

		pSurface   ->Release();
		pMemSurface->Release();
		pMemTexture->Release();
	} catch(LPCTSTR err_str) {
#ifdef _DEBUG
		// エラーメッセージ生成
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateTextureFromWIC)\n"), err_str);
		::OutputDebugString(msg);
#endif
		SafeRelease(pMemTexture);

		SafeRelease(pWICConverter);
		SafeRelease(pWICFrameDec);
		SafeRelease(pWICDecoder);
		SafeRelease(pWICFactory);
	}

	return pTexture;
}

/*
//------------------------------------------------------------------------------
//	テクスチャ生成
//------------------------------------------------------------------------------
IDirect3DTexture9* CDXGraphics9::CreateTextureFromPicture(LPCTSTR inFileName,
														  const D3DFORMAT inFormat,
														  const D3DCOLOR  inColorKey)
{
	// ファイルオープン
	HANDLE   hFile = ::CreateFile(inFileName, GENERIC_READ, 0, NULL, 
								  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CDXGraphics9_CreateTextureFromPicture)\n"));
		return NULL;
	}

	// グローバルメモリ確保
	const DWORD   FILE_SIZE = ::GetFileSize(hFile, NULL);
	HGLOBAL       hGlobal   = ::GlobalAlloc(GPTR, FILE_SIZE);
	if(hGlobal == NULL) {
		::OutputDebugString(TEXT("*** Error - グローバルメモリ確保失敗(CDXGraphics9_CreateTextureFromPicture)\n"));
		::CloseHandle(hFile);
		return NULL;
	}

	// ファイル読み込み
	DWORD   actual;
	::ReadFile(hFile, hGlobal, FILE_SIZE, &actual, NULL);
	::CloseHandle(hFile);

	// ストリーム生成
	IStream*   pStream;
	if(::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ストリーム生成失敗(CDXGraphics9_CreateTextureFromPicture)\n"));
		::GlobalFree(hGlobal);
		return NULL;
	}

	// ピクチャー生成
	IPicture*   pPicture;
	if(::OleLoadPicture(pStream, FILE_SIZE, TRUE, IID_IPicture, (LPVOID*)&pPicture) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ピクチャー生成失敗(CDXGraphics9_CreateTextureFromPicture)\n"));
		pStream->Release();
		::GlobalFree(hGlobal);
		return NULL;
	}

	// ピクチャーサイズ取得
	OLE_XSIZE_HIMETRIC   pic_width;
	pPicture->get_Width (&pic_width);

	OLE_YSIZE_HIMETRIC   pic_height;
	pPicture->get_Height(&pic_height);

	// 単位変換 - HiMetric(1/100mm)からピクセル(2540 = HiMetric / Inch)
	const long   DEST_WIDTH  = ::MulDiv(pic_width , 96, 2540);
	const long   DEST_HEIGHT = ::MulDiv(pic_height, 96, 2540);

	// カラーキー設定
	D3DCOLOR   color_key = inColorKey;
	if(color_key == 0) {
		DWORD   attribute;
		pPicture->get_Attributes(&attribute);
		if(attribute == PICTURE_TRANSPARENT)
			color_key = 0xFF000000;
	}

	// システムメモリテクスチャ生成
	IDirect3DTexture9*   pMemTexture;
	if(::D3DXCreateTexture(m_pD3DDevice, DEST_WIDTH, DEST_HEIGHT, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pMemTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - システムメモリテクスチャ生成失敗(CDXGraphics9_CreateTextureFromPicture)\n"));
		pPicture->Release();
		pStream ->Release();
		::GlobalFree(hGlobal);
		return NULL;
	}

	IDirect3DSurface9*   pMemSurface;
	pMemTexture->GetSurfaceLevel(0, &pMemSurface);

	// ピクチャー転送
	HDC   hDC;
	pMemSurface->GetDC(&hDC);
	pPicture->Render(hDC, 0, 0, DEST_WIDTH, DEST_HEIGHT, 0, pic_height, pic_width, -pic_height, NULL);

	// 解放
	pMemSurface->ReleaseDC(hDC);

	pPicture->Release();
	pStream ->Release();
	::GlobalFree(hGlobal);

	// テクスチャ生成
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTexture(m_pD3DDevice, DEST_WIDTH, DEST_HEIGHT, 1, 0, inFormat, D3DPOOL_MANAGED, &pTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CDXGraphics9_CreateTextureFromPicture)\n"));
		pMemSurface->Release();
		pMemTexture->Release();
		return NULL;
	}

	IDirect3DSurface9*   pSurface;
	pTexture->GetSurfaceLevel(0, &pSurface);

	// システムメモリテクスチャ転送
	::D3DXLoadSurfaceFromSurface(pSurface, NULL, NULL, pMemSurface, NULL, NULL, D3DX_FILTER_POINT, color_key);

	pSurface->Release();

	pMemSurface->Release();
	pMemTexture->Release();

	return pTexture;
}
*/

//------------------------------------------------------------------------------
//	スプライト解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseSprite(ISprite*& pSprite)
{
	m_ProtectedResource.erase(pSprite);		// プロテクト解除

	m_Sprite.remove(pSprite);
	delete pSprite;
	pSprite = NULL;
}

//------------------------------------------------------------------------------
//	全スプライト解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllSprites()
{
	std::list<ISprite*>::iterator   list_it = m_Sprite.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Sprite.end()) {
			delete *list_it;
			list_it++;
		}
		m_Sprite.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_Sprite.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_Sprite.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	フォント生成
//------------------------------------------------------------------------------
IDXGFont* CDXGraphics9::CreateSpriteFont(LPCTSTR inFontName, const int inSize)
{
	D3DXFONT_DESC   font_desc;
	font_desc.Height          = inSize;
	font_desc.Width           = 0;
	font_desc.Weight          = FW_NORMAL;
	font_desc.MipLevels       = 1;
	font_desc.Italic          = FALSE;
#ifdef UNICODE
	font_desc.CharSet         = DEFAULT_CHARSET;
#else
	font_desc.CharSet         = SHIFTJIS_CHARSET;
#endif
	font_desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	font_desc.Quality         = PROOF_QUALITY;
	font_desc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;

	::lstrcpyn(font_desc.FaceName, inFontName, LF_FACESIZE - 1);

	return CreateSpriteFont(font_desc);
}

//------------------------------------------------------------------------------
//	フォント生成
//------------------------------------------------------------------------------
IDXGFont* CDXGraphics9::CreateSpriteFont(const D3DXFONT_DESC& inFontDesc)
{
	IDXGFont*    pFont;
	ID3DXFont*   pD3DXFont;
	if(::D3DXCreateFontIndirect(m_pD3DDevice, &inFontDesc, &pD3DXFont) == D3D_OK) {
		pFont = new CDXGFont(pD3DXFont, m_pD3DXSprite);
		pD3DXFont->Release();
	} else {
		::OutputDebugString(TEXT("*** Error - フォント生成失敗(CDXGraphics9_CreateFont)\n"));
		pFont = new CDXGNullFont();
	}

	m_Font.push_back(pFont);

	return pFont;
}

//------------------------------------------------------------------------------
//	デフォルトフォント生成
//------------------------------------------------------------------------------
IDXGFont* CDXGraphics9::CreateDefaultFont()
{
	// システムフォント情報取得
	LOGFONT   lf;
	::GetObject((HFONT)::GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);

	// フォント情報設定
	D3DXFONT_DESC   font_desc;
	font_desc.Height          = lf.lfHeight;
	font_desc.Width           = lf.lfWidth;
	font_desc.Weight          = lf.lfWeight;
	font_desc.MipLevels       = 1;
	font_desc.Italic          = lf.lfItalic;
	font_desc.CharSet         = lf.lfCharSet;
	font_desc.OutputPrecision = lf.lfOutPrecision;
	font_desc.Quality         = lf.lfQuality;
	font_desc.PitchAndFamily  = lf.lfPitchAndFamily;

	::lstrcpyn(font_desc.FaceName, lf.lfFaceName, LF_FACESIZE - 1);

	IDXGFont*   pFont = CreateSpriteFont(font_desc);
	pFont->PreloadCharacters(0x20, 0x7e);
//	pFont->PreloadCharacters(0xa1, 0xdf);

	return pFont;
}

//------------------------------------------------------------------------------
//	フォント解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseFont(IDXGFont*& pFont)
{
	m_ProtectedResource.erase(pFont);		// プロテクト解除

	m_Font.remove(pFont);
	delete pFont;
	pFont = NULL;
}

//------------------------------------------------------------------------------
//	全フォント解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllFonts()
{
	std::list<IDXGFont*>::iterator   list_it = m_Font.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Font.end()) {
			delete *list_it;
			list_it++;
		}
		m_Font.clear();
	} else {
		std::set<void*>::iterator    set_it;
		std::set<void*>::iterator    set_end = m_ProtectedResource.end();
		while(list_it != m_Font.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_Font.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	ステートブロック生成
//------------------------------------------------------------------------------
IStateBlock* CDXGraphics9::CreateStateBlock()
{
	IStateBlock*   pStateBlock = new CStateBlock(m_pD3DDevice);
	m_StateBlock.push_back(pStateBlock);

	return pStateBlock;
}

//------------------------------------------------------------------------------
//	ステートブロック解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseStateBlock(IStateBlock*& pStateBlock)
{
	m_ProtectedResource.erase(pStateBlock);		// プロテクト解除

	m_StateBlock.remove(pStateBlock);
	delete pStateBlock;
	pStateBlock = NULL;
}

//------------------------------------------------------------------------------
//	全ステートブロック解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllStateBlocks()
{
	std::list<IStateBlock*>::iterator   list_it  = m_StateBlock.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_StateBlock.end()) {
			delete *list_it;
			list_it++;
		}
		m_StateBlock.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_StateBlock.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_StateBlock.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	レンダリングターゲット生成
//------------------------------------------------------------------------------
IRenderTarget* CDXGraphics9::CreateRenderTarget(const UINT inWidth, const UINT inHeight,
												const D3DFORMAT inFormat,
												const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateRenderTarget)\n"));
		return NULL;
	}
#endif

	// 幅、高さ設定
	UINT   width  = inWidth;
	UINT   height = inHeight;

	// テクスチャ生成
	IDirect3DTexture9*   pD3DTexture = NULL;
	if(inFormat != D3DFMT_UNKNOWN) {
		if(::D3DXCreateTexture(m_pD3DDevice, width, height, 1,
							   D3DUSAGE_RENDERTARGET, inFormat, D3DPOOL_DEFAULT,
							  &pD3DTexture)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CDXGraphics9_CreateRenderTarget)\n"));
		}

		// 幅、高さ設定
		D3DSURFACE_DESC   desc;
		pD3DTexture->GetLevelDesc(0, &desc);
		width  = desc.Width;
		height = desc.Height;
	}

	// デプスステンシル生成
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		if(m_pD3DDevice->CreateDepthStencilSurface(width, height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - デプスステンシル生成失敗(CDXGraphics9_CreateRenderTarget)\n"));
		}
	}

	// レンダーターゲット生成
	IRenderTarget*   pRenderTarget;
	if(pD3DTexture != NULL || pDepthStencil != NULL) {
		pRenderTarget = new CRenderTarget(pD3DTexture, pDepthStencil, inWidth, inHeight);

		SafeRelease(pDepthStencil);
		SafeRelease(pD3DTexture);
	} else {
		pRenderTarget = new CNullRenderTarget();
	}

	m_RenderTarget.push_back(pRenderTarget);

	if(m_pRenderTargetVertex == NULL) {
		// 頂点バッファ生成
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// 管理リストから外す

		// 頂点情報を頂点バッファへ転送
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// 座標
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV座標
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// 頂点カラー
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pRenderTarget;
}

//------------------------------------------------------------------------------
//	キューブレンダリングターゲット生成
//------------------------------------------------------------------------------
IRenderTarget* CDXGraphics9::CreateCubeRenderTarget(const UINT inSize,
													const D3DFORMAT inFormat,
													const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateRenderTarget)\n"));
		return NULL;
	}
#endif

	// キューブテクスチャ生成
	IDirect3DCubeTexture9*   pCubeTexture = NULL;
	if(::D3DXCreateCubeTexture(m_pD3DDevice, inSize, 1,
							   D3DUSAGE_RENDERTARGET, inFormat, D3DPOOL_DEFAULT,
							  &pCubeTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - キューブテクスチャ生成失敗(CDXGraphics9_CreateCubeRenderTarget)\n"));
	}

	// デプスステンシル生成
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		D3DSURFACE_DESC   desc;
		pCubeTexture->GetLevelDesc(0, &desc);
		if(m_pD3DDevice->CreateDepthStencilSurface(desc.Width, desc.Height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - デプスステンシル生成失敗(CDXGraphics9_CreateCubeRenderTarget)\n"));
		}
	}

	// レンダーターゲット生成
	IRenderTarget*   pRenderTarget;
	if(pCubeTexture != NULL) {
		pRenderTarget = new CRenderTarget(pCubeTexture, pDepthStencil, inSize);

		SafeRelease(pDepthStencil);
		SafeRelease(pCubeTexture);
	} else {
		pRenderTarget = new CNullRenderTarget();
	}

	m_RenderTarget.push_back(pRenderTarget);

	if(m_pRenderTargetVertex == NULL) {
		// 頂点バッファ生成
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// 管理リストから外す

		// 頂点情報を頂点バッファへ転送
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// 座標
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV座標
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// 頂点カラー
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pRenderTarget;
}

//------------------------------------------------------------------------------
//	HDRレンダリングターゲット生成
//------------------------------------------------------------------------------
IRenderTarget* CDXGraphics9::CreateHDRRenderTarget(const UINT inWidth, const UINT inHeight,
												   const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateHDRRenderTarget)\n"));
		return NULL;
	}
#endif

	HRESULT              hr;
	IDirect3DTexture9*   pD3DTexture;

	// テクスチャ生成
	const D3DFORMAT   FORMAT[] = {D3DFMT_A16B16G16R16F, D3DFMT_A32B32G32R32F, D3DFMT_A16B16G16R16};
	for(UINT i = 0; i < sizeof(FORMAT) / sizeof(FORMAT[0]); i++) {
		hr = ::D3DXCreateTexture(m_pD3DDevice, inWidth, inHeight, 1,
								 D3DUSAGE_RENDERTARGET, FORMAT[i], D3DPOOL_DEFAULT,
								&pD3DTexture);
		if(hr == D3D_OK)
			break;
	}

	// デプスステンシル生成
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		D3DSURFACE_DESC   desc;
		pD3DTexture->GetLevelDesc(0, &desc);
		if(m_pD3DDevice->CreateDepthStencilSurface(desc.Width, desc.Height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - デプスステンシル生成失敗(CDXGraphics9_CreateHDRRenderTarget)\n"));
		}
	}

	// レンダーターゲット生成
	IRenderTarget*   pRenderTarget;
	if(hr == D3D_OK) {
		pRenderTarget = new CRenderTarget(pD3DTexture, pDepthStencil, inWidth, inHeight);
		SafeRelease(pDepthStencil);
		SafeRelease(pD3DTexture  );
	} else {
		::OutputDebugString(TEXT("*** Error - HDRテクスチャ生成失敗(CDXGraphics9_CreateHDRRenderTarget)\n"));
		pRenderTarget = new CNullRenderTarget();
	}

	m_RenderTarget.push_back(pRenderTarget);

	if(m_pRenderTargetVertex == NULL) {
		// 頂点バッファ生成
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// 管理リストから外す

		// 頂点情報を頂点バッファへ転送
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// 座標
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV座標
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// 頂点カラー
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pRenderTarget;
}

//------------------------------------------------------------------------------
//	レンダリングターゲット解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseRenderTarget(IRenderTarget*& pRenderTarget)
{
	m_ProtectedResource.erase(pRenderTarget);	// プロテクト解除

	m_RenderTarget.remove(pRenderTarget);
	delete pRenderTarget;
	pRenderTarget = NULL;

	if(m_RenderTarget.empty()) {
		// レンダリングターゲット用バーテックスバッファ解放
		delete m_pRenderTargetVertex;
		m_pRenderTargetVertex = NULL;
	}
}

//------------------------------------------------------------------------------
//	全レンダリングターゲット解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllRenderTargets()
{
	std::list<IRenderTarget*>::iterator   list_it = m_RenderTarget.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_RenderTarget.end()) {
			delete *list_it;
			list_it++;
		}
		m_RenderTarget.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_RenderTarget.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_RenderTarget.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}

	// レンダリングターゲット用バーテックスバッファ解放
	delete m_pRenderTargetVertex;
	m_pRenderTargetVertex = NULL;
}

//------------------------------------------------------------------------------
//	デフォルトレンダーターゲット設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetDefaultRenderTarget()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetDefaultRenderTarget)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetRenderTarget(0, m_pBackBufferSurface);
	m_pD3DDevice->SetDepthStencilSurface(m_pDepthStencilSurface);

	ResetViewport();
}

//------------------------------------------------------------------------------
//	レンダーターゲット→バックバッファ
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToBackBuffer(const RECT* pDestRect,
											IRenderTarget*& pSrcTarget, const RECT* pSrcRect,
											const D3DTEXTUREFILTERTYPE inFilter)
{
	SetDefaultRenderTarget();
	StretchRect(*pSrcTarget, pSrcRect, m_pBackBufferSurface, pDestRect, inFilter);
/*
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_RenderTargetToBackBuffer)\n"));
		return;
	}
#endif

	// レンダリングターゲットをデフォルトに戻す
	SetDefaultRenderTarget();

	// レンダーターゲットサーフェス取得
	IDirect3DSurface9*   pSrcSurface = pRenderTarget->GetSurface();
	if(pSrcSurface == NULL)
		return;

	// レンダーターゲット→バックバッファ
	m_pD3DDevice->StretchRect(pSrcSurface, pSrcRect, m_pBackBufferSurface, pDestRect, inFilter);
*/
}

//------------------------------------------------------------------------------
//	レンダーターゲット→バックバッファ
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToBackBuffer(IRenderTarget*& pRenderTarget, IEffect*& pEffect)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_RenderTargetToBackBuffer)\n"));
		return;
	}
#endif

	// レンダリングターゲットをデフォルトに戻す
	SetDefaultRenderTarget();

	// レンダリングターゲットサイズ確認
	const UINT   WIDTH  = pRenderTarget->GetWidth ();
	const UINT   HEIGHT = pRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// 頂点バッファ設定
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// 頂点座標
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// フォグ無効
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ｚバッファ無効
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ライティング無効
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// エフェクトを使ってバックバッファへレンダリング
	m_pD3DDevice->SetTexture(0, pRenderTarget->GetTexture());

	const UINT   pass = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

		pEffect->EndPass();
	}
	pEffect->End();

	// ステート復元
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	レンダーターゲット→バックバッファ
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToBackBuffer(IRenderTarget*& pRenderTarget,
											IEffect*& pEffect, const UINT inPass)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_RenderTargetToBackBuffer)\n"));
		return;
	}
#endif

	// レンダリングターゲットをデフォルトに戻す
	SetDefaultRenderTarget();

	// レンダリングターゲットサイズ確認
	const UINT   WIDTH  = pRenderTarget->GetWidth ();
	const UINT   HEIGHT = pRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// 頂点バッファ設定
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// 頂点座標
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// フォグ無効
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ｚバッファ無効
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ライティング無効
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// エフェクトを使ってバックバッファへレンダリング
	m_pD3DDevice->SetTexture(0, pRenderTarget->GetTexture());

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

	pEffect->EndPass();
	pEffect->End();

	// ステート復元
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	レンダーターゲット→レンダーターゲット
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToRenderTarget(IRenderTarget*& pDestRenderTarget,
											  IRenderTarget*&  pSrcRenderTarget,
											  IEffect*&        pEffect)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_RenderTargetToRenderTarget)\n"));
		return;
	}
#endif

	// レンダリングターゲットを転送先レンダーターゲットにする
	pDestRenderTarget->Use();

	// レンダリングターゲットサイズ確認
	const UINT   WIDTH  = pSrcRenderTarget->GetWidth ();
	const UINT   HEIGHT = pSrcRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// 頂点バッファ設定
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// 頂点座標
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// フォグ無効
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ｚバッファ無効
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ライティング無効
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// エフェクトを使って転送先レンダーターゲットへレンダリング
	m_pD3DDevice->SetTexture(0, pSrcRenderTarget->GetTexture());

	const UINT   pass = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

		pEffect->EndPass();
	}
	pEffect->End();

	// ステート復元
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	レンダーターゲット→レンダーターゲット
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToRenderTarget(IRenderTarget*& pDestRenderTarget,
											  IRenderTarget*&  pSrcRenderTarget,
											  IEffect*& pEffect, const UINT inPass)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_RenderTargetToRenderTarget)\n"));
		return;
	}
#endif

	// レンダリングターゲットを転送先レンダーターゲットにする
	pDestRenderTarget->Use();

	// レンダリングターゲットサイズ確認
	const UINT   WIDTH  = pSrcRenderTarget->GetWidth ();
	const UINT   HEIGHT = pSrcRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// 頂点バッファ設定
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// 頂点座標
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// フォグ無効
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ｚバッファ無効
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ライティング無効
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// エフェクトを使って転送先レンダーターゲットへレンダリング
	m_pD3DDevice->SetTexture(0, pSrcRenderTarget->GetTexture());

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

	pEffect->EndPass();
	pEffect->End();

	// ステートを戻す
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	サーフェス転送
//------------------------------------------------------------------------------
void CDXGraphics9::StretchRect(IDirect3DSurface9* pDestSurface, const RECT* pDestRect,
							   IDirect3DSurface9* pSrcSurface,  const RECT* pSrcRect,
							   const D3DTEXTUREFILTERTYPE inFilter)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_StretchRect)\n"));
		return;
	}
#endif

	m_pD3DDevice->StretchRect(pSrcSurface, pSrcRect, pDestSurface, pDestRect, inFilter);
}

//------------------------------------------------------------------------------
//	サーフェスロード
//------------------------------------------------------------------------------
void CDXGraphics9::LoadSurfaceFromSurface(IDirect3DSurface9* pDestSurface,
										  const        RECT* pDestRect,
										  IDirect3DSurface9* pSrcSurface,
										  const        RECT* pSrcRect,
										  const DWORD inFilter , const D3DCOLOR inColorKey)
{
	::D3DXLoadSurfaceFromSurface(pDestSurface, NULL, pDestRect,
								 pSrcSurface,  NULL, pSrcRect,
								 inFilter, inColorKey);
}

//------------------------------------------------------------------------------
//	スプライト3Dレイヤー生成
//------------------------------------------------------------------------------
ISprite3DLayer* CDXGraphics9::CreateSprite3DLayer(const UINT      inWidth , 
												  const UINT      inHeight,
												  const D3DFORMAT inFormat,
												  const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_CreateSprite3DLayer)\n"));
		return NULL;
	}
#endif

	// 幅、高さ設定
	UINT   width  = inWidth;
	UINT   height = inHeight;

	// テクスチャ生成
	IDirect3DTexture9*   pD3DTexture = NULL;
	if(inFormat != D3DFMT_UNKNOWN) {
		if(::D3DXCreateTexture(m_pD3DDevice, width, height, 1,
							   D3DUSAGE_RENDERTARGET, inFormat, D3DPOOL_DEFAULT,
							  &pD3DTexture)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CDXGraphics9_CreateSprite3DLayer)\n"));
		}

		// 幅、高さ設定
		D3DSURFACE_DESC   desc;
		pD3DTexture->GetLevelDesc(0, &desc);
		width  = desc.Width;
		height = desc.Height;
	}

	// デプスステンシル生成
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		if(m_pD3DDevice->CreateDepthStencilSurface(width, height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - デプスステンシル生成失敗(CDXGraphics9_CreateSprite3DLayer)\n"));
		}
	}

	// レンダーターゲット生成
	ISprite3DLayer*   pSprite3DLayer;
	if(pD3DTexture != NULL || pDepthStencil != NULL) {
		pSprite3DLayer = new CSprite3DLayer(pD3DTexture, pDepthStencil, inWidth, inHeight);

		SafeRelease(pDepthStencil);
		SafeRelease(pD3DTexture);
	} else {
		pSprite3DLayer = new CNullSprite3DLayer();
	}

	m_RenderTarget.push_back(pSprite3DLayer);

	if(m_pRenderTargetVertex == NULL) {
		// 頂点バッファ生成
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// 管理リストから外す

		// 頂点情報を頂点バッファへ転送
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// 座標
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV座標
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// 頂点カラー
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pSprite3DLayer;
}

//------------------------------------------------------------------------------
//	リソースプロテクト設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}

//------------------------------------------------------------------------------
//	ブレンディングモード開始
//------------------------------------------------------------------------------
bool CDXGraphics9::BeginAlphaBlend(const DXGBLENDMODE inBlendMode)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_BeginAlphaBlend)\n"));
		return false;
	}
#endif
	// レンダリングステート保存
	m_pD3DDevice->GetRenderState(D3DRS_CULLMODE,              &m_AlphaState.CullMode);

	m_pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,      &m_AlphaState.AlphaEnable);
	m_pD3DDevice->GetRenderState(D3DRS_BLENDOP,               &m_AlphaState.BlendOP);
	m_pD3DDevice->GetRenderState(D3DRS_DESTBLEND,             &m_AlphaState.DestBlend);
	m_pD3DDevice->GetRenderState(D3DRS_SRCBLEND,              &m_AlphaState.SrcBlend);
	m_pD3DDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &m_AlphaState.Diffuse);

	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE,               &m_AlphaState.ZEnable);
	m_pD3DDevice->GetRenderState(D3DRS_ZWRITEENABLE,          &m_AlphaState.ZWriteEnable);

	m_pD3DDevice->GetTextureStageState(0, D3DTSS_ALPHAOP,     &m_AlphaState.AlphaOP);

	// レンダリングステート設定
	// アルファブレンド設定
	SetBlendMode(inBlendMode);

	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      TRUE);

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,     D3DTOP_MODULATE);

	return true;
}

//------------------------------------------------------------------------------
//	ブレンディングモード設定
//------------------------------------------------------------------------------
void CDXGraphics9::SetBlendMode(const DXGBLENDMODE inBlendMode)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_SetBlendMode)\n"));
		return;
	}
#endif

	// ブレンドモード設定
	switch(inBlendMode) {
	  // 乗算合成
	  case DXGBLEND_MODULATE:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;

	  // 加算合成
	  case DXGBLEND_ADD:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;

	  // 減算合成
	  case DXGBLEND_SUBTRACT:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_SUBTRACT);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;

	  case DXGBLEND_SRCCOLOR:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		break;

	  case DXGBLEND_ZEROCOLOR:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
/*
	  // ブレンディングなし
	  case DXGBLEND_NONE:
	  default:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
*/
	}
}

//------------------------------------------------------------------------------
//	アルファブレンド終了
//------------------------------------------------------------------------------
void CDXGraphics9::EndAlphaBlend()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_EndAlphaBlend)\n"));
		return;
	}
#endif

	// レンダリングステート復元
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,     m_AlphaState.AlphaOP);

	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,          m_AlphaState.ZWriteEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,               m_AlphaState.ZEnable);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      m_AlphaState.AlphaEnable);
	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, m_AlphaState.Diffuse);
	m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,               m_AlphaState.BlendOP);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,             m_AlphaState.DestBlend);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              m_AlphaState.SrcBlend);

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,              m_AlphaState.CullMode);
}

//------------------------------------------------------------------------------
//	シャドウレンダリング開始
//------------------------------------------------------------------------------
bool CDXGraphics9::BeginShadowRendering()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_BeginShadowRendering)\n"));
		return false;
	}
#endif
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0);

	// レンダリングステート保存
	m_pD3DDevice->GetRenderState(D3DRS_CULLMODE,  &m_AlphaState.CullMode);
	m_pD3DDevice->GetRenderState(D3DRS_SHADEMODE, &m_AlphaState.ShadeMode);

	m_pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,      &m_AlphaState.AlphaEnable);
	m_pD3DDevice->GetRenderState(D3DRS_BLENDOP,               &m_AlphaState.BlendOP);
	m_pD3DDevice->GetRenderState(D3DRS_DESTBLEND,             &m_AlphaState.DestBlend);
	m_pD3DDevice->GetRenderState(D3DRS_SRCBLEND,              &m_AlphaState.SrcBlend);
	m_pD3DDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &m_AlphaState.Diffuse);

	m_pD3DDevice->GetRenderState(D3DRS_STENCILENABLE, &m_AlphaState.StencilEnable);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILFUNC,   &m_AlphaState.StencilFunc);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILPASS,   &m_AlphaState.StencilPass);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILFAIL,   &m_AlphaState.StencilFail);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILZFAIL,  &m_AlphaState.StencilZFail);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILREF,    &m_AlphaState.StencilRef);

	// レンダリングステート設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,  D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

	// アルファブレンド設定
	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,               D3DBLENDOP_ADD);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,             D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              D3DBLEND_ZERO);
//	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      TRUE);

	// ステンシル設定
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_NOTEQUAL);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_REPLACE);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFAIL,   D3DSTENCILOP_KEEP);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILZFAIL,  D3DSTENCILOP_KEEP);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,    1);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

	return true;
}

//------------------------------------------------------------------------------
//	シャドウレンダリング終了
//------------------------------------------------------------------------------
void CDXGraphics9::EndShadowRendering()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDXGraphics9_EndShadowRendering)\n"));
		return;
	}
#endif

	// レンダリングステート復元
	m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, m_AlphaState.StencilEnable);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,   m_AlphaState.StencilFunc);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,   m_AlphaState.StencilPass);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFAIL,   m_AlphaState.StencilFail);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILZFAIL,  m_AlphaState.StencilZFail);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,    m_AlphaState.StencilRef);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      m_AlphaState.AlphaEnable);
	m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,               m_AlphaState.BlendOP);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,             m_AlphaState.DestBlend);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              m_AlphaState.SrcBlend);
	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, m_AlphaState.Diffuse);

	m_pD3DDevice->SetRenderState(D3DRS_SHADEMODE, m_AlphaState.ShadeMode);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,  m_AlphaState.CullMode);
}

//------------------------------------------------------------------------------
//	デバイスコンテキスト取得
//------------------------------------------------------------------------------
HDC CDXGraphics9::GetDC()
{
#ifdef _DEBUG
	if(m_pBackBufferSurface == NULL) {
		::OutputDebugString(TEXT("*** Error - バックバッファ未取得(CDXGraphics9_GetDC)\n"));
		return NULL;
	}
#endif
	if(m_hBackBufferDC != NULL)
		return m_hBackBufferDC;

	m_pBackBufferSurface->GetDC(&m_hBackBufferDC);

	return m_hBackBufferDC;
}

//------------------------------------------------------------------------------
//	デバイスコンテキスト解放
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseDC()
{
	// バックバッファ解放
	if(m_hBackBufferDC != NULL) {
		m_pBackBufferSurface->ReleaseDC(m_hBackBufferDC);
		m_hBackBufferDC = NULL;
	}
}
