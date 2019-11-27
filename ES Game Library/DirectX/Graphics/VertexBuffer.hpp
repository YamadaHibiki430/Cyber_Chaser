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
	 VertexBuffer.hpp
				���_�o�b�t�@�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "DXGTypes.h"
#include "Effect.hpp"
#include <map>

//------------------------------------------------------------------------------
//	���_�o�b�t�@�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IVertexBuffer {
public:
	virtual ~IVertexBuffer() {}

	// ���b�N
	virtual LPVOID Lock() = 0;
	virtual void Unlock() = 0;

	// �`��
	virtual void Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) = 0;
	virtual void Draw(IEffect*& pEffect,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) = 0;
	virtual void Draw(IEffect*& pEffect, const UINT inPass,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) = 0;

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass,
						  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) = 0;

	virtual void DrawPure(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) = 0;

	virtual void DrawAlpha (const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const float inAlpha, const DWORD inDraw = DXGDRAWA_BOTHFACES) = 0;

	virtual void DrawShadow(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const D3DXMATRIX& inShadowMatrix, const float inDepth) = 0;

	inline void Draw(const D3DPRIMITIVETYPE inType, const UINT inCount)
	{ Draw(inType, 0, inCount); }

	inline void Draw(IEffect*& pEffect, const D3DPRIMITIVETYPE inType, const UINT inCount)
	{ Draw(pEffect, inType, 0, inCount); }

	inline void Draw(IEffect*& pEffect, const UINT inPass,
			  const D3DPRIMITIVETYPE inType, const UINT inCount)
	{ Draw(pEffect, inPass, inType, 0, inCount); }

	inline void DrawPass(IEffect*& pEffect, const UINT inPass,
				  const D3DPRIMITIVETYPE inType, const UINT inCount)
	{ DrawPass(pEffect, inPass, inType, 0, inCount); }

	inline void DrawPure(const D3DPRIMITIVETYPE inType, const UINT inCount)
	{ DrawPure(inType, 0, inCount); }

	inline void DrawAlpha (const D3DPRIMITIVETYPE inType, const UINT inCount,
						   const float inAlpha, const bool inStencil = false)
	{ DrawAlpha (inType, 0, inCount, inAlpha, inStencil); }

	inline void DrawShadow(const D3DPRIMITIVETYPE inType, const UINT inCount,
						   const D3DXMATRIX& inShadowMatrix, const float inDepth)
	{ DrawShadow(inType, 0, inCount, inShadowMatrix, inDepth); }

	// FVF�ݒ�
	virtual void SetFVF(const DWORD inFVF, const UINT inStride = 0) = 0;

	// ���[���h���W�̈ʒu�w��A�X�P�[�����O�w��A��]�p�w��
	virtual void SetPosition(const D3DXVECTOR3& inPosition) = 0;
	virtual void SetScale   (const D3DXVECTOR3& inScale)    = 0;
	virtual void SetRotation(const D3DXVECTOR3& inRotation) = 0;

	virtual void SetPosition(const float x, const float y, const float z) = 0;
	virtual void SetScale	(const float x, const float y, const float z) = 0;
	virtual void SetScale	(const float s) = 0;
	virtual void SetRotation(const float x, const float y, const float z) = 0;

	// �ړ��E��]
	virtual void Move    (const float inRight, const float inUp, const float inFront) = 0;
	virtual void Rotation(const float inX,     const float inY,  const float inZ)     = 0;

	inline  void Move    (const D3DXVECTOR3& inDistance) { Move    (inDistance.x, inDistance.y, inDistance.z); }
	inline  void Rotation(const D3DXVECTOR3& inRotation) { Rotation(inRotation.x, inRotation.y, inRotation.z); }

	// �ێ����Ă���ʒu�A�X�P�[�����O�A��]�p��Ԃ�
	virtual D3DXVECTOR3	GetPosition() const = 0;
	virtual D3DXVECTOR3	GetScale   () const = 0;
	virtual D3DXVECTOR3	GetRotation() const = 0;

	virtual D3DXMATRIX  GetWorldMatrix() const = 0;

	// �}�e���A��
	virtual D3DMATERIAL9 GetMaterial() const = 0;

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) = 0;
	virtual void SetMaterial(const D3DXMATERIAL& inMaterial) = 0;

	// �e�N�X�`��
	virtual IDirect3DTexture9* GetTexture() const = 0;

	virtual void SetTexture(LPCTSTR inFileName) = 0;
	virtual void SetTexture(IDirect3DTexture9* pTexture) = 0;

	// ���E��
	virtual DXGBOUNDINGSPHERE GetBoundingSphere()     = 0;
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere() = 0;

	virtual DXGOBB GetOBB()     = 0;
	virtual DXGOBB GetBaseOBB() = 0;
};

