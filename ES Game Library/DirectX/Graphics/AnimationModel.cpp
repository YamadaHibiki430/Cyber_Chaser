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
	 AnimationModel.cpp
				アニメーションモデルクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "AnimationModel.hpp"
#include <tchar.h>
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CAnimationModel::CAnimationModel(IDirect3DDevice9* pD3DDevice)
	: m_pD3DDevice(pD3DDevice), m_pAnimationController(NULL),
	  m_pFrameRoot(NULL), m_SkinningMethod(DXGSKMTD_DEFAULT),
	  m_pBoneMatrices(NULL), m_NumBoneMatrices(0),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f, 1.0f),
	  m_pBoneOffsetMatrices(NULL), m_NumBoneOffsetMatrices(0),
	  m_WorldMatrixArrayName(NULL), m_NumBonesName(NULL)
{
	assert(m_pD3DDevice != NULL);
	m_pD3DDevice->AddRef();

	// 頂点ブレンディング能力取得
	D3DCAPS9   caps;
	::ZeroMemory(&caps, sizeof(caps));
	m_pD3DDevice->GetDeviceCaps(&caps);
	m_MaxVertexBlendMatrices    = caps.MaxVertexBlendMatrices;
	m_MaxVertexBlendMatrixIndex = caps.MaxVertexBlendMatrixIndex;
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CAnimationModel::~CAnimationModel()
{
	Release();
	m_pD3DDevice->Release();
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CAnimationModel::Release()
{
	// エフェクトテクスチャ解放
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		SafeRelease((*it).pTexture);

	// フレーム破棄
	if(m_pFrameRoot != NULL) {
		DXGAllocateHierarchy   alloc(this);
		::D3DXFrameDestroy(m_pFrameRoot, &alloc);
		m_pFrameRoot = NULL;
	}

	SafeRelease(m_pAnimationController);
}

//------------------------------------------------------------------------------
//	ｘファイル読み込み
//------------------------------------------------------------------------------
bool CAnimationModel::LoadFromX(LPCTSTR inFileName,
								const DXGCOMPUTENORMAL   inCompute,
								const DXGSKINNING_METHOD inSkinMethod)
{
	Release();

	m_SkinningMethod = inSkinMethod;

	// 読み込みフォルダ設定
	// フルパス取得
	TCHAR   FullPath[MAX_PATH + 1];
	::_tfullpath(FullPath, inFileName, MAX_PATH + 1);

	// ファイル名分割
	TCHAR   drive[_MAX_DRIVE + 1];
	TCHAR   dir  [_MAX_DIR   + 1];
	::_tsplitpath_s(FullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);

	// カレントディレクトリ保存
	TCHAR   CurrentDirectory[MAX_PATH + 1];
	::GetCurrentDirectory(MAX_PATH + 1, CurrentDirectory);

	// ファイルのパスをカレントディレクトリに設定
	TCHAR   FilePath[MAX_PATH + 1];
	::wsprintf(FilePath, TEXT("%s%s"), drive, dir);
	::SetCurrentDirectory(FilePath);

	// ｘファイル読み込み
	HRESULT                     hr;
	DXGAllocateHierarchy        alloc(this, inCompute);
	ID3DXAnimationController*   pAnimeCntr;
	if(m_SkinningMethod != DXGSKMTD_DEFAULT) {
		hr = ::D3DXLoadMeshHierarchyFromX(FullPath, D3DXMESH_MANAGED,
										  m_pD3DDevice, &alloc, NULL,
										  &m_pFrameRoot, &pAnimeCntr);
	} else {
		// メソッド読み込み順設定
		DXGSKINNING_METHOD   skin_method[3];
		if(m_MaxVertexBlendMatrixIndex != 0) {
			// ハードウェア頂点ブレンディングサポート
			skin_method[0] = DXGSKMTD_INDEXED;
			skin_method[1] = DXGSKMTD_NONINDEXED;
			skin_method[2] = DXGSKMTD_SOFTWARE;
		} else {
			// ハードウェア頂点ブレンディング非サポート
			skin_method[0] = DXGSKMTD_NONINDEXED;
			skin_method[1] = DXGSKMTD_INDEXED;
			skin_method[2] = DXGSKMTD_SOFTWARE;
		}

		// ｘファイル読み込み
		for(int i = 0; i < 3; i++) {
			m_SkinningMethod = skin_method[i];
			hr = ::D3DXLoadMeshHierarchyFromX(FullPath, D3DXMESH_MANAGED,
											  m_pD3DDevice, &alloc, NULL,
											  &m_pFrameRoot, &pAnimeCntr);
			if(hr == D3D_OK && pAnimeCntr != NULL)
				break;
			Release();
		}
	}
	::SetCurrentDirectory(CurrentDirectory);	// カレントディレクトリ復元
	if(hr != D3D_OK || pAnimeCntr == NULL) {
		::OutputDebugString(TEXT("*** Error - ｘファイル読み込み失敗(CAnimationModel_LoadFromX)\n"));
		Release();
		return false;
	}

	// トラック数をアニメーションセット数に増やす
	UINT   uTracks    = pAnimeCntr->GetMaxNumTracks();
	UINT   uAnimeSets = pAnimeCntr->GetMaxNumAnimationSets();
	if(uTracks < uAnimeSets)
		uTracks = uAnimeSets;

	// スキンメッシュ生成
	if(pAnimeCntr->CloneAnimationController(pAnimeCntr->GetMaxNumAnimationOutputs(),
											pAnimeCntr->GetMaxNumAnimationSets(),
											uTracks,
											pAnimeCntr->GetMaxNumEvents(),
											&m_pAnimationController)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - スキンメッシュ生成失敗(CAnimationModel_LoadFromX)\n"));
		Release();
		return false;
	}

	SafeRelease(pAnimeCntr);

	if(uAnimeSets >= 2){
		D3DXTRACK_DESC   desc;
		desc.Priority = D3DXPRIORITY_LOW;
		desc.Weight   = 1.0f;
		desc.Speed    = 1.0f;
		desc.Position = 0.0;
		desc.Enable   = FALSE;

		// トラックにアニメーションセットを設定
		for(UINT i = 1; i < uAnimeSets; i++){
			// アニメーションセット取得
			ID3DXAnimationSet*   pAnimeSet;
			if(m_pAnimationController->GetAnimationSet(i, &pAnimeSet) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - アニメーションセット取得失敗(CAnimationModel_LoadFromX)\n"));
				return false;
			}

			// アニメーションセット設定
			const HRESULT   hr = m_pAnimationController->SetTrackAnimationSet(i, pAnimeSet);
			pAnimeSet->Release();
			if(hr != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - アニメーションセット設定失敗(CAnimationModel_LoadFromX)\n"));
				return false;
			}

			// デフォルト値設定
			if(m_pAnimationController->SetTrackDesc(i, &desc) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - デフォルト値設定失敗(CAnimationModel_LoadFromX)\n"));
				return false;
			}
		}
	}

	if(SetupBoneMatrixPointers(m_pFrameRoot) == false)
		return false;
	SetBoneOffsetMatrixPointer(m_pFrameRoot);

	m_pAnimationController->AdvanceTime(0.0, NULL);

	return true;
}

//------------------------------------------------------------------------------
//	ボーン行列ポインタ設定
//------------------------------------------------------------------------------
bool CAnimationModel::SetupBoneMatrixPointersOnMesh(D3DXMESHCONTAINER* pMeshContainerBase)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pMeshContainerBase;
	if(pMeshContainer->pSkinInfo != NULL) {
		const UINT   BONES = pMeshContainer->pSkinInfo->GetNumBones();
		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[BONES];
		if(pMeshContainer->ppBoneMatrixPtrs == NULL) {
			::OutputDebugString(TEXT("*** Error - ボーン行列配列生成失敗(CAnimationModel_SetupBoneMatrixPointersOnMesh)\n"));
			return false;
		}

		for(UINT i = 0; i < BONES; i++) {
			DXGFRAME*   pFrame = (DXGFRAME*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(i));
			if(pFrame == NULL) {
				::OutputDebugString(TEXT("*** Error - フレーム検索失敗(CAnimationModel_SetupBoneMatrixPointersOnMesh)\n"));
				DeleteArray(pMeshContainer->ppBoneMatrixPtrs);
				return false;
			}
			pMeshContainer->ppBoneMatrixPtrs[i] = &pFrame->CombinedTransformationMatrix;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//	ボーン行列ポインタ設定
//------------------------------------------------------------------------------
bool CAnimationModel::SetupBoneMatrixPointers(D3DXFRAME* pFrame)
{
	if(pFrame->pMeshContainer != NULL) {
		if(SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer) == false)
			return false;
	}

	if(pFrame->pFrameSibling != NULL) {
		if(SetupBoneMatrixPointers(pFrame->pFrameSibling) == false)
			return false;
	}

	if(pFrame->pFrameFirstChild != NULL) {
		if(SetupBoneMatrixPointers(pFrame->pFrameFirstChild) == false)
			return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	ボーンオフセット行列ポインタ設定
//------------------------------------------------------------------------------
void CAnimationModel::SetBoneOffsetMatrixPointer(D3DXFRAME* pFrame)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;
	if(pMeshContainer != NULL) {
		if(pMeshContainer->pBoneOffsetMatrices != NULL) {
			m_pBoneOffsetMatrices   = pMeshContainer->pBoneOffsetMatrices;
			m_NumBoneOffsetMatrices = pMeshContainer->pSkinInfo->GetNumBones();
			return;
		}
	}

	if(pFrame->pFrameSibling != NULL) {
		SetBoneOffsetMatrixPointer(pFrame->pFrameSibling);
		if(m_pBoneOffsetMatrices != NULL)
			return;
	}

	if(pFrame->pFrameFirstChild != NULL)
		SetBoneOffsetMatrixPointer(pFrame->pFrameFirstChild);
}

//------------------------------------------------------------------------------
//	アニメーションモデル描画
//------------------------------------------------------------------------------
void CAnimationModel::Draw()
{
	UpdateFrameMatrices(m_pFrameRoot, &GetWorldMatrix());
	DrawFrame(m_pFrameRoot);
}

//------------------------------------------------------------------------------
//	アニメーションモデル描画
//------------------------------------------------------------------------------
void CAnimationModel::Draw(IEffect*& pEffect)
{
	SetSkinningMethod(DXGSKMTD_INDEXEDHLSL);

	UpdateFrameMatrices(m_pFrameRoot, &GetWorldMatrix());

	DrawFrame(m_pFrameRoot, pEffect);
}

//------------------------------------------------------------------------------
//	アニメーションモデル描画
//------------------------------------------------------------------------------
void CAnimationModel::Draw(IEffect*& pEffect, const UINT inPass)
{
	SetSkinningMethod(DXGSKMTD_INDEXEDHLSL);

	UpdateFrameMatrices(m_pFrameRoot, &GetWorldMatrix());

	pEffect->Begin();
	DrawFrame(m_pFrameRoot, pEffect, inPass);
	pEffect->End();
}

//------------------------------------------------------------------------------
//	アニメーションモデル描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawPass(IEffect*& pEffect, const UINT inPass)
{
	SetSkinningMethod(DXGSKMTD_INDEXEDHLSL);
	UpdateFrameMatrices(m_pFrameRoot, &GetWorldMatrix());
	DrawFrame(m_pFrameRoot, pEffect, inPass);
}

//------------------------------------------------------------------------------
//	フレーム行列更新
//------------------------------------------------------------------------------
void CAnimationModel::UpdateFrameMatrices(D3DXFRAME* pFrameBase, D3DXMATRIX* pParentMatrix)
{
	DXGFRAME*   pFrame = (DXGFRAME*)pFrameBase;
	if(pParentMatrix != NULL) {
		::D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix,
							 &pFrame->TransformationMatrix, pParentMatrix);
	} else {
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	}

	if(pFrame->pFrameSibling != NULL)
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);

	if(pFrame->pFrameFirstChild != NULL)
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
}

//------------------------------------------------------------------------------
//	フレーム描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawFrame(D3DXFRAME* pFrame)
{
	D3DXMESHCONTAINER*  pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		// 最下層
		DrawMeshContainer(pMeshContainer, pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		DrawFrame(pFrame->pFrameSibling);

	// 子ノード
	if (pFrame->pFrameFirstChild != NULL)
		DrawFrame(pFrame->pFrameFirstChild);
}

//------------------------------------------------------------------------------
//	メッシュコンテナ描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawMeshContainer(D3DXMESHCONTAINER* pMeshContainerBase, D3DXFRAME* pFrameBase)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pMeshContainerBase;
    DXGFRAME*           pFrame         = (DXGFRAME*)pFrameBase;

	if(pMeshContainer->pSkinInfo != NULL) {
		// スキンメッシュ
		switch(m_SkinningMethod) {
		  case DXGSKMTD_NONINDEXED:  return DrawMeshContainerNonIndexed(pMeshContainer, pFrame);
		  case DXGSKMTD_INDEXED:     return DrawMeshContainerIndexed   (pMeshContainer, pFrame);
		  case DXGSKMTD_SOFTWARE:    return DrawMeshContainerSoftware  (pMeshContainer, pFrame);
		}
	} else {
		// 通常メッシュ
		ID3DXBaseMesh*   pBaseMesh = pMeshContainer->MeshData.pMesh;
        m_pD3DDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);
        for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
            m_pD3DDevice->SetMaterial(  &pMeshContainer->pMaterials[i].MatD3D);
            m_pD3DDevice->SetTexture (0, pMeshContainer->ppTextures[i]);
            pBaseMesh->DrawSubset(i);
        }
	}
}

