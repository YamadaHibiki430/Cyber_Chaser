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
	 Effect.hpp
				エフェクトクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <d3dx9.h>
#include <list>

//------------------------------------------------------------------------------
//	エフェクトインタフェース定義
//------------------------------------------------------------------------------
class IEffect {
public:
	virtual ~IEffect() {}

	virtual void OnLostDevice () = 0;
	virtual void OnResetDevice() = 0;

	virtual UINT Begin() = 0;
	virtual void End() = 0;

	virtual bool BeginPass(const UINT inPass) = 0;
	virtual void EndPass() = 0;

	virtual D3DXHANDLE GetTechniqueByName(LPCSTR inParamName) = 0;
	virtual D3DXHANDLE GetParameterHandleByName(LPCSTR inParamName) = 0;

	virtual void SetTechnique(const D3DXHANDLE inParamName) = 0;
	virtual void SetData(const D3DXHANDLE inParamName, LPCVOID lpData, const UINT inBytes = D3DX_DEFAULT) = 0;

	virtual void SetIntArray    (const D3DXHANDLE inParamName, const int         inIntArray   [], UINT inCount) = 0;
	virtual void SetFloatArray  (const D3DXHANDLE inParamName, const float       inFloatArray [], UINT inCount) = 0;
	virtual void SetVector4Array(const D3DXHANDLE inParamName, const D3DXVECTOR4 inVec4Array  [], UINT inCount) = 0;
	virtual void SetMatrixArray (const D3DXHANDLE inParamName, const D3DXMATRIX  inMatrixArray[], UINT inCount) = 0;

	virtual void SetMatrixInverse  (const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix) = 0;
	virtual void SetMatrixTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix) = 0;
	virtual void SetMatrixInverseTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix) = 0;

	virtual void SetTexture(const D3DXHANDLE inParamName, IDirect3DBaseTexture9* pTexture) = 0;
	virtual void SetTexture(const DWORD   inTextureStage, IDirect3DBaseTexture9* pTexture) = 0;

	virtual void RetisterMaterialByName(LPCSTR inDiffuse,  LPCSTR inAmbient, LPCSTR inSpecular,
										LPCSTR inEmissive, LPCSTR inPower,   LPCSTR inTexture) = 0;
	virtual bool SetMaterial(D3DMATERIAL9& inMaterial, IDirect3DBaseTexture9* pTexture) = 0;

	virtual void CommitChanges() = 0;

	virtual void SetEffectMap(LPCSTR inParamName, LPCTSTR inFileName, const D3DFORMAT inFormat = D3DFMT_UNKNOWN) = 0;
	virtual void SetEffectMap(const DWORD inTextureStage, LPCTSTR inFileName, const D3DFORMAT inFormat = D3DFMT_UNKNOWN) = 0;

	virtual void SetCubeMap(LPCSTR inParamName, LPCTSTR inFileName) = 0;
	virtual void SetCubeMap(const DWORD inTextureStage, LPCTSTR inFileName) = 0;

	template<class T> inline void SetParameter(const D3DXHANDLE inParamName, const T& inParam)
	{ SetData(inParamName, &inParam, sizeof(inParam)); }

	template<class T> inline void SetArray(const D3DXHANDLE inParamName, const T inArray[], UINT inCount)
	{ SetData(inParamName, inArray, sizeof(T) * inCount); }

	inline void SetFloat4(const D3DXHANDLE inParamName, const D3DXVECTOR4& inParam)
	{ SetData(inParamName, &inParam, sizeof(inParam)); }

	inline void SetFloat4x4(const D3DXHANDLE inParamName, const D3DXMATRIX& inParam)
	{ SetData(inParamName, &inParam, sizeof(inParam)); }

	inline void SetFloat4x4Array(const D3DXHANDLE inParamName, const D3DXMATRIX inFloat4x4Array[], UINT inCount)
	{ SetMatrixArray(inParamName, inFloat4x4Array, inCount); }

	inline void SetFloat4x4Inverse  (const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
	{ SetMatrixInverse  (inParamName, inMatrix); }
	inline void SetFloat4x4Transpose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
	{ SetMatrixTranspose(inParamName, inMatrix); }
	inline void SetFloat4x4InverseTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
	{ SetMatrixInverseTranspose(inParamName, inMatrix); }
};

//------------------------------------------------------------------------------
//	エフェクトクラス定義
//------------------------------------------------------------------------------
class CEffect : public IEffect {
public:
	CEffect(ID3DXEffect* pD3DXEffect);
	virtual ~CEffect();

	virtual void OnLostDevice ();
	virtual void OnResetDevice();

	virtual UINT Begin();
	virtual void End();

	virtual bool BeginPass(const UINT inPass);
	virtual void EndPass();

	virtual D3DXHANDLE GetTechniqueByName(LPCSTR inTechName)
	{ return m_pD3DXEffect->GetTechniqueByName(inTechName); }

	virtual D3DXHANDLE GetParameterHandleByName(LPCSTR inParamName)
	{ return m_pD3DXEffect->GetParameterByName(NULL, inParamName); }

	virtual void SetTechnique(const D3DXHANDLE inParamName);
	virtual void SetData(const D3DXHANDLE inParamName, LPCVOID lpData, const UINT inBytes);

	virtual void SetIntArray    (const D3DXHANDLE inParamName, const int         inIntArray   [], UINT inCount);
	virtual void SetFloatArray  (const D3DXHANDLE inParamName, const float       inFloatArray [], UINT inCount);
	virtual void SetVector4Array(const D3DXHANDLE inParamName, const D3DXVECTOR4 inVec4Array  [], UINT inCount);
	virtual void SetMatrixArray (const D3DXHANDLE inParamName, const D3DXMATRIX  inMatrixArray[], UINT inCount);

