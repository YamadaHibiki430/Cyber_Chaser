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
	 Live2D.cpp
				Live2Dクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Live2D.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#if !(defined(_DEBUG) || defined(DEBUG))
	#pragma comment(lib, "live2d_directX_mt.lib")
#else
	#pragma comment(lib, "live2d_directX_mtd.lib")
#endif

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CLive2D::CLive2D() : m_pDevice(NULL)
{
	::D3DXMatrixIdentity(&m_View);
	::D3DXMatrixIdentity(&m_Projection);
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CLive2D::~CLive2D()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CLive2D::Initialize(IDirect3DDevice9* pDevice)
{
	assert(pDevice != NULL);

	if(m_pDevice != NULL)
		Release();

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	live2d::Live2D::init();

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CLive2D::Release()
{
	::D3DXMatrixIdentity(&m_View);
	::D3DXMatrixIdentity(&m_Projection);

	m_ProtectedResource.clear();
	ReleaseAllModels();

	live2d::Live2D::dispose();

	SafeRelease(m_pDevice);
}

//------------------------------------------------------------------------------
//	モデル生成
//------------------------------------------------------------------------------
Live2DModel* CLive2D::CreateModelFromFile(const Live2DFile& inFile)
{
	Live2DModel*   pLive2DModel = new Live2DModel();

#if (defined(_DEBUG) || defined(DEBUG))
	if(m_pDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CLive2D_CreateModelFromFile)\n"));
		return pLive2DModel;
	}
#endif

	// Live2Dモデル読み込み
	live2d::Live2DModelD3D*   pModelD3D = live2d::Live2DModelD3D::loadModel(inFile.ModelFileName);
	if(pModelD3D == NULL) {
		::OutputDebugString(TEXT("*** Error - Live2Dモデル読み込み失敗(CLive2D_CreateModelFromFile)\n"));
		return pLive2DModel;
	}
	pModelD3D->setDevice(m_pDevice);

	pLive2DModel->pModel = pModelD3D;
	pLive2DModel->width  = pModelD3D->getModelImpl()->getCanvasWidth ();
	pLive2DModel->height = pModelD3D->getModelImpl()->getCanvasHeight();

	// テクスチャ読み込み
	const UINT   ARRAY_SIZE = inFile.TextureFileName.size();
	for(UINT i = 0; i < ARRAY_SIZE; i++) {
		IDirect3DTexture9*   ptexture;
		if(::D3DXCreateTextureFromFileExA(m_pDevice, inFile.TextureFileName[i].c_str(),
										  0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
										  D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
										  &ptexture)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - テクスチャ読み込み失敗(CLive2D_CreateModelFromFile)\n"));
			ptexture = NULL;
		}

		pModelD3D   ->setTexture(i, ptexture);
		pLive2DModel->pTexture.push_back(ptexture);
	}

	m_ModelList.push_back(pLive2DModel);

	return pLive2DModel;
}

//------------------------------------------------------------------------------
//	モデル解放
//------------------------------------------------------------------------------
void CLive2D::ReleaseModel(Live2DModel*& pModel)
{
	m_ProtectedResource.erase(pModel);		// プロテクト解除
	m_ModelList.remove(pModel);

	delete pModel;
	pModel = NULL;
}

//------------------------------------------------------------------------------
//	全モデル解放
//------------------------------------------------------------------------------
void CLive2D::ReleaseAllModels()
{
	std::list<Live2DModel*>::iterator   list_it = m_ModelList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_ModelList.end()) {
			delete *list_it;
			*list_it = NULL;
			list_it++;
		}
		m_ModelList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_ModelList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				*list_it = NULL;
				list_it  = m_ModelList.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	描画開始
//------------------------------------------------------------------------------
void CLive2D::Begin()
{
#if (defined(_DEBUG) || defined(DEBUG))
	if(m_pDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CLive2D_Begin)\n"));
		return;
	}
#endif

	// 行列保存
	m_pDevice->GetTransform(D3DTS_VIEW, &m_View);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &m_Projection);

	// 行列設定
	D3DXMATRIXA16   view;
	::D3DXMatrixIdentity(&view);
	m_pDevice->SetTransform(D3DTS_VIEW, &view);
}

//------------------------------------------------------------------------------
//	描画終了
//------------------------------------------------------------------------------
void CLive2D::End()
{
#if (defined(_DEBUG) || defined(DEBUG))
	if(m_pDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9未初期化(CLive2D_End)\n"));
		return;
	}
#endif

	// 行列復元
	m_pDevice->SetTransform(D3DTS_VIEW, &m_View);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_Projection);
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CLive2D::Draw(Live2DModel& inLive2DModel)
{
	if(inLive2DModel.pModel == NULL)
		return;

	// ビューポート取得
	D3DVIEWPORT9   view;
	m_pDevice->GetViewport(&view);

	// 射影行列設定
	D3DXMATRIX   ortho;
	::D3DXMatrixOrthoLH(&ortho, inLive2DModel.height,
						-inLive2DModel.height * (float)view.Height / (float)view.Width,
						-1.0f, 1.0f);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &ortho);


	// ワールド行列生成
	D3DXMATRIXA16   world;

	// 回転
	::D3DXMatrixRotationZ(&world, D3DXToRadian(inLive2DModel.rotationZ));

	// スケーリング
	world._11 *= inLive2DModel.scale.x;
	world._12 *= inLive2DModel.scale.x;
	world._13 *= inLive2DModel.scale.x;

	world._21 *= inLive2DModel.scale.y;
	world._22 *= inLive2DModel.scale.y;
	world._23 *= inLive2DModel.scale.y;

	world._31 *= inLive2DModel.scale.z;
	world._32 *= inLive2DModel.scale.z;
	world._33 *= inLive2DModel.scale.z;

	// 移動
	world._41  = inLive2DModel.position.x;
	world._42  = inLive2DModel.position.y;
	world._43  = inLive2DModel.position.z;

	world._44  = 1.0f;

	m_pDevice->SetTransform(D3DTS_WORLD, &world);

	inLive2DModel.pModel->update();
	inLive2DModel.pModel->draw();
}

//------------------------------------------------------------------------------
//	リソースプロテクト設定
//------------------------------------------------------------------------------
void CLive2D::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}
