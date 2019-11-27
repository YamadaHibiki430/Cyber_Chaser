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
	 AnimationModel.hpp
				アニメーションモデルクラスヘッダーファイル

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

#include <string>
#include <vector>
#include <list>
#include <map>

// #include <btBulletDynamicsCommon.h>

//------------------------------------------------------------------------------
//	スキニング方法列挙体
//------------------------------------------------------------------------------
enum DXGSKINNING_METHOD {
    DXGSKMTD_NONINDEXED,
    DXGSKMTD_INDEXED,
	DXGSKMTD_INDEXEDHLSL,
    DXGSKMTD_SOFTWARE,
	DXGSKMTD_DEFAULT = -1
};

//------------------------------------------------------------------------------
//	フレーム構造体
//------------------------------------------------------------------------------
struct DXGFRAME : public D3DXFRAME {
public:
	// 名前設定
	void SetName(LPCSTR inName)
	{
		if(inName != NULL)
			FrameName = inName;
		else
			FrameName = "";

		Name = (LPSTR)FrameName.c_str();
	}

public:
	D3DXMATRIX    CombinedTransformationMatrix;

private:
	std::string   FrameName;
};

//------------------------------------------------------------------------------
//	メッシュコンテナ構造体
//------------------------------------------------------------------------------
struct DXGMESHCONTAINER : public D3DXMESHCONTAINER {
public:
	// 名前設定
	void SetName(LPCSTR inName)
	{
		if(inName != NULL)
			ContainerName = inName;
		else
			ContainerName = "";

		Name = (LPSTR)ContainerName.c_str();
	}

public:
	ID3DXMesh*                 pOrigMesh;

	D3DXATTRIBUTERANGE*        pAttributeTable;
	DWORD                      NumAttributeGroups; 
	DWORD                      NumInfl;

	ID3DXBuffer*               pBoneCombinationBuf;
	D3DXMATRIX**               ppBoneMatrixPtrs;
	D3DXMATRIX*                pBoneOffsetMatrices;

	DWORD                      NumPaletteEntries;
	bool                       UseSoftwareVP;
	DWORD                      iAttributeSW;

	IDirect3DTexture9**        ppTextures;
	std::vector<std::string>   TextureName;

private:
	std::string                ContainerName;
};

//------------------------------------------------------------------------------
//	アニメーションモデルインタフェース定義
//------------------------------------------------------------------------------
class IAnimationModel {
public:
	virtual ~IAnimationModel() {}

