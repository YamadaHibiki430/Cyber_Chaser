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
	 SpriteBatch.hpp
				SpriteBatchクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

/*
#ifndef DXG_SB_DEBUG
#define DXG_SB_DEBUG
#endif
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DXGFont.hpp"
#include "Sprite.hpp"

#include <tchar.h>
#include <memory>

// #include <Box2D.h>

//------------------------------------------------------------------------------
//	描画範囲定数
//------------------------------------------------------------------------------
static const float DXGSPRBAT_TOPMOST    = -10000.0f;
static const float DXGSPRBAT_BOTTOMMOST =  10000.0f;

//------------------------------------------------------------------------------
//	クラス前方参照
//------------------------------------------------------------------------------
class CSpriteBatch;

//------------------------------------------------------------------------------
//	SpriteElement構造体定義
//------------------------------------------------------------------------------
struct SpriteElement {
	friend CSpriteBatch;

private:
	IDirect3DTexture9*   pTexture;

public:
	D3DXVECTOR3          anchorPoint;
	D3DXVECTOR3          position;
	D3DXVECTOR3          scale;
	D3DXVECTOR3          rotation;

	RECT                 srcRect;
	float                alpha;
	bool                 visible;

public:
	SpriteElement()
	{
		pTexture       = NULL;

		anchorPoint .x = 0.0f;
		anchorPoint .y = 0.0f;
		anchorPoint .z = 0.0f;

		position    .x = 0.0f;
		position    .y = 0.0f;
		position    .z = 0.0f;

		scale       .x = 1.0f;
		scale       .y = 1.0f;
		scale       .z = 0.0f;

		rotation    .x = 0.0f;
		rotation    .y = 0.0f;
		rotation    .z = 0.0f;

		srcRect.left   = 0;
		srcRect.top    = 0;
		srcRect.right  = 0;
		srcRect.bottom = 0;

		alpha          = 1.0f;
		visible        = false;
	}

	inline void SetScale(const float inScale)
	{
		scale.x = inScale;
		scale.y = inScale;
	}

	SpriteElement& operator=(IDirect3DTexture9* pTexture)
	{
		this->pTexture = pTexture;
		if(this->pTexture != NULL) {
			visible = true;

			D3DSURFACE_DESC   desc;
			pTexture->GetLevelDesc(0, &desc);

			srcRect.left   = 0;
			srcRect.top    = 0;
			srcRect.right  = desc.Width;
			srcRect.bottom = desc.Height;
		} else {
			visible        = false;

			srcRect.left   = 0;
			srcRect.top    = 0;
			srcRect.right  = 0;
			srcRect.bottom = 0;
		}

		return *this;
	}

	inline SpriteElement& operator=(ISprite* pSprite)
	{
		return (*this = *pSprite);
	}

	inline SpriteElement& operator*() { return *this; }
	inline operator IDirect3DTexture9*() const { return pTexture; }
};

#ifdef BOX2D_H
//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
/*
#ifndef _DEBUG
	inline float B2PTM(const UINT  pixel) { return (float)pixel / PhysicsSprite::PTMRatio; }
	inline float B2MTP(const float metre) { return        metre * PhysicsSprite::PTMRatio; }
#else
	#define B2PTM(pixel) (float(pixel) / PhysicsSprite::PTMRatio)
	#define B2MTP(metre) (      metre  * PhysicsSprite::PTMRatio)
#endif
*/

#define B2PTM(pixel) (float(pixel) / PhysicsSprite::PTMRatio)
#define B2MTP(metre) (      metre  * PhysicsSprite::PTMRatio)
#define b2Vec2PTM(x, y) B2PTM(x),B2PTM(y)

//------------------------------------------------------------------------------
//	PhysicsSprite構造体定義
//------------------------------------------------------------------------------
struct PhysicsSprite {
	friend CSpriteBatch;

public:
	static float         PTMRatio;

	float                z;

