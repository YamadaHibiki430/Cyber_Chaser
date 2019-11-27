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
	 GameApp.hpp
				�Q�[���A�v���P�[�V�����N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "SingleApp.hpp"
#include "GameFrameWindow.hpp"
#include "GameProc.hpp"

//------------------------------------------------------------------------------
// �Q�[���A�v���P�[�V�����N���X��`
//------------------------------------------------------------------------------
class CGameApp {
public:
	virtual ~CGameApp() {}

	bool Initialize(const HINSTANCE hInstance);
	void Release();

	int Run();

	// �A�N�Z�X�֐�
	inline HWND GetHWnd() const { return m_GameFrameWindow.GetHWnd(); }

	// �V���O���g���C���X�^���X�擾
	static CGameApp& GetInstance()
	{
		static CGameApp   theGameApp;
		return theGameApp;
	}


private:
	CGameApp();

	// ���b�Z�[�W����
	bool MessageLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

	LRESULT OnPaint        (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnKeyDown      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysKeyDown   (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnSysCommand   (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnActivateApp  (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnCreate       (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnClose        (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnDestroy      (const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnExitMouseMove(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnNCLButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);
	LRESULT OnNCRButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam);

	// �����o�ϐ�
	static CGameApp*   m_pGameApp;			// ���g�ւ̃|�C���^(�E�B���h�E�v���V�[�W���p)

	bool               m_Windowed;			// �E�B���h�E���[�h�t���O
	bool               m_Active;			// �A�N�e�B�u��ԃt���O

	CSingleApp         m_SingleApp;			// ���d�N�����o
	CGameFrameWindow   m_GameFrameWindow;	// �Q�[���p�t���[���E�B���h�E
	CGameProc          m_GameProc;

	CGameApp(const CGameApp&);				// �R�s�[�R���X�g���N�^�𐶐����Ȃ�
	CGameApp& operator=(const CGameApp&);	// ������Z�q�𐶐����Ȃ�
};

//------------------------------------------------------------------------------
//	�Z�k����`
//------------------------------------------------------------------------------
inline CGameApp& GameApp() { return CGameApp::GetInstance(); }
