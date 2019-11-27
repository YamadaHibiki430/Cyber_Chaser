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
	 FPSTimer.hpp
				FPS����^�C�}�[

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "FPSTimer.hpp"

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CFPSTimer::CFPSTimer() : m_FPS(0),       m_Frequency(0),   m_MaxWait(0),
						 m_LastTime(0),	 m_SurplusTime(0), m_PeriodMin(0),
						 m_DrawCount(0), m_SkipCount(0),
						 m_DrawFPS(0),   m_DrawSkip(0),    m_LastDraw(0)
{
	InitTimer();
	SetFPS(60);
	Reset();
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CFPSTimer::~CFPSTimer()
{
	if(m_PeriodMin != 0)
		::timeEndPeriod(m_PeriodMin);
}

//------------------------------------------------------------------------------
//	�^�C�}�[������
//------------------------------------------------------------------------------
bool CFPSTimer::InitTimer()
{
	// �}���`���f�B�A�^�C�}�\�͎擾
	TIMECAPS   tc;
	::timeGetDevCaps(&tc, sizeof(tc));
	m_PeriodMin = tc.wPeriodMin;

	// �}���`���f�B�A�^�C�}����\�ݒ�
	::timeBeginPeriod(m_PeriodMin);

	// �p�t�H�[�}���X�J�E���^���g���擾
	if(::QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	�ҋ@
//------------------------------------------------------------------------------
void CFPSTimer::Wait()
{
	// �����Ԏ擾
	__int64   CurrentTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

	// �ҋ@�A���ߎ��Ԑݒ�
	__int64   WaitTime = m_MaxWait - (CurrentTime - m_LastTime);
	m_SurplusTime += WaitTime;			// �]�莞�Ԃ̗ݐ�
	if(m_SurplusTime < 0) {
		WaitTime = 0;					// �]�莞�Ԃ����̏ꍇ�͒ǂ����܂ŃE�F�C�g���Ȃ�
		m_SkipCount++;
	} else {
		if(WaitTime != m_SurplusTime)
			WaitTime = m_SurplusTime;	// �ǂ������ꍇ�̃E�F�C�g����
		m_SurplusTime = 0;
		m_DrawCount++;
	}

	// ���[�v�őҋ@���Ԃ�����
	do {
//		::Sleep(0);
		::Sleep(1);
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_LastTime);
	} while(WaitTime > m_LastTime - CurrentTime);
}

//------------------------------------------------------------------------------
//	�����Ԏ擾
//------------------------------------------------------------------------------
__int64 CFPSTimer::GetCount()
{
	__int64   CurrentTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	return CurrentTime / m_Frequency;
}

//------------------------------------------------------------------------------
//	�t���[�����[�g�`��
//------------------------------------------------------------------------------
void CFPSTimer::DrawFPS(const HDC hDC)
{
	const DWORD   CurrentTime = ::timeGetTime();			// ������
	const DWORD   ElapsedTime = CurrentTime - m_LastDraw;	// �o�ߎ���
	if(ElapsedTime >= 1000) {
		m_DrawFPS   = m_DrawCount * 1000 / ElapsedTime;
		m_DrawSkip  = m_SkipCount * 1000 / ElapsedTime;
		m_DrawCount = 0;
		m_SkipCount = 0;
		m_LastDraw  = CurrentTime;
	}

	// FPS�`��
	TCHAR   FPSInfo[32];
	::wsprintf(FPSInfo, TEXT("FPS:%3d Skip:%3d"), m_DrawFPS, m_DrawSkip);
	::TextOut(hDC, 0, 0, FPSInfo, ::lstrlen(FPSInfo));
}
