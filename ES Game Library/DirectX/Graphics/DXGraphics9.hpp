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
	 DXGraphics9.hpp
				DirectX Graphics9クラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SpriteBatch.hpp"

#include "Effect.hpp"
#include "Camera.hpp"

#include "VertexBuffer.hpp"
#include "Model.hpp"
#include "AnimationModel.hpp"

#include "Sprite.hpp"

#include "StateBlock.hpp"
#include "RenderTarget.hpp"

#include "Sprite3D.hpp"

#include <list>
#include <set>
#include <map>

//------------------------------------------------------------------------------
//	DirectX Graphics9クラス定義
//------------------------------------------------------------------------------
class CDXGraphics9 {
public:
	// シングルトンインスタンスの取得
	static CDXGraphics9& GetInstance()
	{
		static CDXGraphics9 theDXGraphics;
		return theDXGraphics;
	}

	virtual ~CDXGraphics9();

	bool Initialize(const HWND hWnd, const UINT inWidth, UINT inHeight, const bool inWindowed);
	void Release();

	bool ChangeMode (const bool inWindowed);
	bool ResetDevice(const bool inTestDevice = true);

	void OnPaint(const PAINTSTRUCT& inPaint);

	// 画面消去
	void Clear(const D3DCOLOR inColor     = 0,
			   const DWORD    inFlags     = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
			   const D3DRECT* inRect      = NULL,
			   const DWORD    inRectCount = 0);
			   
	inline void ClearScreen (const D3DCOLOR inColor = 0) { Clear(inColor, D3DCLEAR_TARGET); }
	inline void ClearZBuffer() { Clear(0, D3DCLEAR_ZBUFFER); }
	inline void ClearStencil() { Clear(0, D3DCLEAR_STENCIL); }

//	void UpdateFrame(const bool inWaitVSync = true);
	void UpdateFrame();

	// カメラ
	inline void SetCamera(CCamera& inCamera) { inCamera.SetDevice(m_pD3DDevice); }

	inline void SetCameraView(CCamera& inCamera)
	{ SetTransform(D3DTS_VIEW, inCamera.GetViewMatrix()); }

	inline void SetCameraProjection(CCamera& inCamera)
	{ SetTransform(D3DTS_PROJECTION, inCamera.GetProjectionMatrix()); }

	// ビューポート
	void ResetViewport();
	void SetViewport(const D3DVIEWPORT9& view);

	D3DVIEWPORT9 GetViewport();
	D3DXMATRIX   GetViewportMatrix();

	// 変換行列
	D3DXMATRIX GetTransform(const D3DTRANSFORMSTATETYPE inTransType);
	void SetTransform(const D3DTRANSFORMSTATETYPE inTransType, const D3DXMATRIX& inTransform);

	// 座標変換
	// ワールド座標→スクリーン座標
	D3DXVECTOR3 WorldToScreen(const D3DXVECTOR3& inWorld);

	// スクリーン座標→ワールド座標
	// @カメラの座標が(0, 0, z)のみ使用可。また、カメラが回転していると誤差が大きくなる
	D3DXVECTOR3 ScreenToWorld(const float inScreenX, const float inScreenY, const float inWorldPosZ);
	inline D3DXVECTOR3 ScreenToWorld(const D3DXVECTOR3& inScreen,  const float inWorldPosZ)
	{ return ScreenToWorld(inScreen.x, inScreen.y, inWorldPosZ); }

	// レンダリング
	bool BeginScene();
	void EndScene();

	void DrawUserPrimitives(const D3DPRIMITIVETYPE inType, const void* pData, const UINT inCount,
							const DWORD inFVF, const UINT inStride = 0);

	// ライト
	void SetLight(const D3DLIGHT9& inLight, const DWORD inIndex = 0, const BOOL inEnable = TRUE);
	void EnableLight(const DWORD inIndex, const BOOL inEnable);
	void ClearLight();
	D3DLIGHT9 GetLight(const DWORD inIndex);

	// レンダリングステート
	DWORD GetRenderState(const D3DRENDERSTATETYPE inState);
	void  SetRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue);
	void  ResetRenderState();

	float GetRenderStateF(const D3DRENDERSTATETYPE inState);
	inline void SetRenderStateF(const D3DRENDERSTATETYPE inState, float inValue)
	{ SetRenderState(inState, *((DWORD*)&inValue)); }
