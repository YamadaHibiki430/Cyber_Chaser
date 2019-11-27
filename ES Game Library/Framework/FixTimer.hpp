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
	 FixTimer.hpp
				�Œ�^�C�}�[�N���X�w�b�_�[�t�@�C��

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
//	�Œ�^�C�}�[�N���X��`
//------------------------------------------------------------------------------
class CFixTimer {
public:
	virtual ~CFixTimer();

	void Wait();

	void Reset() { m_LastTime = ::timeGetTime();  }
	bool IsSkip() const { return false; }

	void SetFPS(const UINT inFPS) { m_FPS = inFPS; m_MaxWait = 1000 / inFPS; }
	UINT GetFPS() const { return m_FPS; }

	inline double GetElapsedSecond()      const { return m_MaxWait / 1000.0; }
	inline DWORD  GetElapsedMilliSecond() const { return m_MaxWait; }

	// �V���O���g���C���X�^���X�擾
	static CFixTimer& GetInstance()
	{
		static CFixTimer   theFixTimer;
		return theFixTimer;
	}

private:
	CFixTimer();

	UINT    m_FPS;			// �P�b������̃t���[����
	UINT    m_PeriodMin;	// �}���`���f�B�A�^�C�}�ŏ�����\
	DWORD   m_MaxWait;		// �ő�x�~����
	DWORD   m_LastTime;		// �t���[���I������
};

//------------------------------------------------------------------------------
//	�}�N����`
//------------------------------------------------------------------------------
inline CFixTimer& FixTimer() { return CFixTimer::GetInstance(); }
