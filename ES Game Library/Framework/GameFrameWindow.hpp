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
	 GameFrameWindow.hpp
				�Q�[���p�t���[���E�B���h�E�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <windows.h>

//------------------------------------------------------------------------------
// �Q�[���p�t���[���E�B���h�E�N���X��`
//------------------------------------------------------------------------------
class CGameFrameWindow {
public:
	CGameFrameWindow();
	virtual ~CGameFrameWindow() { Destroy(); }

	bool Create(const HINSTANCE hInstance, const WNDPROC inWndProc,
				const UINT inWidth, const UINT inHeight, const bool inWindowed);
	void Destroy();

	bool ChangeMode(const bool inWindowed);

	// �A�N�Z�X�֐�
	inline HWND GetHWnd()   const { return m_hWnd;   }
	inline UINT GetWidth()  const { return m_Width;  }
	inline UINT GetHeight() const { return m_Height; }

private:
	// �����o�[�ϐ�
	HWND   m_hWnd;
	UINT   m_Width;
	UINT   m_Height;

	CGameFrameWindow(const CGameFrameWindow&);
	CGameFrameWindow& operator=(const CGameFrameWindow&);
};