//------------------------------------------------------------------------------
//	���_�o�b�t�@�N���X��`
//------------------------------------------------------------------------------
class CVertexBuffer : public IVertexBuffer {
public:
	CVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer,
				  const DWORD inFVF, const UINT inStride = 0, const bool inWriteOnly = false);
	virtual ~CVertexBuffer();

	virtual LPVOID Lock();
	virtual void Unlock();

	virtual void Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);
	virtual void Draw(IEffect*& pEffect,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);
	virtual void Draw(IEffect*& pEffect, const UINT inPass,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass,
						  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);

	virtual void DrawPure(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);

	virtual void DrawAlpha (const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const float inAlpha, const DWORD inDraw);

	virtual void DrawShadow(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const D3DXMATRIX& inShadowMatrix, const float inDepth);

	virtual void SetFVF(const DWORD inFVF, const UINT inStride);

	virtual void SetPosition(const D3DXVECTOR3& inPosition) { m_Position = inPosition; }
	virtual void SetScale   (const D3DXVECTOR3& inScale)    { m_Scale    = inScale;    }
	virtual void SetRotation(const D3DXVECTOR3& inRotation) { m_Rotation = inRotation; }

	virtual void SetPosition(const float x, const float y, const float z)
	{ m_Position.x = x; m_Position.y = y; m_Position.z = z; }

	virtual void SetScale	(const float x, const float y, const float z)
	{ m_Scale.x = x; m_Scale.y = y; m_Scale.z = z; }

	virtual void SetScale	(const float s)
	{ m_Scale.x = s; m_Scale.y = s; m_Scale.z = s; }

	virtual void SetRotation(const float x, const float y, const float z)
	{ m_Rotation.x = x; m_Rotation.y = y; m_Rotation.z = z; }

	virtual void Move    (const float inRight, const float inUp, const float inFront);
	virtual void Rotation(const float inX,     const float inY,  const float inZ);

	virtual D3DXVECTOR3	   GetPosition()    const { return m_Position; }
	virtual D3DXVECTOR3	   GetScale   ()    const { return m_Scale;    }
	virtual D3DXVECTOR3	   GetRotation()    const { return m_Rotation; }
	virtual D3DXMATRIX     GetWorldMatrix() const;

	virtual D3DMATERIAL9      GetMaterial() const { return m_Material; }
	virtual IDirect3DTexture9* GetTexture() const { return m_pTexture; }

#ifdef UNICODE
	virtual void SetMaterial(const D3DXMATERIAL& inMaterial);
#else
	virtual void SetMaterial(const D3DXMATERIAL& inMaterial)
	{ SetMaterial(inMaterial.MatD3D); SetTexture(inMaterial.pTextureFilename); }
#endif

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) { m_Material = inMaterial; }

	virtual void SetTexture(LPCTSTR inFileName);
	virtual void SetTexture(IDirect3DTexture9* pTexture);

	// ���E��
	virtual DXGBOUNDINGSPHERE GetBoundingSphere();
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere();

	// OBB
	virtual DXGOBB GetOBB();
	virtual DXGOBB GetBaseOBB();

