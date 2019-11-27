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
	 OpenGL.hpp
				OpenGL�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_OPENGL
#define _INC_OPENGL

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <windows.h>

#include <gl.h>
#include <glext.h>

#include <d3dx9.h>

//------------------------------------------------------------------------------
//	OpenGL�N���X��`
//------------------------------------------------------------------------------
class COpenGL {
public:
	virtual ~COpenGL();

	// �V���O���g���C���X�^���X�̎擾
	static COpenGL& GetInstance() { static COpenGL theOpenGL; return theOpenGL; }

	// �ڑ�
	bool Attach(IDirect3DDevice9* pD3DDevice);
	bool Attach(const HWND hWnd);

	// �V�[��
	void BeginScene();
	void EndScene();

	// �I�y���[�^
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
//	�C�����C���֐���`
//------------------------------------------------------------------------------
inline COpenGL& OPENGL() { return COpenGL::GetInstance(); }

#endif /* _INC_OPENGL */