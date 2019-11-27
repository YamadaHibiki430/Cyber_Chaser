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
	 DXGFont.hpp
				DirectX Graphics�t�H���g�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "DXGFont.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CDXGFont::CDXGFont(ID3DXFont* pD3DXFont, ID3DXSprite* pD3DXSprite)
	: m_pD3DXFont(pD3DXFont), m_pD3DXSprite(pD3DXSprite)
{
	assert(m_pD3DXFont != NULL);
	m_pD3DXFont->AddRef();

	if(m_pD3DXSprite   != NULL)	
		m_pD3DXSprite->AddRef();
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CDXGFont::~CDXGFont()
{
	SafeRelease(m_pD3DXSprite);
	SafeRelease(m_pD3DXFont  );
}

//------------------------------------------------------------------------------
//	������`��
//------------------------------------------------------------------------------
void CDXGFont::DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
						  const DWORD inFormat, const D3DCOLOR inColor)
{
	D3DXMATRIX   mat;
	m_pD3DXSprite->SetTransform(::D3DXMatrixIdentity(&mat));
	m_pD3DXFont->DrawText(m_pD3DXSprite, pString, inCount, &inDestRect, inFormat, inColor);
}

//------------------------------------------------------------------------------
//	������`��
//------------------------------------------------------------------------------
void CDXGFont::DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
						  const DWORD inFormat, const D3DCOLOR inColor,
						  const D3DXVECTOR2& inScale, 
						  const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter)
{
	// ��]�N�H�[�^�j�I������
	D3DXQUATERNION   rot;
	::D3DXQuaternionRotationYawPitchRoll(&rot,
										 D3DXToRadian(inRotation.y),
										 D3DXToRadian(inRotation.x),
										 D3DXToRadian(inRotation.z));

	// �ϊ��s��ݒ�
	D3DXVECTOR3   scale(inScale.x, inScale.y, 1.0f);
	D3DXMATRIX    mat;
	::D3DXMatrixTransformation(&mat, NULL, NULL, &scale, &inCenter, &rot, NULL);
	m_pD3DXSprite->SetTransform(&mat);

	// �`��
	m_pD3DXFont->DrawText(m_pD3DXSprite, pString, inCount, &inDestRect, inFormat, inColor);
}