/*
	float GetFloatRenderState(const D3DRENDERSTATETYPE inState);
	inline void SetFloatRenderState(const D3DRENDERSTATETYPE inState, float inValue)
	{ SetRenderState(inState, *((DWORD*)&inValue)); }
*/

	// マテリアル
	void SetMaterial(const D3DMATERIAL9& inMaterial);

	// テクスチャ
	void  SetTexture(const DWORD inStage, IDirect3DBaseTexture9* pTexture);

	DWORD GetTextureStageState(const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType);
	void  SetTextureStageState(const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType, const DWORD inValue);

	DWORD GetSamplerState(const DWORD inSampler, const D3DSAMPLERSTATETYPE inType);
	void  SetSamplerState(const DWORD inSampler, const D3DSAMPLERSTATETYPE inType, const DWORD inValue);

	// エフェクト
	IEffect* CreateEffectFromFile(LPCTSTR inFileName);

	void ReleaseEffect(IEffect*& pEffect);
	void ReleaseAllEffects();

	inline IEffect* CompileEffectFromFile(LPCTSTR inFileName)
	{ return CreateEffectFromFile(inFileName); }

	// バーテックスバッファ
	IVertexBuffer* CreateVertexBuffer  (const UINT inSize, const DWORD inFVF,
										const UINT inStride = 0, const bool inWriteOnly = false);
	IVertexBuffer* CreateVertexBufferUP(const UINT inSize, const DWORD inFVF,
										const UINT inStride = 0);

	void ReleaseVertexBuffer(IVertexBuffer*& pVertex);
	void ReleaseAllVertexBuffers();

	// モデル
	IModel* CreateModelFromSimpleShape(const DXGSIMPLESHAPE&  inShape,
									   const DXGCOMPUTENORMAL inCompute = DXGCOMPUTE_NORMAL);
	IModel* CreateModelFromFile(LPCTSTR inFileName,
								const DXGCOMPUTENORMAL inCompute = DXGCOMPUTE_NORMAL,
								const bool inUseSysMem = false);

	IModel* CreateModelFromText(LPCTSTR inText, const LOGFONT& inLogFont, const float inExtrusion,
								const DXGCOMPUTENORMAL inCompute = DXGCOMPUTE_NORMAL);
	IModel* CreateModelFromText(LPCTSTR inText, LPCTSTR inFontName, const int inSize,
								const float inExtrusion,
								const DXGCOMPUTENORMAL inCompute = DXGCOMPUTE_NORMAL);

	bool CreateSharedModelFromFile(LPTSTR inFileName,
								   IModel* pModelArray[], const DWORD inArrayCount,
								   const DXGCOMPUTENORMAL inCompute = DXGCOMPUTE_NORMAL);

	void ReleaseModel(IModel*& pModel);
	void ReleaseAllModels();

	// アニメーションモデル
	IAnimationModel* CreateAnimationModelFromFile(LPCTSTR inFileName,
												  const DXGCOMPUTENORMAL   inCompute    = DXGCOMPUTE_NORMAL,
												  const DXGSKINNING_METHOD inSkinMethod = DXGSKMTD_DEFAULT);

	void ReleaseAnimationModel(IAnimationModel*& pAnimeModel);
	void ReleaseAllAnimationModels();
	inline void ReleaseModel(IAnimationModel*& pAnimeModel) { ReleaseAnimationModel(pAnimeModel); }

	// スプライト
	ISprite* CreateSprite(const UINT inWidth, const UINT inHeight, const D3DFORMAT inFormat);
	ISprite* CreateSpriteFromFile(LPCTSTR inFileName, const D3DFORMAT inFormat = D3DFMT_UNKNOWN,
								  const D3DCOLOR inColorKey = 0);

	inline ISprite* CreateSpriteFromFile(LPCTSTR inFileName, const D3DCOLOR inColorKey,
										 const D3DFORMAT inFormat = D3DFMT_UNKNOWN)
	{ return CreateSpriteFromFile(inFileName, inFormat, inColorKey); }

	void ReleaseSprite(ISprite*& pSprite);
	void ReleaseAllSprites();

	// フォント
	IDXGFont* CreateSpriteFont(LPCTSTR inFontName, const int inSize);
	IDXGFont* CreateSpriteFont(const D3DXFONT_DESC& inFontDesc);
	IDXGFont* CreateDefaultFont();

	void ReleaseFont(IDXGFont*& pFont);
	void ReleaseAllFonts();

	// ステートブロック
	IStateBlock* CreateStateBlock();
	void ReleaseStateBlock(IStateBlock*& pStateBlock);
	void ReleaseAllStateBlocks();

	// レンダリングターゲット
	IRenderTarget* CreateRenderTarget(const UINT inWidth, const UINT inHeight,
									  const D3DFORMAT inFormat,
									  const D3DFORMAT inDepthFormat);
	IRenderTarget* CreateCubeRenderTarget(const UINT inSize,
										  const D3DFORMAT inFormat,
										  const D3DFORMAT inDepthFormat);
	IRenderTarget* CreateHDRRenderTarget(const UINT inWidth, const UINT inHeight,
										 const D3DFORMAT inDepthFormat);

	void ReleaseRenderTarget(IRenderTarget*& pRenderTarge);
	void ReleaseAllRenderTargets();

	// レンダリングターゲット - 設定
	void SetDefaultRenderTarget();
	inline void SetRenderTarget(IRenderTarget*& pRenderTarget) { pRenderTarget->Use(); }

	// レンダリングターゲット - 転送
	void RenderTargetToBackBuffer(const RECT* pDestRect,
								  IRenderTarget*& pSrcTarget, const RECT* pSrcRect,
								  const D3DTEXTUREFILTERTYPE inFilter = D3DTEXF_POINT);

	inline void RenderTargetToBackBuffer(const RECT& inDestRect,
										 IRenderTarget*& pSrcTarget, const RECT& inSrcRect,
										 const D3DTEXTUREFILTERTYPE inFilter = D3DTEXF_POINT)
	{ RenderTargetToBackBuffer(&inDestRect, pSrcTarget, &inSrcRect, inFilter); }

	void RenderTargetToBackBuffer(IRenderTarget*& pRenderTarget, IEffect*& pEffect);
	void RenderTargetToBackBuffer(IRenderTarget*& pRenderTarget, IEffect*& pEffect, const UINT inPass);

	void RenderTargetToRenderTarget(IRenderTarget*& pDestRenderTarget,
									IRenderTarget*&  pSrcRenderTarget, IEffect*& pEffect);
	void RenderTargetToRenderTarget(IRenderTarget*& pDestRenderTarget,
									IRenderTarget*&  pSrcRenderTarget,
									IEffect*& pEffect, const UINT inPass);

	// 転送
	void StretchRect(IDirect3DSurface9* pDestSurface, const RECT* pDestRect,
					 IDirect3DSurface9* pSrcSurface,  const RECT* pSrcRect,
					 const D3DTEXTUREFILTERTYPE inFilter = D3DTEXF_POINT);

	inline void StretchRect(IDirect3DSurface9* pDestSurface, const RECT& inDestRect,
							IDirect3DSurface9* pSrcSurface,  const RECT& inSrcRect,
							const D3DTEXTUREFILTERTYPE inFilter = D3DTEXF_POINT)
	{ StretchRect(pDestSurface, &inDestRect, pSrcSurface, &inSrcRect, inFilter); }

	// サーフェスロード
	void LoadSurfaceFromSurface(IDirect3DSurface9* pDestSurface, const RECT* pDestRect,
								IDirect3DSurface9* pSrcSurface,  const RECT* pSrcRect,
								const DWORD inFilter = D3DX_DEFAULT, const D3DCOLOR inColorKey = 0);

	inline void LoadSurfaceFromSurface(IDirect3DSurface9* pDestSurface, const RECT& inDestRect,
									   IDirect3DSurface9* pSrcSurface,  const RECT& inSrcRect,
									   const DWORD    inFilter   = D3DX_DEFAULT,
									   const D3DCOLOR inColorKey = 0)
	{ LoadSurfaceFromSurface(pDestSurface, &inDestRect, pSrcSurface, &inSrcRect, inFilter, inColorKey); }

	// スプライト3Dレイヤー
	ISprite3DLayer* CreateSprite3DLayer(const UINT inWidth, const UINT inHeight,
										const D3DFORMAT inFormat,
										const D3DFORMAT inDepthFormat);

	// リソースプロテクト設定
	void SetResourceProtect(void* pResource, const bool inProtect);

	// アルファブレンド
	bool BeginAlphaBlend(const DXGBLENDMODE inBlendMode = DXGBLEND_MODULATE);
	void SetBlendMode(const DXGBLENDMODE inBlendMode);
	void EndAlphaBlend();

	// シャドウレンダリング
	bool BeginShadowRendering();
	void EndShadowRendering();

	// バックバッファ
	HDC  GetDC();
	void ReleaseDC();

	// ウィンドウハンドル
	HWND GetHWnd() const { return m_PresentParams.hDeviceWindow; }

	// オペレータ
	operator IDirect3DDevice9*()  const { return m_pD3DDevice; }
	operator IDirect3DSurface9*() const { return m_pBackBufferSurface; }

