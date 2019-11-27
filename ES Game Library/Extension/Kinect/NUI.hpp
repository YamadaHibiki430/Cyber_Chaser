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
	 NUI.hpp
				NUIクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_NUI
#define _INC_NUI

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Kinect.hpp"

#include <d3d9.h>

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	NUIクラス定義
//------------------------------------------------------------------------------
class CNUI {
public:
	virtual ~CNUI();

	// シングルトンインスタンスの取得
	static CNUI& GetInstance() { static CNUI theNUI; return theNUI; }

	bool Initialize(IDirect3DDevice9* pD3DDevice);
	void Release();

	inline bool Attach(IDirect3DDevice9* pD3DDevice)
	{ return Initialize(pD3DDevice); }

	IKinect* CreateKinect(const UINT  inIndex,
						  const DWORD inFlags =   NUI_INITIALIZE_FLAG_USES_COLOR
											    | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
											    | NUI_INITIALIZE_FLAG_USES_SKELETON
											  /*| NUI_INITIALIZE_FLAG_USES_AUDIO*/);
	void ReleaseKinect(IKinect*& pKinect);
	void ReleaseAllKinects();

	// リソースプロテクト設定
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	IDirect3DDevice9*      m_pD3DDevice;

	std::list<IKinect*>    m_KinectList;
	std::set <void*   >    m_ProtectedResource;		// リソースプロテクト設定

private:
	CNUI();

	template <class T> inline void SafeRelease(T& x) {if(x != NULL) { x->Release(); x = NULL; }}

	CNUI(const CNUI&);
	CNUI& operator=(const CNUI&);
};

//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
inline CNUI& Nui() { return CNUI::GetInstance(); }

#endif /* _INC_NUI */