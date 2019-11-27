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
	 VertexBuffer.hpp
				頂点バッファクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "VertexBuffer.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CVertexBuffer::CVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer,
							 const DWORD inFVF, const UINT inStride, const bool inWriteOnly)
	: m_pVertexBuffer(pVertexBuffer), m_pD3DDevice(NULL), m_pTexture(NULL),
	  m_FVF(inFVF), m_Stride(inStride),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f)
{
	assert(m_pVertexBuffer != NULL);
	m_pVertexBuffer->AddRef();

	m_pVertexBuffer->GetDevice(&m_pD3DDevice);
	assert(m_pD3DDevice != NULL);

	// ストライド設定
	if(m_Stride == 0)
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);

	//ロックフラグ設定
	if(inWriteOnly == false)
		m_LockFlags = 0;
	else
		m_LockFlags = D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE;

	::ZeroMemory(&m_Material, sizeof(m_Material));
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CVertexBuffer::~CVertexBuffer()
{
	SafeRelease(m_pTexture);

	m_pD3DDevice   ->Release();
	m_pVertexBuffer->Release();
}

//------------------------------------------------------------------------------
//	ロック
//------------------------------------------------------------------------------
LPVOID CVertexBuffer::Lock()
{
	LPVOID	pBuffer;
	if(m_pVertexBuffer->Lock(0, 0, &pBuffer, m_LockFlags) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ロック失敗(CVertexBuffer_Lock)\n"));
		return NULL;
	}

	return pBuffer;
}

//------------------------------------------------------------------------------
//	ロック解除
//------------------------------------------------------------------------------
void CVertexBuffer::Unlock()
{
	m_pVertexBuffer->Unlock();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CVertexBuffer::Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount)
{
	// シェーダ・フォーマット設定
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ワールド変換行列設定
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// マテリアル設定
	if((m_FVF & D3DFVF_NORMAL) != 0)
		m_pD3DDevice->SetMaterial(&m_Material);

	// テクスチャ設定
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

	// 描画
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CVertexBuffer::Draw(IEffect*& pEffect, const D3DPRIMITIVETYPE inType,
						 const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);

	pEffect->SetMaterial(m_Material, m_pTexture);

	const UINT   pass = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		pEffect->EndPass();
	}
	pEffect->End();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CVertexBuffer::Draw(IEffect*& pEffect, const UINT inPass, const D3DPRIMITIVETYPE inType,
						 const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

	pEffect->EndPass();
	pEffect->End();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CVertexBuffer::DrawPass(IEffect*& pEffect, const UINT inPass, const D3DPRIMITIVETYPE inType,
							 const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->BeginPass(inPass);

	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

	pEffect->EndPass();
}

//------------------------------------------------------------------------------
//	描画
//------------------------------------------------------------------------------
void CVertexBuffer::DrawPure(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetFVF(m_FVF);
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);
}

//------------------------------------------------------------------------------
//	半透明描画
//------------------------------------------------------------------------------
void CVertexBuffer::DrawAlpha(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							  const float inAlpha, const DWORD inDraw)
{
	if(inAlpha <= FLT_EPSILON)
		return;

	// ストリーム結合
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);

	// シェーダ・フォーマット設定
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ワールド変換行列設定
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// マテリアル設定
	const float   ALPHA = m_Material.Diffuse.a;
	if((m_FVF & D3DFVF_NORMAL) != 0) {
		m_Material.Diffuse.a *= inAlpha;
		m_pD3DDevice->SetMaterial(&m_Material);
	}

	// テクスチャ設定
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_BACKFACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_FACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_CULLNONE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
	}

	// マテリアル復元
	m_Material.Diffuse.a = ALPHA;
}

//------------------------------------------------------------------------------
//	影描画
//------------------------------------------------------------------------------
void CVertexBuffer::DrawShadow(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							   const D3DXMATRIX& inShadowMatrix, const float inDepth)
{
	// ワールド変換行列設定
	D3DXMATRIX   ShadowMatrix = GetWorldMatrix() * inShadowMatrix;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &ShadowMatrix);

	// マテリアル設定
	D3DMATERIAL9   material;
	::ZeroMemory(&material, sizeof(material));
	material.Diffuse.a = inDepth;

	m_pD3DDevice->SetMaterial(&material);
	m_pD3DDevice->SetTexture (0, NULL);

	// シェーダ・フォーマット設定
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// 描画
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);
}

