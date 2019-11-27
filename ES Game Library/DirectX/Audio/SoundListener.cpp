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
	 SoundListener.cpp
				サウンドリスナクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SoundListener.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CSoundListener::CSoundListener(IDirectSound3DListener8* pDS3DListener)
{
	assert(pDS3DListener != NULL);
	m_pDS3DListener = pDS3DListener;
	m_pDS3DListener->AddRef();			// 参照カウンタインクリメント
}

//------------------------------------------------------------------------------
//	位置設定
//------------------------------------------------------------------------------
void CSoundListener::SetPosition(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z)
{
	m_pDS3DListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	速度設定
//------------------------------------------------------------------------------
void CSoundListener::SetVelocity(const D3DVALUE x, const D3DVALUE y, const D3DVALUE z)
{
	m_pDS3DListener->SetVelocity(x, y, z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	向き設定
//------------------------------------------------------------------------------
void CSoundListener::SetOrientation(const D3DVECTOR& inFront, const D3DVECTOR& inTop)
{
	m_pDS3DListener->SetOrientation(inFront.x, inFront.y, inFront.z,
									inTop  .x, inTop  .y, inTop  .z, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	ロールオフ係数設定
//------------------------------------------------------------------------------
void CSoundListener::SetRolloffFactor(const D3DVALUE inRolloff)
{
	m_pDS3DListener->SetRolloffFactor(inRolloff, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	ドップラー乗数設定
//------------------------------------------------------------------------------
void CSoundListener::SetDopplerFactor(const D3DVALUE inDoppler)
{
	m_pDS3DListener->SetDopplerFactor(inDoppler, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	距離係数設定
//------------------------------------------------------------------------------
void CSoundListener::SetDistanceFactor(const D3DVALUE inDistance)
{
	m_pDS3DListener->SetDistanceFactor(inDistance, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	全パラメータ設定
//------------------------------------------------------------------------------
void CSoundListener::SetAllParameters(const DS3DLISTENER& inListener)
{
	m_pDS3DListener->SetAllParameters(&inListener, DS3D_IMMEDIATE);
}

//------------------------------------------------------------------------------
//	パラメータ初期化
//------------------------------------------------------------------------------
void CSoundListener::Initialize()
{
	DS3DLISTENER   Listener;
	::ZeroMemory(&Listener, sizeof(Listener));
	Listener.dwSize           = sizeof(Listener);

	Listener.vPosition.x      = 0.0f;							// 位置
	Listener.vPosition.y      = 0.0f;
	Listener.vPosition.z      = 0.0f;

	Listener.vVelocity.x      = 0.0f;							// 速度
	Listener.vVelocity.y      = 0.0f;
	Listener.vVelocity.z      = 0.0f;

	Listener.vOrientFront.x   = 0.0f;							// 前方ベクトル設定
	Listener.vOrientFront.y   = 0.0f;
	Listener.vOrientFront.z   = 1.0f;

	Listener.vOrientTop.x     = 0.0f;							// 上方ベクトル設定
	Listener.vOrientTop.y     = 1.0f;
	Listener.vOrientTop.z     = 0.0f;

	Listener.flDistanceFactor = DS3D_DEFAULTDISTANCEFACTOR;		// 係数設定
	Listener.flRolloffFactor  = DS3D_DEFAULTROLLOFFFACTOR;
	Listener.flDopplerFactor  = DS3D_DEFAULTDOPPLERFACTOR;

	m_pDS3DListener->SetAllParameters(&Listener, DS3D_IMMEDIATE);
}