//------------------------------------------------------------------------------
//	メッシュコンテナ描画－通常バッファ
//------------------------------------------------------------------------------
void CAnimationModel::DrawMeshContainerNonIndexed(DXGMESHCONTAINER* pMeshContainer, DXGFRAME* pFrame)
{
	DWORD                  AttribIdPrev = UNUSED32;
	D3DXBONECOMBINATION*   pBoneComb    = (D3DXBONECOMBINATION*)pMeshContainer->pBoneCombinationBuf->GetBufferPointer();
	ID3DXBaseMesh*         pBaseMesh    = pMeshContainer->MeshData.pMesh;

	UINT                   NumBlend, iAttrib;
	D3DXMATRIXA16          mat;

	// 固定機能
	for(iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
		NumBlend = 0;
		for(DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
			if(pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
				NumBlend = i;
		}

		if(m_MaxVertexBlendMatrices >= NumBlend + 1) {
			for(DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
				const UINT   iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
				if(iMatrixIndex != UINT_MAX) {
					::D3DXMatrixMultiply(&mat,
										 &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
										  pMeshContainer->ppBoneMatrixPtrs   [iMatrixIndex]);
					m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(i), &mat);
				}
			}

			// 頂点ブレンド
			m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

			// マテリアル設定
			if((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32)) {
				m_pD3DDevice->SetMaterial(  &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
				m_pD3DDevice->SetTexture (0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
				AttribIdPrev = pBoneComb[iAttrib].AttribId;
			}

			// 描画
			pBaseMesh->DrawSubset(iAttrib);
		}
	}

	// ハードウェア、ソフトウェア混在
	if(pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups) {
		AttribIdPrev = UNUSED32; 
		m_pD3DDevice->SetSoftwareVertexProcessing(TRUE);
		for(iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
			NumBlend = 0;
			for(DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
				if(pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
					NumBlend = i;
			}

			if(m_MaxVertexBlendMatrices < NumBlend + 1) {
				for(DWORD i = 0; i < pMeshContainer->NumInfl; i++) {
					const UINT   iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
					if(iMatrixIndex != UINT_MAX) {
						::D3DXMatrixMultiply(&mat,
											 &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
											  pMeshContainer->ppBoneMatrixPtrs   [iMatrixIndex]);
						m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(i), &mat);
					}
				}

				// 頂点ブレンド
				m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

				// マテリアル設定
				if((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32)) {
					m_pD3DDevice->SetMaterial(  &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
					m_pD3DDevice->SetTexture (0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
					AttribIdPrev = pBoneComb[iAttrib].AttribId;
				}

				// 描画
				pBaseMesh->DrawSubset(iAttrib);
			}
		}
		m_pD3DDevice->SetSoftwareVertexProcessing(FALSE);
	}
	m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
}

//------------------------------------------------------------------------------
//	メッシュコンテナ描画－インデックスバッファ
//------------------------------------------------------------------------------
void CAnimationModel::DrawMeshContainerIndexed(DXGMESHCONTAINER* pMeshContainer, DXGFRAME* pFrame)
{
	// ソフトウェア使用
	if(pMeshContainer->UseSoftwareVP)
		m_pD3DDevice->SetSoftwareVertexProcessing(TRUE);

	// ブレンディング設定
	if(pMeshContainer->NumInfl == 1)
		m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
	else
		m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1);

	if(pMeshContainer->NumInfl > 0)
		m_pD3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

	D3DXMATRIXA16          mat;
	D3DXBONECOMBINATION*   pBoneComb = (D3DXBONECOMBINATION*)pMeshContainer->pBoneCombinationBuf->GetBufferPointer();
	ID3DXBaseMesh*         pBaseMesh = pMeshContainer->MeshData.pMesh;
	for(UINT iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
		for(UINT iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; iPaletteEntry++) {
			const UINT   iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
			if(iMatrixIndex != UINT_MAX) {
				::D3DXMatrixMultiply(&mat,
									 &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
									  pMeshContainer->ppBoneMatrixPtrs   [iMatrixIndex]);
				m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(iPaletteEntry), &mat);
			}
		}

		// マテリアル設定
		m_pD3DDevice->SetMaterial(  &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
		m_pD3DDevice->SetTexture (0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);

		// 描画
		pBaseMesh->DrawSubset(iAttrib);
	}

	// レンダーステート復元
	m_pD3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);

	if(pMeshContainer->UseSoftwareVP)
		m_pD3DDevice->SetSoftwareVertexProcessing(FALSE);
}

//------------------------------------------------------------------------------
//	メッシュコンテナ描画－ソフトウェア
//------------------------------------------------------------------------------
void CAnimationModel::DrawMeshContainerSoftware(DXGMESHCONTAINER* pMeshContainer, DXGFRAME* pFrame)
{
	// ボーン行列設定
	const DWORD BONES  = pMeshContainer->pSkinInfo->GetNumBones();
	for(DWORD i = 0; i < BONES; i++) {
		::D3DXMatrixMultiply(&m_pBoneMatrices[i],
							 &pMeshContainer->pBoneOffsetMatrices[i], 
							  pMeshContainer->ppBoneMatrixPtrs[i]);
	}

	D3DXMATRIX  Identity;
	::D3DXMatrixIdentity(&Identity);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &Identity);

	// スキンメッシュ更新
	ID3DXBaseMesh*   pBaseMesh     = pMeshContainer->MeshData.pMesh;
	void*            pVerticesSrc  = NULL;
	void*            pVerticesDest = NULL;
	pMeshContainer->pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, &pVerticesSrc);
	pBaseMesh->LockVertexBuffer(0, &pVerticesDest);

	pMeshContainer->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, pVerticesSrc, pVerticesDest);

	pMeshContainer->pOrigMesh->UnlockVertexBuffer();
	pBaseMesh->UnlockVertexBuffer();

	// 描画
	for(UINT iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
		m_pD3DDevice->SetMaterial(  &pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D);
		m_pD3DDevice->SetTexture (0, pMeshContainer->ppTextures[pMeshContainer->pAttributeTable[iAttrib].AttribId]);
		pBaseMesh->DrawSubset(pMeshContainer->pAttributeTable[iAttrib].AttribId);
	}
}

//------------------------------------------------------------------------------
//	フレーム描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawFrame(D3DXFRAME* pFrame, IEffect*& pEffect)
{
	D3DXMESHCONTAINER*  pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		// 最下層
		DrawMeshContainerIndexedHLSL((DXGMESHCONTAINER*)pMeshContainer, (DXGFRAME*)pFrame, pEffect);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		DrawFrame(pFrame->pFrameSibling,    pEffect);

	// 子ノード
	if (pFrame->pFrameFirstChild != NULL)
		DrawFrame(pFrame->pFrameFirstChild, pEffect);
}

//------------------------------------------------------------------------------
//	メッシュコンテナ描画－インデックスバッファHLSL
//------------------------------------------------------------------------------
void CAnimationModel::DrawMeshContainerIndexedHLSL(DXGMESHCONTAINER* pMeshContainer,
												   DXGFRAME* pFrame, IEffect*& pEffect)
{
	ID3DXBaseMesh*   pBaseMesh = pMeshContainer->MeshData.pMesh;
	if(pMeshContainer->pSkinInfo == NULL) {
		// 通常メッシュ
		pEffect->SetParameter(m_WorldMatrixArrayName, pFrame->CombinedTransformationMatrix);
		pEffect->SetParameter(m_NumBonesName, 0);

		const UINT       pass      = pEffect->Begin();
		for(UINT ps = 0; ps < pass; ps++) {
			pEffect->BeginPass(ps);

			// メッシュ描画
			for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
				pEffect  ->SetMaterial(pMeshContainer->pMaterials[i].MatD3D,
									   pMeshContainer->ppTextures[i]);
				pBaseMesh->DrawSubset(i);
			}

			pEffect->EndPass();
		}
		pEffect->End();
		return;
	}

	// スキンメッシュ
	D3DXBONECOMBINATION*   pBoneComb = (D3DXBONECOMBINATION*)pMeshContainer->pBoneCombinationBuf->GetBufferPointer();
	D3DXMATRIXA16          BoneMatrices[MAX_BONES];
	for(UINT iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
		for(UINT iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; iPaletteEntry++) {
			const UINT   MatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
			if(MatrixIndex != UINT_MAX)
				::D3DXMatrixMultiply(&BoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[MatrixIndex],
									 pMeshContainer->ppBoneMatrixPtrs[MatrixIndex]);
		}

		// エフェクトパラメータ設定
		pEffect->SetMatrixArray(m_WorldMatrixArrayName, BoneMatrices, pMeshContainer->NumPaletteEntries);
		pEffect->SetParameter  (m_NumBonesName, pMeshContainer->NumInfl - 1);
		pEffect->SetMaterial   (pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D,
								pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);

		// 描画
		const UINT   PASS = pEffect->Begin();
		for(UINT ps = 0; ps < PASS; ps++) {
			pEffect->BeginPass(ps);
			pBaseMesh->DrawSubset(iAttrib);
			pEffect->EndPass();
		}
		pEffect->End();
	}
}

