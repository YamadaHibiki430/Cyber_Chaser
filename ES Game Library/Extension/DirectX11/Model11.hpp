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
	 Model11.hpp
				モデル(DirectX11)クラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <Model.h>
#include <CommonStates.h>
#include <SimpleMath.h>

#include <d3dx9.h>

#include <btBulletDynamicsCommon.h>

//------------------------------------------------------------------------------
//	モデルインタフェース定義
//------------------------------------------------------------------------------
class IModel11 {
public:
	virtual ~IModel11() {}

	virtual bool IsNull() const = 0;

	virtual void Draw(ID3D11DeviceContext* pContext, DirectX::CommonStates& inStates,
					  DirectX::CXMMATRIX& inView, DirectX::CXMMATRIX& inProjection) = 0;

	virtual void SetPosition (const float x, const float y, const float z) = 0;
	virtual void SetScale    (const float x, const float y, const float z) = 0;
	virtual void SetDirection(const float x, const float y, const float z) = 0;
	virtual void SetDirection(const D3DXQUATERNION& inDirection) = 0;

	inline  void SetPosition (const D3DXVECTOR3& inPosition ) { SetPosition (inPosition .x, inPosition .y, inPosition .z); }
	inline  void SetScale    (const D3DXVECTOR3& inScale)     { SetScale    (inScale    .x, inScale    .y, inScale    .z); }
	inline  void SetScale    (const float s)                  { SetScale    (s, s, s); }
	inline  void SetDirection(const D3DXVECTOR3& inDirection) { SetDirection(inDirection.x, inDirection.y, inDirection.z); }

	inline  void SetRotation (const float x, const float y, const float z)
	{ SetDirection(x, y, z); }
	inline  void SetRotation (const D3DXVECTOR3& inDirection)
	{ SetDirection(inDirection.x, inDirection.y, inDirection.z); }

	virtual void Move     (const float inRight, const float inUp, const float inFront) = 0;
	virtual void Rotation (const float inX,     const float inY,  const float inZ)     = 0;

	inline  void Move     (const D3DXVECTOR3& inDistance) { Move    (inDistance.x, inDistance.y, inDistance.z); }
	inline  void Rotation (const D3DXVECTOR3& inRotation) { Rotation(inRotation.x, inRotation.y, inRotation.z); }

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle) = 0;
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront) = 0;

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY) = 0;

	virtual D3DXVECTOR3	   GetPosition() const = 0;
	virtual D3DXVECTOR3	   GetScale   () const = 0;

	virtual D3DXVECTOR3    GetDirection() const = 0;
	virtual D3DXVECTOR3    GetDirectionRadian() const = 0;
	virtual D3DXQUATERNION GetDirectionQuaternion() const = 0;

	inline  D3DXVECTOR3    GetRotation() const { return GetDirection(); }

	virtual DirectX::SimpleMath::Matrix GetWorldMatrix() const = 0;

	virtual D3DXVECTOR3    GetFrontVector() const = 0;
	virtual D3DXVECTOR3    GetRightVector() const = 0;
	virtual D3DXVECTOR3    GetUpVector   () const = 0;

#ifdef BULLET_DYNAMICS_COMMON_H
	virtual btTransform btGetTransform() = 0;
	virtual void btSetTransform(const btTransform& inTransform) = 0;

	inline  void btSetTransform(btMotionState* pMotion)
	{
		btTransform   transform;
		pMotion->getWorldTransform(transform);
		btSetTransform(transform);
	}
#endif
};

//------------------------------------------------------------------------------
//	モデルクラス定義
//------------------------------------------------------------------------------
class CModel11 : public IModel11 {
public:
	CModel11(std::unique_ptr<DirectX::Model> upModel);
	virtual ~CModel11();

	virtual bool IsNull() const { return false; }

	virtual void Draw(ID3D11DeviceContext* pContext, DirectX::CommonStates& inStates,
					  DirectX::CXMMATRIX& inView, DirectX::CXMMATRIX& inProjection);

	virtual void SetPosition(const float x, const float y, const float z)
	{ m_Position.x = x; m_Position.y = y; m_Position.z = z; }

	virtual void SetScale(const float x, const float y, const float z)
	{ m_Scale.x = x; m_Scale.y = y; m_Scale.z = z; }

