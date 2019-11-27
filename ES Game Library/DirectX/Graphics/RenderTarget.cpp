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
	 RenderTarget.hpp
				�����_�[�^�[�Q�b�g�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "RenderTarget.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CRenderTarget::CRenderTarget(IDirect3DTexture9* pTexture,
							 IDirect3DSurface9* pDepthStencil,
							 const UINT inWidth, const UINT inHeight)
	: m_pD3DDevice(NULL),        m_pBaseTexture(pTexture),
	  m_pTexture(pTexture),      m_pCubeTexture(NULL),
	  m_pRenderSurface(NULL),    m_pDepthStencil(pDepthStencil),
	  m_Width(inWidth),          m_Height(inHeight),
	  m_Format(D3DFMT_UNKNOWN),  m_DepthFormat(D3DFMT_UNKNOWN),
	  m_ClearFlags(0),
	  m_pD3DSysMemSurface(NULL), m_hDC(NULL)
{
	assert(m_pTexture != NULL || m_pDepthStencil != NULL);

	// �����_�[�T�[�t�F�X�ݒ�
	if(m_pTexture != NULL) {
		m_pTexture->AddRef();

		m_pTexture->GetDevice(&m_pD3DDevice);
		m_pTexture->GetSurfaceLevel(0, &m_pRenderSurface);

		// �t�H�[�}�b�g�ۑ�
		D3DSURFACE_DESC   desc;
		m_pRenderSurface->GetDesc(&desc);
		m_Format = desc.Format;

		// �N���A�t���O�ݒ�
		m_ClearFlags |= D3DCLEAR_TARGET;
	}

	// �f�v�X�X�e���V���ݒ�
	if(m_pDepthStencil != NULL) {
		m_pDepthStencil->AddRef();

		if(m_pD3DDevice == NULL)
			m_pDepthStencil->GetDevice(&m_pD3DDevice);

		D3DSURFACE_DESC   desc;
		m_pDepthStencil->GetDesc(&desc);
		m_DepthFormat = desc.Format;

		// �N���A�t���O�ݒ�
		m_ClearFlags |= D3DCLEAR_ZBUFFER;
		if(m_DepthFormat == D3DFMT_D15S1   ||
		   m_DepthFormat == D3DFMT_D24S8   ||
		   m_DepthFormat == D3DFMT_D24X4S4 ||
		   m_DepthFormat == D3DFMT_D24FS8)
			m_ClearFlags |= D3DCLEAR_STENCIL;
	}
}

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CRenderTarget::CRenderTarget(IDirect3DCubeTexture9* pCubeTexture,
							 IDirect3DSurface9* pDepthStencil,
							 const UINT inSize)
	: m_pD3DDevice(NULL),        m_pBaseTexture(pCubeTexture),
	  m_pTexture(NULL),          m_pCubeTexture(pCubeTexture),
	  m_pRenderSurface(NULL),    m_pDepthStencil(pDepthStencil),
	  m_Width(inSize),           m_Height(inSize),
	  m_Format(D3DFMT_UNKNOWN),  m_DepthFormat(D3DFMT_UNKNOWN),
	  m_ClearFlags(0),
	  m_pD3DSysMemSurface(NULL), m_hDC(NULL)
{
	assert(m_pCubeTexture != NULL);

	// �����_�[�T�[�t�F�X
	m_pCubeTexture->AddRef();

	m_pCubeTexture->GetDevice(&m_pD3DDevice);
	m_pCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Z, 0, &m_pRenderSurface);

	D3DSURFACE_DESC   desc;
	m_pRenderSurface->GetDesc(&desc);
	m_Format = desc.Format;

	m_ClearFlags |= D3DCLEAR_TARGET;

	// �f�v�X�X�e���V��
	if(m_pDepthStencil != NULL) {
		m_pDepthStencil->AddRef();

		m_pDepthStencil->GetDesc(&desc);
		m_DepthFormat = desc.Format;

		// �N���A�t���O�ݒ�
		m_ClearFlags |= D3DCLEAR_ZBUFFER;
		if(m_DepthFormat == D3DFMT_D15S1   ||
		   m_DepthFormat == D3DFMT_D24S8   ||
		   m_DepthFormat == D3DFMT_D24X4S4 ||
		   m_DepthFormat == D3DFMT_D24FS8)
			m_ClearFlags |= D3DCLEAR_STENCIL;
	}
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CRenderTarget::~CRenderTarget()
{
	ReleaseDC(true);

	SafeRelease(m_pD3DSysMemSurface);
	SafeRelease(m_pDepthStencil);
	SafeRelease(m_pRenderSurface);
	SafeRelease(m_pCubeTexture);
	SafeRelease(m_pTexture);
	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	�f�o�C�X��������
//------------------------------------------------------------------------------
void CRenderTarget::OnLostDevice()
{
	SafeRelease(m_pDepthStencil);
	SafeRelease(m_pRenderSurface);
	SafeRelease(m_pBaseTexture);
}

//------------------------------------------------------------------------------
//	�f�o�C�X���Z�b�g����
//------------------------------------------------------------------------------
void CRenderTarget::OnResetDevice()
{
	// �e�N�X�`���Đ���
	if(m_Format != D3DFMT_UNKNOWN) {
		if(m_pTexture != NULL) {
			// �e�N�X�`������
			::D3DXCreateTexture(m_pD3DDevice, m_Width, m_Height, 1,
								D3DUSAGE_RENDERTARGET, m_Format, D3DPOOL_DEFAULT,
							   &m_pTexture);

			// �T�[�t�F�X�擾
			m_pTexture->GetSurfaceLevel(0, &m_pRenderSurface);

			// �x�[�X�e�N�X�`���ݒ�
			m_pBaseTexture = m_pTexture;
		} else {
			// �L���[�u�e�N�X�`������
			::D3DXCreateCubeTexture(m_pD3DDevice, m_Width, 1,
									D3DUSAGE_RENDERTARGET, m_Format, D3DPOOL_DEFAULT,
								   &m_pCubeTexture);

			// �T�[�t�F�X�擾
			m_pCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Z, 0, &m_pRenderSurface);

			// �x�[�X�e�N�X�`���ݒ�
			m_pBaseTexture = m_pCubeTexture;
		}
	}

	// �f�v�X�X�e���V���Đ���
	if(m_DepthFormat != D3DFMT_UNKNOWN) {
		if(m_pTexture != NULL) {
			D3DSURFACE_DESC   desc;
			m_pTexture->GetLevelDesc(0, &desc);
			m_pD3DDevice->CreateDepthStencilSurface(desc.Width, desc.Height, m_DepthFormat,
													D3DMULTISAMPLE_NONE, 0, FALSE,
												   &m_pDepthStencil, NULL);
		} else {
			m_pD3DDevice->CreateDepthStencilSurface(m_Width, m_Height, m_DepthFormat,
													D3DMULTISAMPLE_NONE, 0, FALSE,
												   &m_pDepthStencil, NULL);
		}
	}

	// �V�X�e���������T�[�t�F�X�Đ���
	if(m_pD3DSysMemSurface != NULL) {
		SafeRelease(m_pD3DSysMemSurface);
		m_pD3DDevice->CreateOffscreenPlainSurface(m_Width, m_Height, m_Format,
												  D3DPOOL_SYSTEMMEM,
												 &m_pD3DSysMemSurface, NULL);
	}
}

//------------------------------------------------------------------------------
//	�����_�����O�^�[�Q�b�g�K�p
//------------------------------------------------------------------------------
void CRenderTarget::Use()
{
	// �T�[�t�F�X
	if(m_pRenderSurface != NULL)
		m_pD3DDevice->SetRenderTarget(0, m_pRenderSurface);

	// �f�v�X�X�e���V��
	if(m_pDepthStencil != NULL)
		m_pD3DDevice->SetDepthStencilSurface(m_pDepthStencil);

	// �r���[�|�[�g�ݒ�
	D3DVIEWPORT9   view;
	view.X      = 0;
	view.Y      = 0;
	view.Width  = m_Width;
	view.Height = m_Height;
	view.MinZ   = 0.0f;
	view.MaxZ   = 1.0f;
	m_pD3DDevice->SetViewport(&view);
}

//------------------------------------------------------------------------------
//	�����_�����O�^�[�Q�b�g�K�p
//------------------------------------------------------------------------------
void CRenderTarget::Use(const D3DCUBEMAP_FACES inFace)
{
	SetCubeFace(inFace);
	Use();
}

//------------------------------------------------------------------------------
//	�����_�����O�^�[�Q�b�g����
//------------------------------------------------------------------------------
void CRenderTarget::Clear(const D3DCOLOR inColor)
{
	m_pD3DDevice->Clear(0, NULL, m_ClearFlags, inColor, 1.0f, 0);
}

//------------------------------------------------------------------------------
//	�L���[�u�T�[�t�F�X�ݒ�
//------------------------------------------------------------------------------
void CRenderTarget::SetCubeFace(const D3DCUBEMAP_FACES inFace)
{
	if(m_pCubeTexture != NULL) {
		m_pRenderSurface->Release();
		m_pCubeTexture  ->GetCubeMapSurface(inFace, 0, &m_pRenderSurface);
	}
}

//------------------------------------------------------------------------------
//	�T�[�t�F�X���擾
//------------------------------------------------------------------------------
D3DSURFACE_DESC CRenderTarget::GetSurfaceDesc()
{
	D3DSURFACE_DESC   desc;
	m_pRenderSurface->GetDesc(&desc);

	return desc;
}

//------------------------------------------------------------------------------
//	�f�o�C�X�R���e�L�X�g�擾
//------------------------------------------------------------------------------
HDC CRenderTarget::GetDC(const bool inGetImage)
{
	if(m_hDC != NULL)
		return m_hDC;

	// ���b�N�\�T�[�t�F�X����
	if(m_pD3DSysMemSurface == NULL) {
		if(m_pD3DDevice->CreateOffscreenPlainSurface(m_Width, m_Height, m_Format,
													 D3DPOOL_SYSTEMMEM,
													&m_pD3DSysMemSurface, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �V�X�e���������T�[�t�F�X�������s(CRenderTarget_GetDC)\n"));
			return NULL;
		}
	}

	if(inGetImage)
		m_pD3DDevice->GetRenderTargetData(m_pRenderSurface, m_pD3DSysMemSurface);

	m_pD3DSysMemSurface->GetDC(&m_hDC);

	return m_hDC;
}

//------------------------------------------------------------------------------
//	�f�o�C�X�R���e�L�X�g���
//------------------------------------------------------------------------------
void CRenderTarget::ReleaseDC(const bool inRelease)
{
	if(m_hDC == NULL)
		return;

	m_pD3DSysMemSurface->ReleaseDC(m_hDC);
	m_hDC = NULL;

	m_pD3DDevice->UpdateSurface(m_pD3DSysMemSurface, NULL, m_pRenderSurface, NULL);

	if(inRelease)
		SafeRelease(m_pD3DSysMemSurface);
}


//------------------------------------------------------------------------------
//	���b�N
//------------------------------------------------------------------------------
DXGLOCKED_DESC CRenderTarget::Lock(const bool inGetImage)
{
	DXGLOCKED_DESC   desc;
	::ZeroMemory(&desc, sizeof(desc));

	// ���b�N�\�T�[�t�F�X����
	if(m_pD3DSysMemSurface == NULL) {
		if(m_pD3DDevice->CreateOffscreenPlainSurface(m_Width, m_Height, m_Format,
													 D3DPOOL_SYSTEMMEM,
													&m_pD3DSysMemSurface, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �V�X�e���������T�[�t�F�X�������s(CRenderTarget_GetDC)\n"));
			return desc;
		}
	}

	if(inGetImage)
		m_pD3DDevice->GetRenderTargetData(m_pRenderSurface, m_pD3DSysMemSurface);

	D3DLOCKED_RECT   lock;
	m_pD3DSysMemSurface->LockRect(&lock, NULL, 0);

	// �T�[�t�F�X���ݒ�
	desc.Format = m_Format;
	desc.Width  = m_Width;
	desc.Height = m_Height;
	desc.Pitch  = lock.Pitch;
	desc.pBits  = lock.pBits;

	return desc;
}

//------------------------------------------------------------------------------
//	���b�N����
//------------------------------------------------------------------------------
void CRenderTarget::Unlock(const bool inRelease)
{
	if(m_pD3DSysMemSurface == NULL)
		return;

	m_pD3DSysMemSurface->UnlockRect();
	m_pD3DDevice->UpdateSurface(m_pD3DSysMemSurface, NULL, m_pRenderSurface, NULL);

	if(inRelease)
		SafeRelease(m_pD3DSysMemSurface);
}

//------------------------------------------------------------------------------
//	�T�[�t�F�X���擾
//------------------------------------------------------------------------------
D3DSURFACE_DESC CNullRenderTarget::GetSurfaceDesc()
{
	D3DSURFACE_DESC   desc;
	::ZeroMemory(&desc, sizeof(desc));

	return desc;
}

//------------------------------------------------------------------------------
//	���b�N
//------------------------------------------------------------------------------
DXGLOCKED_DESC CNullRenderTarget::Lock(const bool inGetImage)
{
	DXGLOCKED_DESC   desc;
	::ZeroMemory(&desc, sizeof(desc));

	return desc;
}

//------------------------------------------------------------------------------
//	���b�N����
//------------------------------------------------------------------------------
void CNullRenderTarget::Unlock(const bool inRelease)
{
}
