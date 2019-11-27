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
	 Sprite.cpp
				�X�v���C�g�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Sprite.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CSprite::CSprite(IDirect3DTexture9* pD3DTexture)
	: m_pD3DTexture(pD3DTexture), m_pD3DSurface(NULL),
	  m_pPool(NULL), m_PoolMax(0), m_PoolCount(0), m_hDC(NULL)
{
	assert(m_pD3DTexture != NULL);
	m_pD3DTexture->AddRef();

	// �T�[�t�F�X�擾
	m_pD3DTexture->GetSurfaceLevel(0, &m_pD3DSurface);

	// �T�[�t�F�X���擾
	D3DSURFACE_DESC   desc;
	m_pD3DSurface->GetDesc(&desc);
	m_Width  = (float)desc.Width;
	m_Height = (float)desc.Height;
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CSprite::~CSprite()
{
	delete[] m_pPool;

	ReleaseDC();

	m_pD3DSurface->Release();
	m_pD3DTexture->Release();
}

//------------------------------------------------------------------------------
//	�`��v�[���̈搶��
//------------------------------------------------------------------------------
DWORD CSprite::CreatePool(const DWORD inPoolMax)
{
	delete m_pPool;

	m_PoolMax   = inPoolMax;
	m_PoolCount = 0;

	m_pPool = new DXGTLVERTEX[m_PoolMax * 6];	// TRIANGLE LIST�̂���"�~�U"
	if(m_pPool == NULL)
		m_PoolMax = 0;

	return m_PoolMax;
}

//------------------------------------------------------------------------------
//	�`��v�[���̈���
//------------------------------------------------------------------------------
void CSprite::ReleasePool()
{
	delete m_pPool;
	m_pPool = NULL;
}

//------------------------------------------------------------------------------
//	�`��v�[���̈�o�^
//------------------------------------------------------------------------------
void CSprite::DrawPool(const D3DXVECTOR3& inPos, const RECT* pSrc, const D3DCOLOR inColor)
{
#ifdef _DEBUG
	if(m_pPool == NULL) {
		::OutputDebugString(TEXT("*** Error - �`��v�[���̈斢����(CDXGraphics9_DrawPool)\n"));
		return;
	}
#endif

	const DWORD   IDX = m_PoolCount * 6;

	const float   P_X = inPos.x - 0.5f;
	const float   P_Y = inPos.y - 0.5f;

	// �`��v�[���̈�֓o�^
	if(pSrc != NULL) {
		const float  U_LF  = pSrc->left   / m_Width;
		const float  U_RT  = pSrc->right  / m_Width;
		const float  V_TP  = pSrc->top    / m_Height;
		const float  V_BT  = pSrc->bottom / m_Height;

		const float  SRC_W = float(pSrc->right  - pSrc->left);
		const float  SRC_H = float(pSrc->bottom - pSrc->top );

		// ���_�O
		m_pPool[IDX + 0].x     = P_X;
		m_pPool[IDX + 0].y     = P_Y;
		m_pPool[IDX + 0].z     = inPos.z;
		m_pPool[IDX + 0].color = inColor;
		m_pPool[IDX + 0].rhw   = 1.0f;
		m_pPool[IDX + 0].tu    = U_LF;
		m_pPool[IDX + 0].tv    = V_TP;

		// ���_�P
		m_pPool[IDX + 1].x     = P_X + SRC_W;
		m_pPool[IDX + 1].y     = P_Y;
		m_pPool[IDX + 1].z     = inPos.z;
		m_pPool[IDX + 1].color = inColor;
		m_pPool[IDX + 1].rhw   = 1.0f;
		m_pPool[IDX + 1].tu    = U_RT;
		m_pPool[IDX + 1].tv    = V_TP;

		// ���_�Q
		m_pPool[IDX + 2].x     = P_X;
		m_pPool[IDX + 2].y     = P_Y + SRC_H;
		m_pPool[IDX + 2].z     = inPos.z;
		m_pPool[IDX + 2].color = inColor;
		m_pPool[IDX + 2].rhw   = 1.0f;
		m_pPool[IDX + 2].tu    = U_LF;
		m_pPool[IDX + 2].tv    = V_BT;

		// ���_�R
		m_pPool[IDX + 3]       = m_pPool[IDX + 1];

		// ���_�S
		m_pPool[IDX + 4].x     = P_X + SRC_W;
		m_pPool[IDX + 4].y     = P_Y + SRC_H;
		m_pPool[IDX + 4].z     = inPos.z;
		m_pPool[IDX + 4].color = inColor;
		m_pPool[IDX + 4].rhw   = 1.0f;
		m_pPool[IDX + 4].tu    = U_RT;
		m_pPool[IDX + 4].tv    = V_BT;

		// ���_�T
		m_pPool[IDX + 5]       = m_pPool[IDX + 2];
	} else {
		// ���_�O
		m_pPool[IDX + 0].x     = P_X;
		m_pPool[IDX + 0].y     = P_Y;
		m_pPool[IDX + 0].z     = inPos.z;
		m_pPool[IDX + 0].color = inColor;
		m_pPool[IDX + 0].rhw   = 1.0f;
		m_pPool[IDX + 0].tu    = 0.0f;
		m_pPool[IDX + 0].tv    = 0.0f;

		// ���_�P
		m_pPool[IDX + 1].x     = P_X + m_Width;
		m_pPool[IDX + 1].y     = P_Y;
		m_pPool[IDX + 1].z     = inPos.z;
		m_pPool[IDX + 1].color = inColor;
		m_pPool[IDX + 1].rhw   = 1.0f;
		m_pPool[IDX + 1].tu    = 1.0f;
		m_pPool[IDX + 1].tv    = 0.0f;

		// ���_�Q
		m_pPool[IDX + 2].x     = P_X;
		m_pPool[IDX + 2].y     = P_Y + m_Height;
		m_pPool[IDX + 2].z     = inPos.z;
		m_pPool[IDX + 2].color = inColor;
		m_pPool[IDX + 2].rhw   = 1.0f;
		m_pPool[IDX + 2].tu    = 0.0f;
		m_pPool[IDX + 2].tv    = 1.0f;

		// ���_�R
		m_pPool[IDX + 3]       = m_pPool[IDX + 1];

		// ���_�S
		m_pPool[IDX + 4].x     = P_X + m_Width;
		m_pPool[IDX + 4].y     = P_Y + m_Height;
		m_pPool[IDX + 4].z     = inPos.z;
		m_pPool[IDX + 4].color = inColor;
		m_pPool[IDX + 4].rhw   = 1.0f;
		m_pPool[IDX + 4].tu    = 1.0f;
		m_pPool[IDX + 4].tv    = 1.0f;

		// ���_�T
		m_pPool[IDX + 5]       = m_pPool[IDX + 2];
	}

	// �J�E���^����
	m_PoolCount++;
	if(m_PoolCount >= m_PoolMax) {
		FlushPool();
	}
}

//------------------------------------------------------------------------------
//	�`��v�[���̈�o�^
//------------------------------------------------------------------------------
void  CSprite::DrawPool(const float lt_x, const float lt_y, const float rt_x, const float rt_y,
						const float rb_x, const float rb_y, const float lb_x, const float lb_y,
						const float z, const RECT* pSrc, const D3DCOLOR inColor)
{
#ifdef _DEBUG
	if(m_pPool == NULL) {
		::OutputDebugString(TEXT("*** Error - �`��v�[���̈斢����(CDXGraphics9_DrawPool)\n"));
		return;
	}
#endif

	// �`��v�[���̈�֓o�^
	const DWORD   IDX = m_PoolCount * 6;

	// ���_�O
	m_pPool[IDX + 0].x      = lt_x - 0.5f;
	m_pPool[IDX + 0].y      = lt_y - 0.5f;
	m_pPool[IDX + 0].z      = z;
	m_pPool[IDX + 0].color  = inColor;
	m_pPool[IDX + 0].rhw    = 1.0f;

	// ���_�P
	m_pPool[IDX + 1].x      = rt_x - 0.5f;
	m_pPool[IDX + 1].y      = rt_y - 0.5f;
	m_pPool[IDX + 1].z      = z;
	m_pPool[IDX + 1].color  = inColor;
	m_pPool[IDX + 1].rhw    = 1.0f;

	// ���_�Q
	m_pPool[IDX + 2].x      = lb_x - 0.5f;
	m_pPool[IDX + 2].y      = lb_y - 0.5f;
	m_pPool[IDX + 2].z      = z;
	m_pPool[IDX + 2].color  = inColor;
	m_pPool[IDX + 2].rhw    = 1.0f;

	// ���_�S
	m_pPool[IDX + 4].x      = rb_x - 0.5f;
	m_pPool[IDX + 4].y      = rb_y - 0.5f;
	m_pPool[IDX + 4].z      = z;
	m_pPool[IDX + 4].color  = inColor;
	m_pPool[IDX + 4].rhw    = 1.0f;

	// UV���W�ݒ�
	if(pSrc != NULL) {
		const float  U_LF   = pSrc->left   / m_Width;
		const float  U_RT   = pSrc->right  / m_Width;
		const float  V_TP   = pSrc->top    / m_Height;
		const float  V_BT   = pSrc->bottom / m_Height;

		m_pPool[IDX + 0].tu = U_LF;
		m_pPool[IDX + 0].tv = V_TP;

		m_pPool[IDX + 1].tu = U_RT;
		m_pPool[IDX + 1].tv = V_TP;

		m_pPool[IDX + 2].tu = U_LF;
		m_pPool[IDX + 2].tv = V_BT;

		m_pPool[IDX + 4].tu = U_RT;
		m_pPool[IDX + 4].tv = V_BT;
	} else {
		m_pPool[IDX + 0].tu = 0.0f;
		m_pPool[IDX + 0].tv = 0.0f;

		m_pPool[IDX + 1].tu = 1.0f;
		m_pPool[IDX + 1].tv = 0.0f;

		m_pPool[IDX + 2].tu = 0.0f;
		m_pPool[IDX + 2].tv = 1.0f;

		m_pPool[IDX + 4].tu = 1.0f;
		m_pPool[IDX + 4].tv = 1.0f;

	}

	// ���ʒ��_�ݒ�(���_�R�C�T)
	m_pPool[IDX + 3] = m_pPool[IDX + 1];
	m_pPool[IDX + 5] = m_pPool[IDX + 2];

	// �J�E���^����
	m_PoolCount++;
	if(m_PoolCount >= m_PoolMax) {
		FlushPool();
	}
}

//------------------------------------------------------------------------------
//	�`��v�[���̈�`��
//------------------------------------------------------------------------------
void CSprite::FlushPool()
{
#ifdef _DEBUG
	if(m_pPool == NULL) {
		::OutputDebugString(TEXT("*** Error - �`��v�[���̈斢����(CDXGraphics9_FlushPool)\n"));
		return;
	}
#endif

	IDirect3DDevice9*   pD3DDevice;
	m_pD3DTexture->GetDevice(&pD3DDevice);

	pD3DDevice->SetTexture(0, m_pD3DTexture);

	pD3DDevice->SetFVF(DXGFVF_TLVERTEX);
	pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_PoolCount * 2, m_pPool, sizeof(DXGTLVERTEX));

	pD3DDevice->Release();

	m_PoolCount = 0;
}

