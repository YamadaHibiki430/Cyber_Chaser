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
	 DXGTypes.h
				DirectX Graphics型定義

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <d3dx9.h>

//------------------------------------------------------------------------------
//	定数
//------------------------------------------------------------------------------
// FVFフラグ
enum { DXGFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1,
	   DXGFVF_LVERTEX  = D3DFVF_XYZ    | D3DFVF_DIFFUSE | D3DFVF_TEX1,
	   DXGFVF_VERTEX   = D3DFVF_XYZ    | D3DFVF_NORMAL  | D3DFVF_TEX1 };

// ブレンディングモード
enum DXGBLENDMODE {
	DXGBLEND_MODULATE,		// 乗算合成
	DXGBLEND_ADD,			// 加算合成
	DXGBLEND_SUBTRACT,		// 減算合成
	DXGBLEND_SRCCOLOR,
	DXGBLEND_ZEROCOLOR,
	DXGBLEND_NORMAL = DXGBLEND_MODULATE,
	DXGBLEND_NONE   = -1	// ブレンディングなし
};

//------------------------------------------------------------------------------
//	構造体定義
//------------------------------------------------------------------------------
// トランスフォーム済みライティング済みの頂点
struct DXGTLVERTEX {
	float      x, y, z;			// 頂点座標
	float      rhw;				// RHW
	D3DCOLOR   color;			// 頂点色
	float      tu, tv;			// テクスチャ座標

	DXGTLVERTEX() : x(0.0f), y(0.0f), z(0.0f), rhw(1.0f), color(0), tu(0.0f), tv(0.0f) {}
	static inline DWORD FVF() { return DXGFVF_TLVERTEX; }
};

// 未トランスフォームライティング済みの頂点
struct DXGLVERTEX {
	float      x, y, z;			// 頂点座標
	D3DCOLOR   color;			// 頂点色
	float      tu, tv;			// テクスチャ座標

	DXGLVERTEX() : x(0.0f), y(0.0f), z(0.0f), color(0), tu(0.0f), tv(0.0f) {}
	static inline DWORD FVF() { return DXGFVF_LVERTEX; }
};

// 未トランスフォーム未ライティングの頂点
struct DXGVERTEX {
	float      x, y, z;			// 頂点座標
	float      nx, ny, nz;		// 法線ベクトル
	float      tu, tv;			// テクスチャ座標

	DXGVERTEX() : x(0.0f), y(0.0f), z(0.0f), nx(0.0f), ny(0.0f), nz(0.0f), tu(0.0f), tv(0.0f) {}
	static inline DWORD FVF() { return DXGFVF_VERTEX; }
};

//------------------------------------------------------------------------------
//	列挙体定義
//------------------------------------------------------------------------------
// 法線計算
enum DXGCOMPUTENORMAL {
	DXGCOMPUTE_DONOT_INPLACE = -1,	// 入力モデルをそのまま使用
	DXGCOMPUTE_NONE          =  0,	// なし
	DXGCOMPUTE_NORMAL        =  1,	// 法線
	DXGCOMPUTE_TANGENT       =  2,	// 接線
	DXGCOMPUTE_BINORMAL      =  4,	// 従法線
	DXGCOMPUTE_NORMALTANGENT         = DXGCOMPUTE_NORMAL  | DXGCOMPUTE_TANGENT,
	DXGCOMPUTE_NORMALBINORMAL        = DXGCOMPUTE_NORMAL  | DXGCOMPUTE_BINORMAL,
	DXGCOMPUTE_TANGENTBINORMAL       = DXGCOMPUTE_TANGENT | DXGCOMPUTE_BINORMAL,
	DXGCOMPUTE_NORMALTANGENTBINORMAL = DXGCOMPUTE_NORMAL  | DXGCOMPUTE_TANGENT | DXGCOMPUTE_BINORMAL,
	DXGCOMPUTE_ALL                   = DXGCOMPUTE_NORMALTANGENTBINORMAL
};

// シェイプタイプ
enum DXGSHAPETYPE {
	DXGSHAPE_BOX = 1,		// 立方体
	DXGSHAPE_CYLINDER,		// 円柱、円錐
	DXGSHAPE_POLYGON,		// 多角形
	DXGSHAPE_SPHERE,		// 球
	DXGSHAPE_TORUS,			// トーラス
	DXGSHAPE_TEAPOT			// ティーポット
};