	virtual void Draw() = 0;
	virtual void Draw(IEffect*& pEffect) = 0;
	virtual void Draw(IEffect*& pEffect, const UINT inPass) = 0;

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass) = 0;

	virtual void DrawAlpha (const float inAlpha, const DWORD inDraw = DXGDRAWA_BOTHFACES) = 0;
	virtual void DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth) = 0;

	virtual void SetPosition (const float x, const float y, const float z) = 0;
	virtual void SetScale    (const float x, const float y, const float z) = 0;
	virtual void SetDirection(const float x, const float y, const float z) = 0;
	virtual void SetDirection(const D3DXQUATERNION& inDirection) = 0;

	inline  void SetPosition (const D3DXVECTOR3& inPosition)  { SetPosition (inPosition .x, inPosition .y, inPosition .z); }
	inline  void SetScale    (const D3DXVECTOR3& inScale)     { SetScale    (inScale    .x, inScale    .y, inScale    .z); }
	inline  void SetScale    (const float s)                  { SetScale    (s, s, s); }
	inline  void SetDirection(const D3DXVECTOR3& inDirection) { SetDirection(inDirection.x, inDirection.y, inDirection.z); }

	inline  void SetRotation (const float x, const float y, const float z)
	{ SetDirection(x, y, z); }
	inline  void SetRotation (const D3DXVECTOR3& inDirection)
	{ SetDirection(inDirection.x, inDirection.y, inDirection.z); }

	virtual void Move    (const float inRight, const float inUp, const float inFront) = 0;
	virtual void Rotation(const float inX,     const float inY,  const float inZ)     = 0;

	inline  void Move    (const D3DXVECTOR3& inDistance) { Move    (inDistance.x, inDistance.y, inDistance.z); }
	inline  void Rotation(const D3DXVECTOR3& inRotation) { Rotation(inRotation.x, inRotation.y, inRotation.z); }

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle) = 0;
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront) = 0;

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY) = 0;

	virtual void SetBoneOffsetMatrix     (const DWORD inIdx, D3DXMATRIX& inMatrix) = 0;
	virtual void SetBoneOffsetMatrices   (D3DXMATRIX  inMatrices[])                = 0;
	virtual void MultiplyBoneOffsetMatrix(const DWORD inIdx, D3DXMATRIX& inMatrix) = 0;

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) = 0;

	virtual void ChangeTexture(LPCSTR inMasterTexture, LPCSTR inNewTexture) = 0;
	virtual void ChangeTexture(LPCSTR inMasterTexture, IDirect3DTexture9* pTexture) = 0;

	virtual UINT GetNumTracks() const = 0;

	virtual D3DXVECTOR3	GetPosition() const = 0;
	virtual D3DXVECTOR3	GetScale   () const = 0;

	virtual D3DXVECTOR3    GetDirection()           const = 0;
	virtual D3DXVECTOR3    GetDirectionRadian()     const = 0;
	virtual D3DXQUATERNION GetDirectionQuaternion() const = 0;

	inline  D3DXVECTOR3    GetRotation() const { return GetDirection(); }

	virtual D3DXMATRIX  GetBoneOffsetMatrix   (const DWORD inIdx) const = 0;
	virtual void        GetBoneOffsetMatrices (D3DXMATRIX  outMatrices[]) const = 0;
	virtual DWORD       GetNumBoneOffsetMatrix() const = 0;

	virtual D3DXMATRIX  GetWorldMatrix() const = 0;
	virtual D3DXVECTOR3 GetFrontVector() const = 0;
	virtual D3DXVECTOR3 GetRightVector() const = 0;
	virtual D3DXVECTOR3 GetUpVector   () const = 0;

	virtual DXGSKINNING_METHOD GetSkinningMethod() const = 0;

	virtual DXGBOUNDINGSPHERE  GetBoundingSphere()     = 0;
	virtual DXGBOUNDINGSPHERE  GetBaseBoundingSphere() = 0;

	virtual DXGOBB GetOBB()     = 0;
	virtual DXGOBB GetBaseOBB() = 0;

	virtual DWORD  GetOcclusionQuery() = 0;
	virtual DWORD  GetOcclusionQuery(IEffect*& pEffect) = 0;

	virtual double GetTrackPosition(const UINT inTrack) const = 0;

	virtual void SetTrackEnable  (const UINT inTrack, const BOOL   inEnable  ) = 0;
	virtual void SetTrackWeight  (const UINT inTrack, const float  inWeight  ) = 0;
	virtual void SetTrackLoopTime(const UINT inTrack, const float  inLoopTime) = 0;
	virtual void SetTrackPosition(const UINT inTrack, const double inPosition) = 0;

	virtual void SetTrackLoopMode(const UINT inTrack, const D3DXPLAYBACK_TYPE inLoopType) = 0;

	virtual void ChangeAnimation(const UINT inOutTrack, const UINT inInTrack, const double inDuration,
								 const D3DXTRANSITION_TYPE inTransition = D3DXTRANSITION_LINEAR) = 0;
	virtual void ChangeAnimation(const UINT inInTrack, const double inDuration,
								 const D3DXTRANSITION_TYPE inTransition = D3DXTRANSITION_LINEAR) = 0;

	virtual void AdvanceTime(const double inElapsedTime) = 0;

	virtual void SetSkinningMethod(const DXGSKINNING_METHOD inMethod) = 0;

	virtual bool EnableProgressive() = 0;
	virtual void SetLOD(const float inLOD) = 0;

	virtual void LoadEffectMap    (const DWORD inStage, LPCTSTR inFileName) = 0;
	virtual void LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName) = 0;
	virtual void ApplyEffectMap () = 0;
	virtual void RemoveEffectMap() = 0;

	virtual void RegisterBoneMatricesByName(IEffect*& inEffect, LPCSTR inWorldMatrixArray, LPCSTR inNumBones) = 0;

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
//	アニメーションモデルクラス定義
//------------------------------------------------------------------------------
class CAnimationModel : public IAnimationModel {
public:
	CAnimationModel(IDirect3DDevice9* pD3DDevice);
	virtual ~CAnimationModel();

