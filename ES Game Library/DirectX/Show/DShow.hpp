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
	 DShow.hpp
				DirectShowクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Media.hpp"

#include <d3d9.h>

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	DirectShowクラス定義
//------------------------------------------------------------------------------
class CDShow {
public:
	virtual ~CDShow();

	// シングルトンインスタンスの取得
	static CDShow& GetInstance() { static CDShow theDShow; return theDShow; }

	bool Initialize(IDirect3DDevice9* pD3DDevice);
	void Release();

	inline bool Attach(IDirect3DDevice9* pD3DDevice)
	{ return Initialize(pD3DDevice); }

	void Suspend();
	void Resume ();

	IMedia* CreateMediaFromFile(LPTSTR inFileName);

	void ReleaseMedia(IMedia*& pMedia);
	void ReleaseAllMedia();

	// リソースプロテクト設定
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	CDShow();

	IDirect3DDevice9*    m_pD3DDevice;

	std::list<IMedia*>   m_MediaList;
	std::set <void*  >   m_ProtectedResource;	// リソースプロテクト設定

	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	CDShow(const CDShow&);
	CDShow& operator=(const CDShow&);
};

//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
inline CDShow& DShow() { return CDShow::GetInstance(); }
