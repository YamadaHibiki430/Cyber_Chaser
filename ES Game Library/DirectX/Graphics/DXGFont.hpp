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
				DirectX Graphics�t�H���g�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <d3dx9.h>

//------------------------------------------------------------------------------
//	DirectX Graphics�t�H���g�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IDXGFont {
public:
	virtual ~IDXGFont() {}

	virtual void OnLostDevice () = 0;
	virtual void OnResetDevice() = 0;

	virtual void PreloadCharacters(const UINT inStart, const UINT inEnd) = 0;
	virtual void PreloadText(LPCTSTR pString) = 0;

	virtual void Draw(LPCTSTR pString, const int inCount, RECT& inDestRect,
					  const DWORD inFormat, const D3DCOLOR inColor) = 0;

	virtual void DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
							const DWORD inFormat, const D3DCOLOR inColor) = 0;

	virtual void DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
							const DWORD inFormat, const D3DCOLOR inColor,
							const D3DXVECTOR2& inScale,
							const D3DXVECTOR3& inRotation,
							const D3DXVECTOR3& inCenter) = 0;

	inline void Draw(LPCTSTR pString, RECT& inDestRect, const D3DCOLOR inColor)
	{ Draw(pString, -1, inDestRect, DT_WORDBREAK, inColor); }
};

//------------------------------------------------------------------------------
//	�t�H���g�N���X��`
//------------------------------------------------------------------------------
class CDXGFont : public IDXGFont {
public:
	CDXGFont(ID3DXFont* pD3DXFont, ID3DXSprite* pD3DXSprite);
	virtual ~CDXGFont();

	virtual void OnLostDevice () { m_pD3DXFont->OnLostDevice (); }
	virtual void OnResetDevice() { m_pD3DXFont->OnResetDevice(); }

	virtual void PreloadCharacters(const UINT inStart, const UINT inEnd)
	{ m_pD3DXFont->PreloadCharacters(inStart, inEnd); }
	virtual void PreloadText(LPCTSTR pString)
	{ m_pD3DXFont->PreloadText(pString, ::lstrlen(pString)); }

	virtual void Draw(LPCTSTR pString, const int inCount, RECT& inDestRect,
					  const DWORD inFormat, const D3DCOLOR inColor)
	{ m_pD3DXFont->DrawText(NULL, pString, inCount, &inDestRect, inFormat, inColor); }

	virtual void DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
							const DWORD inFormat, const D3DCOLOR inColor);

	virtual void DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
							const DWORD inFormat, const D3DCOLOR inColor,
							const D3DXVECTOR2& inScale,
							const D3DXVECTOR3& inRotation,
							const D3DXVECTOR3& inCenter);

private:
	ID3DXFont*     m_pD3DXFont;
	ID3DXSprite*   m_pD3DXSprite;

	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}
};

//------------------------------------------------------------------------------
//	NULL�t�H���g�N���X��`
//------------------------------------------------------------------------------
class CDXGNullFont : public IDXGFont {
public:
	CDXGNullFont() {}
	virtual ~CDXGNullFont() {}

	virtual void OnLostDevice () {}
	virtual void OnResetDevice() {}

	virtual void PreloadCharacters(const UINT inStart, const UINT inEnd) {}
	virtual void PreloadText(LPCTSTR pString) {}

	virtual void Draw(LPCTSTR pString, const int inCount, RECT& inDestRect,
					  const DWORD inFormat, const D3DCOLOR inColor) {}

	virtual void DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
							const DWORD inFormat, const D3DCOLOR inColor) {}

	virtual void DrawSprite(LPCTSTR pString, const int inCount, RECT& inDestRect,
							const DWORD inFormat, const D3DCOLOR inColor,
							const D3DXVECTOR2& inScale,
							const D3DXVECTOR3& inRotation,
							const D3DXVECTOR3& inCenter) {}
};
