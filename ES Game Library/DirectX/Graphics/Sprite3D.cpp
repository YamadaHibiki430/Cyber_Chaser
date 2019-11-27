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
	 Sprite3D.hpp
				Sprite3D�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Sprite3D.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CSprite3D::CSprite3D() : m_pD3DDevice    (NULL), m_Begin(false),
						 m_pRenderSurface(NULL), m_pDepthStencil(NULL)
{
	::ZeroMemory(&m_View      , sizeof(m_View      ));
	::ZeroMemory(&m_Projection, sizeof(m_Projection));
	::ZeroMemory(&m_Viewport  , sizeof(m_Viewport  ));
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CSprite3D::~CSprite3D()
{
	Release();
}

//------------------------------------------------------------------------------
//	������
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
//	���
//------------------------------------------------------------------------------
void CSprite3D::Release()
{
	m_Begin = false;

	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	�X�v���C�g3D�`��J�n
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
//	�X�v���C�g3D�`��I��
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
//	Sprite3DLayer - �R���X�g���N�^
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
//	Sprite3DLayer - ���������_�[�X�e�[�g�ݒ�
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
//	Sprite3DLayer - �����_�����O�J�n
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

		// �X�e�[�g�ۑ�
		for(rit = m_StateList.begin(); rit != m_StateList.end(); rit++)
			m_pD3DDevice->GetRenderState((*rit).Type, &((*rit).Restore));

		// �X�e�[�g�ݒ�
		for(rit = m_StateList.begin(); rit != m_StateList.end(); rit++)
			m_pD3DDevice->SetRenderState((*rit).Type, (*rit).Value);
	}

}

//------------------------------------------------------------------------------
//	Sprite3DLayer - �����_�����O�I��
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
//	Sprite3DLayer - �����_�����O�̈�ݒ�
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
