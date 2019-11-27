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
	 GameProc.cpp
				�Q�[������

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "GameProc.hpp"

//------------------------------------------------------------------------------
//	���C�����[�v
//------------------------------------------------------------------------------
bool CGameProc::MainLoop(const bool inActive)
{
	int   ret;

	if(inActive) {
		ret = m_pGameScene->ActiveProc();		// �A�N�e�B�u���̏���
	} else {
		ret = m_pGameScene->NonActiveProc();	// ��A�N�e�B�u���̏���
	}

	if(ret ==  0)
		return true;							// �V�[���p��
	if(ret == -1)
		return false;							// �Q�[���I��

	return CreateScene((CGameScene*)ret);		// �V�[���ύX
}

//------------------------------------------------------------------------------
//	�V�[���ύX
//------------------------------------------------------------------------------
bool CGameProc::CreateScene(const CGameScene* pScene)
{
	ReleaseScene();

	m_pGameScene = (CGameScene*)pScene;

	if(m_pGameScene->Initialize() == false)
		return false;
	m_pGameScene->LoadContent();

	return true;
}
