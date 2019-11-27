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
	 GameWindow.hpp
				�Q�[���p�E�B���h�E�N���X�w�b�_�[�t�@�C��

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
// �Q�[���p�E�B���h�E�N���X��`
//------------------------------------------------------------------------------
class CGameWindow {
public:
	CGameWindow();
	virtual ~CGameWindow() { Destroy(); }

	bool Create(const HINSTANCE hInstance, const bool inWindowed);
	void Destroy();

	bool ChangeMode(const bool inWindowed);

	// �A�N�Z�X�֐�
	HWND GetHWnd()   const { return m_hWnd;   }
	bool GetActive() const { return m_Active; }

private:
	bool Adjust(const bool inWindowed);

	// ���b�Z�[�W����
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProcedure(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

	LRESULT OnPaint      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnKeyDown    (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysKeyDown (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysCommand (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnActivateApp(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnCreate     (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnClose      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnDestroy    (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);

	// �����o�[�ϐ�
	HWND   m_hWnd;
	bool   m_Windowed;						// �E�B���h�E���[�h�t���O 
	bool   m_Active;						// �A�N�e�B�u��ԃt���O

	static CGameWindow*   m_pGameWindow;	// ���g�ւ̃|�C���^(�E�B���h�E�v���V�[�W���p)

	CGameWindow(const CGameWindow&);
	CGameWindow& operator=(const CGameWindow&);
};
