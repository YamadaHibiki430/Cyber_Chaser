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
	 Sprite.hpp
				スプライトクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DXGTypes.h"

//------------------------------------------------------------------------------
//	スプライトインタフェース定義
//------------------------------------------------------------------------------
class ISprite {
public:
	virtual ~ISprite() {}

	// 描画プール
	virtual DWORD CreatePool(const DWORD inPoolSize = 2048) = 0;
	virtual void ReleasePool() = 0;

	virtual void DrawPool(const D3DXVECTOR3& inPos, const RECT* pSrc = NULL,
						  const D3DCOLOR inColor = 0xffffffff) = 0;
	virtual void DrawPool(const float lt_x, const float lt_y,
						  const float rt_x, const float rt_y,
						  const float rb_x, const float rb_y,
						  const float lb_x, const float lb_y,
						  const float z,
						  const RECT* pSrc = NULL, const D3DCOLOR inColor = 0xffffffff) = 0;

	inline void DrawPool(const D3DXVECTOR3& inPos, const RECT* pSrc,
						 const float inAlpha, const DWORD inColor = 0x00ffffff)
	{ DrawPool(inPos, pSrc, ((DWORD)(inAlpha * 255.0f) << 24) | (inColor & 0x00ffffff)); }

	inline void DrawPool(const float lt_x,    const float lt_y,
						 const float rt_x,    const float rt_y,
						 const float rb_x,    const float rb_y,
						 const float lb_x,    const float lb_y,
						 const float z,       const RECT* pSrc,
						 const float inAlpha, const DWORD inColor = 0x00ffffff)
	{ DrawPool(lt_x, lt_y, rt_x, rt_y, rb_x, rb_y, lb_x, lb_y, z, pSrc,
			   ((DWORD)(inAlpha * 255.0f) << 24) | (inColor & 0x00ffffff)); }

	virtual void FlushPool() = 0;

	virtual void ColorFill(const RECT* pFill, const D3DCOLOR inColor) = 0;
	virtual void LoadFromFile(LPCTSTR inFileName, const RECT* pSrcRect, const RECT* pDestRect,
							  const DWORD inFilter = D3DX_DEFAULT, const D3DCOLOR inColorKey = 0) = 0;

	// HDC
	virtual HDC  GetDC() = 0;
	virtual void ReleaseDC() = 0;

	// ロック
	virtual DXGLOCKED_DESC Lock() = 0;
	virtual void Unlock() = 0;

	// 情報取得
	virtual D3DSURFACE_DESC GetDesc() = 0;

	virtual float GetWidth () const = 0;
	virtual float GetHeight() const = 0;

	virtual IDirect3DTexture9* GetTexture() const = 0;
	virtual IDirect3DSurface9* GetSurface() const = 0;

	// オペレータ
	virtual operator IDirect3DTexture9*()   const = 0;
	virtual operator IDirect3DSurface9*()   const = 0;
};

//------------------------------------------------------------------------------
//	スプライトクラス定義
//------------------------------------------------------------------------------
class CSprite : public ISprite {
public:
	CSprite(IDirect3DTexture9* pD3DTexture);
	virtual ~CSprite();

	virtual DWORD CreatePool(const DWORD inPoolSize);
	virtual void ReleasePool();

	virtual void DrawPool(const D3DXVECTOR3& inPos, const RECT* pSrc, const D3DCOLOR inColor);
	virtual void DrawPool(const float lt_x, const float lt_y,
						  const float rt_x, const float rt_y,
						  const float rb_x, const float rb_y,
						  const float lb_x, const float lb_y,
						  const float z, const RECT* pSrc, const D3DCOLOR inColor);

	virtual void FlushPool();

	virtual void ColorFill(const RECT* pFill, const D3DCOLOR inColor);

	virtual void LoadFromFile(LPCTSTR inFileName,  const RECT* pSrcRect, const RECT* pDestRect,  
							  const DWORD inFilter, const D3DCOLOR inColorKey);

	virtual HDC  GetDC();
	virtual void ReleaseDC();

	virtual DXGLOCKED_DESC Lock();
	virtual void Unlock();

	virtual D3DSURFACE_DESC GetDesc();

	virtual float GetWidth () const { return m_Width;  }
	virtual float GetHeight() const { return m_Height; }

	virtual IDirect3DTexture9* GetTexture() const { return m_pD3DTexture; }
	virtual IDirect3DSurface9* GetSurface() const { return m_pD3DSurface; }

	virtual operator IDirect3DTexture9*()   const { return m_pD3DTexture; }
	virtual operator IDirect3DSurface9*()   const { return m_pD3DSurface; }

private:
	IDirect3DTexture9*   m_pD3DTexture;
	IDirect3DSurface9*   m_pD3DSurface;

	DXGTLVERTEX*         m_pPool;
	DWORD                m_PoolMax;
	DWORD                m_PoolCount;

	HDC                  m_hDC;

	float                m_Width;
	float                m_Height;

	template<class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}
};

//------------------------------------------------------------------------------
//	NULLスプライトクラス定義
//------------------------------------------------------------------------------
class CNullSprite : public ISprite {
public:
	CNullSprite() {}
	virtual ~CNullSprite() {}

	virtual DWORD CreatePool(const DWORD inPoolSize) { return 0; }
	virtual void  ReleasePool() {}

	virtual void  DrawPool(const D3DXVECTOR3& inPos, const RECT* pSrc, const D3DCOLOR inColor) {}
	virtual void  DrawPool(const float lt_x, const float lt_y,
						   const float rt_x, const float rt_y,
						   const float rb_x, const float rb_y,
						   const float lb_x, const float lb_y,
						   const float z, const RECT* pSrc, const D3DCOLOR inColor) {}

	virtual void  FlushPool() {}

	virtual void ColorFill(const RECT* pFill, const D3DCOLOR inColor) {}

	virtual void LoadFromFile(LPCTSTR inFileName,  const RECT* pSrcRect, const RECT* pDestRect,  
							  const DWORD inFilter, const D3DCOLOR inColorKey) {}

	virtual HDC  GetDC() { return NULL; }
	virtual void ReleaseDC() {}

	virtual DXGLOCKED_DESC Lock();
	virtual void Unlock();

	virtual D3DSURFACE_DESC GetDesc();

	virtual float GetWidth () const { return 0; }
	virtual float GetHeight() const { return 0; }

	virtual IDirect3DTexture9* GetTexture() const { return NULL; }
	virtual IDirect3DSurface9* GetSurface() const { return NULL; }

	virtual operator IDirect3DTexture9*()   const { return NULL; }
	virtual operator IDirect3DSurface9*()   const { return NULL; }
};