private:
	CDXGraphics9();

	void SetPresentParams(const BOOL inWindowed);

	IDirect3DTexture9* CreateTextureFromWIC(LPCTSTR inFileName,
											const D3DFORMAT inFormat,
											const D3DCOLOR  inColorKey);
/*
	IDirect3DTexture9* CreateTextureFromPicture(LPCTSTR inFileName,
												const D3DFORMAT inFormat,
												const D3DCOLOR  inColorKey);
*/
	ID3DXMesh* ModelingMesh(ID3DXMesh* pBaseMesh, const DXGCOMPUTENORMAL inCompute);

	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; }}

	// コピーできないようにする
	CDXGraphics9(const CDXGraphics9&);				// コピーコンストラクタ
	CDXGraphics9& operator=(const CDXGraphics9&);	// 代入演算子

private:
	// デバイス
	IDirect3D9*                                  m_pD3D;
	IDirect3DDevice9*                            m_pD3DDevice;
	ID3DXSprite*                                 m_pD3DXSprite;
	ID3DXEffect*                                 m_pD3DXEffect;
	D3DPRESENT_PARAMETERS                        m_PresentParams;

	// ディスプレイ
	D3DDISPLAYMODE                               m_DisplayMode;
	UINT                                         m_Width;
	UINT                                         m_Height;
	RECT*                                        m_pPresentRect[2];

	// バックバッファ
	IDirect3DSurface9*                           m_pBackBufferSurface;
	IDirect3DSurface9*                           m_pDepthStencilSurface;
	HDC                                          m_hBackBufferDC;

	// オフスクリーンターゲット
	IVertexBuffer*                               m_pRenderTargetVertex;
	UINT                                         m_RenderTargetWidth;
	UINT                                         m_RenderTargetHeight;

	// リソース
	std::list<IEffect*>                          m_Effect;
	std::list<IVertexBuffer*>                    m_VertexBuffer;
	std::list<IModel*>                           m_Model;
	std::list<IAnimationModel*>                  m_AnimationModel;
	std::list<ISprite*>                          m_Sprite;
	std::list<IDXGFont*>                         m_Font;

	std::list<IStateBlock*>                      m_StateBlock;
	std::list<IRenderTarget*>                    m_RenderTarget;
	std::set <DWORD>                             m_LightIndex;

	std::set <void*>                             m_ProtectedResource;

	// ステート保存
	struct DXGSAVESTATE {
		DWORD	Value;		// 現在の設定値
		DWORD	DefValue;	// デフォルトの設定値
	};
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>   m_RenderState;

	// ライト構造体
	struct DXGLIGHT {
		DWORD       Index;
		D3DLIGHT9   Light;
		BOOL        Enable;
	};

	// アルファレンダリングステート保存
	struct DXGSAVEALPHASTATE {
		DWORD   CullMode;
		DWORD   AlphaEnable;
		DWORD   BlendOP;
		DWORD   DestBlend;
		DWORD   SrcBlend;
		DWORD   Diffuse;
		DWORD   AlphaOP;
		union {
			struct {
				DWORD   ZEnable;
				DWORD   ZWriteEnable;
			};
			struct {
				DWORD   ShadeMode;
				DWORD   StencilEnable;
				DWORD   StencilFunc;
				DWORD   StencilPass;
				DWORD   StencilFail;
				DWORD   StencilZFail;
				DWORD   StencilRef;
			};
		};
	};
	DXGSAVEALPHASTATE   m_AlphaState;
};

//------------------------------------------------------------------------------
//	インライン関数
//------------------------------------------------------------------------------
inline CDXGraphics9& DXGraphics() { return CDXGraphics9::GetInstance(); }
