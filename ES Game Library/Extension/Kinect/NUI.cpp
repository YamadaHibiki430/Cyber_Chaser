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
	 NUI.cpp
				NUIクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "NUI.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "Kinect10.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CNUI::CNUI() : m_pD3DDevice(NULL)
{
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CNUI::~CNUI()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CNUI::Initialize(IDirect3DDevice9* pD3DDevice)
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
void CNUI::Release()
{
	m_ProtectedResource.clear();

	ReleaseAllKinects();
	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	Kinect生成
//------------------------------------------------------------------------------
IKinect* CNUI::CreateKinect(const UINT inIndex, const DWORD inFlags)
{
	IKinect*      pKinect;
	INuiSensor*   pSensor = NULL;

	try {
		// Kinect生成
		if(::NuiCreateSensorByIndex(inIndex, &pSensor) != S_OK)
			throw TEXT("Kinect生成失敗");

		// Kinect初期化
		if(pSensor->NuiInitialize(inFlags) != S_OK)
			throw TEXT("Kinect初期化失敗");

		// Kinectインスタンス生成
		pKinect = new CKinect(pSensor, m_pD3DDevice);
	} catch(LPCTSTR err_str) {
		// エラーメッセージ生成
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CNUI_CreateKinect)\n"), err_str);
		::OutputDebugString(msg);

		// 例外が発生した場合は、NULLオブジェクトを生成
		pKinect = new CNullKinect();
	}
	SafeRelease(pSensor);

	// リストへ追加
	m_KinectList.push_back(pKinect);

	return pKinect;
}

//------------------------------------------------------------------------------
//	Kinect解放
//------------------------------------------------------------------------------
void CNUI::ReleaseKinect(IKinect*& pKinect)
{
	m_ProtectedResource.erase(pKinect);		// プロテクト解除

	m_KinectList.remove(pKinect);
	delete pKinect;
	pKinect = NULL;
}

//------------------------------------------------------------------------------
//	全Kinect解放
//------------------------------------------------------------------------------
void CNUI::ReleaseAllKinects()
{
	std::list<IKinect*>::iterator   list_it = m_KinectList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_KinectList.end()) {
			delete *list_it;
			list_it++;
		}
		m_KinectList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_KinectList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_KinectList.erase(list_it);
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
void CNUI::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}
