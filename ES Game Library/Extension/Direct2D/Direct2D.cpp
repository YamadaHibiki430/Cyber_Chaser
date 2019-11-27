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

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Direct2D.hpp"
#include "../Common/ComLibrary.hpp"

#include <wincodec.h>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "windowscodecs.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CDirect2D::CDirect2D() : m_pD2DFactory(NULL), m_pDCRenderTarget(NULL),
						 m_pD3DSurface(NULL), m_hDC(NULL),
						 m_pDWriteFactory(NULL)
{
	CComLibrary::Initialize();
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CDirect2D::~CDirect2D()
{
	Release();
	CComLibrary::Uninitialize();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CDirect2D::Initialize()
{
	Release();

	// Direct2D�t�@�N�g���[����
	if(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_ID2D1Factory,
						   (void**)&m_pD2DFactory)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - Direct2D�t�@�N�g���[�������s(CDirect2D_Initialize)\n"));
		return false;
	}

	// �����_�[�^�[�Q�b�g����
	D2D1_RENDER_TARGET_PROPERTIES   rtprop;
	rtprop.type        = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	rtprop.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED); 
	rtprop.minLevel    = D2D1_FEATURE_LEVEL_DEFAULT;
	rtprop.usage       = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
	rtprop.dpiX        = 0.0f;
	rtprop.dpiY        = 0.0f;

	if(m_pD2DFactory->CreateDCRenderTarget(&rtprop, &m_pDCRenderTarget) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �����_�[�^�[�Q�b�g�������s(CDirect2D_Initialize)\n"));
		return false;
	}

	// DirectWrite�t�@�N�g���[����
	if(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
							 (IUnknown**)&m_pDWriteFactory)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - DirectWrite�t�@�N�g���[�������s(CDirect2D_Initialize)\n"));
//		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	���
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
//	�`��J�n
//------------------------------------------------------------------------------
void CDirect2D::BeginDraw(IDirect3DTexture9* pD3DTexture)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_BeginDraw)\n"));
		return;
	}
	if(pD3DTexture == NULL) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`����������(CDirect2D_BeginDraw)\n"));
		return;
	}
	if(m_hDC != NULL) {
		::OutputDebugString(TEXT("*** Error - EndDraw�����s(CDirect2D_BeginDraw)\n"));
		return;
	}
#endif

	// �T�[�t�F�X�擾
	pD3DTexture->GetSurfaceLevel(0, &m_pD3DSurface);

	// �`��̈�ݒ�
	D3DSURFACE_DESC   desc;
	m_pD3DSurface->GetDesc(&desc);

	RECT   rect;
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = desc.Width;
	rect.bottom = desc.Height;

	// �f�o�C�X�R���e�L�X�g�o�C���h
	if(m_pD3DSurface->GetDC(&m_hDC) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �f�o�C�X�R���e�L�X�g�擾���s(CDirect2D_BeginDraw)\n"));

		m_pD3DSurface->Release();
		m_pD3DSurface = NULL;

		return;
	}
	m_pDCRenderTarget->BindDC(m_hDC, &rect);

	m_pDCRenderTarget->BeginDraw();
}

//------------------------------------------------------------------------------
//	�`��J�n
//------------------------------------------------------------------------------
void CDirect2D::BeginDraw(HDC hDC, RECT& inRect)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_BeginDraw)\n"));
		return;
	}
	if(m_hDC != NULL) {
		::OutputDebugString(TEXT("*** Error - EndDraw�����s(CDirect2D_BeginDraw)\n"));
		return;
	}
#endif

	m_pDCRenderTarget->BindDC(m_hDC, &inRect);
	m_pDCRenderTarget->BeginDraw();
}

//------------------------------------------------------------------------------
//	�`��I��
//------------------------------------------------------------------------------
void CDirect2D::EndDraw()
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_EndDraw)\n"));
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
//	�ϊ��s��ݒ�
//------------------------------------------------------------------------------
void CDirect2D::SetTransform(const D2D_MATRIX_3X2_F& inTransform)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_SetTransform)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->SetTransform(inTransform);
}

//------------------------------------------------------------------------------
//	���\�[�X���
//------------------------------------------------------------------------------
void CDirect2D::ReleaseResource(ID2D1Resource* pResource)
{
	if(pResource == NULL)
		return;

	m_ProtectedResource.erase (pResource);	// �v���e�N�g����
	m_ResourceList     .remove(pResource);

	pResource->Release();
//	pResource = NULL;
}

