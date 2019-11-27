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
	 InputEffect.hpp
				入力デバイスエフェクトクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	バージョン宣言
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <dinput.h>

//------------------------------------------------------------------------------
//	入力デバイスエフェクトインタフェース定義
//------------------------------------------------------------------------------
class IInputEffect {
public:
	virtual ~IInputEffect() {}

	virtual bool IsNull() const = 0;

	virtual bool Start(const DWORD inIterations) = 0;
	virtual void Stop() = 0;
	virtual bool IsPlaying() = 0;
};

//------------------------------------------------------------------------------
//	入力デバイスエフェクトクラス定義
//------------------------------------------------------------------------------
class CInputEffect : public IInputEffect {
public:
	CInputEffect(IDirectInputEffect* pDIEffect);
	virtual ~CInputEffect();

	virtual bool IsNull() const { return false; }

	virtual bool Start(const DWORD inIterations);
	virtual void Stop() { m_pDIEffect->Stop(); }
	virtual bool IsPlaying();

private:
	IDirectInputEffect*   m_pDIEffect;
};

//------------------------------------------------------------------------------
//	NULL入力デバイスエフェクトクラス定義
//------------------------------------------------------------------------------
class CNullInputEffect : public IInputEffect {
public:
	CNullInputEffect() {}
	virtual ~CNullInputEffect() {}

	virtual bool IsNull() const { return true; }

	virtual bool Start(const DWORD inIterations) { return false; }
	virtual void Stop() {}
	virtual bool IsPlaying() { return false; }
};
