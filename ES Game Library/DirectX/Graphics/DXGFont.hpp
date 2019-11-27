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
				DirectX Graphicsフォントクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <d3dx9.h>

//------------------------------------------------------------------------------
//	DirectX Graphicsフォントインタフェース定義
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
//	フォントクラス定義
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
//	NULLフォントクラス定義
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