	virtual void Draw();
	virtual void Draw(IEffect*& pEffect);
	virtual void Draw(IEffect*& pEffect, const UINT inPass);

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass);

	virtual void DrawAlpha (const float inAlpha, const DWORD inDraw);
	virtual void DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth);

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

	virtual void SetBoneOffsetMatrix     (const DWORD inIdx, D3DXMATRIX& inMatrix);
	virtual void SetBoneOffsetMatrices   (D3DXMATRIX  inMatrices[]);
	virtual void MultiplyBoneOffsetMatrix(const DWORD inIdx, D3DXMATRIX& inMatrix);

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial);

	virtual void ChangeTexture(LPCSTR inMasterTexture, LPCSTR inNewTexture);
	virtual void ChangeTexture(LPCSTR inMasterTexture, IDirect3DTexture9* pTexture);

	virtual UINT GetNumTracks() const
	{ return m_pAnimationController->GetMaxNumTracks(); }

	virtual D3DXVECTOR3	GetPosition() const { return m_Position; }
	virtual D3DXVECTOR3	GetScale   () const { return m_Scale;    }

	virtual D3DXVECTOR3    GetDirection() const;
	virtual D3DXVECTOR3    GetDirectionRadian() const;
	virtual D3DXQUATERNION GetDirectionQuaternion() const { return m_Rotation; }

	virtual D3DXMATRIX GetBoneOffsetMatrix   (const DWORD inIdx) const;
	virtual void       GetBoneOffsetMatrices (D3DXMATRIX  outMatrices[]) const;
	virtual DWORD      GetNumBoneOffsetMatrix() const { return m_NumBoneMatrices; }

	virtual D3DXMATRIX  GetWorldMatrix() const;
	virtual D3DXVECTOR3 GetFrontVector() const;
	virtual D3DXVECTOR3 GetRightVector() const;
	virtual D3DXVECTOR3 GetUpVector   () const;

	virtual DXGSKINNING_METHOD GetSkinningMethod() const { return m_SkinningMethod; }

	virtual DXGBOUNDINGSPHERE GetBoundingSphere();
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere();

	virtual DXGOBB GetOBB();
	virtual DXGOBB GetBaseOBB();

	virtual DWORD  GetOcclusionQuery();
	virtual DWORD  GetOcclusionQuery(IEffect*& pEffect);

	// トラックポジション取得
	virtual double GetTrackPosition(const UINT inTrack) const
	{
		D3DXTRACK_DESC   desc;
		m_pAnimationController->GetTrackDesc(inTrack, &desc);

		return desc.Position;
	}

	// トラック有効設定
	virtual void SetTrackEnable(const UINT inTrack, const BOOL  inEnable)
	{ m_pAnimationController->SetTrackEnable(inTrack, inEnable); }

	// トラックウェイト設定
	virtual void SetTrackWeight(const UINT inTrack, const float inWeight)
	{ m_pAnimationController->SetTrackWeight(inTrack, inWeight); }

	// トラックループタイム設定
	virtual void SetTrackLoopTime(const UINT inTrack, const float inTime);

	// トラックポジション設定
	virtual void SetTrackPosition(const UINT inTrack, const double inPosition)
	{
		m_pAnimationController->SetTrackPosition(inTrack, inPosition);
		m_pAnimationController->AdvanceTime(0.0, NULL);
	}

	// ループモード設定
	virtual void SetTrackLoopMode(const UINT inTrack, const D3DXPLAYBACK_TYPE inLoopType);

	// アニメーション切り換え
	virtual void ChangeAnimation(const UINT inOutTrack, const UINT inInTrack,
								 const double inDuration, const D3DXTRANSITION_TYPE inTransition);
	virtual void ChangeAnimation(const UINT inInTrack, const double inDuration,
								 const D3DXTRANSITION_TYPE inTransition);

	// グローバルタイム設定
	virtual void AdvanceTime(const double inElapsedTime)
	{ m_pAnimationController->AdvanceTime(inElapsedTime, NULL); }

	// スキニング方法設定
	virtual void SetSkinningMethod(const DXGSKINNING_METHOD inMethod);

	// プログレッシブモード
	virtual bool EnableProgressive();
	virtual void SetLOD(const float inLOD);

	// エフェクトマップ
	virtual void LoadEffectMap    (const DWORD inStage, LPCTSTR inFileName);
	virtual void LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName);
	virtual void ApplyEffectMap ();
	virtual void RemoveEffectMap();

	// シェーダーパラメータ設定
	virtual void RegisterBoneMatricesByName(IEffect*& inEffect, LPCSTR inWorldMatrixArray, LPCSTR inNumBones);

	// 読み込み
	bool LoadFromX(LPCTSTR inFileName,
				   const DXGCOMPUTENORMAL   inCompute    = DXGCOMPUTE_NORMAL,
				   const DXGSKINNING_METHOD inSkinMethod = DXGSKMTD_DEFAULT);

	inline bool LoadFromX(LPCTSTR inFileName, const DXGCOMPUTENORMAL inCompute)
	{ return LoadFromX(inFileName, inCompute, DXGSKMTD_DEFAULT); }

	inline bool LoadFromX(LPCTSTR inFileName, const DXGSKINNING_METHOD inSkinMethod)
	{ return LoadFromX(inFileName, DXGCOMPUTE_NORMAL, inSkinMethod); }

	// スキンメッシュ生成
	HRESULT GenerateSkinnedMesh(DXGMESHCONTAINER* pMeshContainer);

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
private:
	void Release();

	void UpdateFrameMatrices(D3DXFRAME* pFrameBase, D3DXMATRIX* pParentMatrix);

	void DrawFrame(D3DXFRAME* pFrame);
	void DrawFrame(D3DXFRAME* pFrame, IEffect*& pEffect);
	void DrawFrame(D3DXFRAME* pFrame, IEffect*& pEffect, const UINT& inPass);

	void DrawMeshContainer           (D3DXMESHCONTAINER* pMeshContainerBase, D3DXFRAME* pFrameBase);
	void DrawMeshContainerNonIndexed ( DXGMESHCONTAINER* pMeshContainer,      DXGFRAME* pFrame);
	void DrawMeshContainerIndexed    ( DXGMESHCONTAINER* pMeshContainer,      DXGFRAME* pFrame);
	void DrawMeshContainerSoftware   ( DXGMESHCONTAINER* pMeshContainer,      DXGFRAME* pFrame);
	void DrawMeshContainerIndexedHLSL( DXGMESHCONTAINER* pMeshContainer,      DXGFRAME* pFrame, IEffect*& pEffect);
	void DrawMeshContainerIndexedHLSL( DXGMESHCONTAINER* pMeshContainer,      DXGFRAME* pFrame,
									  IEffect*& pEffect, const UINT& inPass);

	void DrawAlphaFrame(D3DXFRAME* pFrame, const float& inAlpha);
	void DrawAlphaMeshContainer(D3DXMESHCONTAINER* pMeshContainerBase, D3DXFRAME* pFrameBase, const float& inAlpha);

	void DrawShadowFrame(D3DXFRAME* pFrame, const float& inDepth);
	void DrawShadowMeshContainer(D3DXMESHCONTAINER* pMeshContainerBase, D3DXFRAME* pFrameBase, const float& inDepth);

	bool SetupBoneMatrixPointersOnMesh(D3DXMESHCONTAINER* pMeshContainer);
    bool SetupBoneMatrixPointers(D3DXFRAME* pFrame);

	void ChangeSkinningMethod(D3DXFRAME* pFrameBase);

	HRESULT GenerateSkinnedMeshNonIndexed (DXGMESHCONTAINER* pMeshContainer);
	HRESULT GenerateSkinnedMeshIndexed    (DXGMESHCONTAINER* pMeshContainer);
	HRESULT GenerateSkinnedMeshIndexedHLSL(DXGMESHCONTAINER* pMeshContainer);
	HRESULT GenerateSkinnedMeshSoftware   (DXGMESHCONTAINER* pMeshContainer);

	void SetMaterial  (D3DXFRAME* pFrame, const D3DMATERIAL9& inMaterial);

	void ChangeTexture(D3DXFRAME* pFrame, const std::string& inMasterTexture, LPCSTR inNewTexture);
	void ChangeTexture(D3DXFRAME* pFrame, const std::string& inMasterTexture, IDirect3DTexture9* pTexture);

	void GetBaseOBB(D3DXFRAME* pFrame, D3DXVECTOR3& ioMax, D3DXVECTOR3& ioMin);

	void EnableProgressive(D3DXFRAME* pFrameBase);
	void SetLOD(D3DXFRAME* pFrameBase, const float& inLOD);

	void SetBoneOffsetMatrixPointer(D3DXFRAME* pFrame);

