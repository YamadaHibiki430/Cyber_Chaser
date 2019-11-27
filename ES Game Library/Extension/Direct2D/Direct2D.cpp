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
	 Direct2D.hpp
				Direct2Dクラスヘッダ

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Direct2D.hpp"
#include "../Common/ComLibrary.hpp"

#include <wincodec.h>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "windowscodecs.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CDirect2D::CDirect2D() : m_pD2DFactory(NULL), m_pDCRenderTarget(NULL),
						 m_pD3DSurface(NULL), m_hDC(NULL),
						 m_pDWriteFactory(NULL)
{
	CComLibrary::Initialize();
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CDirect2D::~CDirect2D()
{
	Release();
	CComLibrary::Uninitialize();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CDirect2D::Initialize()
{
	Release();

	// Direct2Dファクトリー生成
	if(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_ID2D1Factory,
						   (void**)&m_pD2DFactory)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - Direct2Dファクトリー生成失敗(CDirect2D_Initialize)\n"));
		return false;
	}

	// レンダーターゲット生成
	D2D1_RENDER_TARGET_PROPERTIES   rtprop;
	rtprop.type        = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	rtprop.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED); 
	rtprop.minLevel    = D2D1_FEATURE_LEVEL_DEFAULT;
	rtprop.usage       = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
	rtprop.dpiX        = 0.0f;
	rtprop.dpiY        = 0.0f;

	if(m_pD2DFactory->CreateDCRenderTarget(&rtprop, &m_pDCRenderTarget) != S_OK) {
		::OutputDebugString(TEXT("*** Error - レンダーターゲット生成失敗(CDirect2D_Initialize)\n"));
		return false;
	}

	// DirectWriteファクトリー生成
	if(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
							 (IUnknown**)&m_pDWriteFactory)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - DirectWriteファクトリー生成失敗(CDirect2D_Initialize)\n"));
//		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CDirect2D::Release()
{
	m_ProtectedResource.clear();
	ReleaseAllResources();

	SafeRelease(m_pDWriteFactory);

	if(m_hDC != NULL) {
		m_pD3DSurface->ReleaseDC(m_hDC);
		m_hDC = NULL;
	}

	SafeRelease(m_pD3DSurface);
	SafeRelease(m_pDCRenderTarget);
	SafeRelease(m_pD2DFactory);
}

//------------------------------------------------------------------------------
//	描画開始
//------------------------------------------------------------------------------
void CDirect2D::BeginDraw(IDirect3DTexture9* pD3DTexture)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_BeginDraw)\n"));
		return;
	}
	if(pD3DTexture == NULL) {
		::OutputDebugString(TEXT("*** Error - テクスチャ未初期化(CDirect2D_BeginDraw)\n"));
		return;
	}
	if(m_hDC != NULL) {
		::OutputDebugString(TEXT("*** Error - EndDraw未実行(CDirect2D_BeginDraw)\n"));
		return;
	}
#endif

	// サーフェス取得
	pD3DTexture->GetSurfaceLevel(0, &m_pD3DSurface);

	// 描画領域設定
	D3DSURFACE_DESC   desc;
	m_pD3DSurface->GetDesc(&desc);

	RECT   rect;
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = desc.Width;
	rect.bottom = desc.Height;

	// デバイスコンテキストバインド
	if(m_pD3DSurface->GetDC(&m_hDC) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - デバイスコンテキスト取得失敗(CDirect2D_BeginDraw)\n"));

		m_pD3DSurface->Release();
		m_pD3DSurface = NULL;

		return;
	}
	m_pDCRenderTarget->BindDC(m_hDC, &rect);

	m_pDCRenderTarget->BeginDraw();
}

//------------------------------------------------------------------------------
//	描画開始
//------------------------------------------------------------------------------
void CDirect2D::BeginDraw(HDC hDC, RECT& inRect)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_BeginDraw)\n"));
		return;
	}
	if(m_hDC != NULL) {
		::OutputDebugString(TEXT("*** Error - EndDraw未実行(CDirect2D_BeginDraw)\n"));
		return;
	}
