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
	 SpriteBatch.cpp
				SpriteBatch�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "SpriteBatch.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CSpriteBatch::CSpriteBatch() : m_pD3DDevice(NULL), m_pD3DXSprite(NULL)
{
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CSpriteBatch::~CSpriteBatch()
{
	Release();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CSpriteBatch::Initialize(ID3DXSprite*& pD3DXSprite)
{
	assert(pD3DXSprite != NULL);

	Release();

	m_pD3DXSprite = pD3DXSprite;
	m_pD3DXSprite->AddRef();

	m_pD3DXSprite->GetDevice(&m_pD3DDevice);

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CSpriteBatch::Release()
{
	SafeRelease(m_pD3DXSprite);
	SafeRelease(m_pD3DDevice );
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��J�n
//------------------------------------------------------------------------------
void CSpriteBatch::Begin()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);

	// �v���W�F�N�V�����s��ݒ�
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��J�n
//------------------------------------------------------------------------------
void CSpriteBatch::Begin(const DWORD inBlendMode, const DWORD inSortMode)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	if(inBlendMode == 0) {
		m_pD3DXSprite->Begin(inSortMode);
	} else {
		m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | inSortMode);
		m_pD3DDevice ->SetTextureStageState(0, D3DTSS_COLOROP, inBlendMode);
	}

	// �v���W�F�N�V�����s��ݒ�
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��J�n
//------------------------------------------------------------------------------
void CSpriteBatch::BeginFast()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT);

	// �v���W�F�N�V�����s��ݒ�
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��J�n(�E��n)
//------------------------------------------------------------------------------
void CSpriteBatch::BeginRH()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_BeginRH)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT);

	// �r���[�s��ݒ�
	D3DXMATRIX   view;
	::D3DXMatrixIdentity(&view);
	view._33 = -view._33;
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

	// �v���W�F�N�V�����s��ݒ�
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��J�n(�E��n)
//------------------------------------------------------------------------------
void CSpriteBatch::BeginRH(const DWORD inBlendMode, const DWORD inSortMode)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_BeginRH)\n"));
		return;
	}
#endif

	if(inBlendMode == 0) {
		m_pD3DXSprite->Begin(inSortMode);
	} else {
		m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | inSortMode);
		m_pD3DDevice ->SetTextureStageState(0, D3DTSS_COLOROP, inBlendMode);
	}

	// �r���[�s��ݒ�
	D3DXMATRIX   view;
	::D3DXMatrixIdentity(&view);
	view._33 = -view._33;
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

	// �v���W�F�N�V�����s��ݒ�
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��J�n(�E��n)
//------------------------------------------------------------------------------
void CSpriteBatch::BeginFastRH()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Begin)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);

	// �r���[�s��ݒ�
	D3DXMATRIX   view;
	::D3DXMatrixIdentity(&view);
	view._33 = -view._33;
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

	// �v���W�F�N�V�����s��ݒ�
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	proj._33 = 1.0f              / (DXGSPRBAT_BOTTOMMOST -    DXGSPRBAT_TOPMOST);	//   1  / (far - Near)
	proj._43 = DXGSPRBAT_TOPMOST / (DXGSPRBAT_TOPMOST    - DXGSPRBAT_BOTTOMMOST);	// Near / (Near- far )
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��I��
//------------------------------------------------------------------------------
void CSpriteBatch::End()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_End)\n"));
		return;
	}
#endif

	m_pD3DXSprite->End();
}

//------------------------------------------------------------------------------
//	�X�v���C�g�ϊ��s�񏉊���
//------------------------------------------------------------------------------
void CSpriteBatch::InitTransform()
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_InitTransform)\n"));
		return;
	}
#endif

	D3DXMATRIX   mat;
	::D3DXMatrixIdentity(&mat);
	m_pD3DXSprite->SetTransform(&mat);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�ϊ��s��ݒ�
//------------------------------------------------------------------------------
void CSpriteBatch::SetTransform(const D3DXMATRIX& inTransform)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_SetTransform)\n"));
		return;
	}
#endif

	m_pD3DXSprite->SetTransform(&inTransform);
}

//------------------------------------------------------------------------------
//	�X�v���C�g�`��t�B���^�[�ݒ�
//------------------------------------------------------------------------------
void CSpriteBatch::SetFilter(const D3DTEXTUREFILTERTYPE inFilterType)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_SetFilter)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, inFilterType);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, inFilterType);
}

//------------------------------------------------------------------------------
//	�P���`��
//------------------------------------------------------------------------------
void CSpriteBatch::DrawSimple(IDirect3DTexture9* pTexture,
							  const RECT* pSrc, const D3DXVECTOR3& inPos,
							  const D3DCOLOR inColorMask)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_DrawSimple)\n"));
		return;
	}
#endif

	m_pD3DXSprite->Draw(pTexture, pSrc, NULL, &inPos, inColorMask);
}

//------------------------------------------------------------------------------
//	���@�\�`��
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(IDirect3DTexture9* pTexture,
						const RECT* pSrc, const D3DXVECTOR3& inPos,
						const D3DCOLOR inColorMask)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	// �ϊ��s�񏉊���
	D3DXMATRIX   mat;
	m_pD3DXSprite->SetTransform(::D3DXMatrixIdentity(&mat));

	m_pD3DXSprite->Draw(pTexture, pSrc, NULL, &inPos, inColorMask);
}