//------------------------------------------------------------------------------
//	FVF設定
//------------------------------------------------------------------------------
void CVertexBuffer::SetFVF(const DWORD inFVF, const UINT inStride)
{
	m_FVF = inFVF;

	if(m_Stride != 0)
		m_Stride = inStride;
	else
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);
}

//------------------------------------------------------------------------------
//	ワールド変換行列生成
//------------------------------------------------------------------------------
D3DXMATRIX CVertexBuffer::GetWorldMatrix() const
{
	// クォータニオン生成
	D3DXQUATERNION   rotation;
	::D3DXQuaternionRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
													D3DXToRadian(m_Rotation.x),
													D3DXToRadian(m_Rotation.z));

	// ワールド行列設定
	D3DXMATRIX   world;
	::D3DXMatrixTransformation(&world, NULL, NULL, &m_Scale, NULL, &rotation, &m_Position);

	return world;
}

//------------------------------------------------------------------------------
//	テクスチャ設定
//------------------------------------------------------------------------------
void CVertexBuffer::SetTexture(LPCTSTR inFileName)
{
	SafeRelease(m_pTexture);	// 古いテクスチャの解放

	//テクスチャ生成
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &m_pTexture) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CVertexBuffer_SetTexture)\n"));
}

//------------------------------------------------------------------------------
//	テクスチャ設定
//------------------------------------------------------------------------------
void CVertexBuffer::SetTexture(IDirect3DTexture9* pTexture)
{
	SafeRelease(m_pTexture);	// 古いテクスチャの解放

	if(pTexture != NULL) {
		m_pTexture = pTexture;
		m_pTexture->AddRef();
	}
}

#ifdef UNICODE
//------------------------------------------------------------------------------
//	マテリアル設定
//------------------------------------------------------------------------------
void CVertexBuffer::SetMaterial(const D3DXMATERIAL& inMaterial)
{
	// ファイル名変換
	WCHAR   wzFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, inMaterial.pTextureFilename, -1, wzFileName, MAX_PATH);

	SetMaterial(inMaterial.MatD3D);
	SetTexture(wzFileName);
}
#endif

//------------------------------------------------------------------------------
//	移動
//------------------------------------------------------------------------------
void CVertexBuffer::Move(const float inRight, const float inUp, const float inFront)
{
	// 回転行列生成
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
												D3DXToRadian(m_Rotation.x),
												D3DXToRadian(m_Rotation.z));

	// 方向ベクトルを取り出す
	D3DXVECTOR3   right(rotation._11, rotation._12, rotation._13);
	D3DXVECTOR3   up   (rotation._21, rotation._22, rotation._23);
	D3DXVECTOR3   front(rotation._31, rotation._32, rotation._33);

	// 移動
	right      *= inRight;
	up         *= inUp;
	front      *= inFront;
	m_Position += front + up + right;
}

//------------------------------------------------------------------------------
//	回転
//------------------------------------------------------------------------------
void CVertexBuffer::Rotation(const float inX, const float inY, const float inZ)
{
	m_Rotation.x += inX;
	m_Rotation.y += inY;
	m_Rotation.z += inZ;
}

//------------------------------------------------------------------------------
//	境界球生成
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CVertexBuffer::GetBoundingSphere()
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
DXGBOUNDINGSPHERE CVertexBuffer::GetBaseBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere;

	// 頂点情報取得
	D3DVERTEXBUFFER_DESC   desc;
	if(m_pVertexBuffer->GetDesc(&desc) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 頂点情報取得失敗(VertexBuffer_GetBaseBoundingSphere)\n"));
		return shpere;
	}

	// 頂点バッファロック
	void*   pVertices;
	if(m_pVertexBuffer->Lock(0, 0, &pVertices, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 頂点バッファロック失敗(VertexBuffer_GetBaseBoundingSphere)\n"));
		return shpere;
	}

	// 境界球生成
	::D3DXComputeBoundingSphere((D3DXVECTOR3*)pVertices,
								desc.Size / m_Stride, m_Stride,
								&shpere.Center, &shpere.Radius);

	m_pVertexBuffer->Unlock();

	return shpere;
}