//------------------------------------------------------------------------------
//	�h��Ԃ�
//------------------------------------------------------------------------------
void CSprite::ColorFill(const RECT* pFill, const D3DCOLOR inColor)
{
	IDirect3DDevice9*    pD3DDevice   = NULL;
	IDirect3DSurface9*   pFillSurface = NULL;

	try {
		// Direct3D�f�o�C�X�I�u�W�F�N�g�擾
		if(m_pD3DTexture->GetDevice(&pD3DDevice) != D3D_OK)
			throw TEXT("�f�o�C�X�I�u�W�F�N�g�擾���s");

		// �h��Ԃ��p�̃T�[�t�F�X�𐶐�
		if(pD3DDevice->CreateOffscreenPlainSurface(16, 16, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pFillSurface, NULL)
		   != D3D_OK)
			throw TEXT("�h��Ԃ��T�[�t�F�X�������s");

		// �w�肳�ꂽ�F�œh��Ԃ�
		pD3DDevice->ColorFill(pFillSurface,  NULL, inColor);

		// �h��Ԃ��p�T�[�t�F�X���e�N�X�`���̃T�[�t�F�X�փ��[�h
		::D3DXLoadSurfaceFromSurface(m_pD3DSurface, NULL, pFill, pFillSurface, NULL, NULL, D3DX_FILTER_POINT, 0);
	} catch(LPCTSTR ErrorString) {
		// �G���[���b�Z�[�W����
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CSprite_ColorFill)\n"), ErrorString);
		::OutputDebugString(msg);
	}

	SafeRelease(pFillSurface);
	SafeRelease(pD3DDevice);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���
//------------------------------------------------------------------------------
void CSprite::LoadFromFile(LPCTSTR inFileName, const RECT* pSrcRect,
						   const RECT* pDestRect, const DWORD inFilter, const D3DCOLOR inColorKey)
{
	::D3DXLoadSurfaceFromFile(m_pD3DSurface, NULL, pDestRect, inFileName, pSrcRect, inFilter, inColorKey, NULL);
}

//------------------------------------------------------------------------------
//	�f�o�C�X�R���e�L�X�g�擾
//------------------------------------------------------------------------------
HDC CSprite::GetDC()
{
	if(m_hDC == NULL)
		m_pD3DSurface->GetDC(&m_hDC);

	return m_hDC;
}

//------------------------------------------------------------------------------
//	�f�o�C�X�R���e�L�X�g���
//------------------------------------------------------------------------------
void CSprite::ReleaseDC()
{
	if(m_hDC == NULL)
		return;

	m_pD3DSurface->ReleaseDC(m_hDC);
	m_hDC = NULL;
}

//------------------------------------------------------------------------------
//	���b�N
//------------------------------------------------------------------------------
DXGLOCKED_DESC CSprite::Lock()
{
	// �X�v���C�g�L�q�擾
	D3DSURFACE_DESC   surface;
	m_pD3DTexture->GetLevelDesc(0, &surface);

	// ���b�N
	D3DLOCKED_RECT   lock;
	m_pD3DTexture->LockRect(0, &lock, NULL, 0);

	// �X�v���C�g���ݒ�
	DXGLOCKED_DESC   desc;
	desc.Format = surface.Format;
	desc.Width  = surface.Width;
	desc.Height = surface.Height;
	desc.Pitch  = lock   .Pitch;
	desc.pBits  = lock   .pBits;

	return desc;
}

//------------------------------------------------------------------------------
//	���b�N����
//------------------------------------------------------------------------------
void CSprite::Unlock()
{
	m_pD3DTexture->UnlockRect(0);
}

//------------------------------------------------------------------------------
//	�X�v���C�g���擾
//------------------------------------------------------------------------------
D3DSURFACE_DESC CSprite::GetDesc()
{
	// �e�N�X�`�����擾
	D3DSURFACE_DESC   desc;
	m_pD3DTexture->GetLevelDesc(0, &desc);

	return desc;
}

//------------------------------------------------------------------------------
//	���b�N
//------------------------------------------------------------------------------
DXGLOCKED_DESC CNullSprite::Lock()
{
	DXGLOCKED_DESC   desc;
	::ZeroMemory(&desc, sizeof(desc));

	return desc;
}

//------------------------------------------------------------------------------
//	���b�N����
//------------------------------------------------------------------------------
void CNullSprite::Unlock()
{
}

//------------------------------------------------------------------------------
//	�X�v���C�g���擾
//------------------------------------------------------------------------------
D3DSURFACE_DESC CNullSprite::GetDesc()
{
	D3DSURFACE_DESC   desc;
	::ZeroMemory(&desc, sizeof(desc));

	return desc;
}