//------------------------------------------------------------------------------
//	�S���\�[�X���
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
				// �v���e�N�g�ΏۊO
				(*list_it)->Release();
				list_it = m_ResourceList.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	���\�[�X�v���e�N�g�ݒ�
//------------------------------------------------------------------------------
void CDirect2D::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_ProtectedResource.erase (pResource);		// �v���e�N�g����
	}
}

//------------------------------------------------------------------------------
//	�r�b�g�}�b�v����
//------------------------------------------------------------------------------
ID2D1Bitmap* CDirect2D::CreateBitmapFromFile(LPCWSTR inFileName, const UINT inFrameIdx)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateBitmapFromFile)\n"));
		return NULL;
	}
#endif

	ID2D1Bitmap*             pD2DBitmap    = NULL;

	IWICImagingFactory*      pWICFactory   = NULL;
	IWICBitmapDecoder*       pWICDecoder   = NULL;
	IWICBitmapFrameDecode*   pWICFrameDec  = NULL;
	IWICFormatConverter*     pWICConverter = NULL;

	try {
		// WIC�t�@�N�g���[����
		if(::CoCreateInstance(CLSID_WICImagingFactory, NULL,
							  CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
							  (LPVOID*)&pWICFactory)
		   != S_OK)
			throw TEXT("WIC�t�@�N�g���[�������s");

		// WIC�f�R�[�_�[����
		if(pWICFactory->CreateDecoderFromFilename(inFileName, NULL, GENERIC_READ,
												  WICDecodeMetadataCacheOnLoad, &pWICDecoder)
		   != S_OK)
			throw TEXT("WIC�f�R�[�_�[�������s");

		// WIC�t���[���f�R�[�_�[����
		if(pWICDecoder->GetFrame(inFrameIdx, &pWICFrameDec) != S_OK)
			throw TEXT("WIC�t���[���f�R�[�_�[�������s");

		// WIC�R���o�[�^�[����
		if(pWICFactory->CreateFormatConverter(&pWICConverter) != S_OK)
			throw TEXT("WIC�R���o�[�^�[�������s");

		// WIC�R���o�[�^�[������
		if(pWICConverter->Initialize(pWICFrameDec, GUID_WICPixelFormat32bppPBGRA,
									 WICBitmapDitherTypeNone, NULL, 0.0f,
									 WICBitmapPaletteTypeMedianCut)
		   != S_OK)
			throw TEXT("WIC�R���o�[�^�[���������s");

		// Direct2D�r�b�g�}�b�v����
		if(m_pDCRenderTarget->CreateBitmapFromWicBitmap(pWICConverter, &pD2DBitmap)
		   != S_OK)
			throw TEXT("Direct2D�r�b�g�}�b�v�������s");

		m_ResourceList.push_back(pD2DBitmap);
	} catch(LPCTSTR err_str) {
#ifdef _DEBUG
		// �G���[���b�Z�[�W����
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
//	�X�g���[�N����
//------------------------------------------------------------------------------
ID2D1StrokeStyle* CDirect2D::CreateStrokeStyle(const D2D1_STROKE_STYLE_PROPERTIES& inStrokeStyleProp,
											   const float* pDashes, const UINT inDashesCount)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateStrokeStyle)\n"));
		return NULL;
	}
#endif

	ID2D1StrokeStyle*   pStrokeStyle = NULL;
	if(m_pD2DFactory->CreateStrokeStyle(inStrokeStyleProp, pDashes, inDashesCount, &pStrokeStyle)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - �X�g���[�N�������s(CDirect2D_CreateStrokeStyle)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pStrokeStyle);

	return pStrokeStyle;
}

//------------------------------------------------------------------------------
//	�O���f�B�G���g�X�g�b�v�R���N�V��������
//------------------------------------------------------------------------------
ID2D1GradientStopCollection* CDirect2D::CreateGradientStopCollection(D2D1_GRADIENT_STOP inGradientStops[],
																	 const UINT inGradientStopsCount)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateGradientStopCollection)\n"));
		return NULL;
	}
