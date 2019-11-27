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
	 Thread.hpp
				�X���b�h�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <windows.h>
#include <process.h>

//------------------------------------------------------------------------------
//	�X���b�h�N���X��`
//------------------------------------------------------------------------------
class CThread {
public:
	CThread();
	virtual ~CThread();

	void Start(unsigned (__stdcall *FuncName)(void*), void* arg = NULL);
	void Join(const DWORD inMSTimeout = INFINITE);
	bool IsAlive() const;

	void SetProcessorAffinity(const DWORD inCpu);

	inline HANDLE GetCloseEventHandle() const { return m_hCloseEvent; }
	bool IsClosed();

private:
	HANDLE   m_hThread;
	HANDLE   m_hCloseEvent;

	CThread(const CThread&);
	CThread& operator=(const CThread&);
};