#endif

	m_pDCRenderTarget->BindDC(m_hDC, &inRect);
	m_pDCRenderTarget->BeginDraw();
}

//------------------------------------------------------------------------------
//	描画終了
//------------------------------------------------------------------------------
void CDirect2D::EndDraw()
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_EndDraw)\n"));
		return;
	}
#endif

	m_pDCRenderTarget->EndDraw();

	if(m_hDC != NULL) {
		m_pD3DSurface->ReleaseDC(m_hDC);
		m_hDC = NULL;
	}

	SafeRelease(m_pD3DSurface);
}

//------------------------------------------------------------------------------
//	変換行列設定
//------------------------------------------------------------------------------
void CDirect2D::SetTransform(const D2D_MATRIX_3X2_F& inTransform)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_SetTransform)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->SetTransform(inTransform);
}

//------------------------------------------------------------------------------
//	リソース解放
//------------------------------------------------------------------------------
void CDirect2D::ReleaseResource(ID2D1Resource* pResource)
{
	if(pResource == NULL)
		return;

	m_ProtectedResource.erase (pResource);	// プロテクト解除
	m_ResourceList     .remove(pResource);

	pResource->Release();
//	pResource = NULL;
}

//------------------------------------------------------------------------------
//	全リソース解放
//------------------------------------------------------------------------------
void CDirect2D::ReleaseAllResources()
{
	std::list<IUnknown*>::iterator   list_it = m_ResourceList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_ResourceList.end()) {
			(*list_it)->Release();
			list_it++;
		}
		m_ResourceList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_ResourceList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				(*list_it)->Release();
				list_it = m_ResourceList.erase(list_it);
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
void CDirect2D::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}

//------------------------------------------------------------------------------
//	ビットマップ生成
//------------------------------------------------------------------------------
ID2D1Bitmap* CDirect2D::CreateBitmapFromFile(LPCWSTR inFileName, const UINT inFrameIdx)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateBitmapFromFile)\n"));
		return NULL;
	}
#endif

	ID2D1Bitmap*             pD2DBitmap    = NULL;

	IWICImagingFactory*      pWICFactory   = NULL;
	IWICBitmapDecoder*       pWICDecoder   = NULL;
	IWICBitmapFrameDecode*   pWICFrameDec  = NULL;
	IWICFormatConverter*     pWICConverter = NULL;

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
		if(pWICDecoder->GetFrame(inFrameIdx, &pWICFrameDec) != S_OK)
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

		// Direct2Dビットマップ生成
		if(m_pDCRenderTarget->CreateBitmapFromWicBitmap(pWICConverter, &pD2DBitmap)
		   != S_OK)
			throw TEXT("Direct2Dビットマップ生成失敗");

		m_ResourceList.push_back(pD2DBitmap);
	} catch(LPCTSTR err_str) {
#ifdef _DEBUG
		// エラーメッセージ生成
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDirect2D_CreateBitmapFromFile)\n"), err_str);
		::OutputDebugString(msg);
#endif
	}

	SafeRelease(pWICConverter);
	SafeRelease(pWICFrameDec);
	SafeRelease(pWICDecoder);
	SafeRelease(pWICFactory);

	return pD2DBitmap;
}

//------------------------------------------------------------------------------
//	ストローク生成
//------------------------------------------------------------------------------
ID2D1StrokeStyle* CDirect2D::CreateStrokeStyle(const D2D1_STROKE_STYLE_PROPERTIES& inStrokeStyleProp,
											   const float* pDashes, const UINT inDashesCount)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateStrokeStyle)\n"));
		return NULL;
	}
#endif

	ID2D1StrokeStyle*   pStrokeStyle = NULL;
	if(m_pD2DFactory->CreateStrokeStyle(inStrokeStyleProp, pDashes, inDashesCount, &pStrokeStyle)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - ストローク生成失敗(CDirect2D_CreateStrokeStyle)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pStrokeStyle);

	return pStrokeStyle;
}

