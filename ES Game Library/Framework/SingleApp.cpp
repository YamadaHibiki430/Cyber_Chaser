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
	 SingleApp.cpp
				�V���O���A�v���P�[�V����

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "SingleApp.hpp"
#include <tchar.h>

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CSingleApp::CSingleApp() : m_hMutex(NULL), m_hWnd(NULL)
{
	::ZeroMemory(m_MutexName, sizeof(m_MutexName));

	// �~���[�e�b�N�X���ݒ�
	::GetModuleFileName(NULL, m_MutexName, sizeof(m_MutexName));

	// '\'��'/'�ɕύX
	for(int i = 0; i < lstrlen(m_MutexName); i++) {
		if(m_MutexName[i] == '\\')
			m_MutexName[i] = '/';
	}
}

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CSingleApp::CSingleApp(LPCTSTR inMutexName) : m_hMutex(NULL), m_hWnd(NULL)
{
	// �~���[�e�b�N�X���ݒ�
	::_tcsncpy_s(m_MutexName, inMutexName, MAX_PATH);
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CSingleApp::Initialize()
{
	CSingleApp::Release();

	// �~���[�e�b�N�X����
	m_hMutex = ::CreateMutex(NULL, 0, m_MutexName);

	// �~���[�e�b�N�X�����łɐ�������Ă����ꍇ�A
	// �~���[�e�b�N�X�����L����E�B���h�E��T���ĕ�������
	if(::GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND   hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while(hWnd != NULL) {
			// �~���[�e�b�N�X�������E�B���h�E�����ׂ�
			if(::GetProp(hWnd, m_MutexName) != NULL) {
				// �ŏ�������Ă���ꍇ�͕���
				if(::IsIconic(hWnd) != 0)
					::ShowWindow(hWnd, SW_RESTORE);

				::SetForegroundWindow(hWnd);						// �t�H�A�O���E���h�ɐݒ�
				::SetForegroundWindow(::GetLastActivePopup(hWnd));	// �q�E�B���h�E�Ƀt�H�[�J�X��ݒ�

				break;
			}	// if(GetProp)

			// �v���p�e�B����v���Ȃ��ꍇ�͎��̃E�B���h�E��
			hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
		}	// while(hWnd)

		// �~���[�e�b�N�X���
		::CloseHandle(m_hMutex);
		m_hMutex = NULL;

		return false;
	}	// if(GetLastError())

	// �~���[�e�b�N�X���L���擾
	const DWORD   Ret = ::WaitForSingleObject(m_hMutex, 0);
	if(Ret != WAIT_OBJECT_0 && Ret != WAIT_ABANDONED)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CSingleApp::Release()
{
	// �v���p�e�B����
	RemoveWindow();

	// �~���[�e�b�N�X���
	if(m_hMutex != NULL) {
		::ReleaseMutex(m_hMutex);	// ���L�����
		::CloseHandle(m_hMutex);	// �~���[�e�b�N�X���
		m_hMutex = NULL;
	}
}

//------------------------------------------------------------------------------
//	�E�B���h�E�֘A�Â�
//------------------------------------------------------------------------------
bool CSingleApp::SetWindow(const HWND hWnd)
{
#ifdef _DEBUG
	if(m_hMutex == NULL || hWnd == NULL)
		return false;
#endif
	if(m_hWnd == hWnd)
		return true;

	RemoveWindow();

	// �~���[�e�b�N�X�����E�B���h�E�̃v���p�e�B�ɐݒ肷��
	if(::SetProp(hWnd, m_MutexName, m_hMutex) == 0) {
		::OutputDebugString(TEXT("*** Error - �v���p�e�B�ݒ莸�s(CSingleApp_SetWindow)\n"));
		return false;
	}
	m_hWnd = hWnd;	// �E�B���h�E�n���h���̕ۑ�

	return true;
}

//------------------------------------------------------------------------------
//	�E�B���h�E�֘A�Â�����
//------------------------------------------------------------------------------
void CSingleApp::RemoveWindow()
{
	if(m_hWnd == NULL)
		return;

	// �E�B���h�E�̃v���p�e�B�ɐݒ肵���~���[�e�b�N�X������������
	::RemoveProp(m_hWnd, m_MutexName);
	m_hWnd = NULL;
}