//------------------------------------------------------------------------------
//	OBB生成
//------------------------------------------------------------------------------
DXGOBB CVertexBuffer::GetOBB()
{
	DXGOBB   obb(GetBaseOBB());

	// 中心座標設定
	obb.Center += m_Position;

	// 各軸の傾き設定
	D3DXMATRIX   rot;
	::D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
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
DXGOBB CVertexBuffer::GetBaseOBB()
{
	DXGOBB   obb;

	// 頂点情報取得
	D3DVERTEXBUFFER_DESC   desc;
	if(m_pVertexBuffer->GetDesc(&desc) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 頂点情報取得失敗(VertexBuffer_GetOBB)\n"));
		return obb;
	}

	// 頂点バッファロック
	BYTE*   pVertices;
	if(m_pVertexBuffer->Lock(0, 0, (LPVOID*)&pVertices, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 頂点バッファロック失敗(VertexBuffer_GetOBB)\n"));
		return obb;
	}

	// 各軸の最大、最小座標を求める
	D3DXVECTOR3   pos_max(FLT_MIN, FLT_MIN, FLT_MIN);
	D3DXVECTOR3   pos_min(FLT_MAX, FLT_MAX, FLT_MAX);

	const int   NumVertices = desc.Size / m_Stride;
	for(int i = 0; i < NumVertices; i++) {
		D3DXVECTOR3   pos = *(D3DXVECTOR3*)pVertices;
		if(pos.x < pos_min.x)	pos_min.x = pos.x;
		if(pos.x > pos_max.x)	pos_max.x = pos.x;
		if(pos.y < pos_min.y)	pos_min.y = pos.y;
		if(pos.y > pos_max.y)	pos_max.y = pos.y;
		if(pos.z < pos_min.z)	pos_min.z = pos.z;
		if(pos.z > pos_max.z)	pos_max.z = pos.z;

		pVertices += m_Stride;
	}

	m_pVertexBuffer->Unlock();

	// 中心点設定
	obb.Center = (pos_max + pos_min) * 0.5f;

	// 各軸の長さの半分を設定
	obb.Radius.x = ::fabsf(pos_max.x - pos_min.x) * 0.5f;
	obb.Radius.y = ::fabsf(pos_max.y - pos_min.y) * 0.5f;
	obb.Radius.z = ::fabsf(pos_max.z - pos_min.z) * 0.5f;

	return obb;
}

//------------------------------------------------------------------------------
//	コンストラクタ－ユーザーポインタ
//------------------------------------------------------------------------------
CVertexBufferUP::CVertexBufferUP(IDirect3DDevice9* pD3DDevice,
								 void* pVertexBuffer, const DWORD inBufferSize,
								 const DWORD inFVF,   const UINT  inStride)
	: m_pVertexBuffer(pVertexBuffer), m_pD3DDevice(pD3DDevice), m_pTexture(NULL),
	  m_BufferSize(inBufferSize), m_FVF(inFVF), m_Stride(inStride),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f)
{
	assert(m_pVertexBuffer != NULL && m_BufferSize > 0);
	assert(m_pD3DDevice    != NULL);

	m_pD3DDevice->AddRef();

	::ZeroMemory(m_pVertexBuffer, m_BufferSize);
	::ZeroMemory(&m_Material, sizeof(m_Material));

	// ストライド設定
	if(m_Stride == 0)
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);
}

//------------------------------------------------------------------------------
//	デストラクタ－ユーザーポインタ
//------------------------------------------------------------------------------
CVertexBufferUP::~CVertexBufferUP()
{
	delete[] m_pVertexBuffer;

	SafeRelease(m_pTexture);
	m_pD3DDevice->Release();
}

