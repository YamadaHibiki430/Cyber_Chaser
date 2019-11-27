/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 EffekseerEffect.hpp
				EffekseerEffect�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <Effekseer.h>
#include <d3d.h>

//------------------------------------------------------------------------------
//	EffekseerEffect�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IEffekseerEffect {
public:
	virtual ~IEffekseerEffect() {}

	virtual Effekseer::Handle Play(const float inX, const float inY, const float inZ) = 0;
	inline  Effekseer::Handle Play(const D3DVECTOR& inPosition)
	{ return Play(inPosition.x, inPosition.y, inPosition.z); }

	virtual void Stop(const Effekseer::Handle inHandle) = 0;
	virtual void StopRoot() = 0;

	virtual void Pause (const Effekseer::Handle inHandle) = 0;
	virtual void Resume(const Effekseer::Handle inHandle) = 0;

	virtual void SetVisible(const Effekseer::Handle inHandle, const bool  inVisible) = 0;
	virtual void SetSpeed  (const Effekseer::Handle inHandle, const float inSpeed  ) = 0;

	virtual void SetPosition(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) = 0;
	inline  void SetPosition(const Effekseer::Handle inHandle, const D3DVECTOR& inPosition)
	{ SetPosition(inHandle, inPosition.x, inPosition.y, inPosition.z); }

	virtual void SetRotation(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) = 0;
	inline  void SetRotation(const Effekseer::Handle inHandle, const D3DVECTOR& inRotation)
	{ SetRotation(inHandle, inRotation.x, inRotation.y, inRotation.z); }

	virtual void SetScale(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) = 0;
	inline  void SetScale(const Effekseer::Handle inHandle, const D3DVECTOR& inScale)
	{ SetScale(inHandle, inScale.x, inScale.y, inScale.z); }
	inline  void SetScale(const Effekseer::Handle inHandle, const float inScale)
	{ SetScale(inHandle, inScale, inScale, inScale); }

	virtual void SetTargetPostion(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) = 0;
	inline  void SetTargetPostion(const Effekseer::Handle inHandle, const D3DVECTOR& inTarget)
	{ SetTargetPostion(inHandle, inTarget.x, inTarget.y, inTarget.z); }

	virtual void Move(const Effekseer::Handle inHandle, const float inAddX, const float inAddY, const float inAddZ) = 0;
	inline  void Move(const Effekseer::Handle inHandle, const D3DVECTOR& inAdd)
	{ Move(inHandle, inAdd.x, inAdd.y, inAdd.z); }

	virtual D3DVECTOR GetPosition(const Effekseer::Handle inHandle) = 0;
};

//------------------------------------------------------------------------------
//	EffekseerEffect�N���X��`
//------------------------------------------------------------------------------
class CEffekseerEffect : public IEffekseerEffect {
public:
	CEffekseerEffect(Effekseer::Manager* pManager, Effekseer::Effect* pEffect);
	virtual ~CEffekseerEffect();

	virtual Effekseer::Handle Play(const float inX, const float inY, const float inZ);
	virtual void Stop(const Effekseer::Handle inHandle);
	virtual void StopRoot();

	virtual void Pause (const Effekseer::Handle inHandle);
	virtual void Resume(const Effekseer::Handle inHandle);

	virtual void SetVisible(const Effekseer::Handle inHandle, const bool  inVisible);
	virtual void SetSpeed  (const Effekseer::Handle inHandle, const float inSpeed);

	virtual void SetPosition(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ);
	virtual void SetRotation(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ);
	virtual void SetScale   (const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ);

	virtual void SetTargetPostion(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ);

	virtual void Move(const Effekseer::Handle inHandle, const float inAddX, const float inAddY, const float inAddZ);

	virtual D3DVECTOR GetPosition(const Effekseer::Handle inHandle);

private:
	Effekseer::Manager*   m_manager;
	Effekseer::Effect*    m_effect;
};

//------------------------------------------------------------------------------
//	NullEffekseerEffect�N���X��`
//------------------------------------------------------------------------------
class CNullEffekseerEffect : public IEffekseerEffect {
public:
	CNullEffekseerEffect() {}
	virtual ~CNullEffekseerEffect() {}

	virtual Effekseer::Handle Play(const float inX, const float inY, const float inZ) { return -1; }
	virtual void Stop(const Effekseer::Handle inHandle) {}
	virtual void StopRoot() {}

	virtual void Pause (const Effekseer::Handle inHandle) {}
	virtual void Resume(const Effekseer::Handle inHandle) {}

	virtual void SetVisible(const Effekseer::Handle inHandle, const bool  inVisible) {}
	virtual void SetSpeed  (const Effekseer::Handle inHandle, const float inSpeed  ) {}

	virtual void SetPosition(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) {}
	virtual void SetRotation(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) {}
	virtual void SetScale   (const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) {}

	virtual void SetTargetPostion(const Effekseer::Handle inHandle, const float inX, const float inY, const float inZ) {}

	virtual void Move(const Effekseer::Handle inHandle, const float inAddX, const float inAddY, const float inAddZ) {}

	virtual D3DVECTOR GetPosition(const Effekseer::Handle inHandle);
};
