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
	 OpenGL.hpp
				OpenGLクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "OpenGL.hpp"

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "OpenGL32.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
COpenGL::COpenGL() : m_pD3DDevice(NULL), m_pRenderTxt(NULL),
					 m_hRenderWnd(NULL), m_hRenderDC (NULL), m_hRenderCxt(NULL)
{
	::ZeroMemory(&m_PixFmtDesc, sizeof(m_PixFmtDesc));
	m_PixFmtDesc.nSize           = sizeof(m_PixFmtDesc);
	m_PixFmtDesc.nVersion        = 1;
	m_PixFmtDesc.dwFlags         = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	m_PixFmtDesc.iPixelType      = PFD_TYPE_RGBA;
	m_PixFmtDesc.cColorBits      = 24;
//	m_PixFmtDesc.cRedBits        = 0;
//	m_PixFmtDesc.cRedShift       = 0;
//	m_PixFmtDesc.cGreenBits      = 0;
//	m_PixFmtDesc.cGreenShift     = 0;
//	m_PixFmtDesc.cBlueBits       = 0;
//	m_PixFmtDesc.cBlueShift      = 0;
//	m_PixFmtDesc.cAlphaBits      = 0;
//	m_PixFmtDesc.cAlphaShift     = 0;
//	m_PixFmtDesc.cAccumBits      = 0;
//	m_PixFmtDesc.cAccumRedBits   = 0;
//	m_PixFmtDesc.cAccumGreenBits = 0;
//	m_PixFmtDesc.cAccumBlueBits  = 0;
//	m_PixFmtDesc.cAccumAlphaBits = 0;
	m_PixFmtDesc.cDepthBits      = 24;
	m_PixFmtDesc.cStencilBits    = 8;
//	m_PixFmtDesc.cAuxBuffers     = 0;
//	m_PixFmtDesc.iLayerType      = PFD_MAIN_PLANE;
//	m_PixFmtDesc.bReserved       = 0;
//	m_PixFmtDesc.dwLayerMask     = 0;
//	m_PixFmtDesc.dwVisibleMask   = 0;
//	m_PixFmtDesc.dwDamageMask    = 0;
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
COpenGL::~COpenGL()
{
	Release();
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void COpenGL::Release()
{
	if(m_hRenderCxt != NULL) {
		::wglMakeCurrent(m_hRenderDC, NULL);
		::wglDeleteContext(m_hRenderCxt);
		m_hRenderCxt = NULL;
	}

	if(m_hRenderDC != NULL) {
		::ReleaseDC(m_hRenderWnd, m_hRenderDC);
		m_hRenderDC = NULL;
	}

	m_hRenderWnd = NULL;

	SafeRelease(m_pRenderTxt);
	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	接続
//------------------------------------------------------------------------------
bool COpenGL::Attach(IDirect3DDevice9* pD3DDevice)
{
	if(m_pD3DDevice == pD3DDevice)
		return true;

	Release();
	m_pD3DDevice = pD3DDevice;
	m_pD3DDevice->AddRef();

	D3DDEVICE_CREATION_PARAMETERS   dcp;
	m_pD3DDevice->GetCreationParameters(&dcp);
	m_hRenderWnd = dcp.hFocusWindow;

	// 描画領域取得
	RECT   rect;
	::GetClientRect(m_hRenderWnd, &rect);

	// レンダリングイメージ取得テクスチャ生成
	if(::D3DXCreateTexture(m_pD3DDevice, (UINT)rect.right, (UINT)rect.bottom,
						   1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pRenderTxt)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(COpenGL_Attach)\n"));
		Release();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	接続
//------------------------------------------------------------------------------
bool COpenGL::Attach(const HWND hWnd)
{
	Release();
	m_hRenderWnd = hWnd;

	return true;
}

//------------------------------------------------------------------------------
//	シーン開始
//------------------------------------------------------------------------------
void COpenGL::BeginScene()
{
	if(m_hRenderCxt != NULL)
		return;
	if(m_hRenderWnd == NULL)
		return;

	m_hRenderDC = ::GetDC(m_hRenderWnd);

	const int pf_idx = ::ChoosePixelFormat(m_hRenderDC, &m_PixFmtDesc);
	::SetPixelFormat(m_hRenderDC, pf_idx, &m_PixFmtDesc);

	m_hRenderCxt = ::wglCreateContext(m_hRenderDC);
	::wglMakeCurrent(m_hRenderDC, m_hRenderCxt);
}

//------------------------------------------------------------------------------
//	シーン終了
//------------------------------------------------------------------------------
void COpenGL::EndScene()
{
	if(m_pRenderTxt != NULL) {
		D3DLOCKED_RECT   lock;
		if(m_pRenderTxt->LockRect(0, &lock, NULL, 0) == D3D_OK) {
			RECT   rect;
			::GetClientRect(m_hRenderWnd, &rect);

			::glReadBuffer(GL_BACK);
			::glReadPixels(0, 0, rect.right, rect.bottom,
						   GL_BGRA, GL_UNSIGNED_BYTE, lock.pBits);

			m_pRenderTxt->UnlockRect(0);
		}
	} else {
		::SwapBuffers(m_hRenderDC);
	}

	if(m_hRenderCxt != NULL) {
		::wglMakeCurrent(m_hRenderDC, NULL);
		::wglDeleteContext(m_hRenderCxt);
		m_hRenderCxt = NULL;
	}

	if(m_hRenderDC != NULL) {
		::ReleaseDC(m_hRenderWnd, m_hRenderDC);
		m_hRenderDC = NULL;
	}
}