private:
	IDirect3DVertexBuffer9*	  m_pVertexBuffer;
	IDirect3DDevice9*		  m_pD3DDevice;
	IDirect3DTexture9*        m_pTexture;

	DWORD                     m_FVF;			// FVF�t���O
	UINT                      m_Stride;			// ���̒��_�܂ł̃o�C�g��

	DWORD                     m_LockFlags;		// ���b�N�t���O

	// �ʒu�A�g�嗦�A��]�p
	D3DXVECTOR3               m_Position;
	D3DXVECTOR3		    	  m_Scale;
	D3DXVECTOR3               m_Rotation;

	// �}�e���A��
	D3DMATERIAL9			  m_Material;

	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}
};

//------------------------------------------------------------------------------
//	���_�o�b�t�@�N���X��`(���[�U�[�|�C���^)
//------------------------------------------------------------------------------
class CVertexBufferUP : public IVertexBuffer {
public:
	CVertexBufferUP(IDirect3DDevice9* pD3DDevice,
					void* pVertexBuffer, const DWORD inBufferSize,
					const DWORD inFVF,   const UINT  inStride = 0);
	virtual ~CVertexBufferUP();

	virtual LPVOID Lock() { return m_pVertexBuffer; }
	virtual void Unlock() {}

	virtual void Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);
	virtual void Draw(IEffect*& pEffect,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);
	virtual void Draw(IEffect*& pEffect, const UINT inPass,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass,
						  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);

	virtual void DrawPure(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount);

	virtual void DrawAlpha (const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const float inAlpha, const DWORD inDraw);

	virtual void DrawShadow(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const D3DXMATRIX& inShadowMatrix, const float inDepth);

	virtual void SetFVF(const DWORD inFVF, const UINT inStride);

	virtual void SetPosition(const D3DXVECTOR3& inPosition) { m_Position = inPosition; }
	virtual void SetScale   (const D3DXVECTOR3& inScale)    { m_Scale    = inScale;    }
	virtual void SetRotation(const D3DXVECTOR3& inRotation) { m_Rotation = inRotation; }

	virtual void SetPosition(const float x, const float y, const float z)
	{ m_Position.x = x; m_Position.y = y; m_Position.z = z; }

	virtual void SetScale	(const float x, const float y, const float z)
	{ m_Scale.x = x; m_Scale.y = y; m_Scale.z = z; }

	virtual void SetScale	(const float s)
	{ m_Scale.x = s; m_Scale.y = s; m_Scale.z = s; }

	virtual void SetRotation(const float x, const float y, const float z)
	{ m_Rotation.x = x; m_Rotation.y = y; m_Rotation.z = z; }

	virtual void Move    (const float inRight, const float inUp, const float inFront);
	virtual void Rotation(const float inX,     const float inY,  const float inZ);

	virtual D3DXVECTOR3	   GetPosition()    const { return m_Position; }
	virtual D3DXVECTOR3	   GetScale   ()    const { return m_Scale;    }
	virtual D3DXVECTOR3	   GetRotation()    const { return m_Rotation; }
	virtual D3DXMATRIX     GetWorldMatrix() const;

	virtual D3DMATERIAL9      GetMaterial() const { return m_Material; }
	virtual IDirect3DTexture9* GetTexture() const { return m_pTexture; }

#ifdef UNICODE
	virtual void SetMaterial(const D3DXMATERIAL& inMaterial);
#else
	virtual void SetMaterial(const D3DXMATERIAL& inMaterial)
	{ SetMaterial(inMaterial.MatD3D); SetTexture(inMaterial.pTextureFilename); }
#endif

	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) { m_Material = inMaterial; }

	virtual void SetTexture(LPCTSTR inFileName);
	virtual void SetTexture(IDirect3DTexture9* pTexture);

	// ���E��
	virtual DXGBOUNDINGSPHERE GetBoundingSphere();
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere();

	// OBB
	virtual DXGOBB GetOBB();
	virtual DXGOBB GetBaseOBB();