//------------------------------------------------------------------------------
//	グラディエントストップコレクション生成
//------------------------------------------------------------------------------
ID2D1GradientStopCollection* CDirect2D::CreateGradientStopCollection(D2D1_GRADIENT_STOP inGradientStops[],
																	 const UINT inGradientStopsCount)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateGradientStopCollection)\n"));
		return NULL;
	}
#endif

	ID2D1GradientStopCollection*   pGradientStopCollection = NULL;
	if(m_pDCRenderTarget->CreateGradientStopCollection(inGradientStops,
													   inGradientStopsCount,
													  &pGradientStopCollection)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - グラディエントストップコレクション生成失敗(CDirect2D_CreateGradientStopCollection)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGradientStopCollection);

	return pGradientStopCollection;
}

//------------------------------------------------------------------------------
//	ブラシ生成
//------------------------------------------------------------------------------
ID2D1SolidColorBrush* CDirect2D::CreateSolidColorBrush(const struct _D3DCOLORVALUE& inColor)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateSolidColorBrush)\n"));
		return NULL;
	}
#endif

	ID2D1SolidColorBrush*   pBrush = NULL;
	D2D1_COLOR_F*           pColor = (D2D1_COLOR_F*)&inColor;
	if(m_pDCRenderTarget->CreateSolidColorBrush(*pColor, &pBrush) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ブラシ生成失敗(CDirect2D_CreateSolidColorBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	ビットマップブラシ生成
//------------------------------------------------------------------------------
ID2D1BitmapBrush* CDirect2D::CreateBitmapBrush(ID2D1Bitmap*& pBitmap)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
#endif

	ID2D1BitmapBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateBitmapBrush(pBitmap, &pBrush) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ビットマップブラシ生成失敗(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	ビットマップブラシ生成
//------------------------------------------------------------------------------
ID2D1BitmapBrush* CDirect2D::CreateBitmapBrush(ID2D1Bitmap*& pBitmap,
											   const D2D1_BITMAP_BRUSH_PROPERTIES& inBmpBrushProp)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
#endif

	ID2D1BitmapBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateBitmapBrush(pBitmap, inBmpBrushProp, &pBrush) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ビットマップブラシ生成失敗(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	線状グラデーションブラシ生成
//------------------------------------------------------------------------------
ID2D1LinearGradientBrush* CDirect2D::CreateLinearGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
															   const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& inGradientProp)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateLinearGradientBrush)\n"));
		return NULL;
	}
#endif

	ID2D1LinearGradientBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateLinearGradientBrush(inGradientProp, pGradientStop, &pBrush)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - 線状グラデーションブラシ生成失敗(CDirect2D_CreateLinearGradientBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	放射状グラデーションブラシ生成
//------------------------------------------------------------------------------
ID2D1RadialGradientBrush* CDirect2D::CreateRadialGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
															   const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& inGradientProp)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateRadialGradientBrush)\n"));
		return NULL;
	}