//------------------------------------------------------------------------------
//	フレーム描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawFrame(D3DXFRAME* pFrame, IEffect*& pEffect, const UINT& inPass)
{
	D3DXMESHCONTAINER*  pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		// 最下層
		DrawMeshContainerIndexedHLSL((DXGMESHCONTAINER*)pMeshContainer, (DXGFRAME*)pFrame, pEffect);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		DrawFrame(pFrame->pFrameSibling,    pEffect, inPass);

	// 子ノード
	if (pFrame->pFrameFirstChild != NULL)
		DrawFrame(pFrame->pFrameFirstChild, pEffect, inPass);
}

//------------------------------------------------------------------------------
//	メッシュコンテナ描画－インデックスバッファHLSL
//------------------------------------------------------------------------------
void CAnimationModel::DrawMeshContainerIndexedHLSL(DXGMESHCONTAINER* pMeshContainer, DXGFRAME* pFrame,
												   IEffect*& pEffect, const UINT& inPass)
{
	ID3DXBaseMesh*   pBaseMesh = pMeshContainer->MeshData.pMesh;
	if(pMeshContainer->pSkinInfo == NULL) {
		// 通常メッシュ
		pEffect->SetParameter(m_WorldMatrixArrayName, pFrame->CombinedTransformationMatrix);
		pEffect->SetParameter(m_NumBonesName, 0);

		// メッシュ描画
		pEffect->BeginPass(inPass);

		for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
			pEffect  ->SetMaterial(pMeshContainer->pMaterials[i].MatD3D,
								   pMeshContainer->ppTextures[i]);
			pBaseMesh->DrawSubset(i);
		}

		pEffect->EndPass();

		return;
	}

	// スキンメッシュ
	pEffect->BeginPass(inPass);

	D3DXBONECOMBINATION*   pBoneComb = (D3DXBONECOMBINATION*)pMeshContainer->pBoneCombinationBuf->GetBufferPointer();
	D3DXMATRIXA16          BoneMatrices[MAX_BONES];
	for(UINT iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++) {
		for(UINT iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; iPaletteEntry++) {
			const UINT   MatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
			if(MatrixIndex != UINT_MAX)
				::D3DXMatrixMultiply(&BoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[MatrixIndex],
									 pMeshContainer->ppBoneMatrixPtrs[MatrixIndex]);
		}

		// エフェクトパラメータ設定
		pEffect->SetMatrixArray(m_WorldMatrixArrayName, BoneMatrices, pMeshContainer->NumPaletteEntries);
		pEffect->SetParameter  (m_NumBonesName, pMeshContainer->NumInfl - 1);
		if(pEffect->SetMaterial(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D,
								pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]) == false)
			pEffect->CommitChanges();

		// 描画
		pBaseMesh->DrawSubset(iAttrib);
	}

	pEffect  ->EndPass();
}

//------------------------------------------------------------------------------
//	アニメーションモデル半透明描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawAlpha(const float  inAlpha, const DWORD inDraw)
{
	if(inAlpha <= FLT_EPSILON)
		return;

	UpdateFrameMatrices(m_pFrameRoot, &GetWorldMatrix());

	// カラー出力保存
	DWORD   colorwrite_enable;
	m_pD3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite_enable);

	// 背面
	if((inDraw & DXGDRAWA_BACKFACE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		// ｚ設定
		if((inDraw & DXGDRAWA_BACKFACE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ｚ出力設定
			if((inDraw & (DXGDRAWA_BACKFACE_ZWRDEFER | DXGDRAWA_BACKFACE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		DrawAlphaFrame(m_pFrameRoot, inAlpha);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_BACKFACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			DrawFrame(m_pFrameRoot);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// 表面
	if((inDraw & DXGDRAWA_FACE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		// ｚ設定
		if((inDraw & DXGDRAWA_FACE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ｚ出力設定
			if((inDraw & (DXGDRAWA_FACE_ZWRDEFER | DXGDRAWA_FACE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		DrawAlphaFrame(m_pFrameRoot, inAlpha);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_FACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			DrawFrame(m_pFrameRoot);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// カリングなし
	if((inDraw & DXGDRAWA_CULLNONE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// ｚ設定
		if((inDraw & DXGDRAWA_CULLNONE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ｚ出力設定
			if((inDraw & (DXGDRAWA_CULLNONE_ZWRDEFER | DXGDRAWA_CULLNONE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		DrawAlphaFrame(m_pFrameRoot, inAlpha);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_CULLNONE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			DrawFrame(m_pFrameRoot);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// ｚ書込
	const DWORD   ZWRITE = inDraw & 0x3000;
	if(ZWRITE != 0) {
		if(ZWRITE == DXGDRAWA_ZWRITE_FACE)
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		// 表面
		else if(ZWRITE == DXGDRAWA_ZWRITE_CULLNONE)
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		// カリングなし
		else
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);		// 裏面

		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		DrawFrame(m_pFrameRoot);

		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
	}
}

//------------------------------------------------------------------------------
//	フレーム半透明描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawAlphaFrame(D3DXFRAME* pFrame, const float& inAlpha)
{
	D3DXMESHCONTAINER*  pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		// 最下層
		DrawAlphaMeshContainer(pMeshContainer, pFrame, inAlpha);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		DrawAlphaFrame(pFrame->pFrameSibling,    inAlpha);

	// 子ノード
	if (pFrame->pFrameFirstChild != NULL)
		DrawAlphaFrame(pFrame->pFrameFirstChild, inAlpha);
}

//------------------------------------------------------------------------------
//	メッシュコンテナ半透明描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawAlphaMeshContainer(D3DXMESHCONTAINER* pMeshContainerBase,
											 D3DXFRAME* pFrameBase, const float& inAlpha)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pMeshContainerBase;
    DXGFRAME*           pFrame         = (DXGFRAME*)pFrameBase;

	// マテリアル設定
	std::vector<float>   Diffuse(pMeshContainer->NumMaterials);
	for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
		Diffuse[i] = pMeshContainer->pMaterials[i].MatD3D.Diffuse.a;
		pMeshContainer->pMaterials[i].MatD3D.Diffuse.a *= inAlpha;
	}

	// メッシュコンテナ描画
	if(pMeshContainer->pSkinInfo != NULL) {
		// スキンメッシュ
		switch(m_SkinningMethod) {
		  case DXGSKMTD_NONINDEXED:  DrawMeshContainerNonIndexed(pMeshContainer, pFrame);	break;
		  case DXGSKMTD_INDEXED:     DrawMeshContainerIndexed   (pMeshContainer, pFrame);	break;
		  case DXGSKMTD_SOFTWARE:    DrawMeshContainerSoftware  (pMeshContainer, pFrame);	break;
		}
	} else {
		// 通常メッシュ
		ID3DXBaseMesh*   pBaseMesh = pMeshContainer->MeshData.pMesh;
        m_pD3DDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);
        for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
            m_pD3DDevice->SetMaterial(  &pMeshContainer->pMaterials[i].MatD3D);
            m_pD3DDevice->SetTexture (0, pMeshContainer->ppTextures[i]);
            pBaseMesh->DrawSubset(i);
        }
	}

	// マテリアル復元
	for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++)
		pMeshContainer->pMaterials[i].MatD3D.Diffuse.a = Diffuse[i];
}

//------------------------------------------------------------------------------
//	アニメーションモデル影描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth)
{
	// 変換行列設定
	D3DXMATRIX   ShadowMatrix = GetWorldMatrix() * inShadowMatrix;
	UpdateFrameMatrices(m_pFrameRoot, &ShadowMatrix);

	// 影描画
	DrawShadowFrame(m_pFrameRoot, inDepth);
}

//------------------------------------------------------------------------------
//	フレーム影描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawShadowFrame(D3DXFRAME* pFrame, const float& inDepth)
{
	D3DXMESHCONTAINER*  pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		// 最下層
		DrawShadowMeshContainer(pMeshContainer, pFrame, inDepth);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		DrawShadowFrame(pFrame->pFrameSibling,    inDepth);

	// 子ノード
	if (pFrame->pFrameFirstChild != NULL)
		DrawShadowFrame(pFrame->pFrameFirstChild, inDepth);
}

//------------------------------------------------------------------------------
//	メッシュコンテナ影描画
//------------------------------------------------------------------------------
void CAnimationModel::DrawShadowMeshContainer(D3DXMESHCONTAINER* pMeshContainerBase,
											  D3DXFRAME* pFrameBase, const float& inDepth)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pMeshContainerBase;
    DXGFRAME*           pFrame         = (DXGFRAME*)pFrameBase;

	// マテリアル設定
	std::vector<float>                Diffuse(pMeshContainer->NumMaterials);
	std::vector<IDirect3DTexture9*>   Texture(pMeshContainer->NumMaterials);
	for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
		Diffuse[i] = pMeshContainer->pMaterials[i].MatD3D.Diffuse.a;
		Texture[i] = pMeshContainer->ppTextures[i];

		pMeshContainer->pMaterials[i].MatD3D.Diffuse.a = inDepth;
		pMeshContainer->ppTextures[i]                  = NULL;
	}

	// メッシュコンテナ描画
	if(pMeshContainer->pSkinInfo != NULL) {
		// スキンメッシュ
		switch(m_SkinningMethod) {
		  case DXGSKMTD_NONINDEXED:  DrawMeshContainerNonIndexed(pMeshContainer, pFrame);	break;
		  case DXGSKMTD_INDEXED:     DrawMeshContainerIndexed   (pMeshContainer, pFrame);	break;
		  case DXGSKMTD_SOFTWARE:    DrawMeshContainerSoftware  (pMeshContainer, pFrame);	break;
		}
	} else {
		// 通常メッシュ
		ID3DXBaseMesh*   pBaseMesh = pMeshContainer->MeshData.pMesh;
        m_pD3DDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);
        for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
            m_pD3DDevice->SetMaterial(  &pMeshContainer->pMaterials[i].MatD3D);
            m_pD3DDevice->SetTexture (0, pMeshContainer->ppTextures[i]);
            pBaseMesh->DrawSubset(i);
        }
	}

	// マテリアル復元
	for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
		pMeshContainer->pMaterials[i].MatD3D.Diffuse.a = Diffuse[i];
		pMeshContainer->ppTextures[i]                  = Texture[i];
	}
}

//------------------------------------------------------------------------------
//	スキンメッシュ生成
//------------------------------------------------------------------------------
HRESULT CAnimationModel::GenerateSkinnedMesh(DXGMESHCONTAINER* pMeshContainer)
{
	// スキンメッシュなし
    if(pMeshContainer->pSkinInfo == NULL)
        return S_OK;

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	ID3DXBaseMesh*   pBaseMesh = pMeshContainer->MeshData.pMesh;
    SafeRelease(pBaseMesh);
    SafeRelease(pMeshContainer->pBoneCombinationBuf);

	switch(m_SkinningMethod) {
	  case DXGSKMTD_NONINDEXED:  return GenerateSkinnedMeshNonIndexed (pMeshContainer);
	  case DXGSKMTD_INDEXED:     return GenerateSkinnedMeshIndexed    (pMeshContainer);
	  case DXGSKMTD_INDEXEDHLSL: return GenerateSkinnedMeshIndexedHLSL(pMeshContainer);
	  case DXGSKMTD_SOFTWARE:    return GenerateSkinnedMeshSoftware   (pMeshContainer);
	  default:					 return E_INVALIDARG;
	}

	return S_OK;
}

//------------------------------------------------------------------------------
//	スキンメッシュ生成－通常バッファ
//------------------------------------------------------------------------------
HRESULT CAnimationModel::GenerateSkinnedMeshNonIndexed(DXGMESHCONTAINER* pMeshContainer)
{
	HRESULT   hr;

	// メッシュ変換
	hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh(pMeshContainer->pOrigMesh,
														 D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE, 
														 pMeshContainer->pAdjacency,
														 NULL, NULL, NULL,
														&pMeshContainer->NumInfl,
														&pMeshContainer->NumAttributeGroups,
														&pMeshContainer->pBoneCombinationBuf,
														&pMeshContainer->MeshData.pMesh);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - メッシュ変換失敗(CAnimationModel_GenerateSkinnedMeshNonIndexed)\n"));
		return hr;
	}

	// デバイスが2つの行列しかブレンディングできない場合、ソフトウェアで描画する
	D3DXBONECOMBINATION*   pBoneCombinations = (D3DXBONECOMBINATION*)pMeshContainer->pBoneCombinationBuf->GetBufferPointer();
	for(pMeshContainer->iAttributeSW = 0;
		pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups;
		pMeshContainer->iAttributeSW++) {

		DWORD cInfl = 0;
		for(DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++) {
			if(pBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
				++cInfl;
		}

		if(cInfl > m_MaxVertexBlendMatrices)
			break;
	}

	// ハードウェア、ソフトウェア混在
	if(pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups) {
		ID3DXMesh*   pMeshTmp;
		hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING | pMeshContainer->MeshData.pMesh->GetOptions(), 
														  pMeshContainer->MeshData.pMesh->GetFVF(),
														  m_pD3DDevice, &pMeshTmp);
		if(hr != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - メッシュ生成失敗(CAnimationModel_GenerateSkinnedMeshNoIndexed)\n"));
			SafeRelease(pMeshContainer->MeshData.pMesh);
			return hr;
		}

		pMeshContainer->MeshData.pMesh->Release();
		pMeshContainer->MeshData.pMesh = pMeshTmp;
	}

	// 最適化
	DWORD*   pAdjacency = new DWORD[pMeshContainer->MeshData.pMesh->GetNumFaces() * 3];
	pMeshContainer->MeshData.pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency);
	pMeshContainer->MeshData.pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
													pAdjacency, NULL, NULL, NULL);
	delete[] pAdjacency;

	return S_OK;
}

//------------------------------------------------------------------------------
//	スキンメッシュ生成－インデックスバッファ
//------------------------------------------------------------------------------
HRESULT CAnimationModel::GenerateSkinnedMeshIndexed(DXGMESHCONTAINER* pMeshContainer)
{
	HRESULT   hr;

	// インデックスバッファ取得
	IDirect3DIndexBuffer9*   pIB;
	hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - インデックスバッファ取得失敗(CAnimationModel_GenerateSkinnedMeshIndexed)\n"));
		return hr;
	}

	// 面影響最大値設定
	DWORD   NumMaxFaceInfl;
    hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl);
    pIB->Release();
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 面影響最大値取得失敗(CAnimationModel_GenerateSkinnedMeshIndexed)\n"));
		return hr;
	}
	NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

	DWORD   Flags = D3DXMESHOPT_VERTEXCACHE;
	if(m_MaxVertexBlendMatrixIndex == 0 || m_MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl) {
		pMeshContainer->UseSoftwareVP     = true;
		pMeshContainer->NumPaletteEntries = min(pMeshContainer->pSkinInfo->GetNumBones(), 256);
		Flags |= D3DXMESH_SYSTEMMEM;
	} else {
		pMeshContainer->UseSoftwareVP     = false;
		pMeshContainer->NumPaletteEntries = min((m_MaxVertexBlendMatrixIndex + 1) / 2,
												pMeshContainer->pSkinInfo->GetNumBones());
		Flags |= D3DXMESH_MANAGED;
	}

	// メッシュ変換
	hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh(pMeshContainer->pOrigMesh,
																Flags, 
																pMeshContainer->NumPaletteEntries, 
																pMeshContainer->pAdjacency, 
																NULL, NULL, NULL, 
															   &pMeshContainer->NumInfl,
															   &pMeshContainer->NumAttributeGroups, 
															   &pMeshContainer->pBoneCombinationBuf, 
															   &pMeshContainer->MeshData.pMesh);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - メッシュ変換失敗(CAnimationModel_GenerateSkinnedMeshIndexed)\n"));
		return hr;
	}

	// 最適化
	DWORD*   pAdjacency = new DWORD[pMeshContainer->MeshData.pMesh->GetNumFaces() * 3];
	pMeshContainer->MeshData.pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency);
	pMeshContainer->MeshData.pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
													pAdjacency, NULL, NULL, NULL);
	delete[] pAdjacency;

	return S_OK;
}

