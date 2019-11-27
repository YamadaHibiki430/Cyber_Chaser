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
	 SingleApp.hpp
				�V���O���A�v���P�[�V�����N���X�w�b�_�[�t�@�C��

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
//	�V���O���A�v���P�[�V�����N���X��`
//------------------------------------------------------------------------------
class CSingleApp {
public:
	CSingleApp();
	CSingleApp(LPCTSTR inMutexName);
	virtual ~CSingleApp() { Release(); }

	bool Initialize();
	void Release();
	
	bool SetWindow(const HWND hWnd);
	void RemoveWindow();

	bool OnCreate(const HWND hWnd) { return SetWindow(hWnd); }
	void OnClose() { RemoveWindow(); }

private:
	HANDLE   m_hMutex;
	TCHAR    m_MutexName[MAX_PATH + 1];
	HWND     m_hWnd;

	CSingleApp(const CSingleApp&);
	CSingleApp& operator=(const CSingleApp&);
};
