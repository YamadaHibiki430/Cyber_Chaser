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
	 GameProc.hpp
				�Q�[�������N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../GameScene/GameScene.hpp"

//------------------------------------------------------------------------------
// �Q�[�������N���X��`
//------------------------------------------------------------------------------
class CGameProc {
public:
	CGameProc() : m_pGameScene(NULL) {}
	virtual ~CGameProc() {}

	bool MainLoop(const bool inActive);

	bool CreateScene(const CGameScene* pScene);
	void ReleaseScene() { delete m_pGameScene; m_pGameScene = NULL; }

private:
	CGameScene*   m_pGameScene;

	CGameProc(const CGameProc&);
	CGameProc& operator=(const CGameProc&);
};