//------------------------------------------------------------------------------
//	���@�\�`��
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(IDirect3DTexture9* pTexture, const RECT* pSrc,
						const D3DXVECTOR3& inPos,
						const D3DXVECTOR2& inScale,
						const D3DXVECTOR3& inRotation, const D3DXVECTOR3* pCenter,
						const D3DCOLOR     inColorMask)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	// �ϊ��s��ݒ�
	D3DXMATRIX   transform;

	// ��]
	::D3DXMatrixRotationYawPitchRoll(&transform,
									 D3DXToRadian(inRotation.y),
									 D3DXToRadian(inRotation.x),
									 D3DXToRadian(inRotation.z));
/*
	D3DXQUATERNION   rot;
	::D3DXQuaternionRotationYawPitchRoll(&rot,
										 D3DXToRadian(inRotation.y),
										 D3DXToRadian(inRotation.x),
										 D3DXToRadian(inRotation.z));
	::D3DXMatrixRotationQuaternion(&transform, &rot);
*/

	// �X�P�[�����O
	transform._11 *= inScale.x;
	transform._12 *= inScale.x;
	transform._13 *= inScale.x;

	transform._21 *= inScale.y;
	transform._22 *= inScale.y;
	transform._23 *= inScale.y;

	// �ړ�
	transform._41  = inPos.x;
	transform._42  = inPos.y;
	transform._43  = inPos.z;

	transform._44  = 1.0f;

	// �`��
	m_pD3DXSprite->SetTransform(&transform);
	m_pD3DXSprite->Draw(pTexture, pSrc, pCenter, NULL, inColorMask);
}

//------------------------------------------------------------------------------
//	SpriteElement�`��
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(const SpriteElement& inSpriteElement)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	D3DXMATRIX   transform;

	// ��]
	::D3DXMatrixRotationYawPitchRoll(&transform,
									 D3DXToRadian(inSpriteElement.rotation.y),
									 D3DXToRadian(inSpriteElement.rotation.x),
									 D3DXToRadian(inSpriteElement.rotation.z));

	// �X�P�[�����O
	transform._11 *= inSpriteElement.scale.x;
	transform._12 *= inSpriteElement.scale.x;
	transform._13 *= inSpriteElement.scale.x;

	transform._21 *= inSpriteElement.scale.y;
	transform._22 *= inSpriteElement.scale.y;
	transform._23 *= inSpriteElement.scale.y;

	// �ړ�
	transform._41  = inSpriteElement.position.x;
	transform._42  = inSpriteElement.position.y;
	transform._43  = inSpriteElement.position.z;

	transform._44  = 1.0f;

	// �����x
	const DWORD   COLOR_MASK = ((DWORD)(inSpriteElement.alpha * 255.0f) << 24) | 0x00ffffff;

	m_pD3DXSprite->SetTransform(&transform);
	m_pD3DXSprite->Draw(inSpriteElement.pTexture,    &inSpriteElement.srcRect,
					   &inSpriteElement.anchorPoint, NULL, COLOR_MASK);
}

//------------------------------------------------------------------------------
//	Box2D�g��
//------------------------------------------------------------------------------
#ifdef BOX2D_H

// �X�^�e�B�b�N�����N���C�u����
#ifndef _DEBUG
	#pragma comment(lib, "Box2D.lib")
#else
	#pragma comment(lib, "Box2Dd.lib")
#endif

// �ÓI�����o
float PhysicsSprite::PTMRatio = 32;

//------------------------------------------------------------------------------
//	PhysicsSprite�`��
//------------------------------------------------------------------------------
void CSpriteBatch::Draw(const PhysicsSprite& inPhysicsSprite)
{
#ifdef DXG_SB_DEBUG
	if(m_pD3DXSprite == NULL) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`��������(CSpriteBatch_Draw)\n"));
		return;
	}
#endif
#ifdef _DEBUG
	if(inPhysicsSprite.pBody == NULL) {
		::OutputDebugString(TEXT("*** Error - Body��������(CSpriteBatch_Draw)\n"));
		return;
	}
#endif

	D3DXMATRIX   transform;
	b2Vec2       vec2;

	// ��]
	const float   z_angle = inPhysicsSprite.pBody->GetAngle();
	::D3DXMatrixRotationZ(&transform, D3DXToRadian(z_angle));

	// �X�P�[�����O


	// �ړ�
	vec2           = inPhysicsSprite.pBody->GetPosition();
	transform._41  = vec2.x * PhysicsSprite::PTMRatio;
	transform._42  = vec2.y * PhysicsSprite::PTMRatio;
	transform._43  = inPhysicsSprite.z;

	transform._44  = 1.0f;

	// ��]��
	D3DXVECTOR3   anchor;
	vec2 = inPhysicsSprite.pBody->GetLocalCenter();
	anchor.x = vec2.x * PhysicsSprite::PTMRatio + inPhysicsSprite.anchorPoint.x;
	anchor.y = vec2.y * PhysicsSprite::PTMRatio + inPhysicsSprite.anchorPoint.y;
	anchor.z = 0.0f;

	// �����x
	const DWORD   COLOR_MASK = ((DWORD)(inPhysicsSprite.alpha * 255.0f) << 24) | 0x00ffffff;

	m_pD3DXSprite->SetTransform(&transform);
	m_pD3DXSprite->Draw(inPhysicsSprite.pTexture, &inPhysicsSprite.srcRect,
					   &anchor, NULL, COLOR_MASK);
}

#endif