private:
	enum { MAX_BONES = 26 };

	IDirect3DDevice9*			m_pD3DDevice;
	DWORD                       m_MaxVertexBlendMatrices;
    DWORD                       m_MaxVertexBlendMatrixIndex;

	ID3DXAnimationController*   m_pAnimationController;
    D3DXFRAME*					m_pFrameRoot;

	DXGSKINNING_METHOD          m_SkinningMethod;
    D3DXMATRIXA16*              m_pBoneMatrices;
    UINT                        m_NumBoneMatrices;

	D3DXVECTOR3                 m_Position;				// 位置
	D3DXVECTOR3                 m_Scale;				// 拡大率
	D3DXQUATERNION              m_Rotation;				// 回転クォータニオン

	D3DXMATRIX*                 m_pBoneOffsetMatrices;
	DWORD                       m_NumBoneOffsetMatrices;

	// シェーダーパラメータハンドル
	D3DXHANDLE                  m_WorldMatrixArrayName;
	D3DXHANDLE                  m_NumBonesName;

	// エフェクトマッピング
	struct EFFECTMAP {
		DWORD                    Stage;
		IDirect3DBaseTexture9*   pTexture;

		EFFECTMAP() : Stage(0), pTexture(NULL) {}
		EFFECTMAP(const DWORD stage, IDirect3DBaseTexture9* texture)
			: Stage(stage), pTexture(texture) {}
	};
	std::list<EFFECTMAP>   m_EffectMap;

	template <class    T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; } }
	template <typename T> inline void DeleteArray(T* x) { delete[] x; x = NULL; }
};

