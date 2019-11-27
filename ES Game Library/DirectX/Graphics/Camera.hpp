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
	 Camera.hpp
				カメラクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DXGTypes.h"

//------------------------------------------------------------------------------
//	カメラクラス定義
//------------------------------------------------------------------------------
class CCamera {
public:
	CCamera();
	virtual ~CCamera();

	// 設定
	void SetView(const D3DXMATRIX& inView);
	void SetView(const D3DXVECTOR3& inPosition, const D3DXVECTOR3& inRotation);

	void SetLookAt(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inAt, const D3DXVECTOR3& inUp);
	inline void SetLookAt(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inAt, const float inRoll)
	{ SetLookAt(inEye, inAt, D3DXVECTOR3(-::sinf(D3DXToRadian(inRoll)), ::cosf(D3DXToRadian(inRoll)), 0.0f)); }

	void SetViewFromWorldMatrix(const D3DXMATRIX& inWorldMatrix);

	void SetRevolution(const D3DXVECTOR3& inAt, const D3DXVECTOR3& inRotation);

	void SetPosition(const float inX, const float inY, const float inZ);
	void SetAngle   (const float inX, const float inY, const float inZ);
	void SetAngle   (const D3DXQUATERNION& inAngle) { m_Angle = inAngle; m_Update = true; }
	void SetAngle   (const D3DXMATRIX&     inAngle);

	void Move    (const float inRight, const float inUp, const float inFront);
	void Rotation(const float inX,     const float inY,  const float inZ);

	inline void SetPosition(const D3DXVECTOR3& inPosition) { SetPosition(inPosition.x, inPosition.y, inPosition.z); }
	inline void SetAngle   (const D3DXVECTOR3& inRotation) { SetAngle   (inRotation.x, inRotation.y, inRotation.z); }
	inline void Move       (const D3DXVECTOR3& inDistance) { Move       (inDistance.x, inDistance.y, inDistance.z); }
	inline void Rotation   (const D3DXVECTOR3& inRotation) { Rotation   (inRotation.x, inRotation.y, inRotation.z); }

	void SetProjection(const D3DXMATRIX& inProjection);

	void SetPerspectiveFieldOfView(const float inFovY, const float inAspect, const float inZn, const float inZf);
	inline void SetPerspectiveFieldOfView(const float inFovY, const float inWidth, const float inHeight,
										  const float inZn, const float inZf)
	{ SetPerspectiveFieldOfView(inFovY, inWidth / inHeight, inZn, inZf); }

	void SetOrthographic(const float inWidth, const float inHeight, const float inZn, const float inZf);
	void SetOrthographicOffCenter(const float inLeft,       const float inRight,
								  const float inBottom,     const float inTop,
								  const float inZNearPlane, const float inZFarPlane);

	void SetFieldOfViewY(const float inFovY);

	void SetDevice(IDirect3DDevice9* pD3DDevice);

	// 取得
	inline D3DXVECTOR3    GetPosition()             const { return m_Position;               }
	inline D3DXQUATERNION GetAngleQuaternion()      const { return m_Angle;                  }
	inline D3DXMATRIX     GetViewMatrix()           const { return m_View;                   }
	inline D3DXMATRIX     GetProjectionMatrix()     const { return m_Projection;             }
	inline D3DXMATRIX     GetViewProjectionMatrix() const { return m_View * m_Projection;    }
	inline D3DXMATRIX     GetCameraMatrix()         const { return m_View * m_Projection;    }
	inline D3DXVECTOR4    GetTranslation()                { return (D3DXVECTOR4)m_View.m[3]; }

	D3DXVECTOR3           GetAngle();
	D3DXVECTOR3           GetAngleRadian();
	D3DXMATRIX            GetAngleMatrix();

	D3DXMATRIX            GetWorldMatrix() const;
	D3DXVECTOR3           GetFrontVector() const;
	D3DXVECTOR3           GetRightVector() const;
	D3DXVECTOR3           GetUpVector   () const;

	DXGVIEWFRUSTUM        GetViewFrustum() const;
	DXGBOUNDINGSPHERE     GetBoundingSphere(const float inRadius) const;
	DXGOBB                GetOBB(const float inRadius) const;

	inline void SetRotation(const float inX, const float inY, const float inZ)
	{ SetAngle(inX, inY, inZ); }
	inline void SetRotation(const D3DXVECTOR3& inAngle)
	{ SetAngle(inAngle.x, inAngle.y, inAngle.z); }
	inline D3DXVECTOR3 GetRotation()
	{ return GetAngle(); }

	inline CCamera* operator->() { return this; }

private:
	D3DXVECTOR3          m_Position;
	D3DXQUATERNION       m_Angle;

	D3DXMATRIX           m_View;
	D3DXMATRIX           m_Projection;

	bool                 m_Update;
};