//------------------------------------------------------------------------------
//	スキンメッシュ生成－インデックスバッファHLSL
//------------------------------------------------------------------------------
HRESULT CAnimationModel::GenerateSkinnedMeshIndexedHLSL(DXGMESHCONTAINER* pMeshContainer)
{
	SafeRelease(pMeshContainer->pBoneCombinationBuf);

	ID3DXSkinInfo* pSkinInfo = pMeshContainer->pSkinInfo;
	if(pSkinInfo == NULL) {
		::OutputDebugString(TEXT("*** Error - スキン情報なし(CAnimationModel_GenerateSkinnedMeshIndexedHLSL)\n"));
		return E_INVALIDARG;
	}

	// 最大行列数設定
	pMeshContainer->NumPaletteEntries = min(MAX_BONES, pMeshContainer->pSkinInfo->GetNumBones());

	// メッシュ変換
	HRESULT   hr;
	hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh(pMeshContainer->pOrigMesh,
																D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
																pMeshContainer->NumPaletteEntries,
																pMeshContainer->pAdjacency,
																NULL, NULL, NULL,
															   &pMeshContainer->NumInfl,
															   &pMeshContainer->NumAttributeGroups,
															   &pMeshContainer->pBoneCombinationBuf,
															   &pMeshContainer->MeshData.pMesh);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - メッシュ変換失敗(CAnimationModel_GenerateSkinnedMeshIndexedHLSL)\n"));
		return hr;
	}

	// 最適化
	DWORD*   pAdjacency = new DWORD[pMeshContainer->MeshData.pMesh->GetNumFaces() * 3];
	pMeshContainer->MeshData.pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency);
	pMeshContainer->MeshData.pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
													pAdjacency, NULL, NULL, NULL);
	delete[] pAdjacency;

	return S_OK;
}

//------------------------------------------------------------------------------
//	スキンメッシュ生成－ソフトウェア
//------------------------------------------------------------------------------
HRESULT CAnimationModel::GenerateSkinnedMeshSoftware(DXGMESHCONTAINER* pMeshContainer)
{
	HRESULT   hr;

	// メッシュ生成
	hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED,
												 pMeshContainer->pOrigMesh->GetFVF(),
												 m_pD3DDevice, &pMeshContainer->MeshData.pMesh);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - メッシュ生成失敗(CAnimationModel_GenerateSkinnedMeshSoftware)\n"));
		return hr;
	}

/*
	// 最適化
	DWORD*   pAdjacency = new DWORD[pMeshContainer->MeshData.pMesh->GetNumFaces() * 3];
	pMeshContainer->MeshData.pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency);
	ID3DXBuffer*   buf;
	pMeshContainer->MeshData.pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
													pAdjacency, NULL, NULL, &buf);
	delete[] pAdjacency;

	pMeshContainer->pSkinInfo->Remap(pMeshContainer->MeshData.pMesh->GetNumVertices(),
									 (DWORD*)buf->GetBufferPointer());
	SafeRelease(buf);
*/

	// 属性数取得
	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 属性数取得失敗(CAnimationModel_GenerateSkinnedMeshSoftware)\n"));
		SafeRelease(pMeshContainer->MeshData.pMesh);
		return hr;
	}

	// 属性配列生成
	delete[] pMeshContainer->pAttributeTable;
	pMeshContainer->pAttributeTable  = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
	if(pMeshContainer->pAttributeTable == NULL) {
		::OutputDebugString(TEXT("*** Error - 属性配列生成失敗(CAnimationModel_GenerateSkinnedMeshSoftware)\n"));
		SafeRelease(pMeshContainer->MeshData.pMesh);
		return E_OUTOFMEMORY;
	}

	// 属性取得
	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 属性取得失敗(CAnimationModel_GenerateSkinnedMeshSoftware)\n"));
		SafeRelease(pMeshContainer->MeshData.pMesh);
		return hr;
	}

	// ボーン行列配列生成
	if(m_NumBoneMatrices < pMeshContainer->pSkinInfo->GetNumBones()) {
		m_NumBoneMatrices = pMeshContainer->pSkinInfo->GetNumBones();
		delete[] m_pBoneMatrices; 
		m_pBoneMatrices = new D3DXMATRIXA16[m_NumBoneMatrices];
		if(m_pBoneMatrices == NULL) {
			::OutputDebugString(TEXT("*** Error - ボーン行列配列生成失敗(CAnimationModel_GenerateSkinnedMeshSoftware)\n"));
			SafeRelease(pMeshContainer->MeshData.pMesh );
			DeleteArray(pMeshContainer->pAttributeTable);
			return E_OUTOFMEMORY;
		}
	}

	return S_OK;
}

//------------------------------------------------------------------------------
//	方向設定
//------------------------------------------------------------------------------
void CAnimationModel::SetDirection(const float inX, const float inY, const float inZ)
{
	::D3DXQuaternionIdentity(&m_Rotation);

	// 角軸のクォータニオンを求める
	D3DXQUATERNION   qt;

	// ロール
	if(inZ != 0.0f) {
		qt.x = 0.0f;
		qt.y = 0.0f;
		qt.z = ::sinf(D3DXToRadian(inZ) / 2.0f);
		qt.w = ::cosf(D3DXToRadian(inZ) / 2.0f);
		m_Rotation *= qt;
	}

	// ピッチ
	if(inX != 0.0f) {
		qt.x = ::sinf(D3DXToRadian(inX) / 2.0f);
		qt.y = 0.0f;
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inX) / 2.0f);
		m_Rotation *= qt;
	}

	// ヨー
	if(inY != 0.0f) {
		qt.x = 0.0f;
		qt.y = ::sinf(D3DXToRadian(inY) / 2.0f);
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inY) / 2.0f);
		m_Rotation *= qt;
	}
/*
	// 回転クォータニオンを求める
	::D3DXQuaternionRotationYawPitchRoll(&m_Rotation, D3DXToRadian(inY),
													  D3DXToRadian(inX),
													  D3DXToRadian(inZ));
*/
}

//------------------------------------------------------------------------------
//	移動
//------------------------------------------------------------------------------
void CAnimationModel::Move(const float inRight, const float inUp, const float inFront)
{
	// 回転行列生成
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Rotation);

	// 方向ベクトルを取り出す
	D3DXVECTOR3*   right = (D3DXVECTOR3*)rotation.m[0];
	D3DXVECTOR3*   up    = (D3DXVECTOR3*)rotation.m[1];
	D3DXVECTOR3*   front = (D3DXVECTOR3*)rotation.m[2];

	// 移動
	*right     *= inRight;
	*up        *= inUp;
	*front     *= inFront;
	m_Position += *front + *up + *right;
}

//------------------------------------------------------------------------------
//	回転
//------------------------------------------------------------------------------
void CAnimationModel::Rotation(const float inX, const float inY, const float inZ)
{
	// クォータニオンから軸を求める
	D3DXMATRIX   axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXQUATERNION   qt;

	// ヨー
	if (inY != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[1], D3DXToRadian(inY));
		m_Rotation *= qt;
	}

	// ピッチ
	if (inX != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[0], D3DXToRadian(inX));
		m_Rotation *= qt;
	}

	// ロール
	if (inZ != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[2], D3DXToRadian(inZ));
		m_Rotation *= qt;
	}
}

