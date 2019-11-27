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
				OpenGLクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_OPENGL
#define _INC_OPENGL

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <windows.h>

#include <gl.h>
#include <glext.h>

#include <d3dx9.h>

//------------------------------------------------------------------------------
//	OpenGLクラス定義
//------------------------------------------------------------------------------
class COpenGL {
public:
	virtual ~COpenGL();

	// シングルトンインスタンスの取得
	static COpenGL& GetInstance() { static COpenGL theOpenGL; return theOpenGL; }

	// 接続
	bool Attach(IDirect3DDevice9* pD3DDevice);
	bool Attach(const HWND hWnd);

	// シーン
	void BeginScene();
	void EndScene();

	// オペレータ
	operator IDirect3DTexture9*() const { return m_pRenderTxt; }

private:
	IDirect3DDevice9*       m_pD3DDevice;
	IDirect3DTexture9*      m_pRenderTxt;

	HWND                    m_hRenderWnd;
	HDC                     m_hRenderDC;
	HGLRC                   m_hRenderCxt;

	PIXELFORMATDESCRIPTOR   m_PixFmtDesc;

private:
	COpenGL();
	void Release();

	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; }}

	COpenGL(const COpenGL&);
	COpenGL& operator=(const COpenGL&);
};

//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
inline COpenGL& OPENGL() { return COpenGL::GetInstance(); }

#endif /* _INC_OPENGL */