// アルファブレンド描画フラグ
enum DXGDRAWALPHA {
	// 描画フラグ
	// 表面
	DXGDRAWA_FACE                = 0x0008,	// 表面描画
	DXGDRAWA_FACE_ZDISABLE       = 0x0004,	// ｚなし　　(ｚ比較なし)
	DXGDRAWA_FACE_ZWRDISABLE     = 0x0002,	// ｚ書込なし(ｚ比較あり)
	DXGDRAWA_FACE_ZWRDEFER       = 0x0001,	// ｚ遅延書込
	// 背面
	DXGDRAWA_BACKFACE            = 0x0080,
	DXGDRAWA_BACKFACE_ZDISABLE   = 0x0040,
	DXGDRAWA_BACKFACE_ZWRDISABLE = 0x0020,
	DXGDRAWA_BACKFACE_ZWRDEFER   = 0x0010,
	// カリングなし
	DXGDRAWA_CULLNONE            = 0x0800,
	DXGDRAWA_CULLNONE_ZDISABLE   = 0x0400,
	DXGDRAWA_CULLNONE_ZWRDISABLE = 0x0200,
	DXGDRAWA_CULLNONE_ZWRDEFER   = 0x0100,
	// ｚ出力
	DXGDRAWA_ZWRITE_FACE         = 0x1000,	// 最後に表面のｚ出力
	DXGDRAWA_ZWRITE_BACKFACE     = 0x2000,	// 最後に裏面のｚ出力
	DXGDRAWA_ZWRITE_CULLNONE     = 0x3000,	// 最後に両面のｚ出力
	// 複合フラグ
	DXGDRAWA_FACE2               = DXGDRAWA_FACE      | DXGDRAWA_FACE_ZWRDEFER,
	DXGDRAWA_FACE3               = DXGDRAWA_FACE2     | DXGDRAWA_FACE_ZDISABLE,
	DXGDRAWA_BACKFACE2           = DXGDRAWA_BACKFACE  | DXGDRAWA_BACKFACE_ZWRDEFER,
	DXGDRAWA_BACKFACE3           = DXGDRAWA_BACKFACE2 | DXGDRAWA_BACKFACE_ZDISABLE,
	DXGDRAWA_CULLNONE2           = DXGDRAWA_CULLNONE  | DXGDRAWA_CULLNONE_ZWRDEFER,
	DXGDRAWA_CULLNONE3           = DXGDRAWA_CULLNONE2 | DXGDRAWA_CULLNONE_ZDISABLE,
	DXGDRAWA_BOTHFACES           = DXGDRAWA_FACE      | DXGDRAWA_BACKFACE,
	DXGDRAWA_BOTHFACES2          = DXGDRAWA_FACE      | DXGDRAWA_FACE_ZWRDISABLE     |
								   DXGDRAWA_BACKFACE  | DXGDRAWA_BACKFACE_ZWRDISABLE | DXGDRAWA_ZWRITE_CULLNONE,
	DXGDRAWA_BOTHFACES3          = DXGDRAWA_FACE      | DXGDRAWA_BACKFACE3,
	DXGDRAWA_ALLFACES            = DXGDRAWA_CULLNONE2 | DXGDRAWA_CULLNONE_ZDISABLE,
};

//------------------------------------------------------------------------------
//	構造体定義
//------------------------------------------------------------------------------
// シンプルシェイプ構造体
struct DXGSIMPLESHAPE {
	DXGSHAPETYPE   Type;			// 形状
	float		   Width;			// 立方体の幅
	float		   Height;			// 立方体の高さ
	union{
		float	   Depth;			// 立方体の奥行き
		float	   Length;			// 円柱、多角形の辺の長さ
	};
	union{
		float	   Radius;			// 球の半径
		float	   Radius1;			// 円柱の先頭の半径
		float	   InnerRadius;		// トーラスの内側の半径
	};
	union{
		float	   Radius2;			// 円柱の後尾の半径
		float	   OuterRadius;		// トーラスの外側の半径
	};
	union{
		UINT	   Sides;			// 多角形、トーラスの横断面の辺の数
		UINT	   Slices;			// 円柱、球の横断面の辺の数
	};
	union{
		UINT	   Stacks;			// 横断面の数
		UINT	   Rings;			// トーラスを構成する環の数
	};
};

// ロック情報構造体
struct DXGLOCKED_DESC {
	D3DFORMAT   Format;
    UINT        Width;
    UINT        Height;
	int         Pitch;
	void*       pBits;
};

// レイ
struct DXGRAY {
	D3DXVECTOR3   Position;
	D3DXVECTOR3   Direction;

	DXGRAY() : Position(0.0f, 0.0f, 0.0f), Direction(0.0f, 0.0f, 0.0f) {}
	DXGRAY(const D3DXVECTOR3& pos, const D3DXVECTOR3& dir) : Position(pos), Direction(dir) {}
};

// 境界球
struct DXGBOUNDINGSPHERE {
	D3DXVECTOR3   Center;
	float         Radius;

	DXGBOUNDINGSPHERE() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	DXGBOUNDINGSPHERE(const D3DXVECTOR3& center, const float radius)
		: Center(center), Radius(radius) {}
};

// OBB(Oriented Bounding Box)
struct DXGOBB {
	D3DXVECTOR3   Center;
	D3DXVECTOR3   Axis[3];
	D3DXVECTOR3   Radius;

	DXGOBB() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f, 0.0f, 0.0f)
	{
		Axis[0] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		Axis[1] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Axis[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}

	DXGOBB(const DXGOBB& obb) : Center(obb.Center), Radius(obb.Radius)
	{
		Axis[0] = obb.Axis[0];
		Axis[1] = obb.Axis[1];
		Axis[2] = obb.Axis[2];
	}

	inline DXGOBB& operator =(const DXGOBB& obb)
	{
		Center  = obb.Center;

		Axis[0] = obb.Axis[0];
		Axis[1] = obb.Axis[1];
		Axis[2] = obb.Axis[2];

		Radius  = obb.Radius;

		return *this;
	}
};

// 視錐台
struct DXGVIEWFRUSTUM {
	D3DXPLANE   Left;
	D3DXPLANE   Right;
	D3DXPLANE   Top;
	D3DXPLANE   Bottom;
	D3DXPLANE   Near;
	D3DXPLANE   Far;
};
