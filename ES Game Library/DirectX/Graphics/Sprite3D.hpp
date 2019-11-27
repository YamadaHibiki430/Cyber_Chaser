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
	 Sprite3D.hpp
				Sprite3Dクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	ワーニング抑制
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4250)

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "RenderTarget.hpp"
#include "Camera.hpp"

#include <list>

//------------------------------------------------------------------------------
//	Sprite3Dクラス定義
//------------------------------------------------------------------------------
class CSprite3D {
public:
	// シングルトンインスタンスの取得
	static CSprite3D& GetInstance()
	{
		static CSprite3D theSprite3D;
		return theSprite3D;
	}

	virtual ~CSprite3D();

	bool Initialize(IDirect3DDevice9* pD3Device);
	void Release();

	void Begin();
	void End(const bool inRestoreState = true);


private:
	CSprite3D();

	template <class T>
	inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	// コピーできないようにする
	CSprite3D(const CSprite3D&);				// コピーコンストラクタ
	CSprite3D& operator=(const CSprite3D&);		// 代入演算子

private:
	IDirect3DDevice9*    m_pD3DDevice;

	bool                 m_Begin;

	IDirect3DSurface9*   m_pRenderSurface;
	IDirect3DSurface9*   m_pDepthStencil;

	D3DXMATRIX           m_View;
	D3DXMATRIX           m_Projection;
	D3DVIEWPORT9         m_Viewport;
};

//------------------------------------------------------------------------------
//	インライン関数
//------------------------------------------------------------------------------
inline CSprite3D& DXGSprite3D() { return CSprite3D::GetInstance(); }

//------------------------------------------------------------------------------
//	Sprite3DLayerインタフェース定義
//------------------------------------------------------------------------------
class ISprite3DLayer : public virtual IRenderTarget {
public:
	virtual void SetInitialView(const D3DXMATRIX& inView) = 0;
	virtual void SetInitialView(const CCamera&  inCamera) = 0;

	virtual void SetInitialProjection(const D3DXMATRIX& inProjection) = 0;
	virtual void SetInitialProjection(const CCamera&    inCamera) = 0;

	virtual void SetInitialCameraSetting(const CCamera& inCamera) = 0;

	virtual void SetInitialRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue) = 0;
	virtual void SetInitialFloatRenderState(const D3DRENDERSTATETYPE inState, float inValue)
	{ SetInitialRenderState(inState, *((DWORD*)&inValue)); }

	virtual void ClearInitialRenderState() = 0;

	virtual void BeginRender(const bool inInitView     = true,
							 const bool inInitProj     = true,
							 const bool inSetState     = true) = 0;
	virtual void EndRender  (const bool inRestoreState = true) = 0;

	virtual void SetRenderRect(const RECT& inRenderRect) = 0;

	virtual void Clear(const D3DCOLOR inColor = 0) = 0;
};

//------------------------------------------------------------------------------
//	Sprite3DLayerクラス定義
//------------------------------------------------------------------------------
class CSprite3DLayer : public ISprite3DLayer, public CRenderTarget {
public:
	CSprite3DLayer(IDirect3DTexture9* pTexture, IDirect3DSurface9* pDepthStencil,
				   const UINT inWidth, const UINT inHeight);

	virtual ~CSprite3DLayer() {}

	virtual void SetInitialView(const D3DXMATRIX& inView)
	{ 
		m_InitialView = inView;
	}

	virtual void SetInitialView(const CCamera& inCamera)
	{ 
		m_InitialView = inCamera.GetViewMatrix();
	}

	virtual void SetInitialProjection(const D3DXMATRIX& inProjection)
	{ 
		m_InitialProjection = inProjection;
	}

	virtual void SetInitialProjection(const CCamera& inCamera)
	{ 
		m_InitialProjection = inCamera.GetProjectionMatrix();
	}

	virtual void SetInitialCameraSetting(const CCamera& inCamera)
	{ 
		m_InitialView       = inCamera.GetViewMatrix();
		m_InitialProjection = inCamera.GetProjectionMatrix();
	}

	virtual void SetInitialRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue);

	virtual void ClearInitialRenderState()
	{
		m_StateList.clear();
	}

	virtual void BeginRender(const bool inInitView, const bool inInitProj,
							 const bool inSetState);
	virtual void EndRender(const bool inRestoreState);

	virtual void SetRenderRect(const RECT& inRenderRect);

	virtual void Clear(const D3DCOLOR inColor)
	{ CRenderTarget::Clear(0x00ffffff & inColor); }

private:
	D3DXMATRIX   m_InitialView;
	D3DXMATRIX   m_InitialProjection;

	// ステート値保存構造体
	struct STATE {
		D3DRENDERSTATETYPE Type;	// ステートタイプ
		DWORD	           Value;	// 設定値
		DWORD	           Restore;	// 復元値
	};
	std::list<STATE>   m_StateList;
};

//------------------------------------------------------------------------------
//	NullSprite3DLayerクラス定義
//------------------------------------------------------------------------------
class CNullSprite3DLayer : public ISprite3DLayer, public CNullRenderTarget {
public:
	virtual void SetInitialView(const D3DXMATRIX& inView) {}
	virtual void SetInitialView(const CCamera&  inCamera) {}

	virtual void SetInitialProjection(const D3DXMATRIX& inProjection) {}
	virtual void SetInitialProjection(const CCamera&    inCamera)     {}

	virtual void SetInitialCameraSetting(const CCamera& inCamera) {}

	virtual void SetInitialRenderState(const D3DRENDERSTATETYPE inState,
									   const DWORD inValue) {}
	virtual void ClearInitialRenderState() {}

	virtual void BeginRender(const bool inInitView, const bool inInitProj,
							 const bool inSetState) {}
	virtual void EndRender  (const bool inRestoreState) {}
	virtual void SetRenderRect(const RECT& inRenderRect) {}

	virtual void Clear(const D3DCOLOR inColor) {}
};

#pragma warning(pop)