//------------------------------------------------------------------------------
//	NULLアニメーションモデルクラス定義
//------------------------------------------------------------------------------
class CNullAnimationModel : public IAnimationModel {
public:
	virtual ~CNullAnimationModel() {}

	virtual void Draw() {}
	virtual void Draw(IEffect*& pEffect) {}
	virtual void Draw(IEffect*& pEffect, const UINT inPass) {}

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass) {}

	virtual void DrawAlpha (const float inAlpha, const DWORD inDraw) {}
	virtual void DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth) {}

	virtual void SetPosition (const float x, const float y, const float z) {}
	virtual void SetScale    (const float x, const float y, const float z) {}
	virtual void SetDirection(const float x, const float y, const float z) {}
	virtual void SetDirection(const D3DXQUATERNION& inDirection) {}

	virtual void Move     (const float inRight, const float inUp, const float inFront) {}
	virtual void Rotation (const float inX,     const float inY,  const float inZ)     {}

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle) {}
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront) {}

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY) {}

	virtual void SetBoneOffsetMatrix     (const DWORD inIdx, D3DXMATRIX& inMatrix) {}
	virtual void SetBoneOffsetMatrices   (D3DXMATRIX  inMatrices[]) {}
	virtual void MultiplyBoneOffsetMatrix(const DWORD inIdx, D3DXMATRIX& inMatrix) {}

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) {}

	virtual void ChangeTexture(LPCSTR inMasterTexture, LPCSTR inNewTexture) {}
	virtual void ChangeTexture(LPCSTR inMasterTexture, IDirect3DTexture9* pTexture) {}

	virtual UINT GetNumTracks() const { return 0; }

	virtual D3DXVECTOR3	   GetPosition()           const { return D3DXVECTOR3   (0.0f, 0.0f, 0.0f);       }
	virtual D3DXVECTOR3	   GetScale   ()           const { return D3DXVECTOR3   (1.0f, 1.0f, 1.0f);       }

	virtual D3DXVECTOR3    GetDirection()           const { return D3DXVECTOR3   (0.0f, 0.0f, 0.0f);       }
	virtual D3DXVECTOR3    GetDirectionRadian()     const { return D3DXVECTOR3   (0.0f, 0.0f, 0.0f);       }
	virtual D3DXQUATERNION GetDirectionQuaternion() const { return D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f); }

	virtual D3DXMATRIX GetBoneOffsetMatrix(const DWORD inIdx) const
	{
		D3DXMATRIX   mat;
		::D3DXMatrixIdentity(&mat);
		return mat;
	}

	virtual void  GetBoneOffsetMatrices(D3DXMATRIX outMatrices[]) const {}
	virtual DWORD GetNumBoneOffsetMatrix() const { return 0; }

	virtual D3DXMATRIX GetWorldMatrix() const
	{
		D3DXMATRIX   world;
		::D3DXMatrixIdentity(&world);
		return world;
	}

	virtual D3DXVECTOR3 GetFrontVector() const { return D3DXVECTOR3(0.0f, 0.0f, 1.0f); }
	virtual D3DXVECTOR3 GetRightVector() const { return D3DXVECTOR3(1.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3 GetUpVector   () const { return D3DXVECTOR3(0.0f, 1.0f, 0.0f); }

	virtual DXGSKINNING_METHOD GetSkinningMethod() const { return DXGSKMTD_SOFTWARE; }

	virtual DXGBOUNDINGSPHERE GetBoundingSphere()     { return DXGBOUNDINGSPHERE(); }
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere() { return DXGBOUNDINGSPHERE(); }

	virtual DXGOBB GetOBB()     { return DXGOBB(); }
	virtual DXGOBB GetBaseOBB() { return DXGOBB(); }

	virtual DWORD GetOcclusionQuery() { return 0; }
	virtual DWORD GetOcclusionQuery(IEffect*& pEffect) { return 0; }

	virtual double GetTrackPosition(const UINT inTrack) const { return 0.0; }

	virtual void SetTrackEnable  (const UINT inTrack, const BOOL   inEnable  ) {}
	virtual void SetTrackWeight  (const UINT inTrack, const float  inWeight  ) {}
	virtual void SetTrackLoopTime(const UINT inTrack, const float  inLoopTime) {}
	virtual void SetTrackPosition(const UINT inTrack, const double inPosition) {}

	virtual void SetTrackLoopMode(const UINT inTrack, const D3DXPLAYBACK_TYPE inLoopType) {}

	virtual void ChangeAnimation(const UINT inOutTrack, const UINT inInTrack,
								 const double inDuration, const D3DXTRANSITION_TYPE inTransition) {}
	virtual void ChangeAnimation(const UINT inInTrack, const double inDuration,
								 const D3DXTRANSITION_TYPE inTransition) {}

	virtual void AdvanceTime(const double inElapsedTime) {}

	virtual void SetSkinningMethod(const DXGSKINNING_METHOD inMethod) {}

	virtual bool EnableProgressive() { return false; }
	virtual void SetLOD(const float inLOD) {}

	virtual void LoadEffectMap    (const DWORD inStage, LPCTSTR inFileName) {}
	virtual void LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName) {}
	virtual void ApplyEffectMap () {}
	virtual void RemoveEffectMap() {}

	virtual void RegisterBoneMatricesByName(IEffect*& inEffect, LPCSTR inWorldMatrixArray, LPCSTR inNumBones) {}

#ifdef BULLET_DYNAMICS_COMMON_H
	virtual btTransform btGetTransform() { return btTransform::getIdentity(); }
	virtual void btSetTransform(const btTransform& inTransform) {}
#endif
};

