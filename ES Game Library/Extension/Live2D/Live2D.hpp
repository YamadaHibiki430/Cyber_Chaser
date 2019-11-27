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
	 Live2D.hpp
				Live2Dクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

/*
//------------------------------------------------------------------------------
//	パラメーターID一覧
//------------------------------------------------------------------------------
"PARAM_ANGLE_X"			// 角度 X
"PARAM_ANGLE_Y"			// 角度 Y
"PARAM_ANGLE_Z"			// 角度 Z
"PARAM_EYE_L_OPEN"		// 左眼 開閉
"PARAM_EYE_L_SMILE"		// 左眼 笑顔
"PARAM_EYE_R_OPEN"		// 右眼 開閉
"PARAM_EYE_R_SMILE"		// 右眼 笑顔
"PARAM_EYE_FORM"		// 眼 変形
"PARAM_EYE_BALL_X"		// 目玉 X
"PARAM_EYE_BALL_Y"		// 目玉 Y
"PARAM_EYE_BALL_FORM"	// 目玉 収縮
"PARAM_BROW_L_Y"		// 左眉 上下
"PARAM_BROW_R_Y"		// 右眉 上下
"PARAM_BROW_L_X"		// 左眉 左右
"PARAM_BROW_R_X"		// 右眉 左右
"PARAM_BROW_L_ANGLE"	// 左眉 角度
"PARAM_BROW_R_ANGLE"	// 右眉 角度
"PARAM_BROW_L_FORM"		// 左眉 変形
"PARAM_BROW_R_FORM"		// 右眉 変形
"PARAM_MOUTH_FORM"		// 口 変形
"PARAM_MOUTH_OPEN_Y"	// 口 開閉
"PARAM_CHEEK"			// 照れ
"PARAM_BODY_ANGLE_X"	// 体の回転 X
"PARAM_BODY_ANGLE_Y"	// 体の回転 X
"PARAM_BODY_ANGLE_Z"	// 体の回転 Z
"PARAM_BREATH"			// 呼吸
"PARAM_ARM_L_A"			// 左腕 A
"PARAM_ARM_R_A"			// 右腕 A
"PARAM_ARM_L_A"			// 左腕 B
"PARAM_ARM_R_A"			// 右腕 B 
"PARAM_BUST_Y"			// 胸 揺れ   
"PARAM_HAIR_FRONT"		// 髪揺れ 前
"PARAM_HAIR_BACK"		// 髪揺れ 後
*/

#ifndef _INC_LIVE2D
#define _INC_LIVE2D

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	宣言
//------------------------------------------------------------------------------
#define L2D_TARGET_D3D

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <Live2D.h>
#include <Live2DModelD3D.h>
#include <..\..\DirectX\Include\d3dx9.h>

#include <list>
#include <set>
#include <vector>

//------------------------------------------------------------------------------
//	LIVE2DFILE構造体定義
//------------------------------------------------------------------------------
struct Live2DFile {
	char*                                    ModelFileName;
	std::vector< std::basic_string<char> >   TextureFileName;
};

//------------------------------------------------------------------------------
//	LIVE2DMODEL構造体定義
//------------------------------------------------------------------------------
struct Live2DModel {
friend class CLive2D;

private:
	live2d::Live2DModelD3D*           pModel;
	std::vector<IDirect3DTexture9*>   pTexture;

	template <class T>
	inline void SafeRelease(T& x)
	{
		if(x != NULL) {
			x->Release();
			x = NULL;
		}
	}

public:
	D3DXVECTOR3   position;
	D3DXVECTOR3   scale;
	float         rotationZ;

	float         width;
	float         height;

	Live2DModel()
	{
		pModel     = NULL;

		position.x = 0.0f;
		position.y = 0.0f;
		position.z = 0.0f;

		scale   .x = 1.0f;
		scale   .y = 1.0f;
		scale   .z = 1.0f;

		rotationZ  = 0.0f;
	}

	virtual ~Live2DModel()
	{
		const UINT   ARRAY_SIZE = pTexture.size();
		for(UINT i = 0; i < ARRAY_SIZE; i++) {
			if(pTexture[i] != NULL)
				pTexture[i]->Release();
		}

		delete pModel;
	}

	inline void SetScale(const float inScale)
	{
		scale.x = inScale;
		scale.y = inScale;
		scale.z = 1.0f;
	}

//	Live2DModel& operator=(live2d::Live2DModelD3D* pModel)
//	{
//		this->pModel = pModel;
//
//		width  = pModel->getModelImpl()->getCanvasWidth ();
//		height = pModel->getModelImpl()->getCanvasHeight();
//
//		return *this;
//	}

	inline operator live2d::Live2DModelD3D*  () const { return pModel; }
//	inline Live2DModel&            operator* ()       { return *this;  }
	inline live2d::Live2DModelD3D* operator->()       { return pModel; }
};

//------------------------------------------------------------------------------
//	Live2Dクラス定義
//------------------------------------------------------------------------------
class CLive2D {
public:
	virtual ~CLive2D();

	// シングルトンインスタンスの取得
	static CLive2D& GetInstance()
	{
		static CLive2D   theLive2D;
		return theLive2D;
	}

	// 初期化・解放
	bool Initialize(IDirect3DDevice9* pDevice);
	inline bool Attach(IDirect3DDevice9* pDevice) { return Initialize(pDevice); }

	void Release();

	// モデル生成・解放
	Live2DModel* CreateModelFromFile(const Live2DFile& inFile);

	void ReleaseModel(Live2DModel*& inLive2DModel);
	void ReleaseAllModels();

	// 描画
	void Begin();
	void End();

	void Draw(Live2DModel& inLive2DModel);
	inline void Draw(Live2DModel* pLive2DModel) { Draw(*pLive2DModel); }

	// プロテクト設定
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	CLive2D();

	template <class T>
	inline void SafeRelease(T& x)
	{
		if(x != NULL) {
			x->Release();
			x = NULL;
		}
	}

	CLive2D(const CLive2D&) = delete;
	CLive2D& operator=(const CLive2D&) = delete;

private:
	IDirect3DDevice9*         m_pDevice;
	std::list<Live2DModel*>   m_ModelList;

	D3DXMATRIXA16             m_View;
	D3DXMATRIXA16             m_Projection;

	std::set<void*>           m_ProtectedResource;
};

//------------------------------------------------------------------------------
//	マクロ定義
//------------------------------------------------------------------------------
inline CLive2D& LIVE2D() { return CLive2D::GetInstance(); }

#endif /* _INC_LIVE2D */