//------------------------------------------------------------------------------
//	視点の正面を向くように回転
//------------------------------------------------------------------------------
void CAnimationModel::SetRotationBillboard(const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle)
{
	m_Rotation = -inAngle;

	// 視点の方向へ向ける
	D3DXVECTOR3   dir = m_Position - inEye;
	float   angleY = ::atan2f(dir.z, dir.x);

	D3DXQUATERNION   qt;
	qt.x = 0.0f;
	qt.y = ::sinf(angleY / 2.0f);
	qt.z = 0.0f;
	qt.w = ::cosf(angleY / 2.0f);

	m_Rotation *= qt;
}

//------------------------------------------------------------------------------
//	視点の正面を向くようにY軸を回転させる
//------------------------------------------------------------------------------
void CAnimationModel::SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	D3DXVECTOR3   dir;

	// ｚ方向を視点ベクトルと一致させる
	// ｚ軸設定
	dir = -inFront;
	::D3DXVec3Normalize(z_axis, &dir);

	// ｘ軸設定
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// 回転クォータニオンに変換
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);

	// 視点の方向へ向ける
	dir = m_Position - inEye;
	float   angleY = ::atan2f(dir.z, dir.x);

	D3DXQUATERNION   qt;
	qt.x = 0.0f;
	qt.y = ::sinf(angleY / 2.0f);
	qt.z = 0.0f;
	qt.w = ::cosf(angleY / 2.0f);

	m_Rotation *= qt;
}

//------------------------------------------------------------------------------
//	ｙ軸を法線の傾きに合わせる
//------------------------------------------------------------------------------
void CAnimationModel::AlignmentAxisY(const D3DXVECTOR3& inAxisY)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	// ｙ軸設定
	::D3DXVec3Normalize(y_axis, &inAxisY);

	// ｘ軸設定
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// ｚ軸設定
//	::D3DXVec3Cross(z_axis, x_axis, y_axis);

	// 軸を回転クォータニオンに変換
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);
}

//------------------------------------------------------------------------------
//	ボーンオフセット行列設定
//------------------------------------------------------------------------------
void CAnimationModel::SetBoneOffsetMatrix(const DWORD inIdx, D3DXMATRIX& inMatrix)
{
#ifdef _DEBUG
	if(inIdx >= m_NumBoneMatrices)
		return;
#endif
	m_pBoneOffsetMatrices[inIdx] = inMatrix;
}

//------------------------------------------------------------------------------
//	ボーンオフセット行列設定
//------------------------------------------------------------------------------
void CAnimationModel::SetBoneOffsetMatrices(D3DXMATRIX inMatrices[])
{
	::CopyMemory(m_pBoneOffsetMatrices, inMatrices, sizeof(D3DXMATRIX) * m_NumBoneMatrices);
}

//------------------------------------------------------------------------------
//	ボーンオフセット行列乗算
//------------------------------------------------------------------------------
void CAnimationModel::MultiplyBoneOffsetMatrix(const DWORD inIdx, D3DXMATRIX& inMatrix)
{
#ifdef _DEBUG
	if(inIdx >= m_NumBoneMatrices)
		return;
#endif
	m_pBoneOffsetMatrices[inIdx] *= inMatrix;
}

//------------------------------------------------------------------------------
//	マテリアル設定
//------------------------------------------------------------------------------
void CAnimationModel::SetMaterial(const D3DMATERIAL9& inMaterial)
{
	SetMaterial(m_pFrameRoot, inMaterial);
}

//------------------------------------------------------------------------------
//	マテリアル設定
//------------------------------------------------------------------------------
void CAnimationModel::SetMaterial(D3DXFRAME* pFrame,const D3DMATERIAL9& inMaterial)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;
	while(pMeshContainer != NULL) {
		// 最下層
		for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
			pMeshContainer->pMaterials[i].MatD3D = inMaterial;
		}

		// 次のコンテナへ
		pMeshContainer = (DXGMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		SetMaterial(pFrame->pFrameSibling,    inMaterial);

	// 子ノード
	if(pFrame->pFrameFirstChild != NULL)
		SetMaterial(pFrame->pFrameFirstChild, inMaterial);
}

//------------------------------------------------------------------------------
//	テクスチャ変更
//------------------------------------------------------------------------------
void CAnimationModel::ChangeTexture(LPCSTR inMasterTexture, LPCSTR inNewTexture)
{
	if(inMasterTexture == NULL)
		return;

	// 検索ファイル名を小文字に変換
	std::string   MasterTexture = inMasterTexture;
	::CharLowerA((LPSTR)MasterTexture.c_str());

	ChangeTexture(m_pFrameRoot, MasterTexture, inNewTexture);
}

//------------------------------------------------------------------------------
//	テクスチャ変更
//------------------------------------------------------------------------------
void CAnimationModel::ChangeTexture(D3DXFRAME* pFrame,
									const std::string& inMasterTexture,
									LPCSTR inNewTexture)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;
	while(pMeshContainer != NULL) {
		// 最下層
		for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
			if(pMeshContainer->pMaterials[i].pTextureFilename == NULL)
				continue;

			if(pMeshContainer->TextureName[i] != inMasterTexture)
				continue;

			if(inNewTexture == NULL) {
				SafeRelease(pMeshContainer->ppTextures[i]);
				continue;
			}

			// テクスチャ読み込み
			IDirect3DTexture9*   texture;
			if(::D3DXCreateTextureFromFileA(m_pD3DDevice, inNewTexture, &texture) == D3D_OK) {
				SafeRelease(pMeshContainer->ppTextures[i]);
				pMeshContainer->ppTextures[i] = texture;
			} else {
				::OutputDebugString(TEXT("*** Error - テクスチャ読み込み失敗(CAnimationModel_ChangeTexture)\n"));
			}
		}

		// 次のコンテナへ
		pMeshContainer = (DXGMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		ChangeTexture(pFrame->pFrameSibling,    inMasterTexture, inNewTexture);

	// 子ノード
	if(pFrame->pFrameFirstChild != NULL)
		ChangeTexture(pFrame->pFrameFirstChild, inMasterTexture, inNewTexture);
}

//------------------------------------------------------------------------------
//	テクスチャ変更
//------------------------------------------------------------------------------
void CAnimationModel::ChangeTexture(LPCSTR inMasterTexture, IDirect3DTexture9* pTexture)
{
	if(inMasterTexture == NULL)
		return;

	// 検索ファイル名を小文字に変換
	std::string   MasterTexture = inMasterTexture;
	::CharLowerA((LPSTR)MasterTexture.c_str());

	ChangeTexture(m_pFrameRoot, MasterTexture, pTexture);
}

//------------------------------------------------------------------------------
//	テクスチャ変更
//------------------------------------------------------------------------------
void CAnimationModel::ChangeTexture(D3DXFRAME* pFrame,
									const std::string& inMasterTexture,
									IDirect3DTexture9* pTexture)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;
	while(pMeshContainer != NULL) {
		// 最下層
		for(DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
			if(pMeshContainer->pMaterials[i].pTextureFilename == NULL)
				continue;

			if(pMeshContainer->TextureName[i] != inMasterTexture)
				continue;

			// テクスチャ設定
			SafeRelease(pMeshContainer->ppTextures[i]);
			if(pTexture != NULL) {
				pMeshContainer->ppTextures[i] = pTexture;
				pMeshContainer->ppTextures[i]->AddRef();
			}
		}

		// 次のコンテナへ
		pMeshContainer = (DXGMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		ChangeTexture(pFrame->pFrameSibling,    inMasterTexture, pTexture);

	// 子ノード
	if (pFrame->pFrameFirstChild != NULL)
		ChangeTexture(pFrame->pFrameFirstChild, inMasterTexture, pTexture);
}

//------------------------------------------------------------------------------
//	回転角取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CAnimationModel::GetDirection() const
{
	D3DXVECTOR3   rotation = GetDirectionRadian();

	rotation.x = D3DXToDegree(rotation.x);
	rotation.y = D3DXToDegree(rotation.y);
	rotation.z = D3DXToDegree(rotation.z);

	return rotation;
}

//------------------------------------------------------------------------------
//	回転角取得(ラジアン)
//------------------------------------------------------------------------------
D3DXVECTOR3 CAnimationModel::GetDirectionRadian() const
{
	D3DXVECTOR3   rotation;

	rotation.x = ::asinf(m_Rotation.x) * 2.0f;
	rotation.y = ::asinf(m_Rotation.y) * 2.0f;
	rotation.z = ::asinf(m_Rotation.z) * 2.0f;

	return rotation;
/*
	// クォータニオン→回転行列
	D3DXMATRIX    m;
	::D3DXMatrixRotationQuaternion(&m, &m_Rotation);

	// ｘ, y軸取得
	D3DXVECTOR3   x_axis(m._11, m._21, m._31);
	D3DXVECTOR3   y_axis(m._12, m._22, m._32);

	// 回転角取得
	D3DXVECTOR3   rotation;

	// ｚ回転角
	rotation.z = ::atan2f(-y_axis.x, y_axis.y);

	// ｘ回転角
	const float  cos_z = ::cosf(rotation.z);
	rotation.x = ::atan2f(cos_z * -y_axis.z, ::fabs(y_axis.y));

	// ｙ回転角
	const float  sin_z = ::sinf(rotation.z);
	const float  cos_x = ::cosf(rotation.x);
	rotation.y = ::atan2(cos_x * -(sin_z * y_axis.y - cos_x * y_axis.x), y_axis.z);

	return rotation;
*/
}

//------------------------------------------------------------------------------
//	ボーンオフセット行列取得
//------------------------------------------------------------------------------
D3DXMATRIX CAnimationModel::GetBoneOffsetMatrix(const DWORD inIdx) const
{
#ifdef _DEBUG
	if(inIdx >= m_NumBoneMatrices) {
		D3DXMATRIX   mat;
		::ZeroMemory(&mat, sizeof(mat));
	}
#endif
	return m_pBoneOffsetMatrices[inIdx];
}

//------------------------------------------------------------------------------
//	ボーンオフセット行列取得
//------------------------------------------------------------------------------
void CAnimationModel::GetBoneOffsetMatrices(D3DXMATRIX outMatrices[]) const
{
	::CopyMemory(outMatrices, m_pBoneOffsetMatrices, sizeof(D3DXMATRIX) * m_NumBoneMatrices);
}

//------------------------------------------------------------------------------
//	ワールド変換行列生成
//------------------------------------------------------------------------------
D3DXMATRIX CAnimationModel::GetWorldMatrix() const
{
	// ワールド行列設定
	D3DXMATRIX   world;

	// 回転行列生成
	::D3DXMatrixRotationQuaternion(&world, &m_Rotation);

	// スケーリング
	world._11 *= m_Scale.x;
	world._12 *= m_Scale.x;
	world._13 *= m_Scale.x;

	world._21 *= m_Scale.y;
	world._22 *= m_Scale.y;
	world._23 *= m_Scale.y;

	world._31 *= m_Scale.z;
	world._32 *= m_Scale.z;
	world._33 *= m_Scale.z;

	// 移動
	world._41  = m_Position.x;
	world._42  = m_Position.y;
	world._43  = m_Position.z;

	world._44  = 1.0f;

	return world;
}

//------------------------------------------------------------------------------
//	前方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CAnimationModel::GetFrontVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[2];
}