//------------------------------------------------------------------------------
//	描画－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount)
{
	// シェーダ・フォーマット設定
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ワールド変換行列設定
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// マテリアル設定
	if((m_FVF & D3DFVF_NORMAL) != 0)
		m_pD3DDevice->SetMaterial(&m_Material);

	// テクスチャ設定
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

	// 描画
	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);
}

//------------------------------------------------------------------------------
//	描画－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::Draw(IEffect*& pEffect, const D3DPRIMITIVETYPE inType,
						   const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	const UINT    pass   = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		pEffect->EndPass();
	}
	pEffect->End();
}

//------------------------------------------------------------------------------
//	描画－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::Draw(IEffect*& pEffect, const UINT inPass,
						   const D3DPRIMITIVETYPE inType,
						   const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

	pEffect->EndPass();
	pEffect->End();
}

//------------------------------------------------------------------------------
//	描画－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawPass(IEffect*& pEffect, const UINT inPass,
							   const D3DPRIMITIVETYPE inType,
							   const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->BeginPass(inPass);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

	pEffect->EndPass();
}

//------------------------------------------------------------------------------
//	描画－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawPure(const D3DPRIMITIVETYPE inType,
							   const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetFVF(m_FVF);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);
}

//------------------------------------------------------------------------------
//	半透明描画－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawAlpha(const D3DPRIMITIVETYPE inType,
								const UINT  inStart, const UINT  inCount,
								const float inAlpha, const DWORD inDraw)
{
	if(inAlpha <= FLT_EPSILON)
		return;

	// シェーダ・フォーマット設定
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ワールド変換行列設定
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// マテリアル設定
	const float   ALPHA = m_Material.Diffuse.a;
	if((m_FVF & D3DFVF_NORMAL) != 0) {
		m_Material.Diffuse.a *= inAlpha;
		m_pD3DDevice->SetMaterial(&m_Material);
	}

	// テクスチャ設定
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

	// カラー出力保存
	DWORD   colorwrite_enable;
	m_pD3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite_enable);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_BACKFACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_FACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		// ｚ遅延出力
		if((inDraw & DXGDRAWA_CULLNONE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
	}

	// マテリアル復元
	m_Material.Diffuse.a = ALPHA;
}

//------------------------------------------------------------------------------
//	影描画－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawShadow(const D3DPRIMITIVETYPE inType,
								 const UINT inStart, const UINT inCount,
								 const D3DXMATRIX& inShadowMatrix, const float inDepth)
{
	// ワールド変換行列設定
	D3DXMATRIX   ShadowMatrix = GetWorldMatrix() * inShadowMatrix;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &ShadowMatrix);

	// マテリアル設定
	D3DMATERIAL9   material;
	::ZeroMemory(&material, sizeof(material));
	material.Diffuse.a = inDepth;

	m_pD3DDevice->SetMaterial(&material);
	m_pD3DDevice->SetTexture (0, NULL);

	// シェーダ・フォーマット設定
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// 描画
	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);
}

//------------------------------------------------------------------------------
//	FVF設定－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::SetFVF(const DWORD inFVF, const UINT inStride)
{
	m_FVF = inFVF;

	if(m_Stride != 0)
		m_Stride = inStride;
	else
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);
}

//------------------------------------------------------------------------------
//	ワールド変換行列生成－ユーザーポインタ
//------------------------------------------------------------------------------
D3DXMATRIX CVertexBufferUP::GetWorldMatrix() const
{
	// クォータニオン生成
	D3DXQUATERNION   rotation;
	::D3DXQuaternionRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
													D3DXToRadian(m_Rotation.x),
													D3DXToRadian(m_Rotation.z));

	// ワールド行列設定
	D3DXMATRIX   world;
	::D3DXMatrixTransformation(&world, NULL, NULL, &m_Scale, NULL, &rotation, &m_Position);

	return world;
}

//------------------------------------------------------------------------------
//	テクスチャ設定－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::SetTexture(LPCTSTR inFileName)
{
	SafeRelease(m_pTexture);	// 古いテクスチャの解放

	//テクスチャ生成
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &m_pTexture) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CVertexBufferUP_SetTexture)\n"));
}

