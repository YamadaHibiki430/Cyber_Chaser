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
	 Model.hpp
				モデルクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DXGTypes.h"
#include "Effect.hpp"

#include <vector>
#include <list>
#include <map>

#include <btBulletDynamicsCommon.h>

//------------------------------------------------------------------------------
//	モデルインタフェース定義
//------------------------------------------------------------------------------
class IModel {
public:
	virtual ~IModel() {}

	virtual bool Optimize() = 0;

	virtual void Draw() = 0;
	virtual void DrawPure() = 0;

	virtual void Draw(IEffect*& pEffect) = 0;
	virtual void Draw(IEffect*& pEffect, const UINT inPass) = 0;

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass) = 0;

//	virtual void Draw(IEffect* EffectArray[]) = 0;

	virtual void DrawAlpha (const float inAlpha, const DWORD inDraw = DXGDRAWA_BOTHFACES) = 0;
	virtual void DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth) = 0;

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial, const DWORD inMeshIdx) = 0;
	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) = 0;

	virtual void SetTexture    (LPCTSTR inFileName, const DWORD inMeshIdx = 0) = 0;
	virtual void SetCubeTexture(LPCTSTR inFileName, const DWORD inMeshIdx = 0) = 0;
	virtual void SetTexture(IDirect3DBaseTexture9* pTexture, const DWORD inMeshIdx = 0) = 0;

	virtual void SetPosition (const float x, const float y, const float z) = 0;
	virtual void SetScale    (const float x, const float y, const float z) = 0;
	virtual void SetDirection(const float x, const float y, const float z) = 0;
	virtual void SetDirection(const D3DXQUATERNION& inDirection) = 0;

	inline  void SetPosition (const D3DXVECTOR3& inPosition ) { SetPosition (inPosition .x, inPosition .y, inPosition .z); }
	inline  void SetScale    (const D3DXVECTOR3& inScale)     { SetScale    (inScale    .x, inScale    .y, inScale    .z); }
	inline  void SetScale    (const float s)                  { SetScale    (s, s, s); }
	inline  void SetDirection(const D3DXVECTOR3& inDirection) { SetDirection(inDirection.x, inDirection.y, inDirection.z); }

	inline  void SetRotation (const float x, const float y, const float z)
	{ SetDirection(x, y, z); }
	inline  void SetRotation (const D3DXVECTOR3& inDirection)
	{ SetDirection(inDirection.x, inDirection.y, inDirection.z); }

	virtual void Move     (const float inRight, const float inUp, const float inFront) = 0;
	virtual void Rotation (const float inX,     const float inY,  const float inZ)     = 0;

	inline  void Move     (const D3DXVECTOR3& inDistance) { Move    (inDistance.x, inDistance.y, inDistance.z); }
	inline  void Rotation (const D3DXVECTOR3& inRotation) { Rotation(inRotation.x, inRotation.y, inRotation.z); }

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle) = 0;
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront) = 0;

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY) = 0;

	virtual DWORD          GetMeshes  () const = 0;

	virtual D3DXVECTOR3	   GetPosition() const = 0;
	virtual D3DXVECTOR3	   GetScale   () const = 0;

	virtual D3DXVECTOR3    GetDirection() const = 0;
	virtual D3DXVECTOR3    GetDirectionRadian() const = 0;
	virtual D3DXQUATERNION GetDirectionQuaternion() const = 0;

	inline  D3DXVECTOR3    GetRotation() const { return GetDirection(); }

	virtual D3DXMATRIX     GetWorldMatrix() const = 0;
	virtual D3DXVECTOR3    GetFrontVector() const = 0;
	virtual D3DXVECTOR3    GetRightVector() const = 0;
	virtual D3DXVECTOR3    GetUpVector   () const = 0;

	virtual D3DMATERIAL9 GetMaterial(const DWORD inIdx = 0) const = 0;
	virtual IDirect3DBaseTexture9* GetTexture(const DWORD inIdx = 0) const = 0;

	virtual BOOL IntersectRay(D3DXVECTOR3& inPos,  D3DXVECTOR3& inDir,
							  float* pDist = NULL, D3DXVECTOR3* pNormal = NULL) = 0;
	inline  BOOL IntersectRay(DXGRAY& inRay, float* pDist = NULL, D3DXVECTOR3* pNormal = NULL)
	{ return IntersectRay(inRay.Position, inRay.Direction, pDist, pNormal); }

	virtual DXGBOUNDINGSPHERE GetBoundingSphere()     = 0;
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere() = 0;

	virtual DXGOBB GetOBB()     = 0;
	virtual DXGOBB GetBaseOBB() = 0;

	virtual DWORD GetOcclusionQuery() = 0;
	virtual DWORD GetOcclusionQuery(IEffect*& pEffect) = 0;

	virtual bool EnableProgressive() = 0;
	virtual void SetLOD(const float inLOD) = 0;

	virtual void LoadEffectMap    (const DWORD inStage, LPCTSTR inFileName) = 0;
	virtual void LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName) = 0;
	virtual void ApplyEffectMap()  = 0;
	virtual void RemoveEffectMap() = 0;