//------------------------------------------------------------------------------
//	右方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CAnimationModel::GetRightVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[0];
}

//------------------------------------------------------------------------------
//	上方ベクトル取得
//------------------------------------------------------------------------------
D3DXVECTOR3 CAnimationModel::GetUpVector() const
{
	// クォータニオン→回転行列
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[1];
}

//------------------------------------------------------------------------------
//	境界球生成
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CAnimationModel::GetBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere(GetBaseBoundingSphere());

	float   scale = m_Scale.x;
	if(scale < m_Scale.y)
		scale = m_Scale.y;
	if(scale < m_Scale.z)
		scale = m_Scale.z;

	shpere.Center += m_Position;
	shpere.Radius *= scale;

	return shpere;
}

//------------------------------------------------------------------------------
//	基本境界球生成
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CAnimationModel::GetBaseBoundingSphere()
{
	// 境界球生成
	DXGBOUNDINGSPHERE   shpere;
	::D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &shpere.Center, &shpere.Radius);

	return shpere;
}

//------------------------------------------------------------------------------
//	OBB生成
//------------------------------------------------------------------------------
DXGOBB CAnimationModel::GetOBB()
{
	DXGOBB   obb(GetBaseOBB());
	if(m_SkinningMethod == DXGSKMTD_SOFTWARE)
		return obb;

	// 中心座標設定
	obb.Center += m_Position;

	// 各軸の傾き設定
	D3DXMATRIX   rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);
	obb.Axis[0] = D3DXVECTOR3(rot._11, rot._12, rot._13);
	obb.Axis[1] = D3DXVECTOR3(rot._21, rot._22, rot._23);
	obb.Axis[2] = D3DXVECTOR3(rot._31, rot._32, rot._33);

	// 各軸の長さ設定
	obb.Radius.x *= m_Scale.x;
	obb.Radius.y *= m_Scale.y;
	obb.Radius.z *= m_Scale.z;

	return obb;
}

//------------------------------------------------------------------------------
//	基本OBB生成
//------------------------------------------------------------------------------
DXGOBB CAnimationModel::GetBaseOBB()
{
	D3DXVECTOR3   pos_max(FLT_MIN, FLT_MIN, FLT_MIN);
	D3DXVECTOR3   pos_min(FLT_MAX, FLT_MAX, FLT_MAX);
	GetBaseOBB(m_pFrameRoot, pos_max, pos_min);

	DXGOBB   obb;

	// 中心点設定
	obb.Center = (pos_max + pos_min) * 0.5f;

	// 各軸の長さの半分を設定
	obb.Radius.x = fabsf(pos_max.x - pos_min.x) * 0.5f;
	obb.Radius.y = fabsf(pos_max.y - pos_min.y) * 0.5f;
	obb.Radius.z = fabsf(pos_max.z - pos_min.z) * 0.5f;

	return obb;
}

//------------------------------------------------------------------------------
//	基本OBB生成
//------------------------------------------------------------------------------
void CAnimationModel::GetBaseOBB(D3DXFRAME* pFrame, D3DXVECTOR3& ioMax, D3DXVECTOR3& ioMin)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;
	while(pMeshContainer != NULL) {
		// 最下層
		// 頂点バッファロック
		BYTE*   pVertices;
		if(pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, (LPVOID*)&pVertices) != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - 頂点バッファロック失敗(CAnimationModel_GetOBB)\n"));
			return;
		}

		// 各軸の最大、最小座標を求める
		const int   NumVertices = pMeshContainer->MeshData.pMesh->GetNumVertices();
		const int   STRIDE      = ::D3DXGetFVFVertexSize(pMeshContainer->MeshData.pMesh->GetFVF());
		for(int i = 0; i < NumVertices; i++) {
			D3DXVECTOR3   pos = *(D3DXVECTOR3*)pVertices;
			if(pos.x < ioMin.x)	ioMin.x = pos.x;
			if(pos.x > ioMax.x)	ioMax.x = pos.x;
			if(pos.y < ioMin.y)	ioMin.y = pos.y;
			if(pos.y > ioMax.y)	ioMax.y = pos.y;
			if(pos.z < ioMin.z)	ioMin.z = pos.z;
			if(pos.z > ioMax.z)	ioMax.z = pos.z;

			pVertices += STRIDE;
		}

		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

		// 次のコンテナへ
		pMeshContainer = (DXGMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
	}

	// 兄弟ノード
	if(pFrame->pFrameSibling != NULL)
		GetBaseOBB(pFrame->pFrameSibling,    ioMax, ioMin);

	// 子ノード
	if (pFrame->pFrameFirstChild != NULL)
		GetBaseOBB(pFrame->pFrameFirstChild, ioMax, ioMin);
}

//------------------------------------------------------------------------------
//	オクルージョンクエリ取得
//------------------------------------------------------------------------------
DWORD CAnimationModel::GetOcclusionQuery()
{
	IDirect3DQuery9*   pQuery;
	const HRESULT   hr = m_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - オクルージョンクエリ取得失敗(CModel_GetOcclusionQuery)\n"));
		return 0xffffffff;
	}

	pQuery->Issue(D3DISSUE_BEGIN);
	Draw();
	pQuery->Issue(D3DISSUE_END  );

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	DWORD   drawn = 0xffffffff;
	while(pQuery->GetData(&drawn, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE)
		;	// none

	pQuery->Release();

	return drawn;
}

//------------------------------------------------------------------------------
//	オクルージョンクエリ取得
//------------------------------------------------------------------------------
DWORD CAnimationModel::GetOcclusionQuery(IEffect*& pEffect)
{
	IDirect3DQuery9*   pQuery;
	const HRESULT   hr = m_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - オクルージョンクエリ取得失敗(CModel_GetOcclusionQuery)\n"));
		return 0xffffffff;
	}

	pQuery->Issue(D3DISSUE_BEGIN);
	Draw(pEffect);
	pQuery->Issue(D3DISSUE_END  );

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	DWORD   drawn = 0xffffffff;
	while(pQuery->GetData(&drawn, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE)
		;	// none

	pQuery->Release();

	return drawn;
}

//------------------------------------------------------------------------------
//	トラックループタイム設定
//------------------------------------------------------------------------------
void CAnimationModel::SetTrackLoopTime(const UINT inTrack, const float inLoopTime)
{
	// アニメーションセット取得
	ID3DXAnimationSet*   pAnimeSet = NULL;
	m_pAnimationController->GetAnimationSet(inTrack, &pAnimeSet);
	if(pAnimeSet == NULL) {
		::OutputDebugString(TEXT("*** Error - アニメーションセット取得失敗(CAnimationModel_SetTrackLoopTime)\n"));
		return;
	}

	// トラックループタイム設定
	m_pAnimationController->SetTrackSpeed(inTrack, (float)pAnimeSet->GetPeriod() / inLoopTime);

	pAnimeSet->Release();
}

//------------------------------------------------------------------------------
//	トラックループモード設定
//------------------------------------------------------------------------------
void CAnimationModel::SetTrackLoopMode(const UINT inTrack, const D3DXPLAYBACK_TYPE inLoopType)
{
	HRESULT   hr;

	// アニメーションセット取得
	ID3DXAnimationSet*   pAnimeSet = NULL;
	m_pAnimationController->GetAnimationSet(inTrack, &pAnimeSet);
	if(pAnimeSet == NULL) {
		::OutputDebugString(TEXT("*** Error - アニメーションセット取得失敗(CAnimationModel_SetTrackLoopMode)\n"));
		return;
	}

	// キーフレームアニメーションセット取得
	ID3DXKeyframedAnimationSet*   pKeyFramedAnimeSet;
	hr = pAnimeSet->QueryInterface(IID_ID3DXKeyframedAnimationSet, (LPVOID*)&pKeyFramedAnimeSet);
	pAnimeSet->Release();
	if(hr != S_OK) {
		::OutputDebugString(TEXT("*** Error - キーフレームアニメーションセット取得失敗(CAnimationModel_SetTrackLoopMode)\n"));
		return;
	}

	// ループモードチェック
	if(pKeyFramedAnimeSet->GetPlaybackType() == inLoopType) {
		pKeyFramedAnimeSet->Release();
		return;
	}

	// キーフレームアニメーション情報取得
	LPCSTR         NAME       = pKeyFramedAnimeSet->GetName();
	const double   TPS        = pKeyFramedAnimeSet->GetSourceTicksPerSecond();
	const UINT     ANIMATIONS = pKeyFramedAnimeSet->GetNumAnimations();

	// キーフレームアニメーションセット生成
	ID3DXKeyframedAnimationSet*   pNewKeyFramedAnimeSet;
	if(::D3DXCreateKeyframedAnimationSet(NAME, TPS, inLoopType, ANIMATIONS,
										 0, NULL, &pNewKeyFramedAnimeSet)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - キーフレームアニメーションセット生成失敗(CAnimationModel_SetTrackLoopMode)\n"));
		pKeyFramedAnimeSet->Release();
		return;
	}

	// キーフレームアニメーションコピー
	for(UINT i = 0; i < ANIMATIONS; i++) {
		// スケールキー取得
		LPD3DXKEY_VECTOR3   pScaleKeys = NULL;
		const UINT          SCALE_KEYS = pKeyFramedAnimeSet->GetNumScaleKeys(i);
		if(SCALE_KEYS > 0) {
			pScaleKeys = new D3DXKEY_VECTOR3[SCALE_KEYS];
			pKeyFramedAnimeSet->GetScaleKeys(i, pScaleKeys);
		}

		// 回転キー取得
		LPD3DXKEY_QUATERNION   pRotateKeys = NULL;
		const UINT             ROTATE_KEYS = pKeyFramedAnimeSet->GetNumRotationKeys(i);
		if(ROTATE_KEYS > 0) {
			pRotateKeys = new D3DXKEY_QUATERNION[ROTATE_KEYS];
			pKeyFramedAnimeSet->GetRotationKeys(i, pRotateKeys);
		}

		// 移動キー取得
		LPD3DXKEY_VECTOR3   pTransKeys = NULL;
		const UINT          TRANS_KEYS = pKeyFramedAnimeSet->GetNumTranslationKeys(i);
		if(TRANS_KEYS > 0) {
			pTransKeys = new D3DXKEY_VECTOR3[TRANS_KEYS];
			pKeyFramedAnimeSet->GetTranslationKeys(i, pTransKeys);
		}

		// アニメーション名取得
		LPCSTR   anime_name;
		pKeyFramedAnimeSet->GetAnimationNameByIndex(i, &anime_name);

		// SRT登録
		if(pNewKeyFramedAnimeSet->RegisterAnimationSRTKeys(anime_name,
														   SCALE_KEYS, ROTATE_KEYS, TRANS_KEYS,
														   pScaleKeys, pRotateKeys, pTransKeys,
														   NULL)
		   != S_OK)
			::OutputDebugString(TEXT("*** Error - SRT登録失敗(CAnimationModel_SetTrackLoopMode)\n"));

		// SRT配列解放
		delete[]  pTransKeys;
		delete[] pRotateKeys;
		delete[]  pScaleKeys;
	}

	// アニメーションセット登録
	m_pAnimationController->UnregisterAnimationSet(pKeyFramedAnimeSet);
	if(m_pAnimationController->RegisterAnimationSet(pNewKeyFramedAnimeSet) != S_OK) {
		::OutputDebugString(TEXT("*** Error - アニメーションセット登録失敗(CAnimationModel_SetTrackLoopMode)\n"));

		m_pAnimationController->RegisterAnimationSet(pKeyFramedAnimeSet);
		m_pAnimationController->SetTrackAnimationSet(inTrack, pKeyFramedAnimeSet);

		pNewKeyFramedAnimeSet->Release();
		pKeyFramedAnimeSet   ->Release();

		return;
	}
	pKeyFramedAnimeSet->Release();

	// アニメーションセットをトラックに設定
	hr = m_pAnimationController->SetTrackAnimationSet(inTrack, pNewKeyFramedAnimeSet);
	pNewKeyFramedAnimeSet->Release();
	if(hr != S_OK) {
		::OutputDebugString(TEXT("*** Error - アニメーションセット設定失敗(CAnimationModel_SetTrackLoopMode)\n"));
		return;
	}
}

