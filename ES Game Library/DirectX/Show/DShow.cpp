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
	 DShow.cpp
				DirectShowクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DShow.hpp"
#include "../Common/ComLibrary.hpp"

#include <cassert>

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CDShow::CDShow() : m_pD3DDevice(NULL)
{
	CComLibrary::Initialize();		// COMライブラリ初期化
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CDShow::~CDShow()
{
	Release();
	CComLibrary::Uninitialize();	// COMライブラリ解放
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CDShow::Initialize(IDirect3DDevice9* pD3DDevice)
{
	assert(pD3DDevice != NULL);

	if(m_pD3DDevice == pD3DDevice)
		return true;

	Release();
	m_pD3DDevice = pD3DDevice;
	m_pD3DDevice->AddRef();

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CDShow::Release()
{
	m_ProtectedResource.clear();
	ReleaseAllMedia();
	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	サスペンド処理
//------------------------------------------------------------------------------
void CDShow::Suspend()
{
	if(m_pD3DDevice == NULL)
		return;

	for(std::list<IMedia*>::iterator it = m_MediaList.begin(); it != m_MediaList.end(); it++)
		(*it)->Suspend();
}

//------------------------------------------------------------------------------
//	レジューム処理
//------------------------------------------------------------------------------
void CDShow::Resume()
{
	for(std::list<IMedia*>::iterator it = m_MediaList.begin(); it != m_MediaList.end(); it++)
		(*it)->Resume();
}

//------------------------------------------------------------------------------
//	メディア生成
//------------------------------------------------------------------------------
IMedia* CDShow::CreateMediaFromFile(LPTSTR inFileName)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CDShow_CreateMediaFromFile)\n"));
		return NULL;
	}
#endif

	CMedia*   pMediaLoader = new CMedia(m_pD3DDevice);
	if(pMediaLoader == NULL) {
		::OutputDebugString(TEXT("*** Error - メディアインスタンス生成失敗(CDShow_CreateMediaFromFile)\n"));
		return NULL;
	}

	// ファイル読み込み
	IMedia*   pMedia = pMediaLoader;
	if(pMediaLoader->LoadFromFile(inFileName) == false) {
		::OutputDebugString(TEXT("*** Error - ファイル読み込み失敗(CDShow_CreateMediaFromFile)\n"));
		delete pMediaLoader;
		pMedia = new CNullMedia();
	}
	m_MediaList.push_back(pMedia);

	return pMedia;
}

//------------------------------------------------------------------------------
//	メディア解放
//------------------------------------------------------------------------------
void CDShow::ReleaseMedia(IMedia*& pMedia)
{
	m_ProtectedResource.erase(pMedia);		// プロテクト解除

	m_MediaList.remove(pMedia);
	delete pMedia;
	pMedia = NULL;
}

//------------------------------------------------------------------------------
//	全メディア解放
//------------------------------------------------------------------------------
void CDShow::ReleaseAllMedia()
{
	std::list<IMedia*>::iterator   list_it = m_MediaList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_MediaList.end()) {
			delete *list_it;
			list_it++;
		}
		m_MediaList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_MediaList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_MediaList.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	リソースプロテクト設定
//------------------------------------------------------------------------------
void CDShow::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}
