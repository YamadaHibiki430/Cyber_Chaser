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
	 DXGraphics11.cpp
				DirectX Graphics11�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <Effects.h>
#include "DXGraphics11.hpp"

#include <cassert>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "..\\..\\..\\SDK\\DirectX\\DirectX11_2\\Lib\\x86\\d3d11.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
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
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CDXGraphics11::~CDXGraphics11()
{
	Release();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
int CDXGraphics11::Initialize(const UINT inWidth, UINT inHeight)
{
	Release();

	D3D_FEATURE_LEVEL   level;
	try {
		HRESULT   result;

		// �f�o�C�X����
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
			::OutputDebugString(TEXT("*** Error - Direct3D11�f�o�C�X�������s(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		D3D11_TEXTURE2D_DESC   tex_desc;

		// �����_�[�^�[�Q�b�g����
		// �e�N�X�`��
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
			::OutputDebugString(TEXT("*** Error - �����_�[�^�[�Q�b�g�e�N�X�`���������s(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// �����_�[�^�[�Q�b�g�r���[
		result = m_pDevice->CreateRenderTargetView(m_pRenderTexture, NULL, &m_pRenderView);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - �����_�[�^�[�Q�b�g�r���[�������s(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// �f�v�X�X�e���V������
		// �e�N�X�`��
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
			::OutputDebugString(TEXT("*** Error - �f�v�X�X�e���V���e�N�X�`���������s(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// �f�v�X�X�e���V���r���[
		D3D11_DEPTH_STENCIL_VIEW_DESC   dsv_desc;
		dsv_desc.Format		   = tex_desc.Format;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsv_desc.Flags         = 0;
		result = m_pDevice->CreateDepthStencilView(m_pDepthStenciTexture, &dsv_desc, &m_pDepthStencilView);
		if(FAILED(result)) {
			::OutputDebugString(TEXT("*** Error - �f�v�X�X�e���V���r���[�������s(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// �t���[���o�b�t�@�e�N�X�`������
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
			::OutputDebugString(TEXT("*** Error - �t���[���o�b�t�@�e�N�X�`���������s(CDXGraphics11_Initialize)\n")); 
			throw 0;
		}

		// �e�N�X�`�����擾
		m_pFrameBufferTexture->GetDesc(&tex_desc);
		m_FrameBufferWidth  = tex_desc.Width;
		m_FrameBufferHeight = tex_desc.Height;
	} catch(int err_code) {
		Release();
		return err_code;
	}

	// �����_�[�^�[�Q�b�g�ݒ�
	m_pContext->OMSetRenderTargets(1, &m_pRenderView, m_pDepthStencilView);

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT   vp;
	vp.Width    = (float)inWidth;
	vp.Height   = (float)inHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pContext->RSSetViewports(1, &vp);

	// �߂�l
	switch(level) {
	case D3D_FEATURE_LEVEL_11_0:	return 110;	// 11.0
	case D3D_FEATURE_LEVEL_10_1:	return 101;	// 10.1
	case D3D_FEATURE_LEVEL_10_0:	return 100;	// 10.0
	}

	return 111;	// 11.1
}

//------------------------------------------------------------------------------
//	���
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
//	��ʃN���A
//------------------------------------------------------------------------------
void CDXGraphics11::Clear(const float inR, const float inG, const float inB, const float inA)
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pContext == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectX Graphics11��������(CDXGraphics11_Clear)\n"));
		return;
	}
#endif
	const float  RGBA[] = {inR, inB, inG, inA};
	m_pContext->ClearRenderTargetView(m_pRenderView, RGBA);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//------------------------------------------------------------------------------
//	�����_�[�^�[�Q�b�g���t���[���o�b�t�@
//------------------------------------------------------------------------------
void CDXGraphics11::Present()
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pContext == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectX Graphics11��������(CDXGraphics11_Present)\n"));
		return;
	}
#endif
	m_pContext->CopyResource(m_pFrameBufferTexture, m_pRenderTexture);
}

//------------------------------------------------------------------------------
//	�t���[���o�b�t�@�擾
//------------------------------------------------------------------------------
void CDXGraphics11::GetFrameBuffer(void* pImage)
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pContext == NULL) {
		::OutputDebugString(TEXT("*** Error - DirectX Graphics11��������(CDXGraphics11_GetFrameBuffer)\n"));
		return;
	}
#endif
//	m_pContext->CopyResource(m_pRenderCopyTexture, m_pRenderTexture);

	D3D11_MAPPED_SUBRESOURCE   map_subres;
#if !(defined(_DEBUG) || defined(DEBUG))
	m_pContext->Map(m_pFrameBufferTexture, 0, D3D11_MAP_READ, 0, &map_subres);
#else
	if(FAILED(m_pContext->Map(m_pFrameBufferTexture, 0, D3D11_MAP_READ, 0, &map_subres))) {
		::OutputDebugString(TEXT("*** Error - �}�b�v���s(CDXGraphics11_GetFrameBuffer)\n"));
		return;
	}
#endif
	::CopyMemory(pImage, map_subres.pData, map_subres.RowPitch * m_FrameBufferHeight);
	m_pContext->Unmap(m_pFrameBufferTexture, 0);
}

//------------------------------------------------------------------------------
//	���f������
//------------------------------------------------------------------------------
IModel11* CDXGraphics11::CreateModelFromFile(LPCWSTR inFileName)
{
	IModel11*   pModel = NULL;

	try {
		DirectX::EffectFactory   fx(m_pDevice);
		auto   model = DirectX::Model::CreateFromCMO(m_pDevice, inFileName, fx, false);
		pModel = new CModel11(std::move(model));
	} catch(...) {
		::OutputDebugString(TEXT("*** Error - ���f���������s(CDXGraphics11_CreateModelFromFile)\n"));
		pModel = new CNullModel11();
	}
	m_ModelList.push_back(pModel);

	return pModel;
}

//------------------------------------------------------------------------------
//	���f�����
//------------------------------------------------------------------------------
void CDXGraphics11::ReleaseModel(IModel11*& pModel)
{
	m_ProtectedResource.erase(pModel);		// �v���e�N�g����

	m_ModelList.remove(pModel);
	delete pModel;
	pModel = NULL;
}

//------------------------------------------------------------------------------
//	�S���f�����
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
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_ModelList.erase(list_it);
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
void CDXGraphics11::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_ProtectedResource.erase (pResource);		// �v���e�N�g����
	}
}

//------------------------------------------------------------------------------
//	DirectX9�g�� - ������
//------------------------------------------------------------------------------
int CDXGraphics11::Initialize(IDirect3DDevice9* pDevice9)
{
	assert(pDevice9 != NULL);

	// �o�b�N�o�b�t�@�擾
	IDirect3DSurface9*   pSurface;
	if(pDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �o�b�N�o�b�t�@�擾���s(CDXGraphics11_Initialize)\n"));
		return 0;
	}

	D3DSURFACE_DESC   desc;
	pSurface->GetDesc(&desc);

	// DirectX11������
	const int   DXVER = Initialize(desc.Width, desc.Height);
	pSurface->Release();
	if(DXVER == 0)
		return 0;

	// DirectX9�e�N�X�`������
	if(::D3DXCreateTexture(pDevice9, m_FrameBufferWidth, m_FrameBufferHeight, 1, 0,
						   D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture9)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - DirectX9�e�N�X�`���������s(CDXGraphics11_Initialize)\n"));
		Release();
		return 0;
	}

	m_pDevice9 = pDevice9;
	m_pDevice9->AddRef();

	m_upCommonStates.reset(new DirectX::CommonStates(m_pDevice));

	return DXVER;
}

//------------------------------------------------------------------------------
//	DirectX9�g�� - �V�[���J�n
//------------------------------------------------------------------------------
void CDXGraphics11::BeginScene()
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pDevice9 == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3D9��������(CDXGraphics11_BeginScene)\n"));
		return;
	}
#endif

	D3DXMATRIX   matrix;

	// �r���[�s��ݒ�
	m_pDevice9->GetTransform(D3DTS_VIEW, &matrix);
	::CopyMemory(m_View.m, matrix.m, sizeof(float) * 4 * 4);

	// �v���W�F�N�V�����s��ݒ�
	m_pDevice9->GetTransform(D3DTS_PROJECTION, &matrix);
	::CopyMemory(m_Projection.m, matrix.m, sizeof(float) * 4 * 4);
}

//------------------------------------------------------------------------------
//	DirectX9�g�� - �V�[���I��
//------------------------------------------------------------------------------
void CDXGraphics11::EndScene()
{
}

//------------------------------------------------------------------------------
//	DirectX9�g�� - �`��
//------------------------------------------------------------------------------
void CDXGraphics11::Draw(IModel11*& pModel)
{
	pModel->Draw(m_pContext, *m_upCommonStates, m_View, m_Projection); 
}

//------------------------------------------------------------------------------
//	DirectX9�g�� - DirectX11�e�N�X�`����DirectX9�e�N�X�`��
//------------------------------------------------------------------------------
void CDXGraphics11::MapTexture()
{
#if defined(_DEBUG) || defined(DEBUG)
	if(m_pTexture9 == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3D9��������(CDXGraphics11_Initialize)\n"));
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
//	DirectX9�g�� - ���Z�qIDirect3DTexture9*
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

