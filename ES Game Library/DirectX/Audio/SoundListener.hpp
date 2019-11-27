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
	 SoundListener.hpp
				サウンドリスナクラスヘッダ

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <dsound.h>

//------------------------------------------------------------------------------
//	サウンドリスナインタフェース定義
//------------------------------------------------------------------------------
class ISoundListener {
public:
	virtual ~ISoundListener() {}

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) = 0;
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) = 0;
	virtual void SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop) = 0;
	virtual void SetRolloffFactor(const D3DVALUE inRolloff) = 0;
	virtual void SetDopplerFactor(const D3DVALUE inDoppler) = 0;
	virtual void SetDistanceFactor(const D3DVALUE inDistance) = 0;
	virtual void SetAllParameters(const DS3DLISTENER& inListener) = 0;

	virtual void Initialize() = 0;
};

//------------------------------------------------------------------------------
//	サウンドリスナクラス定義
//------------------------------------------------------------------------------
class CSoundListener : public ISoundListener {
public:
	CSoundListener(IDirectSound3DListener8* pDS3DListener);
	virtual ~CSoundListener() { m_pDS3DListener->Release(); }

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z);
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z);
	virtual void SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop);
	virtual void SetRolloffFactor(const D3DVALUE inRolloff);
	virtual void SetDopplerFactor(const D3DVALUE inDoppler);
	virtual void SetDistanceFactor(const D3DVALUE inDistance);
	virtual void SetAllParameters(const DS3DLISTENER& inListener);

	virtual void Initialize();

private:
	IDirectSound3DListener8*   m_pDS3DListener;
};

//------------------------------------------------------------------------------
//	NULLサウンドリスナクラス定義
//------------------------------------------------------------------------------
class CNullSoundListener : public ISoundListener {
public:
	CNullSoundListener() {}
	virtual ~CNullSoundListener() {}

	virtual void SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) {}
	virtual void SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z) {}
	virtual void SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop)  {}
	virtual void SetRolloffFactor(const D3DVALUE inRolloff) {}
	virtual void SetDopplerFactor(const D3DVALUE inDoppler) {}
	virtual void SetDistanceFactor(const D3DVALUE inDistance) {}
	virtual void SetAllParameters(const DS3DLISTENER& inListener) {}

	virtual void Initialize() {}
};