#endif

	ID2D1GradientStopCollection*   pGradientStopCollection = NULL;
	if(m_pDCRenderTarget->CreateGradientStopCollection(inGradientStops,
													   inGradientStopsCount,
													  &pGradientStopCollection)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - �O���f�B�G���g�X�g�b�v�R���N�V�����������s(CDirect2D_CreateGradientStopCollection)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGradientStopCollection);

	return pGradientStopCollection;
}

//------------------------------------------------------------------------------
//	�u���V����
//------------------------------------------------------------------------------
ID2D1SolidColorBrush* CDirect2D::CreateSolidColorBrush(const struct _D3DCOLORVALUE& inColor)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateSolidColorBrush)\n"));
		return NULL;
	}
#endif

	ID2D1SolidColorBrush*   pBrush = NULL;
	D2D1_COLOR_F*           pColor = (D2D1_COLOR_F*)&inColor;
	if(m_pDCRenderTarget->CreateSolidColorBrush(*pColor, &pBrush) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �u���V�������s(CDirect2D_CreateSolidColorBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	�r�b�g�}�b�v�u���V����
//------------------------------------------------------------------------------
ID2D1BitmapBrush* CDirect2D::CreateBitmapBrush(ID2D1Bitmap*& pBitmap)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
#endif

	ID2D1BitmapBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateBitmapBrush(pBitmap, &pBrush) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �r�b�g�}�b�v�u���V�������s(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	�r�b�g�}�b�v�u���V����
//------------------------------------------------------------------------------
ID2D1BitmapBrush* CDirect2D::CreateBitmapBrush(ID2D1Bitmap*& pBitmap,
											   const D2D1_BITMAP_BRUSH_PROPERTIES& inBmpBrushProp)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
#endif

	ID2D1BitmapBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateBitmapBrush(pBitmap, inBmpBrushProp, &pBrush) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �r�b�g�}�b�v�u���V�������s(CDirect2D_CreateBitmapBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	����O���f�[�V�����u���V����
//------------------------------------------------------------------------------
ID2D1LinearGradientBrush* CDirect2D::CreateLinearGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
															   const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& inGradientProp)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateLinearGradientBrush)\n"));
		return NULL;
	}