#ifdef BULLET_DYNAMICS_COMMON_H
	virtual btTransform btGetTransform() = 0;
	virtual void btSetTransform(const btTransform& inTransform) = 0;

	inline  void btSetTransform(btMotionState* pMotion)
	{
		btTransform   transform;
		pMotion->getWorldTransform(transform);
		btSetTransform(transform);
	}
#endif
};

//------------------------------------------------------------------------------
//	モデルクラス定義
//------------------------------------------------------------------------------
class CModel : public IModel {
public:
	CModel(ID3DXMesh* pMesh, const DWORD inMeshes = 1);
	virtual ~CModel();

	virtual bool Optimize();

	virtual void Draw();
	virtual void DrawPure();

	virtual void Draw(IEffect*& pEffect);
	virtual void Draw(IEffect*& pEffect, const UINT inPass);

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass);

//	virtual void Draw(IEffect* EffectArray[]);

	virtual void DrawAlpha (const float inAlpha, const DWORD inDraw);
	virtual void DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth);

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial, const DWORD inMeshIdx);
	virtual void SetMaterial(const D3DMATERIAL9& inMaterial);

	virtual void SetTexture    (LPCTSTR inFileName, const DWORD inMeshIdx);
	virtual void SetCubeTexture(LPCTSTR inFileName, const DWORD inMeshIdx);
	virtual void SetTexture(IDirect3DBaseTexture9* pTexture, const DWORD inMeshIdx);

	virtual void SetPosition(const float x, const float y, const float z)
	{ m_Position.x = x; m_Position.y = y; m_Position.z = z; }

	virtual void SetScale(const float x, const float y, const float z)
	{ m_Scale.x = x; m_Scale.y = y; m_Scale.z = z; }

	virtual void SetDirection(const float x, const float y, const float z);

	virtual void SetDirection(const D3DXQUATERNION& inDirection)
	{ m_Rotation = inDirection; }

	virtual void Move    (const float inRight, const float inUp, const float inFront);
	virtual void Rotation(const float inX,     const float inY,  const float inZ);

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle);
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront);

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY);

	virtual DWORD GetMeshes() const { return m_Meshes; }

	virtual D3DXVECTOR3	   GetPosition() const { return m_Position; }
	virtual D3DXVECTOR3	   GetScale   () const { return m_Scale;    }

	virtual D3DXVECTOR3    GetDirection() const;
	virtual D3DXVECTOR3    GetDirectionRadian() const;
	virtual D3DXQUATERNION GetDirectionQuaternion() const { return m_Rotation; }

	virtual D3DXMATRIX     GetWorldMatrix() const;
	virtual D3DXVECTOR3    GetFrontVector() const;
	virtual D3DXVECTOR3    GetRightVector() const;
	virtual D3DXVECTOR3    GetUpVector   () const;

	virtual D3DMATERIAL9 GetMaterial(const DWORD inIdx) const { return m_Material[inIdx]; }
	virtual IDirect3DBaseTexture9* GetTexture(const DWORD inIdx) const { return m_Texture[inIdx]; }

	virtual BOOL IntersectRay(D3DXVECTOR3& inPos, D3DXVECTOR3& inDir, float* pDist, D3DXVECTOR3* pNormal);

	virtual DXGBOUNDINGSPHERE GetBoundingSphere();
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere();

	virtual DXGOBB GetOBB();
	virtual DXGOBB GetBaseOBB();

	virtual DWORD GetOcclusionQuery();
	virtual DWORD GetOcclusionQuery(IEffect*& pEffect);

	virtual bool EnableProgressive();
	virtual void SetLOD(const float inLOD);

	virtual void LoadEffectMap    (const DWORD inStage, LPCTSTR inFileName);
	virtual void LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName);
	virtual void ApplyEffectMap();
	virtual void RemoveEffectMap();

#ifdef BULLET_DYNAMICS_COMMON_H
	virtual btTransform btGetTransform()
	{
		btQuaternion   qt (m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);
		btVector3      pos(m_Position.x, m_Position.y, m_Position.z);
		return btTransform(qt, pos);
	}

	virtual void btSetTransform(const btTransform& inTransform)
	{
		btVector3     pos = inTransform.getOrigin();
		m_Position.x = pos.getX();
		m_Position.y = pos.getY();
		m_Position.z = pos.getZ();

		btQuaternion  qt = inTransform.getRotation();
		m_Rotation.x = qt.getX();
		m_Rotation.y = qt.getY();
		m_Rotation.z = qt.getZ();
		m_Rotation.w = qt.getW();
	}
#endif

protected:
	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; } }

