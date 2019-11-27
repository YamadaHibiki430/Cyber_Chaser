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
	 DXGraphics11.hpp
				DirectX Graphics11�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_DXGRAPHICS11
#define _INC_DXGRAPHICS11

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4005)
#include "Model11.hpp"
#pragma warning(pop)

// #include <..\DirectX11_2\Include\d3d11.h>
// #include <d3dx9.h>

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	DXGraphics11�N���X��`
//------------------------------------------------------------------------------
class CDXGraphics11 {
public:
	virtual ~CDXGraphics11();

	// �V���O���g���C���X�^���X�̎擾
	static CDXGraphics11& GetInstance()
	{
		static CDXGraphics11   theDXGraphics11;
		return theDXGraphics11;
	}

	int  Initialize(const UINT inWidth, UINT inHeight);
	void Release();

	void Clear(const float inR, const float inG, const float inB, const float inA = 0.0f);

	void Present();
	void GetFrameBuffer(void* pImage);

	// �擾
	ID3D11Device*        GetDevice()  const { return m_pDevice;  }
	ID3D11DeviceContext* GetContext() const { return m_pContext; }

	UINT GetFrameBufferWidth () const { return m_FrameBufferWidth;  }
	UINT GetFrameBufferHeight() const { return m_FrameBufferHeight; }

	// �I�y���[�^
	operator ID3D11Device*()        const { return m_pDevice;  }
	operator ID3D11DeviceContext*() const { return m_pContext; }

	// ���f�������E���
	IModel11* CreateModelFromFile(LPCWSTR inFileName);

	void ReleaseModel(IModel11*& pModel);
	void ReleaseAllModels();

	// �v���e�N�g�ݒ�
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	ID3D11Device*             m_pDevice;
	ID3D11DeviceContext*      m_pContext;

	ID3D11Texture2D*          m_pRenderTexture;
	ID3D11RenderTargetView*   m_pRenderView;

	ID3D11Texture2D*          m_pDepthStenciTexture;
	ID3D11DepthStencilView*   m_pDepthStencilView;

	ID3D11Texture2D*          m_pFrameBufferTexture;
	UINT                      m_FrameBufferWidth;
	UINT                      m_FrameBufferHeight;

	std::list<IModel11*>      m_ModelList;
	std::set <void*>          m_ProtectedResource;

private:
	template <class T>
	inline void SafeRelease(T& x)
	{
		if(x != NULL) {
			x->Release();
			x = NULL;
		}
	}

	CDXGraphics11();

	CDXGraphics11(const CDXGraphics11&);
	CDXGraphics11& operator=(const CDXGraphics11&);

// DirectX9�g��
public:
	int Initialize(IDirect3DDevice9* pDevice9);
	inline int Attach(IDirect3DDevice9* pDevice9) { return Initialize(pDevice9); }

	inline DirectX::CommonStates* GetCommonStates() const { return m_upCommonStates.get(); }

	// (BeginScene��Draw...��EndScene��)Present��MapTexture��GetTexture��SpriteBatch.Draw or
	// (BeginScene��Draw...��EndScene��)Present��operator IDirect3DTexture9*
	void BeginScene();
	void EndScene();

	void Draw(IModel11*& pModel);

	void MapTexture();
	inline IDirect3DTexture9* GetTexture() const { return m_pTexture9; }

	operator IDirect3DTexture9*() const;

private:
	IDirect3DDevice9*    m_pDevice9;
	IDirect3DTexture9*   m_pTexture9;

	std::unique_ptr<DirectX::CommonStates>   m_upCommonStates;
	DirectX::SimpleMath::Matrix              m_View;
	DirectX::SimpleMath::Matrix              m_Projection;
};

//------------------------------------------------------------------------------
//	�}�N����`
//------------------------------------------------------------------------------
inline CDXGraphics11& DXGraphics11() { return CDXGraphics11::GetInstance(); }

#endif /* _INC_DXGRAPHICS11 */