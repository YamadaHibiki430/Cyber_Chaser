/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft WindowsXP�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2008

 �y�v���O�����z
	 XAduio2.hpp
				XAudio2�N���X�w�b�_

 �y�o�[�W�����z
	 * Version    2014.03.00      2014/03/14  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "SoundVoice.hpp"
#include <list>

//------------------------------------------------------------------------------
//	XAudio2�N���X��`
//------------------------------------------------------------------------------
class CXAudio2 {
public:
	virtual ~CXAudio2();

	// �V���O���g���C���X�^���X�̎擾
	static CXAudio2& GetInstance()
	{ static CXAudio2 theXAudio; return theXAudio; }

	// �������E���
	bool Initialize();
	void Release();

	// �}�X�^�[�{�����[��
	void  SetMasterVolume(const float inVolume);
	float GetMasterVolume() const;

	// �\�[�X�{�C�X����
	ISoundVoice* CreateVoiceFromFile(LPTSTR inFileName);

private:
	IXAudio2*                 m_pXAudio;
	IXAudio2MasteringVoice*   m_pMasterVoice;

//	std::list<ISoundVoice*>   m_SoundVoiceList;		// �T�E���h�{�C�X���X�g

private:
	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	CXAudio2();
	CXAudio2(const CXAudio2&);
	CXAudio2& operator=(const CXAudio2&);
};

//------------------------------------------------------------------------------
//	�A�N�Z�X�֐�
//------------------------------------------------------------------------------
inline CXAudio2& XAudio() { return CXAudio2::GetInstance(); }