private:
	void*                     m_pVertexBuffer;
	IDirect3DDevice9*		  m_pD3DDevice;
	IDirect3DTexture9*        m_pTexture;

	const DWORD               m_BufferSize;		// �o�b�t�@�[�T�C�Y
	DWORD                     m_FVF;			// FVF�t���O
	UINT                      m_Stride;			// ���̒��_�܂ł̃o�C�g��

	// �ʒu�A�g�嗦�A��]�p
	D3DXVECTOR3               m_Position;
	D3DXVECTOR3		    	  m_Scale;
	D3DXVECTOR3               m_Rotation;

	// �}�e���A��
	D3DMATERIAL9			  m_Material;

	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}
};

//------------------------------------------------------------------------------
//	NULL���_�o�b�t�@�N���X��`
//------------------------------------------------------------------------------
class CNullVertexBuffer : public IVertexBuffer {
public:
	virtual ~CNullVertexBuffer() {}

	virtual LPVOID Lock() { return NULL; }
	virtual void Unlock() {}

	virtual void Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) {}
	virtual void Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
					  const bool inTransform, const bool inMaterial) {}

	virtual void Draw(IEffect*& pEffect,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) {}
	virtual void Draw(IEffect*& pEffect, const UINT inPass,
					  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) {}

	virtual void DrawPass(IEffect*& pEffect, const UINT inPass,
						  const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) {}

	virtual void DrawPure(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount) {}

	virtual void DrawAlpha (const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const float inAlpha, const DWORD inDraw) {}

	virtual void DrawShadow(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							const D3DXMATRIX& inShadowMatrix, const float inDepth) {}

	virtual void SetFVF(const DWORD inFVF, const UINT inStride) {}

	virtual void SetPosition(const D3DXVECTOR3& inPosition) {}
	virtual void SetScale   (const D3DXVECTOR3& inScale)    {}
	virtual void SetRotation(const D3DXVECTOR3& inRotation) {}

	virtual void SetPosition(const float x, const float y, const float z) {}
	virtual void SetScale	(const float x, const float y, const float z) {}
	virtual void SetScale	(const float s) {}
	virtual void SetRotation(const float x, const float y, const float z) {}

	virtual void Move    (const float inRight, const float inUp, const float inFront) {}
	virtual void Rotation(const float inX,     const float inY,  const float inZ)     {}

	virtual D3DXVECTOR3	GetPosition() const { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3	GetScale   () const { return D3DXVECTOR3(1.0f, 1.0f, 1.0f); }
	virtual D3DXVECTOR3	GetRotation() const { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }

	virtual D3DXMATRIX  GetWorldMatrix() const
	{ 
		D3DXMATRIX   world;
		::D3DXMatrixIdentity(&world);
		return world;
	}

	virtual D3DMATERIAL9 GetMaterial() const 
	{
		D3DMATERIAL9 inMaterial;
		::ZeroMemory(&inMaterial, sizeof(inMaterial));
		return inMaterial;
	}

	virtual IDirect3DTexture9* GetTexture() const { return NULL; }

	virtual void SetMaterial(const D3DXMATERIAL& inMaterial) {}
	virtual void SetMaterial(const D3DMATERIAL9& inMaterial) {}

	virtual void SetTexture(LPCTSTR inFileName) {}	
	virtual void SetTexture(IDirect3DTexture9* pTexture) {}

	virtual DXGBOUNDINGSPHERE GetBoundingSphere()     { return DXGBOUNDINGSPHERE(); }
	virtual DXGBOUNDINGSPHERE GetBaseBoundingSphere() { return DXGBOUNDINGSPHERE(); }

	virtual DXGOBB GetOBB()     { return DXGOBB(); }
	virtual DXGOBB GetBaseOBB() { return DXGOBB(); }
};