	virtual void SetMatrixInverse  (const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix);
	virtual void SetMatrixTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix);
	virtual void SetMatrixInverseTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix);

	virtual void SetTexture(const D3DXHANDLE inParamName, IDirect3DBaseTexture9* pTexture);
	virtual void SetTexture(const DWORD   inTextureStage, IDirect3DBaseTexture9* pTexture);

	virtual void RetisterMaterialByName(LPCSTR inDiffuse,  LPCSTR inAmbient, LPCSTR inSpecular,
										LPCSTR inEmissive, LPCSTR inPower,   LPCSTR inTexture);
	virtual bool SetMaterial(D3DMATERIAL9& inMaterial, IDirect3DBaseTexture9* pTexture);

	virtual void CommitChanges() { m_pD3DXEffect->CommitChanges(); }

	virtual void SetEffectMap(LPCSTR inParamName, LPCTSTR inFileName, const D3DFORMAT inFormat);
	virtual void SetEffectMap(const DWORD inTextureStage, LPCTSTR inFileName, const D3DFORMAT inFormat);

	virtual void SetCubeMap(LPCSTR inParamName, LPCTSTR inFileName);
	virtual void SetCubeMap(const DWORD inTextureStage, LPCTSTR inFileName);

private:
	ID3DXEffect*                  m_pD3DXEffect;
	IDirect3DDevice9*             m_pD3DDevice;

	// エフェクト用マテリアル
	struct EFFECTMATERIAL {
		D3DXHANDLE   Diffuse;
		D3DXHANDLE   Ambient;
		D3DXHANDLE   Specular;
		D3DXHANDLE   Emissive;
		D3DXHANDLE   Power;
		D3DXHANDLE   Texture;

		EFFECTMATERIAL() : Diffuse(NULL),  Ambient(NULL), Specular(NULL),
						   Emissive(NULL), Power(NULL),   Texture(NULL) {}
	};
	EFFECTMATERIAL                m_Material;

	// エフェクトマッピング情報
	struct DXGEFFECTMAPINFO {
		D3DXHANDLE               hParameter;
		DWORD                    TextureStage;
		IDirect3DBaseTexture9*   pTexture;

		DXGEFFECTMAPINFO() : hParameter(NULL), TextureStage(0), pTexture(NULL) {}
		DXGEFFECTMAPINFO(const D3DXHANDLE param, const DWORD stage, IDirect3DBaseTexture9* texture)
			: hParameter(param), TextureStage(stage), pTexture(texture) {}
	};
	std::list<DXGEFFECTMAPINFO>   m_EffectMap;
	bool                          m_UseStage0;

	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}
};

//------------------------------------------------------------------------------
//	NULLエフェクトクラス定義
//------------------------------------------------------------------------------
class CNullEffect : public IEffect {
public:
	CNullEffect() {}
	virtual ~CNullEffect() {}

	virtual void OnLostDevice () {}
	virtual void OnResetDevice() {}

	virtual UINT Begin() { return 0; }
	virtual void End() {}

	virtual bool BeginPass(const UINT inPass) { return false; }
	virtual void EndPass() {}

	virtual D3DXHANDLE GetTechniqueByName(LPCSTR inTechName) { return NULL; }
	virtual D3DXHANDLE GetParameterHandleByName(LPCSTR inParamName) { return NULL; }

	virtual void SetTechnique(const D3DXHANDLE inParamName) {}
	virtual void SetData(const D3DXHANDLE inParamName, LPCVOID lpData, const UINT inBytes) {}

	virtual void SetIntArray    (const D3DXHANDLE inParamName, const int         inIntArray   [], UINT inCount) {}
	virtual void SetFloatArray  (const D3DXHANDLE inParamName, const float       inFloatArray [], UINT inCount) {}
	virtual void SetVector4Array(const D3DXHANDLE inParamName, const D3DXVECTOR4 inVec4Array  [], UINT inCount) {}
	virtual void SetMatrixArray (const D3DXHANDLE inParamName, const D3DXMATRIX  inMatrixArray[], UINT inCount) {}

	virtual void SetMatrixInverse  (const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix) {}
	virtual void SetMatrixTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix) {}
	virtual void SetMatrixInverseTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix) {}

	virtual void SetTexture(const D3DXHANDLE inParamName, IDirect3DBaseTexture9* pTexture) {}
	virtual void SetTexture(const DWORD   inTextureStage, IDirect3DBaseTexture9* pTexture) {}

	virtual void RetisterMaterialByName(LPCSTR inDiffuse,  LPCSTR inAmbient, LPCSTR inSpecular,
										LPCSTR inEmissive, LPCSTR inPower,   LPCSTR inTexture) {}
	virtual bool SetMaterial(D3DMATERIAL9& inMaterial, IDirect3DBaseTexture9* pTexture) { return false; }

	virtual void CommitChanges() {}

	virtual void SetEffectMap(LPCSTR inParamName, LPCTSTR inFileName, const D3DFORMAT inFormat) {}
	virtual void SetEffectMap(const DWORD inTextureStage, LPCTSTR inFileName, const D3DFORMAT inFormat) {}

	virtual void SetCubeMap(LPCSTR inParamName, LPCTSTR inFileName) {}
	virtual void SetCubeMap(const DWORD inTextureStage, LPCTSTR inFileName) {}
};
