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
	 Sprite3D.hpp
				Sprite3D�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	���[�j���O�}��
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4250)

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "RenderTarget.hpp"
#include "Camera.hpp"

#include <list>

//------------------------------------------------------------------------------
//	Sprite3D�N���X��`
//------------------------------------------------------------------------------
class CSprite3D {
public:
	// �V���O���g���C���X�^���X�̎擾
	static CSprite3D& GetInstance()
	{
		static CSprite3D theSprite3D;
		return theSprite3D;
	}

	virtual ~CSprite3D();

	bool Initialize(IDirect3DDevice9* pD3Device);
	void Release();

	void Begin();
	void End(const bool inRestoreState = true);


private:
	CSprite3D();

	template <class T>
	inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	// �R�s�[�ł��Ȃ��悤�ɂ���
	CSprite3D(const CSprite3D&);				// �R�s�[�R���X�g���N�^
	CSprite3D& operator=(const CSprite3D&);		// ������Z�q

private:
	IDirect3DDevice9*    m_pD3DDevice;

	bool                 m_Begin;

	IDirect3DSurface9*   m_pRenderSurface;
	IDirect3DSurface9*   m_pDepthStencil;

	D3DXMATRIX           m_View;
	D3DXMATRIX           m_Projection;
	D3DVIEWPORT9         m_Viewport;
};

//------------------------------------------------------------------------------
//	�C�����C���֐�
//------------------------------------------------------------------------------
inline CSprite3D& DXGSprite3D() { return CSprite3D::GetInstance(); }

//------------------------------------------------------------------------------
//	Sprite3DLayer�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class ISprite3DLayer : public virtual IRenderTarget {
public:
	virtual void SetInitialView(const D3DXMATRIX& inView) = 0;
	virtual void SetInitialView(const CCamera&  inCamera) = 0;

	virtual void SetInitialProjection(const D3DXMATRIX& inProjection) = 0;
	virtual void SetInitialProjection(const CCamera&    inCamera) = 0;

	virtual void SetInitialCameraSetting(const CCamera& inCamera) = 0;

	virtual void SetInitialRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue) = 0;
	virtual void SetInitialFloatRenderState(const D3DRENDERSTATETYPE inState, float inValue)
	{ SetInitialRenderState(inState, *((DWORD*)&inValue)); }

	virtual void ClearInitialRenderState() = 0;

	virtual void BeginRender(const bool inInitView     = true,
							 const bool inInitProj     = true,
							 const bool inSetState     = true) = 0;
	virtual void EndRender  (const bool inRestoreState = true) = 0;

	virtual void SetRenderRect(const RECT& inRenderRect) = 0;

	virtual void Clear(const D3DCOLOR inColor = 0) = 0;
};

//------------------------------------------------------------------------------
//	Sprite3DLayer�N���X��`
//------------------------------------------------------------------------------
class CSprite3DLayer : public ISprite3DLayer, public CRenderTarget {
public:
	CSprite3DLayer(IDirect3DTexture9* pTexture, IDirect3DSurface9* pDepthStencil,
				   const UINT inWidth, const UINT inHeight);

	virtual ~CSprite3DLayer() {}

	virtual void SetInitialView(const D3DXMATRIX& inView)
	{ 
		m_InitialView = inView;
	}

	virtual void SetInitialView(const CCamera& inCamera)
	{ 
		m_InitialView = inCamera.GetViewMatrix();
	}

	virtual void SetInitialProjection(const D3DXMATRIX& inProjection)
	{ 
		m_InitialProjection = inProjection;
	}

	virtual void SetInitialProjection(const CCamera& inCamera)
	{ 
		m_InitialProjection = inCamera.GetProjectionMatrix();
	}

	virtual void SetInitialCameraSetting(const CCamera& inCamera)
	{ 
		m_InitialView       = inCamera.GetViewMatrix();
		m_InitialProjection = inCamera.GetProjectionMatrix();
	}

	virtual void SetInitialRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue);

	virtual void ClearInitialRenderState()
	{
		m_StateList.clear();
	}

	virtual void BeginRender(const bool inInitView, const bool inInitProj,
							 const bool inSetState);
	virtual void EndRender(const bool inRestoreState);

	virtual void SetRenderRect(const RECT& inRenderRect);

	virtual void Clear(const D3DCOLOR inColor)
	{ CRenderTarget::Clear(0x00ffffff & inColor); }

private:
	D3DXMATRIX   m_InitialView;
	D3DXMATRIX   m_InitialProjection;

	// �X�e�[�g�l�ۑ��\����
	struct STATE {
		D3DRENDERSTATETYPE Type;	// �X�e�[�g�^�C�v
		DWORD	           Value;	// �ݒ�l
		DWORD	           Restore;	// �����l
	};
	std::list<STATE>   m_StateList;
};

//------------------------------------------------------------------------------
//	NullSprite3DLayer�N���X��`
//------------------------------------------------------------------------------
class CNullSprite3DLayer : public ISprite3DLayer, public CNullRenderTarget {
public:
	virtual void SetInitialView(const D3DXMATRIX& inView) {}
	virtual void SetInitialView(const CCamera&  inCamera) {}

	virtual void SetInitialProjection(const D3DXMATRIX& inProjection) {}
	virtual void SetInitialProjection(const CCamera&    inCamera)     {}

	virtual void SetInitialCameraSetting(const CCamera& inCamera) {}

	virtual void SetInitialRenderState(const D3DRENDERSTATETYPE inState,
									   const DWORD inValue) {}
	virtual void ClearInitialRenderState() {}

	virtual void BeginRender(const bool inInitView, const bool inInitProj,
							 const bool inSetState) {}
	virtual void EndRender  (const bool inRestoreState) {}
	virtual void SetRenderRect(const RECT& inRenderRect) {}

	virtual void Clear(const D3DCOLOR inColor) {}
};

#pragma warning(pop)