#endif

	ID2D1LinearGradientBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateLinearGradientBrush(inGradientProp, pGradientStop, &pBrush)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - ����O���f�[�V�����u���V�������s(CDirect2D_CreateLinearGradientBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	���ˏ�O���f�[�V�����u���V����
//------------------------------------------------------------------------------
ID2D1RadialGradientBrush* CDirect2D::CreateRadialGradientBrush(ID2D1GradientStopCollection*& pGradientStop,
															   const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& inGradientProp)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateRadialGradientBrush)\n"));
		return NULL;
	}
#endif

	ID2D1RadialGradientBrush*   pBrush = NULL;
	if(m_pDCRenderTarget->CreateRadialGradientBrush(inGradientProp, pGradientStop, &pBrush)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - ���ˏ�O���f�[�V�����u���V�������s(CDirect2D_CreateRadialGradientBrush)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pBrush);

	return pBrush;
}

//------------------------------------------------------------------------------
//	��`�W�I���g������
//------------------------------------------------------------------------------
ID2D1RectangleGeometry* CDirect2D::CreateRectangleGeometry(const D2D1_RECT_F& inRectangle)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateRectangleGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1RectangleGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateRectangleGeometry(inRectangle, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ��`�W�I���g���������s(CDirect2D_CreateRectangleGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	�ی^��`�W�I���g������
//------------------------------------------------------------------------------
ID2D1RoundedRectangleGeometry* CDirect2D::CreateRoundedRectangleGeometry(const D2D1_ROUNDED_RECT& inRectangle)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateRoundedRectangleGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1RoundedRectangleGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateRoundedRectangleGeometry(inRectangle, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �ی^��`�W�I���g���������s(CDirect2D_CreateRoundedRectangleGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	�ȉ~�W�I���g������
//------------------------------------------------------------------------------
ID2D1EllipseGeometry* CDirect2D::CreateEllipseGeometry(const D2D1_ELLIPSE& inEllipse)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateEllipseGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1EllipseGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateEllipseGeometry(inEllipse, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �ȉ~�W�I���g���������s(CDirect2D_CreateEllipseGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	�p�X�W�I���g������
//------------------------------------------------------------------------------
ID2D1PathGeometry* CDirect2D::CreatePathGeometry()
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreatePathGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1PathGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreatePathGeometry(&pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �p�X�W�I���g���������s(CDirect2D_CreatePathGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	�ϊ��W�I���g������
//------------------------------------------------------------------------------
ID2D1TransformedGeometry* CDirect2D::CreateTransformedGeometry(ID2D1Geometry*& pSourceGeometry,
															   const D2D1_MATRIX_3X2_F &inTransform)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateTransformedGeometry)\n"));
		return NULL;
	}
#endif

	ID2D1TransformedGeometry*   pGeometry = NULL;
	if(m_pD2DFactory->CreateTransformedGeometry(pSourceGeometry, inTransform,
												&pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �ϊ��W�I���g���������s(CDirect2D_CreateTransformedGeometry)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	�W�I���g���O���[�v����
//------------------------------------------------------------------------------
ID2D1GeometryGroup* CDirect2D::CreateGeometryGroup(const D2D1_FILL_MODE inFillMode,
												   ID2D1Geometry* pGeometries[],
												   const UINT inGeometriesCount)
{
#ifdef _DEBUG
	if(m_pD2DFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateGeometryGroup)\n"));
		return NULL;
	}
#endif

	ID2D1GeometryGroup*   pGeometry = NULL;
	if(m_pD2DFactory->CreateGeometryGroup(inFillMode, pGeometries,
										  inGeometriesCount, &pGeometry) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �W�I���g���O���[�v�������s(CDirect2D_CreateGeometryGroup)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pGeometry);

	return pGeometry;
}

//------------------------------------------------------------------------------
//	���C���[����
//------------------------------------------------------------------------------
ID2D1Layer* CDirect2D::CreateLayer()
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
#endif

	ID2D1Layer*   pLayer = NULL;
	if(m_pDCRenderTarget->CreateLayer(&pLayer) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ���C���[�������s(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pLayer);

	return pLayer;
}

//------------------------------------------------------------------------------
//	���C���[����
//------------------------------------------------------------------------------
ID2D1Layer* CDirect2D::CreateLayer(const D2D1_SIZE_F& inSize)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
#endif

	ID2D1Layer*   pLayer = NULL;
	if(m_pDCRenderTarget->CreateLayer(inSize, &pLayer) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ���C���[�������s(CDirect2D_CreateLayer)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pLayer);

	return pLayer;
}

//------------------------------------------------------------------------------
//	���C���[�v�b�V��
//------------------------------------------------------------------------------
void CDirect2D::PushLayer(ID2D1Layer*& pLayer, const D2D1_LAYER_PARAMETERS& inLayerParams)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_PushLayer)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->PushLayer(inLayerParams, pLayer);
}

//------------------------------------------------------------------------------
//	���C���[�|�b�v
//------------------------------------------------------------------------------
void CDirect2D::PopLayer()
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_PopLayer)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->PopLayer();
}

//------------------------------------------------------------------------------
//	�e�L�X�g�t�H�[�}�b�g����
//------------------------------------------------------------------------------
IDWriteTextFormat* CDirect2D::CreateTextFormat(const WCHAR*              pFontFamilyName,
											   const DWRITE_FONT_WEIGHT  inFontWeight,
											   const DWRITE_FONT_STYLE   inFontStyle,
											   const DWRITE_FONT_STRETCH inFontStretch,
											   const float inFontSize, const WCHAR* pLocaleName)
{
#ifdef _DEBUG
	if(m_pDWriteFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectWrite��������(CDirect2D_CreateTextFormat)\n"));
		return NULL;
	}
#endif

	IDWriteTextFormat*   pTextFormat = NULL;
	if(m_pDWriteFactory->CreateTextFormat(pFontFamilyName, NULL,
										  inFontWeight, inFontStyle, inFontStretch,
										  inFontSize, pLocaleName, &pTextFormat)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - �e�L�X�g�t�H�[�}�b�g�������s(CDirect2D_CreateTextFormat)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pTextFormat);

	return pTextFormat;
}

//------------------------------------------------------------------------------
//	�e�L�X�g���C�A�E�g����
//------------------------------------------------------------------------------
IDWriteTextLayout* CDirect2D::CreateTextLayout(const WCHAR* pString, const UINT inLength,
											   IDWriteTextFormat*& pTextFormat,
											   const float inMaxWidth, const float inMaxHeight)
{
#ifdef _DEBUG
	if(m_pDWriteFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectWrite��������(CDirect2D_CreateTextLayout)\n"));
		return NULL;
	}
#endif

	IDWriteTextLayout*   pTextLayout = NULL;
	if(m_pDWriteFactory->CreateTextLayout(pString, inLength, pTextFormat,
										  inMaxWidth, inMaxHeight, &pTextLayout)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - �e�L�X�g���C�A�E�g�������s(CDirect2D_CreateTextLayout)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pTextLayout);

	return pTextLayout;
}

//------------------------------------------------------------------------------
//	�^�C�|�O���t�B����
//------------------------------------------------------------------------------
IDWriteTypography* CDirect2D::CreateTypography()
{
#ifdef _DEBUG
	if(m_pDWriteFactory == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectWrite��������(CDirect2D_CreateTypography)\n"));
		return NULL;
	}
#endif

	IDWriteTypography*   pTypography = NULL;
	if(m_pDWriteFactory->CreateTypography(&pTypography) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �e�L�X�g���C�A�E�g�������s(CDirect2D_CreateTypography)\n"));
		return NULL;
	}
	m_ResourceList.push_back(pTypography);

	return pTypography;
}

/*
//------------------------------------------------------------------------------
//	�`��̈�N���A
//------------------------------------------------------------------------------
void CDirect2D::Clear(const struct _D3DCOLORVALUE& inColor)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_Clear)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->Clear((D2D1_COLOR_F*)&inColor);
}

//------------------------------------------------------------------------------
//	�r�b�g�}�b�v�`��
//------------------------------------------------------------------------------
void CDirect2D::DrawBitmap(ID2D1Bitmap*& pBitmap,
						   const D2D_RECT_F& inDestRect,
						   const D2D_RECT_F& inSrcRect , const float inAlpha,
						   const D2D1_BITMAP_INTERPOLATION_MODE inMode)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_DrawBitmap)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawBitmap(pBitmap, inDestRect, inAlpha, inMode, inSrcRect);
}

