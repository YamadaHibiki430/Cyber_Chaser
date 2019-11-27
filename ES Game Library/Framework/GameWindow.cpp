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
	 GameWindow.cpp
				�Q�[���p�E�B���h�E

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "GameWindow.hpp"
#include "../../Resource/resource.h"
#include <imm.h>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "imm32.lib")

//------------------------------------------------------------------------------
//	�N���X�ϐ�
//------------------------------------------------------------------------------
CGameWindow*   CGameWindow::m_pGameWindow = NULL;

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CGameWindow::CGameWindow() : m_hWnd(NULL), m_Windowed(true), m_Active(false)
{
	m_pGameWindow = this;
}

//------------------------------------------------------------------------------
//	�E�B���h�E����
//------------------------------------------------------------------------------
bool CGameWindow::Create(const HINSTANCE hInstance, const bool inWindowed)
{
	Destroy();

	LPCTSTR   WindowClassName = L"OOGAMEPROG_WINDOWCLASS";	// �E�B���h�E�N���X��

	WNDCLASSEX   wcx;
	// �E�B���h�E�N���X���o�^�ς݂����ׂ�
	if(::GetClassInfoEx(hInstance, WindowClassName, &wcx) == 0) {
		// �E�B���h�E�N���X�o�^
		::ZeroMemory(&wcx, sizeof(wcx));
		wcx.cbSize        = sizeof(wcx);
		wcx.style         = 0;
		wcx.lpfnWndProc   = CGameWindow::WndProc;
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
			::OutputDebugString(L"*** Error - �E�B���h�E�o�^���s(CGameWindow_Create)\n");
			return false;
		}
	}

	// �E�B���h�E����
	m_hWnd = ::CreateWindowEx(0, WindowClassName, L"ES Game Library", 0,
							  0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if(m_hWnd == NULL) {
		::OutputDebugString(L"*** Error - �E�B���h�E�������s(CGameWindow_Create)\n");
		return false;
	}

	Adjust(inWindowed);						// �E�B���h�E����
	::ImmAssociateContext(m_hWnd, NULL);	// IME����
	::ShowWindow(m_hWnd, SW_SHOW);			// �E�B���h�E�\��
	::SetFocus(m_hWnd);						// �t�H�[�J�X�ݒ�

	return true;
}

//------------------------------------------------------------------------------
//	�E�B���h�E����
//------------------------------------------------------------------------------
void CGameWindow::Destroy()
{
	// �E�B���h�E�j��
	if(m_hWnd != NULL) {
		::ShowWindow(m_hWnd, SW_HIDE);
		::DestroyWindow(m_hWnd);

		m_Active   = false;
		m_Windowed = true;
		m_hWnd     = NULL;
	}
}

//------------------------------------------------------------------------------
//	�E�B���h�E���[�h�ύX
//------------------------------------------------------------------------------
bool CGameWindow::ChangeMode(const bool inWindowed)
{
	return Adjust(inWindowed);
}

//------------------------------------------------------------------------------
//	�E�B���h�E�`��E�ʒu�ݒ�
//------------------------------------------------------------------------------
bool CGameWindow::Adjust(const bool inWindowed)
{
	m_Windowed = inWindowed;

#ifdef _DEBUG
	if(m_hWnd == NULL) {
		::OutputDebugString(L"*** Error - �E�B���h�E��������(CGameWindow_Adjust)\n");
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
		::SetRect(&WndRect, 0, 0, 1280, 720);

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

//------------------------------------------------------------------------------
//	�E�B���h�E�v���V�[�W��
//------------------------------------------------------------------------------
LRESULT CALLBACK CGameWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameWindow->WindowProcedure(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::WindowProcedure(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	switch(uMsg) {
	  case WM_PAINT:		return OnPaint      (hWnd, wParam, lParam);
	  case WM_KEYDOWN:		return OnKeyDown    (hWnd, wParam, lParam);
	  case WM_SYSKEYDOWN:	return OnSysKeyDown (hWnd, wParam, lParam);
	  case WM_SYSCOMMAND:	return OnSysCommand (hWnd, wParam, lParam);
	  case WM_ACTIVATEAPP:	return OnActivateApp(hWnd, wParam, lParam);
	  case WM_CREATE:		return OnCreate     (hWnd, wParam, lParam);
	  case WM_CLOSE:		return OnClose      (hWnd, wParam, lParam);
	  case WM_DESTROY:		return OnDestroy    (hWnd, wParam, lParam);
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_PAINT���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnPaint(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	PAINTSTRUCT   ps;
	::BeginPaint(hWnd, &ps);

	::EndPaint(hWnd, &ps);

	return 0;
}

//------------------------------------------------------------------------------
//	WM_KEYDOWN���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	switch(wParam) {
	  case VK_ESCAPE:
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_SYSKEYDOWN���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnSysKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	switch(wParam) {
	  case VK_RETURN:
		// �E�B���h�E���[�h�ύX
		ChangeMode(m_Windowed ^ true);
		return 0;

	  case VK_F4:
		Destroy();
		return 0;
	}

	return ::DefWindowProc(hWnd, WM_SYSKEYDOWN, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_SYSCOMMAND���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnSysCommand(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	// �X�N���[���Z�[�o�N���h�~
	switch(wParam) {
	  case SC_SCREENSAVE:
		return 1;
	}

	return ::DefWindowProc(hWnd, WM_SYSCOMMAND, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_ACTIVATEAPP���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnActivateApp(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	if(wParam != 0) {
		m_Active = true;
	} else {
		m_Active = false;
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_CREATE���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnCreate(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	return 0;
}

//------------------------------------------------------------------------------
//	WM_CLOSE���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnClose(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	Destroy();
	return 0;
}

//------------------------------------------------------------------------------
//	WM_DESTROY���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameWindow::OnDestroy(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	::PostQuitMessage(0);
	return 0;
}
