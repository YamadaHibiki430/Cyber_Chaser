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
	 EffekseerEffect.hpp
				EffekseerEffectクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "EffekseerEffect.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CEffekseerEffect::CEffekseerEffect(Effekseer::Manager* pManager,
								   Effekseer::Effect*  pEffect)
	: m_manager(pManager), m_effect(pEffect)
{
	assert(m_manager != NULL && m_effect != NULL);
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CEffekseerEffect::~CEffekseerEffect()
{
	ES_SAFE_RELEASE(m_effect);
	m_manager = NULL;
}

//------------------------------------------------------------------------------
//	再生
//------------------------------------------------------------------------------
Effekseer::Handle CEffekseerEffect::Play(const float inX, const float inY, const float inZ)
{
	return m_manager->Play(m_effect, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	停止
//------------------------------------------------------------------------------
void CEffekseerEffect::Stop(const Effekseer::Handle inHandle)
{
	 m_manager->StopEffect(inHandle);
}

//------------------------------------------------------------------------------
//	ルート停止
//------------------------------------------------------------------------------
void CEffekseerEffect::StopRoot()
{
	 m_manager->StopRoot(m_effect);
}

//------------------------------------------------------------------------------
//	一時停止
//------------------------------------------------------------------------------
void CEffekseerEffect::Pause(const Effekseer::Handle inHandle)
{
	m_manager->SetPaused(inHandle, true);
}

//------------------------------------------------------------------------------
//	再開
//------------------------------------------------------------------------------
void CEffekseerEffect::Resume(const Effekseer::Handle inHandle)
{
	m_manager->SetPaused(inHandle, false);
}

//------------------------------------------------------------------------------
//	可視設定
//------------------------------------------------------------------------------
void CEffekseerEffect::SetVisible(const Effekseer::Handle inHandle, const bool inVisible)
{
	m_manager->SetShown(inHandle, inVisible);
}

//------------------------------------------------------------------------------
//	再生スピード設定
//------------------------------------------------------------------------------
void CEffekseerEffect::SetSpeed(const Effekseer::Handle inHandle, const float inSpeed)
{
	m_manager->SetSpeed(inHandle, inSpeed);
}

//------------------------------------------------------------------------------
//	座標設定
//------------------------------------------------------------------------------
void CEffekseerEffect::SetPosition(const Effekseer::Handle inHandle,
								   const float inX, const float inY, const float inZ)
{
	m_manager->SetLocation(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	角度設定
//------------------------------------------------------------------------------
void CEffekseerEffect::SetRotation(const Effekseer::Handle inHandle,
								   const float inX, const float inY, const float inZ)
{
	m_manager->SetRotation(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	スケーリング設定
//------------------------------------------------------------------------------
void CEffekseerEffect::SetScale(const Effekseer::Handle inHandle,
								const float inX, const float inY, const float inZ)
{
	m_manager->SetScale(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	ターゲット座標設定
//------------------------------------------------------------------------------
void CEffekseerEffect::SetTargetPostion(const Effekseer::Handle inHandle,
										const float inX, const float inY, const float inZ)
{
	m_manager->SetTargetLocation(inHandle, inX, inY, inZ);
}

//------------------------------------------------------------------------------
//	移動
//------------------------------------------------------------------------------
void CEffekseerEffect::Move(const Effekseer::Handle inHandle,
							const float inAddX, const float inAddY, const float inAddZ)
{
	m_manager->AddLocation(inHandle, Effekseer::Vector3D(inAddX, inAddY, inAddZ));
}

//------------------------------------------------------------------------------
//	座標取得
//------------------------------------------------------------------------------
D3DVECTOR CEffekseerEffect::GetPosition(const Effekseer::Handle inHandle)
{
	Effekseer::Vector3D   location = m_manager->GetLocation(inHandle);
	D3DVECTOR   pos = {location.X, location.Y, location.Z};
	return pos;
}

//------------------------------------------------------------------------------
//	座標取得
//------------------------------------------------------------------------------
D3DVECTOR CNullEffekseerEffect::GetPosition(const Effekseer::Handle inHandle)
{
	D3DVECTOR   pos = {0.0f, 0.0f, 0.0f};
	return pos;
}