//------------------------------------------------------------------------------
//	アニメーション切り換え
//------------------------------------------------------------------------------
void CAnimationModel::ChangeAnimation(const UINT inOutTrack, const UINT inInTrack,
									  const double inDuration,
									  const D3DXTRANSITION_TYPE inTransition)
{
	// グローバルアニメーションタイム取得
	const double   GAT = m_pAnimationController->GetTime();

	// アウトトラック設定
	m_pAnimationController->UnkeyAllTrackEvents(inOutTrack);
	m_pAnimationController->KeyTrackWeight(inOutTrack, 0.0f , GAT,  inDuration, inTransition);
	m_pAnimationController->KeyTrackEnable(inOutTrack, FALSE, GAT + inDuration);

	// イントラック設定
	m_pAnimationController->UnkeyAllTrackEvents(inInTrack );
	m_pAnimationController->KeyTrackWeight(inInTrack,  1.0f,  GAT,  inDuration, inTransition);
	m_pAnimationController->SetTrackEnable(inInTrack,  TRUE);
}

//------------------------------------------------------------------------------
//	アニメーション切り換え
//------------------------------------------------------------------------------
void CAnimationModel::ChangeAnimation(const UINT inInTrack, const double inDuration,
									  const D3DXTRANSITION_TYPE inTransition)
{
	// グローバルアニメーションタイム取得
	const double   GAT = m_pAnimationController->GetTime();

	// 有効になっているトラックを検索
	const UINT   TRACKS = m_pAnimationController->GetMaxNumTracks();
	for(UINT i = 0; i < TRACKS; i++) {
		// トラック情報取得
		D3DXTRACK_DESC   desc;
		m_pAnimationController->GetTrackDesc(i, &desc);

		if(desc.Enable != FALSE) {
			// アウトトラック設定
			m_pAnimationController->UnkeyAllTrackEvents(i);
			m_pAnimationController->KeyTrackWeight(i, 0.0f , GAT,  inDuration, inTransition);
			m_pAnimationController->KeyTrackEnable(i, FALSE, GAT + inDuration);
		}
	}

	// イントラック設定
	m_pAnimationController->UnkeyAllTrackEvents(inInTrack);
	m_pAnimationController->KeyTrackWeight(inInTrack, 1.0f, GAT, inDuration, inTransition);
	m_pAnimationController->SetTrackEnable(inInTrack, TRUE);
}

//------------------------------------------------------------------------------
//	スキニング方法設定
//------------------------------------------------------------------------------
void CAnimationModel::SetSkinningMethod(const DXGSKINNING_METHOD inMethod)
{
	if(m_SkinningMethod == inMethod || inMethod == DXGSKMTD_DEFAULT)
		return;
	m_SkinningMethod = inMethod;
	ChangeSkinningMethod(m_pFrameRoot);
}

//------------------------------------------------------------------------------
//	スキニング方法更新
//------------------------------------------------------------------------------
void CAnimationModel::ChangeSkinningMethod(D3DXFRAME* pFrameBase)
{
	DXGFRAME*           pFrame         = (DXGFRAME*)pFrameBase;
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		GenerateSkinnedMesh(pMeshContainer);
		pMeshContainer = (DXGMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
	}

	if(pFrame->pFrameSibling    != NULL)
		ChangeSkinningMethod(pFrame->pFrameSibling);

	if(pFrame->pFrameFirstChild != NULL)
		ChangeSkinningMethod(pFrame->pFrameFirstChild);
}

//------------------------------------------------------------------------------
//	プログレッシブモード
//------------------------------------------------------------------------------
bool CAnimationModel::EnableProgressive()
{
	if(m_SkinningMethod == DXGSKMTD_SOFTWARE) {
		::OutputDebugString(TEXT("*** Error - ソフトウェアモード未対応(CAnimationModel_EnableProgressive)\n"));
		return false;
	}

	EnableProgressive(m_pFrameRoot);
	return true;
}

//------------------------------------------------------------------------------
//	プログレッシブモード
//------------------------------------------------------------------------------
void CAnimationModel::EnableProgressive(D3DXFRAME* pFrameBase)
{
	DXGFRAME*           pFrame         = (DXGFRAME*)pFrameBase;
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		if(pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH) {
			// 隣接面生成
			DWORD*   pAdjacency = new DWORD[pMeshContainer->MeshData.pMesh->GetNumFaces() * 3];
			if(pMeshContainer->MeshData.pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency) != D3D_OK) {
				delete[] pAdjacency;
				::OutputDebugString(TEXT("*** Error - 隣接面生成失敗(CAnimationModel_EnableProgressive)\n"));
				return;
			}

			// メッシュクリーニング
			ID3DXMesh*   pCleanMesh;
			if(::D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION, pMeshContainer->MeshData.pMesh, pAdjacency,
							   &pCleanMesh, pAdjacency, NULL) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - メッシュクリーニング失敗(CAnimationModel_EnableProgressive)\n"));
				delete[] pAdjacency;
				return;
			}
			SafeRelease(pMeshContainer->MeshData.pMesh);
			pMeshContainer->MeshData.pMesh = pCleanMesh;
/*
			// メッシュ結合
			::D3DXWeldVertices(pMeshContainer->MeshData.pMesh, D3DXWELDEPSILONS_WELDALL, NULL, pAdjacency,
							   pAdjacency, NULL, NULL);
*/
			// プログレッシブメッシュ生成
			ID3DXPMesh*   pPMesh;
			if(::D3DXGeneratePMesh(pMeshContainer->MeshData.pMesh, pAdjacency, NULL, NULL, 1, D3DXMESHSIMP_FACE, &pPMesh) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - プログレッシブメッシュ生成失敗(CAnimationModel_EnableProgressive)\n"));
				delete[] pAdjacency;
				return;
			}

			delete[] pAdjacency;

			// 最適化
			pPMesh->SetNumFaces(pPMesh->GetMaxFaces());
			pPMesh->OptimizeBaseLOD(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, NULL);

			SafeRelease(pMeshContainer->MeshData.pMesh);
			pMeshContainer->MeshData.pPMesh = pPMesh;
			pMeshContainer->MeshData.Type   = D3DXMESHTYPE_PMESH;
		}

		// 次のコンテナへ
		pMeshContainer = (DXGMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
	}

	if(pFrame->pFrameSibling    != NULL)
		EnableProgressive(pFrame->pFrameSibling);

	if(pFrame->pFrameFirstChild != NULL)
		EnableProgressive(pFrame->pFrameFirstChild);
}

//------------------------------------------------------------------------------
//	LOD設定
//------------------------------------------------------------------------------
void CAnimationModel::SetLOD(const float inLOD)
{
	SetLOD(m_pFrameRoot, inLOD);
}

//------------------------------------------------------------------------------
//	LOD設定
//------------------------------------------------------------------------------
void CAnimationModel::SetLOD(D3DXFRAME* pFrameBase, const float& inLOD)
{
	DXGFRAME*           pFrame         = (DXGFRAME*)pFrameBase;
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pFrame->pMeshContainer;

	while(pMeshContainer != NULL) {
		if(pMeshContainer->MeshData.Type == D3DXMESHTYPE_PMESH) {
			// 詳細レベル設定
			const DWORD   MaxFaces = pMeshContainer->MeshData.pPMesh->GetMaxFaces();
			const DWORD   MinFaces = pMeshContainer->MeshData.pPMesh->GetMinFaces();
			pMeshContainer->MeshData.pPMesh->SetNumFaces((DWORD)(MinFaces + ((MaxFaces - MinFaces) * inLOD + 0.5f)));
		}

		// 次のコンテナへ
		pMeshContainer = (DXGMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
	}

	if(pFrame->pFrameSibling    != NULL)
		SetLOD(pFrame->pFrameSibling,    inLOD);

	if(pFrame->pFrameFirstChild != NULL)
		SetLOD(pFrame->pFrameFirstChild, inLOD);
}

//------------------------------------------------------------------------------
//	エフェクトマップ読み込み
//------------------------------------------------------------------------------
void CAnimationModel::LoadEffectMap(const DWORD inStage, LPCTSTR inFileName)
{
	// キューブテクスチャ生成
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &pTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CModel_LoadEffectMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(EFFECTMAP(inStage, pTexture));
}

//------------------------------------------------------------------------------
//	キューブエフェクトマップ読み込み
//------------------------------------------------------------------------------
void CAnimationModel::LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName)
{
	// キューブテクスチャ生成
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - キューブテクスチャ生成失敗(CModel_LoadCubeEffectMap)\n"));
		return;
	}

	// リストへ追加
	m_EffectMap.push_back(EFFECTMAP(inStage, pCubeTexture));
}

//------------------------------------------------------------------------------
//	エフェクトマップ適用
//------------------------------------------------------------------------------
void CAnimationModel::ApplyEffectMap()
{
	// エフェクトテクスチャ設定
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		m_pD3DDevice->SetTexture((*it).Stage, (*it).pTexture);
}

//------------------------------------------------------------------------------
//	エフェクトマップ解除
//------------------------------------------------------------------------------
void CAnimationModel::RemoveEffectMap()
{
	// エフェクトテクスチャ解除
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		m_pD3DDevice ->SetTexture((*it).Stage, NULL);
}

//------------------------------------------------------------------------------
//	シェーダーパラメータ設定
//------------------------------------------------------------------------------
void CAnimationModel::RegisterBoneMatricesByName(IEffect*& inEffect, LPCSTR inWorldMatrixArray, LPCSTR inNumBones)
{
	m_WorldMatrixArrayName = inEffect->GetParameterHandleByName(inWorldMatrixArray);
	m_NumBonesName         = inEffect->GetParameterHandleByName(inNumBones);
}

//------------------------------------------------------------------------------
//	フレーム生成
//------------------------------------------------------------------------------
HRESULT DXGAllocateHierarchy::CreateFrame(LPCSTR inName, LPD3DXFRAME* ppNewFrame)
{
	*ppNewFrame = NULL;

	// フレーム領域生成
	DXGFRAME*   pFrame = new DXGFRAME;
	if(pFrame == NULL) {
		::OutputDebugString(TEXT("*** Error - フレーム領域生成失敗(DXGAllocateHierarchy_CreateFrame)\n"));
		return E_OUTOFMEMORY;
	}

	pFrame->pMeshContainer   = NULL;
	pFrame->pFrameSibling    = NULL;
	pFrame->pFrameFirstChild = NULL;

	pFrame->SetName(inName);

	::D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	::D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

	*ppNewFrame = pFrame;

    return D3D_OK;
}