	virtual void SetDirection(const float x, const float y, const float z);

	virtual void SetDirection(const D3DXQUATERNION& inDirection)
	{ m_Rotation = inDirection; }

	virtual void Move    (const float inRight, const float inUp, const float inFront);
	virtual void Rotation(const float inX,     const float inY,  const float inZ);

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle);
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront);

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY);

	virtual D3DXVECTOR3	   GetPosition() const { return m_Position; }
	virtual D3DXVECTOR3	   GetScale   () const { return m_Scale;    }

	virtual D3DXVECTOR3    GetDirection() const;
	virtual D3DXVECTOR3    GetDirectionRadian() const;
	virtual D3DXQUATERNION GetDirectionQuaternion() const { return m_Rotation; }

	virtual DirectX::SimpleMath::Matrix GetWorldMatrix() const;

	virtual D3DXVECTOR3    GetFrontVector() const;
	virtual D3DXVECTOR3    GetRightVector() const;
	virtual D3DXVECTOR3    GetUpVector   () const;

#ifdef BULLET_DYNAMICS_COMMON_H
public:
	virtual btTransform btGetTransform()
	{
		btQuaternion   qt (m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);
		btVector3      pos(m_Position.x, m_Position.y, m_Position.z);
		return btTransform(qt, pos);
	}

	virtual void btSetTransform(const btTransform& inTransform)
	{
		btVector3     pos = inTransform.getOrigin();
		m_Position.x = pos.getX();
		m_Position.y = pos.getY();
		m_Position.z = pos.getZ();

		btQuaternion  qt = inTransform.getRotation();
		m_Rotation.x = qt.getX();
		m_Rotation.y = qt.getY();
		m_Rotation.z = qt.getZ();
		m_Rotation.w = qt.getW();
	}
#endif

private:
	std::unique_ptr<DirectX::Model>   m_upModel;

	D3DXVECTOR3      m_Position;
	D3DXVECTOR3      m_Scale;
	D3DXQUATERNION   m_Rotation;
};

//------------------------------------------------------------------------------
//	NULLモデルクラス定義
//------------------------------------------------------------------------------
class CNullModel11 : public IModel11 {
public:
	virtual ~CNullModel11() {}

	virtual bool IsNull() const { return true; }

	virtual void Draw(ID3D11DeviceContext* pContext, DirectX::CommonStates& inStates,
					  DirectX::CXMMATRIX& inView, DirectX::CXMMATRIX& inProjection) {}

	virtual void SetPosition (const float x, const float y, const float z) {}
	virtual void SetScale    (const float x, const float y, const float z) {}
	virtual void SetDirection(const float x, const float y, const float z) {}
	virtual void SetDirection(const D3DXQUATERNION& inRotation) {}

	virtual void Move    (const float inRight, const float inUp, const float inFront) {}
	virtual void Rotation(const float inX,     const float inY,  const float inZ)     {}

	virtual void SetRotationBillboard (const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle) {}
	virtual void SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront) {}

	virtual void AlignmentAxisY(const D3DXVECTOR3& inAxisY) {}

	virtual DWORD GetMeshes() const { return 0; }

	virtual D3DXVECTOR3	GetPosition() const { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3	GetScale   () const { return D3DXVECTOR3(1.0f, 1.0f, 1.0f); }

	virtual D3DXVECTOR3    GetDirection()           const { return D3DXVECTOR3   (0.0f, 0.0f, 0.0f);       }
	virtual D3DXVECTOR3    GetDirectionRadian()     const { return D3DXVECTOR3   (0.0f, 0.0f, 0.0f);       }
	virtual D3DXQUATERNION GetDirectionQuaternion() const { return D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f); }

	virtual DirectX::SimpleMath::Matrix GetWorldMatrix() const
	{
		DirectX::SimpleMath::Matrix   world;
		return world;
	}

	virtual D3DXVECTOR3 GetFrontVector() const { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3 GetRightVector() const { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3 GetUpVector   () const { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }

#ifdef BULLET_DYNAMICS_COMMON_H
	virtual btTransform btGetTransform() { return btTransform::getIdentity(); }
	virtual void btSetTransform(const btTransform& inTransform) {}
#endif
};