	D3DXVECTOR3          anchorPoint;
	RECT                 srcRect;
	float                alpha;
	bool                 visible;

private:
	IDirect3DTexture9*   pTexture;
	b2Body*              pBody;

public:
	PhysicsSprite()
	{
		pTexture       = NULL;
		pBody          = NULL;

		z              = 0.0f;

		anchorPoint .x = 0.0f;
		anchorPoint .y = 0.0f;
		anchorPoint .z = 0.0f;

		srcRect.left   = 0;
		srcRect.top    = 0;
		srcRect.right  = 0;
		srcRect.bottom = 0;

		alpha          = 1.0f;
		visible        = false;
	}

	inline void    SetB2Body(b2Body* pBody) { this->pBody = pBody; }
	inline b2Body* GetB2Body() const        { return pBody; }

	inline void  SetPTMRatio(const float inPTMRatio) { PTMRatio = inPTMRatio; }
	inline float GetPTMRatio() const                 { return PTMRatio; }

	void SetTransform(D3DXVECTOR3& inPos, const float inZAngle)
	{
	#ifdef _DEBUG
		if(pBody == NULL) {
			::OutputDebugString(TEXT("*** Error - Body未初期化(PhysicsSprite_SetTransform)\n"));
			return;
		}
	#endif
		b2Vec2   pos;
		pos.x = inPos.x / PTMRatio;
		pos.y = inPos.y / PTMRatio;
		pBody->SetTransform(pos, inZAngle * (3.141592f / 180.0f));

		z     = inPos.z;
	}

	D3DXVECTOR3 GetPosition()
	{
	#ifdef _DEBUG
		if(pBody == NULL) {
			::OutputDebugString(TEXT("*** Error - Body未初期化(PhysicsSprite_GetPosition)\n"));
			return D3DXVECTOR3(0.0f, 0.0f, z);
		}
	#endif
		b2Vec2   vec2 = pBody->GetPosition();

		D3DXVECTOR3   pos;
		pos.x = vec2.x * PTMRatio;
		pos.y = vec2.y * PTMRatio;
		pos.z = z;

		return pos;
	}

	float GetRotationZ()
	{
	#ifdef _DEBUG
		if(pBody == NULL) {
			::OutputDebugString(TEXT("*** Error - Body未初期化(PhysicsSprite_GetRotationZ)\n"));
			return 0.0f;
		}
	#endif
		return pBody->GetAngle() * (180.0f / 3.141592f);
	}

	PhysicsSprite& operator=(IDirect3DTexture9* pTexture)
	{
		this->pTexture = pTexture;
		if(this->pTexture != NULL) {
			visible = true;

			IDirect3DSurface9*   pSurface;
			pTexture->GetSurfaceLevel(0, &pSurface);

			D3DSURFACE_DESC   desc;
			pSurface->GetDesc(&desc);

			srcRect.left   = 0;
			srcRect.top    = 0;
			srcRect.right  = desc.Width;
			srcRect.bottom = desc.Height;

			pSurface->Release();
		} else {
			visible        = false;

			srcRect.left   = 0;
			srcRect.top    = 0;
			srcRect.right  = 0;
			srcRect.bottom = 0;
		}

		return *this;
	}

	inline PhysicsSprite& operator=(ISprite* pSprite)
	{
		return (*this = *pSprite);
	}

	PhysicsSprite& operator=(b2Body* pBody)
	{
		this->pBody = pBody;
		return *this;
	}

	inline b2Body*        operator->  ()       { return pBody;    }
	inline PhysicsSprite& operator*   ()       { return *this;    }
	inline operator IDirect3DTexture9*() const { return pTexture; }
};
#endif

//------------------------------------------------------------------------------
//	SpriteBatchクラス定義
//------------------------------------------------------------------------------
class CSpriteBatch {
public:
	// シングルトンインスタンスの取得
	static CSpriteBatch& GetInstance()
	{
		static CSpriteBatch theSpriteBatch;
		return theSpriteBatch;
	}