//------------------------------------------------------------------------------
//	階層構造割り当てクラス
//------------------------------------------------------------------------------
class DXGAllocateHierarchy : public ID3DXAllocateHierarchy {
public:
	DXGAllocateHierarchy(CAnimationModel* pAnimeModel)
		: m_pAnimeModel(pAnimeModel), m_ComputeNormal(DXGCOMPUTE_NONE) {}
	DXGAllocateHierarchy(CAnimationModel* pAnimeModel, const DXGCOMPUTENORMAL inCompute)
		: m_pAnimeModel(pAnimeModel), m_ComputeNormal(inCompute) {}

	STDMETHOD(CreateFrame)(THIS_ LPCSTR inName, LPD3DXFRAME *ppNewFrame);

	STDMETHOD(CreateMeshContainer)(THIS_
								   LPCSTR                    inName, 
								   CONST D3DXMESHDATA*       pMeshData,
								   CONST D3DXMATERIAL*       pMaterials, 
								   CONST D3DXEFFECTINSTANCE* pEffectInstances, 
								   DWORD                     inNumMaterials, 
								   CONST DWORD*              pAdjacency, 
								   LPD3DXSKININFO            pSkinInfo, 
								   LPD3DXMESHCONTAINER*      ppNewMeshContainer);

	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrame);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

public:
    CAnimationModel*   m_pAnimeModel;
	DXGCOMPUTENORMAL   m_ComputeNormal;

private:
	ID3DXMesh* ModelingMesh(IDirect3DDevice9*& pD3DDevice, ID3DXMesh* pBaseMesh,
							const DXGCOMPUTENORMAL inCompute);

	template <class    T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; } }
	template <typename T> inline void DeleteArray(T* x) { delete[] x; x = NULL; }
};