//------------------------------------------------------------------------------
//	メッシュコンテナ生成
//------------------------------------------------------------------------------
HRESULT DXGAllocateHierarchy::CreateMeshContainer(LPCSTR                    inName, 
												  CONST D3DXMESHDATA*       pMeshData,
												  CONST D3DXMATERIAL*       pMaterials, 
												  CONST D3DXEFFECTINSTANCE* pEffectInstances, 
												  DWORD                     inNumMaterials, 
												  CONST DWORD*              pAdjacency, 
												  LPD3DXSKININFO            pSkinInfo, 
												  LPD3DXMESHCONTAINER*      ppNewMeshContainer) 
{
	*ppNewMeshContainer = NULL;

	IDirect3DDevice9*   pD3DDevice     = NULL;
	DXGMESHCONTAINER*   pMeshContainer = NULL;
	try {
		ID3DXMesh*   pMesh = pMeshData->pMesh;
		if(pMesh == NULL) {
			::OutputDebugString(TEXT("*** Error - メッシュ未生成(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_FAIL;
		}

		// メッシュの形式を調べる
		if(pMeshData->Type != D3DXMESHTYPE_MESH) {
			::OutputDebugString(TEXT("*** Error - メッシュ形式未対応(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_FAIL;
		}

		// メッシュのフォーマットを調べる
		if(pMesh->GetFVF() == 0) {
			::OutputDebugString(TEXT("*** Error - メッシュ形式未対応(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_FAIL;
		}

		// Direct3DDevice9取得
		if(pMesh->GetDevice(&pD3DDevice) != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - Direct3DDevice9取得失敗(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_FAIL;
		}

		// メッシュコンテナ生成
		pMeshContainer = new DXGMESHCONTAINER;
		if(pMeshContainer == NULL) {
			::OutputDebugString(TEXT("*** Error - メッシュコンテナ生成失敗(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_OUTOFMEMORY;
		}
		::ZeroMemory(pMeshContainer, sizeof(DXGMESHCONTAINER));
		pMeshContainer->SetName(inName);

		// モデル成形
		pMeshContainer->MeshData.Type  = D3DXMESHTYPE_MESH;
		pMeshContainer->MeshData.pMesh = ModelingMesh(pD3DDevice, pMesh, m_ComputeNormal);

		// 隣接面領域生成
		const UINT   NumFaces      = pMesh->GetNumFaces();
		pMeshContainer->pAdjacency = new DWORD[NumFaces * 3];
		if(pMeshContainer->pAdjacency == NULL) {
			::OutputDebugString(TEXT("*** Error - 隣接面領域生成失敗(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_OUTOFMEMORY;
		}
		if(pMeshContainer->MeshData.pMesh->GenerateAdjacency(FLT_EPSILON, pMeshContainer->pAdjacency) != D3D_OK)
			::ZeroMemory(pMeshContainer->pAdjacency, sizeof(DWORD) * NumFaces * 3);

		// マテリアル配列生成
		pMeshContainer->NumMaterials  = max(inNumMaterials, 1);
		pMeshContainer->pMaterials    = new D3DXMATERIAL[pMeshContainer->NumMaterials];
		if(pMeshContainer->pMaterials == NULL) {
			::OutputDebugString(TEXT("*** Error - マテリアル配列生成失敗(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_OUTOFMEMORY;
		}

		// テクスチャ配列生成
		pMeshContainer->ppTextures = new IDirect3DTexture9*[pMeshContainer->NumMaterials];
		if(pMeshContainer->ppTextures == NULL) {
			::OutputDebugString(TEXT("*** Error - テクスチャ配列生成失敗(DXGAllocateHierarchy_CreateMeshContainer)\n"));
			throw E_OUTOFMEMORY;
		}
		::ZeroMemory(pMeshContainer->ppTextures, sizeof(IDirect3DTexture9*) * pMeshContainer->NumMaterials);

		// マテリアル設定
		if(inNumMaterials > 0) {
			::CopyMemory(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * inNumMaterials);
//			pMeshContainer->TextureName.resize(inNumMaterials, "");	// VC++2010だと例外発生
			pMeshContainer->TextureName.reserve(inNumMaterials);	// VC++2010対策
			for(DWORD i = 0; i < inNumMaterials; i++) {
				pMeshContainer->TextureName.push_back("");			// VC++2010対策
				if(pMeshContainer->pMaterials[i].pTextureFilename != NULL) {
					// テクスチャファイル名保存
					pMeshContainer->TextureName[i] = pMeshContainer->pMaterials[i].pTextureFilename;
					::CharLowerA((LPSTR)pMeshContainer->TextureName[i].c_str());
					// テクスチャ読み込み
					if(::D3DXCreateTextureFromFileA(pD3DDevice,
													pMeshContainer->pMaterials[i].pTextureFilename, 
												   &pMeshContainer->ppTextures[i])
					   != D3D_OK)
						pMeshContainer->ppTextures[i] = NULL;

//					pMeshContainer->pMaterials[i].pTextureFilename = NULL;
				}
			}
		} else {
			// マテリアル情報がない場合は、デフォルト値を設定
			pMeshContainer->pMaterials[0].MatD3D.Diffuse   = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
			pMeshContainer->pMaterials[0].MatD3D.Ambient   = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
			pMeshContainer->pMaterials[0].MatD3D.Specular  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
			pMeshContainer->pMaterials[0].MatD3D.Emissive  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			pMeshContainer->pMaterials[0].MatD3D.Power     = 1.0f;
			pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		}

		// スキン生成
		if(pSkinInfo != NULL) {
			pMeshContainer->pSkinInfo = pSkinInfo;
			pSkinInfo->AddRef();

			pMeshContainer->pOrigMesh = pMeshContainer->MeshData.pMesh;
			pMeshContainer->pOrigMesh->AddRef();

			// ボーン行列配列生成
			const UINT   BONES = pSkinInfo->GetNumBones();
			pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[BONES];
			if(pMeshContainer->pBoneOffsetMatrices == NULL) {
				::OutputDebugString(TEXT("*** Error - ボーン行列配列生成失敗(DXGAllocateHierarchy_CreateMeshContainer)\n"));
				throw E_OUTOFMEMORY;
			}

			for(UINT i = 0; i < BONES; i++) {
				pMeshContainer->pBoneOffsetMatrices[i] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
			}

			const HRESULT   hr = m_pAnimeModel->GenerateSkinnedMesh(pMeshContainer);
			if(FAILED(hr))
				throw hr;
		}

		*ppNewMeshContainer = pMeshContainer;
		SafeRelease(pD3DDevice);
	} catch(const HRESULT hr) {
		SafeRelease(pD3DDevice);

		if(pMeshContainer != NULL)
			DestroyMeshContainer(pMeshContainer);

		return hr;
	}

    return D3D_OK;
}

//------------------------------------------------------------------------------
//	フレーム破棄
//------------------------------------------------------------------------------
HRESULT DXGAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrame) 
{
	delete pFrame;
	pFrame = NULL;

	return S_OK;
}

//------------------------------------------------------------------------------
//	メッシュコンテナ破棄
//------------------------------------------------------------------------------
HRESULT DXGAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	DXGMESHCONTAINER*   pMeshContainer = (DXGMESHCONTAINER*)pMeshContainerBase;

	DeleteArray(pMeshContainer->pAdjacency         );
	DeleteArray(pMeshContainer->pMaterials         );
	DeleteArray(pMeshContainer->pBoneOffsetMatrices);

	if(pMeshContainer->ppTextures != NULL) {
		for (DWORD i = 0; i < pMeshContainer->NumMaterials; i++) {
			SafeRelease(pMeshContainer->ppTextures[i]);
		}
	}

	DeleteArray(pMeshContainer->ppTextures      );
	DeleteArray(pMeshContainer->ppBoneMatrixPtrs);

	SafeRelease(pMeshContainer->pBoneCombinationBuf);
	SafeRelease(pMeshContainer->MeshData.pMesh     );
	SafeRelease(pMeshContainer->pSkinInfo          );
	SafeRelease(pMeshContainer->pOrigMesh          );

	delete pMeshContainer;
	pMeshContainer = NULL;

	return S_OK;
}

//------------------------------------------------------------------------------
//	接線ベクトル生成
//------------------------------------------------------------------------------
ID3DXMesh* DXGAllocateHierarchy::ModelingMesh(IDirect3DDevice9*& pD3DDevice, ID3DXMesh* pBaseMesh,
											  const DXGCOMPUTENORMAL inCompute)
{
	if(inCompute == DXGCOMPUTE_DONOT_INPLACE) {
		pBaseMesh->AddRef();
		return pBaseMesh;
	}

    // Create a new vertex declaration to hold all the required data
	D3DVERTEXELEMENT9   VertexDecl[8] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END()
	};

	UINT    decl_idx =  1;	// 座標設定済みのため
	WORD    offset   = 12;	// float x 3 = 12byte

	// 法線
	DWORD   NormalSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_NORMAL) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_NORMAL;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		NormalSemantic = D3DDECLUSAGE_NORMAL;
	}

	// 接線
	DWORD   TangentSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_TANGENT) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_TANGENT;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		TangentSemantic = D3DDECLUSAGE_TANGENT;
	}

	// 従法線
	DWORD   BinormalSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_BINORMAL) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_BINORMAL;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		BinormalSemantic = D3DDECLUSAGE_BINORMAL;
	}

	// テクスチャ座標
	DWORD   TextureSemantic = D3DX_DEFAULT;
	if((pBaseMesh->GetFVF() & D3DFVF_TEX1) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT2;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_TEXCOORD;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 8;	// float x 2 = 8byte

		TextureSemantic = D3DDECLUSAGE_TEXCOORD;
	}

	// 合成比率
	VertexDecl[decl_idx].Stream     = 0;
	VertexDecl[decl_idx].Offset     = offset;
	VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT4;
	VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
	VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_BLENDWEIGHT;
	VertexDecl[decl_idx].UsageIndex = 0;

	decl_idx++;
	offset += 16;	// float x 4 = 16byte

	// 合成インデックス
	VertexDecl[decl_idx].Stream     = 0;
	VertexDecl[decl_idx].Offset     = offset;
	VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT4;
	VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
	VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_BLENDINDICES;
	VertexDecl[decl_idx].UsageIndex = 0;

	decl_idx++;
	offset += 16;	// float x 4 = 16byte

	// メッシュ生成
	ID3DXMesh*   pNewMesh;
	if(pBaseMesh->CloneMesh(pBaseMesh->GetOptions(), VertexDecl, pD3DDevice, &pNewMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - メッシュ生成失敗(DXGAllocateHierarchy_ModelingMesh)\n"));
		pBaseMesh->AddRef();
		return pBaseMesh;
	}

	// 法線・接線・従法線計算不要
	if(inCompute == DXGCOMPUTE_NONE   ||
	   inCompute == DXGCOMPUTE_NORMAL && (pBaseMesh->GetFVF() & D3DFVF_NORMAL) != 0)
		return pNewMesh;

	// 法線・接線・従法線計算
	if(::D3DXComputeTangentFrameEx(pNewMesh,
								   TextureSemantic , 0,
								   TangentSemantic , 0,
								   BinormalSemantic, 0,
								   NormalSemantic  , 0,
								   D3DXTANGENT_GENERATE_IN_PLACE, NULL,
								   -1.0f, 0.0f, -1.0f,
								   NULL, NULL)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 法線・接線・従法線計算失敗(DXGAllocateHierarchy_ModelingMesh)\n"));
	}

	return pNewMesh;
}