	virtual ~CSpriteBatch();

	bool Initialize(ID3DXSprite*& pD3DXSprite);
	void Release();

	// 左手系
	void Begin();
	void Begin(const DWORD inBlendMode, const DWORD inSortMode);

	inline void Begin(const DXGBLENDMODE inBlendMode,
					  const DWORD inSortMode, const DWORD inSaveState)
	{ Begin(inBlendMode, inSortMode | inSaveState); }

	void BeginFast();

	// 右手系
	void BeginRH();
	void BeginRH(const DWORD inBlendMode, const DWORD inSortMode);

	inline void BeginRH(const DXGBLENDMODE inBlendMode,
						const DWORD inSortMode, const DWORD inSaveState)
	{ BeginRH(inBlendMode, inSortMode | inSaveState); }

	void BeginFastRH();

	void End();

	void InitTransform();
	void SetTransform (const D3DXMATRIX& inTransform);

	void SetFilter(const D3DTEXTUREFILTERTYPE inFilterType);

	void DrawSimple(IDirect3DTexture9* pTexture, const RECT* pSrc,
					const D3DXVECTOR3& inPos,
					const D3DCOLOR inColorMask = 0xffffffff);

	void Draw(IDirect3DTexture9* pTexture, const RECT* pSrc,
			  const D3DXVECTOR3& inPos,
			  const D3DCOLOR inColorMask = 0xffffffff);

	void Draw(IDirect3DTexture9* pTexture, const RECT* pSrc,
			  const D3DXVECTOR3& inPos, const D3DXVECTOR2& inScale,
			  const D3DXVECTOR3& inRotation, const D3DXVECTOR3* pCenter = NULL,
			  const D3DCOLOR inColorMask = 0xffffffff);

	void Draw(const SpriteElement& inSpriteElement);

	// オペレータ
	operator IDirect3DDevice9*()  const { return m_pD3DDevice; }
	operator ID3DXSprite*()       const { return m_pD3DXSprite; }

public:
	// スプライトコンテナ描画
	template< template< class T, class Allocator = std::allocator<T> > class C >
	void Draw(C<SpriteElement>& inSpriteContainer)
	{
	#ifdef DXG_SB_DEBUG
		if(m_pD3DXSprite == NULL) {
			::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Draw)\n"));
			return;
		}
	#endif

