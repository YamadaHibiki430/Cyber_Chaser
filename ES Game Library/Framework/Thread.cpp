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
	 Thread.cpp
				�X���b�h�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Thread.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CThread::CThread() : m_hThread(NULL)
{
	// �N���[�Y�C�x���g����
	m_hCloseEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CThread::~CThread()
{
	// �C�x���g�N���[�Y
	if(m_hCloseEvent != NULL) {
		::SetEvent(m_hCloseEvent);
		::CloseHandle(m_hCloseEvent);
		m_hCloseEvent = NULL;
	}

	// �X���b�h�I���҂�
	Join(1);

	// �X���b�h�n���h���N���[�Y
	if(m_hThread != NULL) {
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

//------------------------------------------------------------------------------
//	�J�n
//------------------------------------------------------------------------------
void CThread::Start(unsigned (__stdcall *FuncName)(void*), void* arg)
{
	// �ȑO�̃X���b�h�����s�������ׂ�
	if(m_hThread != NULL) {
		DWORD   exit_code;
		::GetExitCodeThread(m_hThread, &exit_code);
		if(exit_code == STILL_ACTIVE)
			return;

		// �X���b�h�n���h���N���[�Y
		::CloseHandle(m_hThread);
	}

	// �X���b�h����
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, FuncName, arg, 0, NULL);
}

//------------------------------------------------------------------------------
//	�I���҂�
//------------------------------------------------------------------------------
void CThread::Join(const DWORD inMSTimeout)
{
	if(m_hThread == NULL)
		return;
	::WaitForSingleObject(m_hThread, inMSTimeout);
}

//------------------------------------------------------------------------------
//	�I���҂�
//------------------------------------------------------------------------------
bool CThread::IsAlive() const
{
	if(::WaitForSingleObject(m_hThread, 0) != WAIT_OBJECT_0)
		return false;
	return true;
}

//------------------------------------------------------------------------------
//	�v���Z�b�T�[�A�t�B�j�e�B�ݒ�
//------------------------------------------------------------------------------
void CThread::SetProcessorAffinity(const DWORD inCpu)
{
	if(m_hThread == NULL)
		return;
	::SetThreadAffinityMask(m_hThread, 1 << inCpu);
}

//------------------------------------------------------------------------------
//	�X���b�h�N���[�Y�C�x���g���N���Ă��邩
//------------------------------------------------------------------------------
bool CThread::IsClosed()
{
	if(m_hCloseEvent == NULL)
		return true;

	if(::WaitForSingleObject(m_hCloseEvent, 0) == WAIT_TIMEOUT)
		return false;

	return true;
}
