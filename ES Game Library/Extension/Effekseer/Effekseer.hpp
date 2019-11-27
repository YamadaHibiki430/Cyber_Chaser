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
	 Effekseer.hpp
				Effekseerクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_EFFEKSEER
#define _INC_EFFEKSEER

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <EffekseerRendererDX9.h>
#include "EffekseerEffect.hpp"

#include <d3dx9.h>
#include <list>
#include <set>

#include "..\..\DirectX\Graphics\Camera.hpp"

//------------------------------------------------------------------------------
//	Effekseerクラス定義
//------------------------------------------------------------------------------
class CEffekseer {
public:
	virtual ~CEffekseer();

	// シングルトンインスタンスの取得
	static CEffekseer& GetInstance()
	{
		static CEffekseer   theEffekseer;
		return theEffekseer;
	}

	bool Initialize(IDirect3DDevice9* pD3DDevice, const int inMaxSprite = 2048);
	void Release();

	inline bool Attach(IDirect3DDevice9* pD3DDevice, const int inMaxSprite = 2048)
	{ return Initialize(pD3DDevice, inMaxSprite); }

	IEffekseerEffect* CreateEffectFromFile(LPCWSTR inFileName);
	void ReleaseEffect(IEffekseerEffect*& pEffect);
	void ReleaseAllEffects();

	void SetResourceProtect(void* pResource, const bool inProtect);

//	void SetView      (const Effekseer::Matrix44& inMatrix);
//	void SetProjection(const Effekseer::Matrix44& inMatrix);

	void SetView      (D3DMATRIX& inMatrix);
	void SetProjection(D3DMATRIX& inMatrix);
	void SetCamera    (CCamera&   inCamera);

	void Update();
	void Draw();

	void CreateCullingWorld(const float inWidth, const float inHeight,
							const float inDepth, const int   inLayerCount);
	void CalculateCulling();

	D3DXVECTOR3 ScreenToWorld(const float inScreenX, const float inScreenY,
							  const float inEffectPosZ);
	inline D3DXVECTOR3 ScreenToWorld(const D3DXVECTOR3& inScreen, const float inEffectPosZ)
	{ return ScreenToWorld(inScreen.x, inScreen.y, inEffectPosZ); }

private:
	Effekseer::Manager*               m_manager;
	EffekseerRendererDX9::Renderer*   m_renderer;

	std::list<IEffekseerEffect*>      m_effect_list;
	std::set <void*>                  m_protected_resource;

private:
	CEffekseer();

	CEffekseer(const CEffekseer&);
	CEffekseer& operator=(const CEffekseer&);
};

//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
inline CEffekseer& EffekseerManager() { return CEffekseer::GetInstance(); }

#endif /* _INC_EFFEKSEER */