		C<SpriteElement>::iterator   it  = inSpriteContainer.begin();
		C<SpriteElement>::iterator   end = inSpriteContainer.end();
		while(it != end) {
			const SpriteElement   sprite = *it;
			if(sprite.visible == false)
				continue;

			D3DXMATRIX   transform;

			// 回転
			::D3DXMatrixRotationYawPitchRoll(&transform,
											 D3DXToRadian(sprite.rotation.y),
											 D3DXToRadian(sprite.rotation.x),
											 D3DXToRadian(sprite.rotation.z));

			// スケーリング
			transform._11 *= sprite.scale.x;
			transform._12 *= sprite.scale.x;
			transform._13 *= sprite.scale.x;

			transform._21 *= sprite.scale.y;
			transform._22 *= sprite.scale.y;
			transform._23 *= sprite.scale.y;

			// 移動
			transform._41  = sprite.position.x;
			transform._42  = sprite.position.y;
			transform._43  = sprite.position.z;

			transform._44  = 1.0f;

			// 透明度
			const DWORD   COLOR_MASK = ((DWORD)(sprite.alpha * 255.0f) << 24) | 0x00ffffff;

			m_pD3DXSprite->SetTransform(&transform);
			m_pD3DXSprite->Draw(sprite.pTexture,    &sprite.srcRect,
							   &sprite.anchorPoint, NULL, COLOR_MASK);

			++it;
		}
	}

	// 単純描画
	inline void DrawSimple(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos)
	{
		DrawSimple(pTexture, NULL, inPos);
	}

	inline void DrawSimple(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
						   const int inAlpha)
	{
		DrawSimple(pTexture, NULL, inPos, (DWORD)inAlpha << 24 | 0x00ffffff);
	}

	inline void DrawSimple(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
						   const float inAlpha)
	{
		DrawSimple(pTexture, NULL, inPos, ((DWORD)(inAlpha * 255.0f) << 24) | 0x00ffffff);
	}

	inline void DrawSimple(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
						   const RECT& inSrcRect)
	{
		DrawSimple(pTexture, &inSrcRect, inPos);
	}

	inline void DrawSimple(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
						   const RECT& inSrcRect, const int inAlpha)
	{
		DrawSimple(pTexture, &inSrcRect, inPos, (DWORD)inAlpha << 24 | 0x00ffffff);
	}

	inline void DrawSimple(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
						   const RECT& inSrcRect, const D3DCOLOR inColorMask)
	{
		DrawSimple(pTexture, &inSrcRect, inPos, inColorMask);
	}

	// 多機能描画
	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
					 const D3DCOLOR inColorMask = 0xffffffff)
	{
		Draw(pTexture, NULL, inPos, inColorMask);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
					 const RECT& inSrcRect, const D3DCOLOR inColorMask = 0xffffffff)
	{
		Draw(pTexture, &inSrcRect, inPos, inColorMask);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
					 const float inAlpha)
	{
		Draw(pTexture, NULL, inPos, ((DWORD)(inAlpha * 255.0f) << 24) | 0x00ffffff);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos,
					 const RECT& inSrcRect, const float inAlpha)
	{
		Draw(pTexture, &inSrcRect, inPos, ((DWORD)(inAlpha * 255.0f) << 24) | 0x00ffffff);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const D3DCOLOR inColorMask,
					 const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter, const D3DXVECTOR2& inScale)
	{
		D3DXVECTOR3   pos(inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		Draw(pTexture, NULL, pos, inScale, inRotation, &inCenter, inColorMask);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const RECT& inSrcRect,
					 const D3DCOLOR inColorMask, D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter,
					 const D3DXVECTOR2& inScale)
	{
		D3DXVECTOR3   pos(inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		Draw(pTexture, &inSrcRect, pos, inScale, inRotation, &inCenter, inColorMask);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const D3DCOLOR inColorMask,
					 const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter, const float inScale)
	{
		D3DXVECTOR3   pos  (inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		D3DXVECTOR2   scale(inScale, inScale);
		Draw(pTexture, NULL, pos, scale, inRotation, &inCenter, inColorMask);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const RECT& inSrcRect,
					 const D3DCOLOR inColorMask, const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter,
					 const float inScale)
	{
		D3DXVECTOR3   pos  (inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		D3DXVECTOR2   scale(inScale, inScale);
		Draw(pTexture, &inSrcRect, pos, scale, inRotation, &inCenter, inColorMask);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const float inAlpha,
					 const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter, const D3DXVECTOR2& inScale)
	{
		D3DXVECTOR3   pos(inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		Draw(pTexture, NULL, pos, inScale, inRotation, &inCenter,
			 ((DWORD)(inAlpha * 255.0f) << 24) | 0x00ffffff);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const RECT& inSrcRect,
					 const float inAlpha, const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter,
					 const D3DXVECTOR2& inScale)
	{
		D3DXVECTOR3   pos(inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		Draw(pTexture, &inSrcRect, pos, inScale, inRotation, &inCenter,
			 ((DWORD)(inAlpha * 255.0f) << 24) | 0x00ffffff);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const float inAlpha,
					 const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter, const float inScale)
	{
		D3DXVECTOR3   pos  (inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		D3DXVECTOR2   scale(inScale, inScale);
		Draw(pTexture, NULL, pos, scale, inRotation, &inCenter,
			 ((DWORD)(inAlpha * 255.0f) << 24) | 0x00ffffff);
	}

	inline void Draw(IDirect3DTexture9* pTexture, const D3DXVECTOR3& inPos, const RECT& inSrcRect,
					 const float inAlpha, const D3DXVECTOR3& inRotation, const D3DXVECTOR3& inCenter,
					 const float inScale)
	{
		D3DXVECTOR3   pos  (inPos.x + inCenter.x, inPos.y + inCenter.y, inPos.z);
		D3DXVECTOR2   scale(inScale, inScale);
		Draw(pTexture, &inSrcRect, pos, scale, inRotation, &inCenter,
			 ((DWORD)(inAlpha * 255.0f) << 24) | 0x00ffffff);
	}

	// 文字列描画
	inline void DrawString(IDXGFont*& pFont, LPCTSTR inString, const D3DXVECTOR2& inPos, const D3DCOLOR inColor)
	{
		RECT   dest;
		dest.left   = static_cast<long>(inPos.x + 0.5f);
		dest.top    = static_cast<long>(inPos.y + 0.5f);
		dest.right  = dest.left + 1;
		dest.bottom = dest.top  + 1;

		pFont->DrawSprite(inString, -1, dest, DT_NOCLIP | DT_SINGLELINE, inColor);
	}

	inline void DrawString(IDXGFont*& pFont, LPCTSTR inString, const int inCount, const D3DXVECTOR2& inPos, const D3DCOLOR inColor)
	{
		RECT   dest;
		dest.left   = static_cast<long>(inPos.x + 0.5f);
		dest.top    = static_cast<long>(inPos.y + 0.5f);
		dest.right  = dest.left + 1;
		dest.bottom = dest.top  + 1;

		pFont->DrawSprite(inString, inCount, dest, DT_NOCLIP | DT_SINGLELINE, inColor);
	}

	void DrawString(IDXGFont*& pFont, const D3DXVECTOR2& inPos, const D3DCOLOR inColor, LPCTSTR inString, ...)
	{
		va_list   args;
		va_start(args, inString);

		const int   LENGTH = ::_vsctprintf(inString, args) + 1;
		TCHAR*      buffer = new TCHAR[LENGTH];
		::_vstprintf_s(buffer, LENGTH, inString, args);

		va_end(args);

		RECT   dest;
		dest.left   = static_cast<long>(inPos.x + 0.5f);
		dest.top    = static_cast<long>(inPos.y + 0.5f);
		dest.right  = dest.left + 1;
		dest.bottom = dest.top  + 1;

		pFont->DrawSprite(buffer, -1, dest, DT_NOCLIP | DT_SINGLELINE, inColor);

		delete[] buffer;
	}

	inline void DrawText(IDXGFont*& pFont, LPCTSTR inString, RECT& inDestRect, const D3DCOLOR inColor)
	{
		pFont->DrawSprite(inString, -1, inDestRect, DT_WORDBREAK, inColor);
	}

	inline void DrawText(IDXGFont*& pFont, LPCTSTR inString, const int inCount, RECT& inDestRect, const D3DCOLOR inColor)
	{
		pFont->DrawSprite(inString, inCount, inDestRect, DT_WORDBREAK, inColor);
	}

	void DrawText(IDXGFont*& pFont, RECT& inDestRect, const D3DCOLOR inColor, LPCTSTR inString, ...)
	{
		va_list   args;
		va_start(args, inString);

		const int   LENGTH = ::_vsctprintf(inString, args) + 1;
		TCHAR*      buffer = new TCHAR[LENGTH];
		::_vstprintf_s(buffer, LENGTH, inString, args);

		va_end(args);

		pFont->DrawSprite(buffer, -1, inDestRect, DT_WORDBREAK, inColor);

		delete[] buffer;
	}

	// 拡大・縮小、回転付き文字列描画
	inline void DrawString(IDXGFont*& pFont, LPCTSTR inString, const D3DXVECTOR2& inPos, const D3DCOLOR inColor,
						   const D3DXVECTOR2& inScale, const D3DXVECTOR3& inRot, const D3DXVECTOR3& inCenter)
	{
		RECT   dest;
		dest.left   = static_cast<long>(inPos.x / inScale.x + 0.5f);
		dest.top    = static_cast<long>(inPos.y / inScale.y + 0.5f);
		dest.right  = dest.left + 1;
		dest.bottom = dest.top  + 1;

		pFont->DrawSprite(inString, -1, dest, DT_NOCLIP | DT_SINGLELINE, inColor, inScale, inRot, inCenter);
	}

	inline void DrawString(IDXGFont*& pFont, LPCTSTR inString, const int inCount,
						   const D3DXVECTOR2& inPos, const D3DCOLOR inColor,
						   const D3DXVECTOR2& inScale, const D3DXVECTOR3& inRot, const D3DXVECTOR3& inCenter)
	{
		RECT   dest;
		dest.left   = static_cast<long>(inPos.x / inScale.x + 0.5f);
		dest.top    = static_cast<long>(inPos.y / inScale.y + 0.5f);
		dest.right  = dest.left + 1;
		dest.bottom = dest.top  + 1;

		pFont->DrawSprite(inString, inCount, dest, DT_NOCLIP | DT_SINGLELINE, inColor, inScale, inRot, inCenter);
	}

	void DrawString(IDXGFont*& pFont, const D3DXVECTOR2& inPos, const D3DCOLOR inColor, 
					const D3DXVECTOR2& inScale, const D3DXVECTOR3& inRot, const D3DXVECTOR3& inCenter,
					LPCTSTR inString, ...)
	{
		va_list   args;
		va_start(args, inString);

		const int   LENGTH = ::_vsctprintf(inString, args) + 1;
		TCHAR*      buffer = new TCHAR[LENGTH];
		::_vstprintf_s(buffer, LENGTH, inString, args);

		va_end(args);

		RECT   dest;
		dest.left   = static_cast<long>(inPos.x / inScale.x + 0.5f);
		dest.top    = static_cast<long>(inPos.y / inScale.y + 0.5f);
		dest.right  = dest.left + 1;
		dest.bottom = dest.top  + 1;

		pFont->DrawSprite(buffer, -1, dest, DT_NOCLIP | DT_SINGLELINE, inColor, inScale, inRot, inCenter);

		delete[] buffer;
	}

	inline void DrawText(IDXGFont*& pFont, LPCTSTR inString, RECT& inDestRect, const D3DCOLOR inColor,
						 const D3DXVECTOR2& inScale, const D3DXVECTOR3& inRot, const D3DXVECTOR3& inCenter)

	{
		RECT   dest;
		dest.left   = static_cast<long>(inDestRect.left / inScale.x + 0.5f);
		dest.top    = static_cast<long>(inDestRect.top  / inScale.y + 0.5f);
		dest.right  = inDestRect.right  - inDestRect.left;
		dest.bottom = inDestRect.bottom - inDestRect.top;

		pFont->DrawSprite(inString, -1, dest, DT_WORDBREAK, inColor, inScale, inRot, inCenter);
	}

	inline void DrawText(IDXGFont*& pFont, LPCTSTR inString, const int inCount,
						 RECT& inDestRect, const D3DCOLOR inColor,
						 const D3DXVECTOR2& inScale, const D3DXVECTOR3& inRot, const D3DXVECTOR3& inCenter)

	{
		RECT   dest;
		dest.left   = static_cast<long>(inDestRect.left / inScale.x + 0.5f);
		dest.top    = static_cast<long>(inDestRect.top  / inScale.y + 0.5f);
		dest.right  = inDestRect.right  - inDestRect.left;
		dest.bottom = inDestRect.bottom - inDestRect.top;

		pFont->DrawSprite(inString, inCount, dest, DT_WORDBREAK, inColor, inScale, inRot, inCenter);
	}

	void DrawText(IDXGFont*& pFont, RECT& inDestRect, const D3DCOLOR inColor,
				  const D3DXVECTOR2& inScale, const D3DXVECTOR3& inRot, const D3DXVECTOR3& inCenter,
				  LPCTSTR inString, ...)
	{
		va_list   args;
		va_start(args, inString);

		const int   LENGTH = ::_vsctprintf(inString, args) + 1;
		TCHAR*      buffer = new TCHAR[LENGTH];
		::_vstprintf_s(buffer, LENGTH, inString, args);

		va_end(args);

		RECT   dest;
		dest.left   = static_cast<long>(inDestRect.left / inScale.x + 0.5f);
		dest.top    = static_cast<long>(inDestRect.top  / inScale.y + 0.5f);
		dest.right  = inDestRect.right  - inDestRect.left;
		dest.bottom = inDestRect.bottom - inDestRect.top;

		pFont->DrawSprite(buffer, -1, dest, DT_WORDBREAK, inColor, inScale, inRot, inCenter);

		delete[] buffer;
	}

// Box2D拡張
#ifdef BOX2D_H
public:
	void Draw(const PhysicsSprite& inPhysicsSprite);

	// スプライトコンテナ描画
	template< template< class T, class Allocator = std::allocator<T> > class C >
	void Draw(C<PhysicsSprite>& inSpriteContainer)
	{
	#ifdef DXG_SB_DEBUG
		if(m_pD3DXSprite == NULL) {
			::OutputDebugString(TEXT("*** Error - スプライトバッチ未初期化(CSpriteBatch_Draw)\n"));
			return;
		}
	#endif

		C<SpriteElement>::iterator   it  = inSpriteContainer.begin();
		C<SpriteElement>::iterator   end = inSpriteContainer.end();
		while(it != end) {
			const PhysicsSprite   sprite = *it;
#ifdef _DEBUG
			if(sprite.pBody == NULL) {
				::OutputDebugString(TEXT("*** Warning - Body未初期化(CSpriteBatch_Draw)\n"));
				continue;
			}
#endif
			if(sprite.visible == false)
				continue;

			D3DXMATRIX   transform;
			b2Vec2       vec2;

			// 回転
			const float   z_angle = sprite.pBody->GetAngle();
			::D3DXMatrixRotationZ(&transform, D3DXToRadian(z_angle));

			// スケーリング


			// 移動
			vec2           = sprite.pBody->GetPosition();
			transform._41  = vec2.x * PhysicsSprite::PTMRatio;
			transform._42  = vec2.y * PhysicsSprite::PTMRatio;
			transform._43  = sprite.z;

			transform._44  = 1.0f;

			// 回転軸
			D3DXVECTOR3   anchor;
			vec2 = sprite->GetLocalCenter();
			anchor.x = vec2.x * PhysicsSprite::PTMRatio;
			anchor.y = vec2.y * PhysicsSprite::PTMRatio;
			anchor.z = 0.0f;

			// 透明度
			const DWORD   COLOR_MASK = ((DWORD)(sprite.alpha * 255.0f) << 24) | 0x00ffffff;

			m_pD3DXSprite->SetTransform(&transform);
			m_pD3DXSprite->Draw(sprite.pTexture, &sprite.srcRect,
							   &anchor, NULL, COLOR_MASK);

			++it;
		}
	}
#endif

private:
	CSpriteBatch();

	template <class T>
	inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	// コピーできないようにする
	CSpriteBatch(const CSpriteBatch&);				// コピーコンストラクタ
	CSpriteBatch& operator=(const CSpriteBatch&);	// 代入演算子

private:
	IDirect3DDevice9*   m_pD3DDevice;
	ID3DXSprite*        m_pD3DXSprite;

	enum DXGBLENDSTATE {
		DXGBLEND_ALPHAENABLE  = 0x01,
		DXGBLEND_RESTORESTATE = 0x02
	};
};

//------------------------------------------------------------------------------
//	インライン関数
//------------------------------------------------------------------------------
inline CSpriteBatch& DXGSpriteBatch() { return CSpriteBatch::GetInstance(); }
