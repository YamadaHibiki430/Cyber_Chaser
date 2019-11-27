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
	 GameFrameWindow.cpp
				�Q�[���p�t���[���E�B���h�E

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "GameFrameWindow.hpp"
#include "../../Resource/resource.h"
#include <imm.h>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "imm32.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CGameFrameWindow::CGameFrameWindow() : m_hWnd(NULL)
{
}

//------------------------------------------------------------------------------
//	�E�B���h�E����
//------------------------------------------------------------------------------
bool CGameFrameWindow::Create(const HINSTANCE hInstance, const WNDPROC inWndProc,
							  const UINT inWidth, const UINT inHeight, const bool inWindowed)
{
	Destroy();

	LPCTSTR   WindowClassName = TEXT("OOGAMEPROG_WINDOWCLASS");	// �E�B���h�E�N���X��

	WNDCLASSEX   wcx;
	// �E�B���h�E�N���X���o�^�ς݂����ׂ�
	if(::GetClassInfoEx(hInstance, WindowClassName, &wcx) == 0) {
		// �E�B���h�E�N���X�o�^
		::ZeroMemory(&wcx, sizeof(wcx));
		wcx.cbSize        = sizeof(wcx);
		wcx.style         = 0;
		wcx.lpfnWndProc   = inWndProc;
		wcx.cbClsExtra    = 0;
		wcx.cbWndExtra    = 0;
		wcx.hInstance     = hInstance;
		wcx.hIcon         = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MENUICON));
		wcx.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
		wcx.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
		wcx.lpszClassName = WindowClassName;
		wcx.lpszMenuName  = NULL;
		wcx.hIconSm       = NULL;

		if(::RegisterClassEx(&wcx) == 0) {
			::OutputDebugString(TEXT("*** Error - �E�B���h�E�o�^���s(CGameWindow_Create)\n"));
			return false;
		}
	}

	// �E�B���h�E����
	m_hWnd = ::CreateWindowEx(0, WindowClassName, TEXT("ES Game Library - ��������"), 0,
							  0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if(m_hWnd == NULL) {
		::OutputDebugString(TEXT("*** Error - �E�B���h�E�������s(CGameWindow_Create)\n"));
		return false;
	}

	// �E�B���h�E�ݒ�
	m_Width  = inWidth;
	m_Height = inHeight;
	ChangeMode(inWindowed);

	::ImmAssociateContext(m_hWnd, NULL);	// IME����
	::ShowWindow(m_hWnd, SW_SHOW);			// �E�B���h�E�\��
	::SetFocus(m_hWnd);						// �t�H�[�J�X�ݒ�

	return true;
}

//------------------------------------------------------------------------------
//	�E�B���h�E����
//------------------------------------------------------------------------------
void CGameFrameWindow::Destroy()
{
	// �E�B���h�E�j��
	if(m_hWnd != NULL) {
		::ShowWindow(m_hWnd, SW_HIDE);
		::DestroyWindow(m_hWnd);

		m_hWnd   = NULL;
		m_Width  = 0;
		m_Height = 0;
	}
}

//------------------------------------------------------------------------------
//	�E�B���h�E���[�h�ύX
//------------------------------------------------------------------------------
bool CGameFrameWindow::ChangeMode(const bool inWindowed)
{
#ifdef _DEBUG
	if(m_hWnd == NULL) {
		::OutputDebugString(TEXT("*** Error - �E�B���h�E��������(CGameWindow_ChangeMode)\n"));
		return false;
	}
#endif

	// �f�B�X�v���C�T�C�Y�擾
	const int   ScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
	const int   ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	DWORD   WndExStyle = 0, WndStyle = 0;
	RECT    WndRect;
	HWND    hZOder = HWND_TOP;
	if(inWindowed == false) {
		// �t���X�N���[�����[�h
#ifdef NDEBUG
		WndExStyle = WS_EX_TOPMOST;
		hZOder     = HWND_TOPMOST;
#endif
		WndStyle   = WS_POPUP | WS_SYSMENU;
		::SetRect(&WndRect, 0, 0, ScreenWidth, ScreenHeight);

		// �J�[�\������
		while(::ShowCursor(FALSE) >= 0)
			;
	} else {
		// �E�B���h�E���[�h
		WndStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
		::SetRect(&WndRect, 0, 0, m_Width, m_Height);

		// �J�[�\���\��
		while(::ShowCursor(TRUE) < 0)
			;
	}

	// �E�B���h�E�X�^�C���ݒ�
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, WndExStyle);
	::SetWindowLong(m_hWnd, GWL_STYLE,   WndStyle);

	// �E�B���h�E�T�C�Y�ݒ�
	::AdjustWindowRectEx(&WndRect, WndStyle, FALSE, WndExStyle);
	const long   WndWidth  = WndRect.right  - WndRect.left;			// �E�B���h�E��
	const long   WndHeight = WndRect.bottom - WndRect.top;			// �E�B���h�E����

	// �E�B���h�E�ʒu�ݒ�
	const int    WndPosX   = (ScreenWidth  - WndWidth ) / 2;
	const int    WndPosY   = (ScreenHeight - WndHeight) / 2;

	// �E�B���h�E�ύX
	::SetWindowPos(m_hWnd, hZOder, WndPosX, WndPosY, WndWidth, WndHeight, SWP_FRAMECHANGED | SWP_DRAWFRAME | SWP_SHOWWINDOW);

	// �I�[�i�[�E�B���h�E�ĕ`��
	HWND   hOwnerWnd = ::GetWindow(m_hWnd, GW_OWNER);
	::InvalidateRect(hOwnerWnd, NULL, TRUE);
	::UpdateWindow(hOwnerWnd);

	return true;
}
