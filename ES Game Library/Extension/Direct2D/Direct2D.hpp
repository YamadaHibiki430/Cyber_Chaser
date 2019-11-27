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

#ifndef _INC_DIRECT2D
#define _INC_DIRECT2D

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4005)
#include <D2D1.h>
#include <Dwrite.h>
#pragma warning(pop)

#include <d3d9.h>
#include <list>
#include <set>

//------------------------------------------------------------------------------
//	Direct2Dクラス定義
//------------------------------------------------------------------------------
class CDirect2D {
public:
	virtual ~CDirect2D();

	// シングルトンインスタンスの取得
	static CDirect2D& GetInstance()
	{
		static CDirect2D theDirect2D;
		return theDirect2D;
	}

	// オペレーター
	inline ID2D1DCRenderTarget* operator->() { return m_pDCRenderTarget; }

	// Direct2D初期化・解放
	bool Initialize();
	void Release();

	// 描画開始・終了
	void BeginDraw(IDirect3DTexture9* pD3DTexture);
	void BeginDraw(HDC hDC, RECT& inRect);
	void EndDraw();

	// 変換行列
	void SetTransform(const D2D_MATRIX_3X2_F& inTransform);

	inline void SetTransform(const D3DMATRIX& inTransform)
	{
		D2D_MATRIX_3X2_F   transform;
		transform._11 = inTransform._11;
		transform._12 = inTransform._12;
		transform._21 = inTransform._21;
		transform._22 = inTransform._22;
		transform._31 = inTransform._31;
		transform._32 = inTransform._32;
		SetTransform(transform);
	}

	// リソース解放
	void ReleaseResource(ID2D1Resource* pResource);
	void ReleaseAllResources();

	void SetResourceProtect(void* pResource, const bool inProtect);

	// ビットマップ生成	
	ID2D1Bitmap* CreateBitmapFromFile(LPCWSTR inFileName, const UINT inFrameIdx = 0);

	// ストローク生成
	ID2D1StrokeStyle* CreateStrokeStyle(const D2D1_STROKE_STYLE_PROPERTIES& inStrokeStyleProp,
										const float* pDashes = NULL, const UINT inDashesCount = 0);

	// グラデーション生成
	ID2D1GradientStopCollection* CreateGradientStopCollection(D2D1_GRADIENT_STOP inGradientStops[],
															  const UINT inGradientStopsCount);

	// ブラシ生成
	ID2D1SolidColorBrush* CreateSolidColorBrush(const struct _D3DCOLORVALUE& inColor);

	ID2D1BitmapBrush* CreateBitmapBrush(ID2D1Bitmap*& pBitmap);
	ID2D1BitmapBrush* CreateBitmapBrush(ID2D1Bitmap*& pBitmap,
										const D2D1_BITMAP_BRUSH_PROPERTIES& inBmpBrushProp);

	ID2D1LinearGradientBrush* CreateLinearGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
														const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& inGradientProp);

	ID2D1RadialGradientBrush* CreateRadialGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
														const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& inGradientProp);

	// ジオメトリ
	ID2D1RectangleGeometry*        CreateRectangleGeometry(const D2D1_RECT_F& inRectangle);
	ID2D1RoundedRectangleGeometry* CreateRoundedRectangleGeometry(const D2D1_ROUNDED_RECT& inRectangle);
	ID2D1EllipseGeometry*          CreateEllipseGeometry(const D2D1_ELLIPSE& inEllipse);
	ID2D1PathGeometry*             CreatePathGeometry();
	ID2D1TransformedGeometry*      CreateTransformedGeometry(ID2D1Geometry*& pSourceGeometry,
															 const D2D1_MATRIX_3X2_F &inTransform);
	ID2D1GeometryGroup*            CreateGeometryGroup(const D2D1_FILL_MODE inFillMode,
													   ID2D1Geometry* pGeometries[],
													   const UINT inGeometriesCount);

	// レイヤー
	ID2D1Layer* CreateLayer();
	ID2D1Layer* CreateLayer(const D2D1_SIZE_F& inSize);

	void PushLayer(ID2D1Layer*& pLayer, const D2D1_LAYER_PARAMETERS& inLayerParams);
	void PopLayer();

	// DirectWrite
	IDWriteTextFormat* CreateTextFormat(const WCHAR*              pFontFamilyName,
										const DWRITE_FONT_WEIGHT  inFontWeight,
										const DWRITE_FONT_STYLE   inFontStyle,
										const DWRITE_FONT_STRETCH inFontStretch,
										const float inFontSize, const WCHAR* pLocaleName = L"ja");

	IDWriteTextLayout* CreateTextLayout(const WCHAR* pString, const UINT inLength,
										IDWriteTextFormat*& pTextFormat,
										const float inMaxWidth, const float inMaxHeight);

	IDWriteTypography* CreateTypography();


	// 描画関連の機能はすべて->演算子で呼び出し
/*
	// クリア
	void Clear(const struct _D3DCOLORVALUE& inColor);

	// 描画
	void DrawBitmap(ID2D1Bitmap*& pBitmap, const D2D_RECT_F& inDestRect,
					const D2D_RECT_F& inSrcRect, const float inAlpha = 1.0f,
					const D2D1_BITMAP_INTERPOLATION_MODE inMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

	void DrawEllipse(const D2D1_ELLIPSE &inEllipse, ID2D1Brush*& pBrush,
					 const float inWidth = 1.0f, ID2D1StrokeStyle* pStrokeStyle = NULL);

	void DrawGeometry(ID2D1Geometry*& pGgeometry, ID2D1Brush*& pBrush,
					  const float inWidth = 1.0f, ID2D1StrokeStyle* pStrokeStyle = NULL);

	void DrawLine(D2D1_POINT_2F& inStart, D2D1_POINT_2F& inEnd,
				  ID2D1Brush*& pBrush, const float inWidth,
				  ID2D1StrokeStyle* pStrokeStyle = NULL);

	void DrawRectangle(const D2D1_RECT_F &inRect, ID2D1Brush*& pBrush,
					   const float inWidth = 1.0f, ID2D1StrokeStyle* pStrokeStyle = NULL);

	void DrawRoundedRectangle(const D2D1_ROUNDED_RECT &inRoundRect, ID2D1Brush*& pBrush,
					   const float inWidth = 1.0f, ID2D1StrokeStyle* pStrokeStyle = NULL);

	// 塗りつぶし
	void FillEllipse(const D2D1_ELLIPSE &inEllipse, ID2D1Brush*& pBrush);
*/

private:
	ID2D1Factory*          m_pD2DFactory;
	ID2D1DCRenderTarget*   m_pDCRenderTarget;

	IDirect3DSurface9*     m_pD3DSurface;
	HDC                    m_hDC;

	IDWriteFactory*        m_pDWriteFactory;

	std::list<IUnknown*>   m_ResourceList;			// リソースリスト
	std::set <void*>       m_ProtectedResource;		// プロテクトリソース

private:
	template<class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; }}

	CDirect2D();
	CDirect2D(const CDirect2D&);
	CDirect2D& operator=(const CDirect2D&);
};

//------------------------------------------------------------------------------
//	アクセス関数
//------------------------------------------------------------------------------
inline CDirect2D& D2D() { return CDirect2D::GetInstance(); }

#endif /* _INC_DIRECT2D */