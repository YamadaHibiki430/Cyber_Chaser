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
				FPS����^�C�}�[�N���X�w�b�_�[�t�@�C��

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
//	FPS����^�C�}�[�N���X��`
//------------------------------------------------------------------------------
class CFPSTimer {
public:
	virtual ~CFPSTimer();

	void Wait();
	void Reset() { ::QueryPerformanceCounter((LARGE_INTEGER*)&m_LastTime); m_SurplusTime = 0; }

	bool IsSkip() const { return (m_SurplusTime < 0) ? true : false; }

	void SetFPS(const UINT inFPS) {	m_FPS = inFPS; m_MaxWait = m_Frequency / inFPS; }
	UINT GetFPS() const { return m_FPS; }
	__int64 GetCount();

	void DrawFPS(const HDC hDC);

	// �V���O���g���C���X�^���X�̎擾
	static CFPSTimer& GetInstance()
	{
		static CFPSTimer   theFPSTimer;
		return theFPSTimer;
	}

private:
	CFPSTimer();
	bool InitTimer();

	UINT      m_FPS;			// �P�b������̃t���[����
	__int64   m_Frequency;		// �p�t�H�[�}���X�J�E���^���g��
	__int64   m_MaxWait;		// �ő�x�~����

	__int64   m_LastTime;		// �O�t���[���I������
	__int64   m_SurplusTime;	// �]�莞��

	UINT      m_PeriodMin;		// �}���`���f�B�A�^�C�}�ŏ�����\

	// �t���[�����[�g�v���p�ϐ�
	int       m_DrawCount;		// �`�搔
	int       m_SkipCount;		// �X�L�b�v��
	int       m_DrawFPS;		// �t���[�����[�g�`��p
	int       m_DrawSkip;		// �X�L�b�v���`��p
	DWORD     m_LastDraw;		// �O�`�掞��
};

//------------------------------------------------------------------------------
//	�Z�k����`
//------------------------------------------------------------------------------
inline CFPSTimer& FPSTimer() { return CFPSTimer::GetInstance(); }
