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
	 GameApp.cpp
				�Q�[���A�v���P�[�V����

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "GameApp.hpp"
#include "..\..\Resource\resource.h"

//------------------------------------------------------------------------------
//	�N���X�ϐ�
//------------------------------------------------------------------------------
CGameApp* CGameApp::m_pGameApp = NULL;

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CGameApp::CGameApp() : m_Windowed(true), m_Active(false),
					   m_SingleApp(TEXT("O&O_GameProgramming")), m_GameFrameWindow(), m_GameProc()
{
	m_pGameApp = this;
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CGameApp::Initialize(const HINSTANCE hInstance)
{
	// �A�v�����d�N�����o
	if(m_SingleApp.Initialize() == false)
		return false;

	// �A�v���P�[�V�����ݒ�ǂݍ���
	TCHAR   string_buffer[MAX_PATH];
	UINT    width = 1280, height = 720, fps = 60;

	// �X�N���[����
	if(::LoadString(hInstance, IDS_SCREENWIDTH, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		width = (UINT)::_tstoi(string_buffer);
	}

	// �X�N���[������
	if(::LoadString(hInstance, IDS_SCREENHEIGHT, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		height = (UINT)::_tstoi(string_buffer);
	}

	// �t���[�����[�g
	if(::LoadString(hInstance, IDS_FRAMERATE, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		fps = (UINT)::_tstoi(string_buffer);
	}

	// �X�N���[�����[�h
	m_Windowed = true;
	if(::LoadString(hInstance, IDS_FULLSCREEN, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		if((UINT)::_tstoi(string_buffer))
			m_Windowed = false;
//		else
//			m_Windowed = true;
	}

	// �}�E�X�J�[�\���\��
	if(::LoadString(hInstance, IDS_MOUSECURSOR, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   != 0) {
		if((UINT)::_tstoi(string_buffer) == 0) {
			// �}�E�X�J�[�\������
			while(::ShowCursor(FALSE) >= 0)
				;
		}
	}

	// �R���e���c�t�H���_�[
	if(::LoadString(hInstance, IDS_CONTENTFOLDER, string_buffer,
					sizeof(string_buffer) / sizeof(string_buffer[0]))
	   == 0) {
		::_tcscpy_s(string_buffer, _T("Content"));
	}

	// �E�B���h�E����
	if(m_GameFrameWindow.Create(hInstance, CGameApp::WndProc, width, height, m_Windowed) == false)
		return false;

	// DirectX Graphics������
	if(DXGraphics().Initialize(m_GameFrameWindow.GetHWnd  (),
							   m_GameFrameWindow.GetWidth (),
							   m_GameFrameWindow.GetHeight(),
							   m_Windowed)
	   == false)
		return false;

	// DirectInput������
	if(DInput().Initialize(m_GameFrameWindow.GetHWnd()) == false)
		return false;

	// �L�[�{�[�h����
	if(DInput().CreateKeyboard() == false)
		return false;

	// DirectXAudio������
	DXAudio().Initialize(m_GameFrameWindow.GetHWnd());

	// FPS�ݒ�
	FixTimer().SetFPS(fps);

	// �f�t�H���g�t�H���_�ݒ�
	::SetCurrentDirectory(string_buffer);

	// �Q�[���V�[���ݒ�
	if(m_GameProc.CreateScene(new GameMain()) == false)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CGameApp::Release()
{
	m_GameProc.ReleaseScene();		// �Q�[���V�[�����

	DShow()     .Release();			// DirectShow���
	DXAudio()   .Release();			// DirectX Audio���
	DInput()    .Release();			// DirectInput���
	DXGraphics().Release();			// DirectX Graphics���

	m_SingleApp.RemoveWindow();		// �E�B���h�E�֘A�Â�����
	m_GameFrameWindow.Destroy();	// �E�B���h�E�t���[�����
}

//------------------------------------------------------------------------------
//	���s
//------------------------------------------------------------------------------
int CGameApp::Run()
{
	// �A�v���P�[�V�������C�����[�v
	while(true) {
		// ���b�Z�[�W����
		if(MessageLoop() == false)
			break;
		// �Q�[������
		if(m_GameProc.MainLoop(m_Active) == false)
			break;
	}

	return 0;
}

//------------------------------------------------------------------------------
//	���b�Z�[�W���[�v
//------------------------------------------------------------------------------
bool CGameApp::MessageLoop()
{
	MSG   msg;
	while(::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) != 0) {
		if(::GetMessage(&msg, NULL, 0, 0) <= 0)
			return false;
//		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return true;
}

//------------------------------------------------------------------------------
//	�E�B���h�E�v���V�[�W��
//------------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameApp->WindowProc(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::WindowProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	switch(uMsg) {
	  case WM_PAINT:			return OnPaint        (hWnd, wParam, lParam);
	  case WM_KEYDOWN:			return OnKeyDown      (hWnd, wParam, lParam);
	  case WM_SYSKEYDOWN:		return OnSysKeyDown   (hWnd, wParam, lParam);
	  case WM_SYSCOMMAND:		return OnSysCommand   (hWnd, wParam, lParam);
	  case WM_ACTIVATEAPP:		return OnActivateApp  (hWnd, wParam, lParam);
	  case WM_CREATE:			return OnCreate       (hWnd, wParam, lParam);
	  case WM_CLOSE:			return OnClose        (hWnd, wParam, lParam);
	  case WM_DESTROY:			return OnDestroy      (hWnd, wParam, lParam);
//	  case WM_EXITSIZEMOVE:		return OnExitMouseMove(hWnd, wParam, lParam);
//	  case WM_NCLBUTTONDOWN:	return OnNCLButtonDown(hWnd, wParam, lParam);
//	  case WM_NCRBUTTONDOWN:	return OnNCRButtonDown(hWnd, wParam, lParam);
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_PAINT���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnPaint(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	PAINTSTRUCT   ps;
	::BeginPaint(hWnd, &ps);

	DXGraphics().OnPaint(ps);

	::EndPaint(hWnd, &ps);

	return 0;
}

//------------------------------------------------------------------------------
//	WM_KEYDOWN���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
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
LRESULT CGameApp::OnSysKeyDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	switch(wParam) {
	  case VK_RETURN:
		// ���[�h�ύX
		m_Windowed ^= true;

		if(DXGraphics().ChangeMode(m_Windowed) == false) {
			Release();
			return 0;
		}
		m_GameFrameWindow.ChangeMode(m_Windowed);

		return 0;

	  case VK_F4:
		Release();
		return 0;
	}

	return ::DefWindowProc(hWnd, WM_SYSKEYDOWN, wParam, lParam);
}

//------------------------------------------------------------------------------
//	WM_SYSCOMMAND���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnSysCommand(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	// �^�C�}�[���Z�b�g
//	FPSTimer().Reset();

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
LRESULT CGameApp::OnActivateApp(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	if(wParam != 0) {
		m_Active = true;
		DShow()     .Resume();
		DXAudio()   .Resume();
		DXGraphics().ResetDevice();
		DInput()    .ResetDevice();
		FixTimer()  .Reset();
	} else {
		m_Active = false;
		DShow()     .Suspend();
		DXAudio()   .Suspend();
		DInput()    .SuspendDevice();
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_CREATE���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnCreate(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	m_SingleApp.OnCreate(hWnd);	// �E�B���h�E�֘A�Â�
	return 0;
}

//------------------------------------------------------------------------------
//	WM_CLOSE���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnClose(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	Release();
	return 0;
}

//------------------------------------------------------------------------------
//	WM_DESTROY���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnDestroy(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	::PostQuitMessage(0);
	return 0;
}

//------------------------------------------------------------------------------
//	WM_EXITMOUSEMOVE���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnExitMouseMove(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
//	FPSTimer().Reset();		// �^�C�}�[���Z�b�g
	FixTimer().Reset();		// �^�C�}�[���Z�b�g
	return 0;
}

//------------------------------------------------------------------------------
//	WM_NCLBUTTONUP���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnNCLButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
//	FPSTimer().Reset();		// �^�C�}�[���Z�b�g

	switch(wParam) {
	  case HTMINBUTTON:
	  case HTCLOSE:
		return ::DefWindowProc(hWnd, WM_NCLBUTTONDOWN, wParam, lParam);
	  case HTCAPTION:
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE, 0);
		return 0;
//		return ::SendMessage(hWnd, WM_SYSCOMMAND, SC_MOVE, lParam);
	}

	return 0;
}

//------------------------------------------------------------------------------
//	WM_NCRBUTTONUP���b�Z�[�W����
//------------------------------------------------------------------------------
LRESULT CGameApp::OnNCRButtonDown(const HWND hWnd, const WPARAM wParam, const LPARAM lParam)
{
	return 0;
}