private:
	ID3DXBaseMesh*                        m_pBaseMesh;			// ベースメッシュ
	ID3DXMesh*                            m_pMesh;				// メッシュ
	ID3DXPMesh*                           m_pPMesh;				// プログレッシブメッシュ

	IDirect3DDevice9*                     m_pD3DDevice;

	DWORD                                 m_Meshes;				// サブセット数

	std::vector<D3DMATERIAL9>             m_Material;			// マテリアル
	std::vector<IDirect3DBaseTexture9*>   m_Texture;			// テクスチャ

	D3DXVECTOR3                           m_Position;			// 位置
	D3DXVECTOR3                           m_Scale;				// 拡大率
	D3DXQUATERNION                        m_Rotation;			// 回転クォータニオン

	// エフェクトマッピング
	struct EFFECTMAP {
		DWORD                    Stage;
		IDirect3DBaseTexture9*   pTexture;

		EFFECTMAP() : Stage(0), pTexture(NULL) {}
		EFFECTMAP(const DWORD stage, IDirect3DBaseTexture9* texture)
			: Stage(stage), pTexture(texture) {}
	};
	std::list<EFFECTMAP>   m_EffectMap;
};

//------------------------------------------------------------------------------
//	NULLモデルクラス定義
//------------------------------------------------------------------------------
class CNullModel : public IModel {
public:
	virtual ~CNullModel() {}

	virtual bool Optimize() { return false; }

	virtual void Draw() {}
	virtual void DrawPure() {}

	virtual void Draw(IEffect*& pEffect) {}
	virtual void Draw(IEffect*& pEffect, const UINT inPass) {}

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass) {}

//	virtual void Draw(IEffect* EffectArray[]) {}

	virtual void DrawAlpha (const float inAlpha, const DWORD inDraw) {}
	virtual void DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth) {}

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial, const DWORD inMeshIdx) {}
	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) {}

	virtual void SetTexture    (LPCTSTR inFileName, const DWORD inMeshIdx) {}
	virtual void SetCubeTexture(LPCTSTR inFileName, const DWORD inMeshIdx) {}
	virtual void SetTexture(IDirect3DBaseTexture9* pTexture, const DWORD inMeshIdx) {}

	virtual void SetPosition (const float x, const float y, const float z) {}
	virtual void SetScale    (const float x, const float y, const float z) {}
	virtual void SetDirection(const float x, const float y, const float z) {}
	virtual void SetDirection(const D3DXQUATERNION& inRotation) {}

	virtual void Move    (const float inRight, const float inUp, const float inFront) {}
	virtual void Rotation(const float inX,     const float inY,  const float inZ)     {}

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle) {}
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront) {}

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY) {}

	virtual DWORD GetMeshes() const { return 0; }

	virtual D3DXVECTOR3	GetPosition() const { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3	GetScale   () const { return D3DXVECTOR3(1.0f, 1.0f, 1.0f); }

	virtual D3DXVECTOR3    GetDirection()           const { return D3DXVECTOR3   (0.0f, 0.0f, 0.0f);       }
	virtual D3DXVECTOR3    GetDirectionRadian()     const { return D3DXVECTOR3   (0.0f, 0.0f, 0.0f);       }
	virtual D3DXQUATERNION GetDirectionQuaternion() const { return D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f); }

	virtual D3DXMATRIX GetWorldMatrix() const
	{
		D3DXMATRIX   world;
		::D3DXMatrixIdentity(&world);
		return world;
	}

	virtual D3DXVECTOR3 GetFrontVector() const { return D3DXVECTOR3(0.0f, 0.0f, 1.0f); }
	virtual D3DXVECTOR3 GetRightVector() const { return D3DXVECTOR3(1.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3 GetUpVector()    const { return D3DXVECTOR3(0.0f, 1.0f, 0.0f); }

	virtual D3DMATERIAL9 GetMaterial(const DWORD inIdx) const 
	{
		D3DMATERIAL9   mtrl;
		::ZeroMemory(&mtrl, sizeof(mtrl));
		return mtrl;
	}

	virtual IDirect3DBaseTexture9* GetTexture(const DWORD inIdx) const { return NULL; }

	virtual BOOL IntersectRay(D3DXVECTOR3& inPos, D3DXVECTOR3& inDir, float* pDist, D3DXVECTOR3* pNormal)
	{
		if(pDist   != NULL)
			*pDist   = 0.0f;
		if(pNormal != NULL)
			*pNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		return FALSE;
	}

	virtual DXGBOUNDINGSPHERE  GetBoundingSphere()     { return DXGBOUNDINGSPHERE(); }
	virtual DXGBOUNDINGSPHERE  GetBaseBoundingSphere() { return DXGBOUNDINGSPHERE(); }

	virtual DXGOBB GetOBB()     { return DXGOBB(); }
	virtual DXGOBB GetBaseOBB() { return DXGOBB(); }

	virtual DWORD GetOcclusionQuery() { return 0; }
	virtual DWORD GetOcclusionQuery(IEffect*& pEffect) { return 0; }

	virtual bool EnableProgressive() { return true; }
	virtual void SetLOD(const float inLOD) {}

	virtual void LoadEffectMap    (const DWORD inStage, LPCTSTR inFileName) {}
	virtual void LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName) {}
	virtual void ApplyEffectMap()  {}
	virtual void RemoveEffectMap() {}

#ifdef BULLET_DYNAMICS_COMMON_H
	virtual btTransform btGetTransform() { return btTransform::getIdentity(); }
	virtual void btSetTransform(const btTransform& inTransform) {}
#endif
};
