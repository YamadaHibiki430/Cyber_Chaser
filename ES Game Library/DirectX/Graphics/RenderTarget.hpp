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
	 RenderTarget.hpp
				レンダーターゲットヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <d3dx9.h>
#include "DXGTypes.h"

//------------------------------------------------------------------------------
//	レンダーターゲットインタフェース定義
//------------------------------------------------------------------------------
class IRenderTarget {
public:
	virtual ~IRenderTarget() {}

	virtual void OnLostDevice () = 0;
	virtual void OnResetDevice() = 0;

	virtual void Use() = 0;
	virtual void Use(const D3DCUBEMAP_FACES inFace) = 0;

	virtual void Clear(const D3DCOLOR inColor = 0) = 0;

	virtual void SetCubeFace(const D3DCUBEMAP_FACES inFace) = 0;

	virtual UINT GetWidth () const = 0;
	virtual UINT GetHeight() const = 0;

	virtual IDirect3DBaseTexture9* GetTexture() const = 0;
	virtual IDirect3DSurface9*     GetSurface() const = 0;

	virtual D3DSURFACE_DESC GetSurfaceDesc() = 0;
	inline  D3DSURFACE_DESC GetTextureDesc() { return GetSurfaceDesc(); }

	virtual HDC  GetDC    (const bool inGetImage = true ) = 0;
	virtual void ReleaseDC(const bool inRelease  = false) = 0;

	virtual DXGLOCKED_DESC Lock(const bool inGetImage = true ) = 0;
	virtual void Unlock(const bool inRelease = false) = 0;

	virtual operator IDirect3DTexture9*() const = 0;
	virtual operator IDirect3DSurface9*() const = 0;
};

//------------------------------------------------------------------------------
//	レンダーターゲットクラス定義
//------------------------------------------------------------------------------
class CRenderTarget : public virtual IRenderTarget {
public:
	CRenderTarget(IDirect3DTexture9* pTexture, IDirect3DSurface9* pDepthStencil,
				  const UINT inWidth, const UINT inHeight);
	CRenderTarget(IDirect3DCubeTexture9* pCubeTexture, IDirect3DSurface9* pDepthStencil,
				  const UINT inSize);

	virtual ~CRenderTarget();

	virtual void OnLostDevice ();
	virtual void OnResetDevice();

	virtual void Use();
	virtual void Use(const D3DCUBEMAP_FACES inFace);

	virtual void Clear(const D3DCOLOR inColor);

	virtual void SetCubeFace(const D3DCUBEMAP_FACES inFace);

	virtual UINT GetWidth () const { return m_Width;  }
	virtual UINT GetHeight() const { return m_Height; }

	virtual IDirect3DBaseTexture9* GetTexture() const { return m_pBaseTexture;   }
	virtual IDirect3DSurface9*     GetSurface() const { return m_pRenderSurface; }

	virtual D3DSURFACE_DESC GetSurfaceDesc();

	virtual HDC  GetDC(const bool inGetImage);
	virtual void ReleaseDC(const bool inRelease);

	virtual DXGLOCKED_DESC Lock(const bool inGetImage);
	virtual void Unlock(const bool inRelease);

	virtual operator IDirect3DTexture9*() const { return m_pTexture; }
	virtual operator IDirect3DSurface9*() const { return m_pRenderSurface; }

protected:
	IDirect3DDevice9*        m_pD3DDevice;

private:
	IDirect3DBaseTexture9*   m_pBaseTexture;
	IDirect3DTexture9*       m_pTexture;
	IDirect3DCubeTexture9*   m_pCubeTexture;

	IDirect3DSurface9*       m_pRenderSurface;
	IDirect3DSurface9*       m_pDepthStencil;

	UINT                     m_Width;
	UINT                     m_Height;

	D3DFORMAT                m_Format;
	D3DFORMAT                m_DepthFormat;

	DWORD                    m_ClearFlags;

	IDirect3DSurface9*       m_pD3DSysMemSurface;
	HDC                      m_hDC;

	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}
};

//------------------------------------------------------------------------------
//	NULLレンダーターゲットクラス定義
//------------------------------------------------------------------------------
class CNullRenderTarget : public virtual IRenderTarget {
public:
	virtual ~CNullRenderTarget() {}

	virtual void OnLostDevice () {}
	virtual void OnResetDevice() {}

	virtual void Use() {}
	virtual void Use(const D3DCUBEMAP_FACES inFace) {}

	virtual void Clear(const D3DCOLOR inColor) {}

	virtual void SetCubeFace(const D3DCUBEMAP_FACES inFace) {}

	virtual UINT GetWidth () const { return 0; }
	virtual UINT GetHeight() const { return 0; }

	virtual IDirect3DBaseTexture9* GetTexture() const { return NULL; }
	virtual IDirect3DSurface9*     GetSurface() const { return NULL; }

	virtual D3DSURFACE_DESC GetSurfaceDesc();

	virtual HDC  GetDC(const bool inGetImage) { return NULL; }
	virtual void ReleaseDC(const bool inRelease) {}

	virtual DXGLOCKED_DESC Lock(const bool inGetImage);
	virtual void Unlock(const bool inRelease);

	virtual operator IDirect3DTexture9*() const { return NULL; }
	virtual operator IDirect3DSurface9*() const { return NULL; }
};
