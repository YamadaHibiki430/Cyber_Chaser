/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 Direct2D.hpp
				Direct2D�N���X�w�b�_

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_DIRECT2D
#define _INC_DIRECT2D

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
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
//	Direct2D�N���X��`
//------------------------------------------------------------------------------
class CDirect2D {
public:
	virtual ~CDirect2D();

	// �V���O���g���C���X�^���X�̎擾
	static CDirect2D& GetInstance()
	{
		static CDirect2D theDirect2D;
		return theDirect2D;
	}

	// �I�y���[�^�[
	inline ID2D1DCRenderTarget* operator->() { return m_pDCRenderTarget; }

	// Direct2D�������E���
	bool Initialize();
	void Release();

	// �`��J�n�E�I��
	void BeginDraw(IDirect3DTexture9* pD3DTexture);
	void BeginDraw(HDC hDC, RECT& inRect);
	void EndDraw();

	// �ϊ��s��
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

	// ���\�[�X���
	void ReleaseResource(ID2D1Resource* pResource);
	void ReleaseAllResources();

	void SetResourceProtect(void* pResource, const bool inProtect);

	// �r�b�g�}�b�v����	
	ID2D1Bitmap* CreateBitmapFromFile(LPCWSTR inFileName, const UINT inFrameIdx = 0);

	// �X�g���[�N����
	ID2D1StrokeStyle* CreateStrokeStyle(const D2D1_STROKE_STYLE_PROPERTIES& inStrokeStyleProp,
										const float* pDashes = NULL, const UINT inDashesCount = 0);

	// �O���f�[�V��������
	ID2D1GradientStopCollection* CreateGradientStopCollection(D2D1_GRADIENT_STOP inGradientStops[],
															  const UINT inGradientStopsCount);

	// �u���V����
	ID2D1SolidColorBrush* CreateSolidColorBrush(const struct _D3DCOLORVALUE& inColor);

	ID2D1BitmapBrush* CreateBitmapBrush(ID2D1Bitmap*& pBitmap);
	ID2D1BitmapBrush* CreateBitmapBrush(ID2D1Bitmap*& pBitmap,
										const D2D1_BITMAP_BRUSH_PROPERTIES& inBmpBrushProp);

	ID2D1LinearGradientBrush* CreateLinearGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
														const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& inGradientProp);

	ID2D1RadialGradientBrush* CreateRadialGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
														const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& inGradientProp);

	// �W�I���g��
	ID2D1RectangleGeometry*        CreateRectangleGeometry(const D2D1_RECT_F& inRectangle);
	ID2D1RoundedRectangleGeometry* CreateRoundedRectangleGeometry(const D2D1_ROUNDED_RECT& inRectangle);
	ID2D1EllipseGeometry*          CreateEllipseGeometry(const D2D1_ELLIPSE& inEllipse);
	ID2D1PathGeometry*             CreatePathGeometry();
	ID2D1TransformedGeometry*      CreateTransformedGeometry(ID2D1Geometry*& pSourceGeometry,
															 const D2D1_MATRIX_3X2_F &inTransform);
	ID2D1GeometryGroup*            CreateGeometryGroup(const D2D1_FILL_MODE inFillMode,
													   ID2D1Geometry* pGeometries[],
													   const UINT inGeometriesCount);

	// ���C���[
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


	// �`��֘A�̋@�\�͂��ׂ�->���Z�q�ŌĂяo��
/*
	// �N���A
	void Clear(const struct _D3DCOLORVALUE& inColor);

	// �`��
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

	// �h��Ԃ�
	void FillEllipse(const D2D1_ELLIPSE &inEllipse, ID2D1Brush*& pBrush);
*/

private:
	ID2D1Factory*          m_pD2DFactory;
	ID2D1DCRenderTarget*   m_pDCRenderTarget;

	IDirect3DSurface9*     m_pD3DSurface;
	HDC                    m_hDC;

	IDWriteFactory*        m_pDWriteFactory;

	std::list<IUnknown*>   m_ResourceList;			// ���\�[�X���X�g
	std::set <void*>       m_ProtectedResource;		// �v���e�N�g���\�[�X

private:
	template<class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; }}

	CDirect2D();
	CDirect2D(const CDirect2D&);
	CDirect2D& operator=(const CDirect2D&);
};

//------------------------------------------------------------------------------
//	�A�N�Z�X�֐�
//------------------------------------------------------------------------------
inline CDirect2D& D2D() { return CDirect2D::GetInstance(); }

#endif /* _INC_DIRECT2D */