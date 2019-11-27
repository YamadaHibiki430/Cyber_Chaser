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
	 DXGFont.hpp
				DirectX Graphicsフォントクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DXGFont.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
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
//	デストラクタ
//------------------------------------------------------------------------------
CDXGFont::~CDXGFont()
{
	SafeRelease(m_pD3DXSprite);
	SafeRelease(m_pD3DXFont  );
}

//------------------------------------------------------------------------------
//	文字列描画
//------------------------------------------------------------------------------
void CDXGFont::DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
						  const DWORD inFormat, const D3DCOLOR inColor)
{
	D3DXMATRIX   mat;
	m_pD3DXSprite->SetTransform(::D3DXMatrixIdentity(&mat));
	m_pD3DXFont->DrawText(m_pD3DXSprite, pString, inCount, &inDestRect, inFormat, inColor);
}

//------------------------------------------------------------------------------
//	文字列描画
//------------------------------------------------------------------------------
void CDXGFont::DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
						  const DWORD inFormat, const D3DCOLOR inColor,
						  const D3DXVECTOR2& inScale, 
						  const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter)
{
	// 回転クォータニオン生成
	D3DXQUATERNION   rot;
	::D3DXQuaternionRotationYawPitchRoll(&rot,
										 D3DXToRadian(inRotation.y),
										 D3DXToRadian(inRotation.x),
										 D3DXToRadian(inRotation.z));

	// 変換行列設定
	D3DXVECTOR3   scale(inScale.x, inScale.y, 1.0f);
	D3DXMATRIX    mat;
	::D3DXMatrixTransformation(&mat, NULL, NULL, &scale, &inCenter, &rot, NULL);
	m_pD3DXSprite->SetTransform(&mat);

	// 描画
	m_pD3DXFont->DrawText(m_pD3DXSprite, pString, inCount, &inDestRect, inFormat, inColor);
}