//------------------------------------------------------------------------------
//	�ȉ~�`��
//------------------------------------------------------------------------------
void CDirect2D::DrawEllipse(const D2D1_ELLIPSE &inEllipse, ID2D1Brush*& pBrush,
							const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_DrawEllipse)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawEllipse(inEllipse, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	�W�I���g���`��
//------------------------------------------------------------------------------
void CDirect2D::DrawGeometry(ID2D1Geometry*& pGgeometry, ID2D1Brush*& pBrush,
							 const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_DrawGeometry)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawGeometry(pGgeometry, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	���`��
//------------------------------------------------------------------------------
void CDirect2D::DrawLine(D2D1_POINT_2F& inStart, D2D1_POINT_2F& inEnd, ID2D1Brush*& pBrush,
						 const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_DrawLine)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawLine(inStart, inEnd, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	��`�`��
//------------------------------------------------------------------------------
void CDirect2D::DrawRectangle(const D2D1_RECT_F &inRect, ID2D1Brush*& pBrush,
							  const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_DrawRectangle)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawRectangle(inRect, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	�ۋ�`�`��
//------------------------------------------------------------------------------
void CDirect2D::DrawRoundedRectangle(const D2D1_ROUNDED_RECT &inRoundRect, ID2D1Brush*& pBrush,
									 const float inWidth, ID2D1StrokeStyle* pStrokeStyle)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_DrawRoundedRectangle)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->DrawRoundedRectangle(inRoundRect, pBrush, inWidth, pStrokeStyle);
}

//------------------------------------------------------------------------------
//	�h��Ԃ��ȉ~�`��
//------------------------------------------------------------------------------
void CDirect2D::FillEllipse(const D2D1_ELLIPSE &inEllipse, ID2D1Brush*& pBrush)
{
#ifdef _DEBUG
	if(m_pDCRenderTarget == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct2D��������(CDirect2D_FillEllipse)\n"));
		return;
	}
#endif
	m_pDCRenderTarget->FillEllipse(inEllipse, pBrush);
}
*/