//------------------------------------------------------------------------------
//	テクスチャ設定－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::SetTexture(IDirect3DTexture9* pTexture)
{
	SafeRelease(m_pTexture);	// 古いテクスチャの解放

	if(pTexture != NULL) {
		m_pTexture = pTexture;
		m_pTexture->AddRef();
	}
}

#ifdef UNICODE
//------------------------------------------------------------------------------
//	マテリアル設定－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::SetMaterial(const D3DXMATERIAL& inMaterial)
{
	// ファイル名変換
	WCHAR   wzFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, inMaterial.pTextureFilename, -1, wzFileName, MAX_PATH);

	SetMaterial(inMaterial.MatD3D);
	SetTexture(wzFileName);
}
#endif

//------------------------------------------------------------------------------
//	移動－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::Move(const float inRight, const float inUp, const float inFront)
{
	// 回転行列生成
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
												D3DXToRadian(m_Rotation.x),
												D3DXToRadian(m_Rotation.z));

	// 方向ベクトルを取り出す
	D3DXVECTOR3   right(rotation._11, rotation._12, rotation._13);
	D3DXVECTOR3   up   (rotation._21, rotation._22, rotation._23);
	D3DXVECTOR3   front(rotation._31, rotation._32, rotation._33);

	// 移動
	right      *= inRight;
	up         *= inUp;
	front      *= inFront;
	m_Position += front + up + right;
}

//------------------------------------------------------------------------------
//	回転－ユーザーポインタ
//------------------------------------------------------------------------------
void CVertexBufferUP::Rotation(const float inX, const float inY, const float inZ)
{
	m_Rotation.x += inX;
	m_Rotation.y += inY;
	m_Rotation.z += inZ;
}

//------------------------------------------------------------------------------
//	境界球生成－ユーザーポインタ
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CVertexBufferUP::GetBoundingSphere()
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
//	基本境界球生成－ユーザーポインタ
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CVertexBufferUP::GetBaseBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere;

	// 境界球生成
	::D3DXComputeBoundingSphere((D3DXVECTOR3*)m_pVertexBuffer,
								m_BufferSize / m_Stride, m_Stride,
								&shpere.Center, &shpere.Radius);

	return shpere;
}

//------------------------------------------------------------------------------
//	OBB生成－ユーザーポインタ
//------------------------------------------------------------------------------
DXGOBB CVertexBufferUP::GetOBB()
{
	DXGOBB   obb(GetBaseOBB());

	// 中心座標設定
	obb.Center += m_Position;

	// 各軸の傾き設定
	D3DXMATRIX   rot;
	::D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
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
//	基本OBB生成－ユーザーポインタ
//------------------------------------------------------------------------------
DXGOBB CVertexBufferUP::GetBaseOBB()
{
	DXGOBB   obb;
	BYTE*    pVertices = (BYTE*)m_pVertexBuffer;

	// 各軸の最大、最小座標を求める
	D3DXVECTOR3   pos_max(FLT_MIN, FLT_MIN, FLT_MIN);
	D3DXVECTOR3   pos_min(FLT_MAX, FLT_MAX, FLT_MAX);

	const int   NumVertices = m_BufferSize / m_Stride;
	for(int i = 0; i < NumVertices; i++) {
		D3DXVECTOR3   pos = *(D3DXVECTOR3*)pVertices;
		if(pos.x < pos_min.x)	pos_min.x = pos.x;
		if(pos.x > pos_max.x)	pos_max.x = pos.x;
		if(pos.y < pos_min.y)	pos_min.y = pos.y;
		if(pos.y > pos_max.y)	pos_max.y = pos.y;
		if(pos.z < pos_min.z)	pos_min.z = pos.z;
		if(pos.z > pos_max.z)	pos_max.z = pos.z;

		pVertices += m_Stride;
	}

	// 中心点設定
	obb.Center = (pos_max + pos_min) * 0.5f;

	// 各軸の長さの半分を設定
	obb.Radius.x = ::fabsf(pos_max.x - pos_min.x) * 0.5f;
	obb.Radius.y = ::fabsf(pos_max.y - pos_min.y) * 0.5f;
	obb.Radius.z = ::fabsf(pos_max.z - pos_min.z) * 0.5f;

	return obb;
}