#endif

	ID2D1RadialGradientBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateRadialGradientBrush(inGradientProp, pGradientStop, &pBrush)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - 放射状グラデーションブラシ生成失敗(CDirect2D_CreateRadialGradientBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	矩形ジオメトリ生成
//------------------------------------------------------------------------------
ID2D1RectangleGeometry* CDirect2D::CreateRectangleGeometry(const D2D1_RECT_F& inRectangle)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateRectangleGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1RectangleGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateRectangleGeometry(inRectangle, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - 矩形ジオメトリ生成失敗(CDirect2D_CreateRectangleGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	丸型矩形ジオメトリ生成
//------------------------------------------------------------------------------
ID2D1RoundedRectangleGeometry* CDirect2D::CreateRoundedRectangleGeometry(const D2D1_ROUNDED_RECT& inRectangle)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateRoundedRectangleGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1RoundedRectangleGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateRoundedRectangleGeometry(inRectangle, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - 丸型矩形ジオメトリ生成失敗(CDirect2D_CreateRoundedRectangleGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	楕円ジオメトリ生成
//------------------------------------------------------------------------------
ID2D1EllipseGeometry* CDirect2D::CreateEllipseGeometry(const D2D1_ELLIPSE& inEllipse)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateEllipseGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1EllipseGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateEllipseGeometry(inEllipse, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - 楕円ジオメトリ生成失敗(CDirect2D_CreateEllipseGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	パスジオメトリ生成
//------------------------------------------------------------------------------
ID2D1PathGeometry* CDirect2D::CreatePathGeometry()
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreatePathGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1PathGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreatePathGeometry(&pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - パスジオメトリ生成失敗(CDirect2D_CreatePathGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	変換ジオメトリ生成
//------------------------------------------------------------------------------
ID2D1TransformedGeometry* CDirect2D::CreateTransformedGeometry(ID2D1Geometry*& pSourceGeometry,
															   const D2D1_MATRIX_3X2_F &inTransform)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateTransformedGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1TransformedGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateTransformedGeometry(pSourceGeometry, inTransform,
												&pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - 変換ジオメトリ生成失敗(CDirect2D_CreateTransformedGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	ジオメトリグループ生成
//------------------------------------------------------------------------------
ID2D1GeometryGroup* CDirect2D::CreateGeometryGroup(const D2D1_FILL_MODE inFillMode,
												   ID2D1Geometry* pGeometries[],
												   const UINT inGeometriesCount)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateGeometryGroup)\n"));
		return NULL;
	}
#endif

	ID2D1GeometryGroup*   pGeometry = NULL;
	if(m_pD2DFactory->CreateGeometryGroup(inFillMode, pGeometries,
										  inGeometriesCount, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ジオメトリグループ生成失敗(CDirect2D_CreateGeometryGroup)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	レイヤー生成
//------------------------------------------------------------------------------
ID2D1Layer* CDirect2D::CreateLayer()
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
#endif

	ID2D1Layer*   pLayer = NULL;
	if(m_pDCRenderTarget->CreateLayer(&pLayer) != S_OK) {
		::OutputDebugString(TEXT("*** Error - レイヤー生成失敗(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pLayer);

	return pLayer;
}

//------------------------------------------------------------------------------
//	レイヤー生成
//------------------------------------------------------------------------------
ID2D1Layer* CDirect2D::CreateLayer(const D2D1_SIZE_F& inSize)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
#endif

	ID2D1Layer*   pLayer = NULL;
	if(m_pDCRenderTarget->CreateLayer(inSize, &pLayer) != S_OK) {
		::OutputDebugString(TEXT("*** Error - レイヤー生成失敗(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pLayer);

	return pLayer;
}

//------------------------------------------------------------------------------
//	レイヤープッシュ
//------------------------------------------------------------------------------
void CDirect2D::PushLayer(ID2D1Layer*& pLayer, const D2D1_LAYER_PARAMETERS& inLayerParams)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_PushLayer)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->PushLayer(inLayerParams, pLayer);
}

//------------------------------------------------------------------------------
//	レイヤーポップ
//------------------------------------------------------------------------------
void CDirect2D::PopLayer()
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_PopLayer)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->PopLayer();
}

//------------------------------------------------------------------------------
//	テキストフォーマット生成
//------------------------------------------------------------------------------
IDWriteTextFormat* CDirect2D::CreateTextFormat(const WCHAR*              pFontFamilyName,
											   const DWRITE_FONT_WEIGHT  inFontWeight,
											   const DWRITE_FONT_STYLE   inFontStyle,
											   const DWRITE_FONT_STRETCH inFontStretch,
											   const float inFontSize, const WCHAR* pLocaleName)
{
#ifdef _DEBUG
	if(m_pDWriteFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectWrite未初期化(CDirect2D_CreateTextFormat)\n"));
		return NULL;
	}
#endif

	IDWriteTextFormat*   pTextFormat = NULL;
	if(m_pDWriteFactory->CreateTextFormat(pFontFamilyName, NULL,
										  inFontWeight, inFontStyle, inFontStretch,
										  inFontSize, pLocaleName, &pTextFormat)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - テキストフォーマット生成失敗(CDirect2D_CreateTextFormat)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pTextFormat);

	return pTextFormat;
}

//------------------------------------------------------------------------------
//	テキストレイアウト生成
//------------------------------------------------------------------------------
IDWriteTextLayout* CDirect2D::CreateTextLayout(const WCHAR* pString, const UINT inLength,
											   IDWriteTextFormat*& pTextFormat,
											   const float inMaxWidth, const float inMaxHeight)
{
#ifdef _DEBUG
	if(m_pDWriteFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectWrite未初期化(CDirect2D_CreateTextLayout)\n"));
		return NULL;
	}
#endif

	IDWriteTextLayout*   pTextLayout = NULL;
	if(m_pDWriteFactory->CreateTextLayout(pString, inLength, pTextFormat,
										  inMaxWidth, inMaxHeight, &pTextLayout)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - テキストレイアウト生成失敗(CDirect2D_CreateTextLayout)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pTextLayout);

	return pTextLayout;
}

//------------------------------------------------------------------------------
//	タイポグラフィ生成
//------------------------------------------------------------------------------
IDWriteTypography* CDirect2D::CreateTypography()
{
#ifdef _DEBUG
	if(m_pDWriteFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectWrite未初期化(CDirect2D_CreateTypography)\n"));
		return NULL;
	}
#endif

	IDWriteTypography*   pTypography = NULL;
	if(m_pDWriteFactory->CreateTypography(&pTypography) != S_OK) {
		::OutputDebugString(TEXT("*** Error - テキストレイアウト生成失敗(CDirect2D_CreateTypography)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pTypography);

	return pTypography;
}

/*
//------------------------------------------------------------------------------
//	描画領域クリア
//------------------------------------------------------------------------------
void CDirect2D::Clear(const struct _D3DCOLORVALUE& inColor)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_Clear)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->Clear((D2D1_COLOR_F*)&inColor);
}

//------------------------------------------------------------------------------
//	ビットマップ描画
//------------------------------------------------------------------------------
void CDirect2D::DrawBitmap(ID2D1Bitmap*& pBitmap,
						   const D2D_RECT_F& inDestRect,
						   const D2D_RECT_F& inSrcRect , const float inAlpha,
						   const D2D1_BITMAP_INTERPOLATION_MODE inMode)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_DrawBitmap)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawBitmap(pBitmap, inDestRect, inAlpha, inMode, inSrcRect);
}

//------------------------------------------------------------------------------
//	楕円描画
//------------------------------------------------------------------------------
void CDirect2D::DrawEllipse(const D2D1_ELLIPSE &inEllipse, ID2D1Brush*& pBrush,
							const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_DrawEllipse)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawEllipse(inEllipse, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	ジオメトリ描画
//------------------------------------------------------------------------------
void CDirect2D::DrawGeometry(ID2D1Geometry*& pGgeometry, ID2D1Brush*& pBrush,
							 const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_DrawGeometry)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawGeometry(pGgeometry, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	線描画
//------------------------------------------------------------------------------
void CDirect2D::DrawLine(D2D1_POINT_2F& inStart, D2D1_POINT_2F& inEnd, ID2D1Brush*& pBrush,
						 const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_DrawLine)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawLine(inStart, inEnd, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	矩形描画
//------------------------------------------------------------------------------
void CDirect2D::DrawRectangle(const D2D1_RECT_F &inRect, ID2D1Brush*& pBrush,
							  const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_DrawRectangle)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawRectangle(inRect, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	丸矩形描画
//------------------------------------------------------------------------------
void CDirect2D::DrawRoundedRectangle(const D2D1_ROUNDED_RECT &inRoundRect, ID2D1Brush*& pBrush,
									 const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_DrawRoundedRectangle)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawRoundedRectangle(inRoundRect, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	塗りつぶし楕円描画
//------------------------------------------------------------------------------
void CDirect2D::FillEllipse(const D2D1_ELLIPSE &inEllipse, ID2D1Brush*& pBrush)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D未初期化(CDirect2D_FillEllipse)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->FillEllipse(inEllipse, pBrush);
}
*/
