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
	 Squirrel.hpp
				Squirrel�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_SQUIRREL
#define _INC_SQUIRREL

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Script.hpp"

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	Squirrel�N���X��`
//------------------------------------------------------------------------------
class CSquirrel {
public:
	virtual ~CSquirrel();

	// �V���O���g���C���X�^���X�̎擾
	static CSquirrel& GetInstance()
	{ static CSquirrel theSquirrel; return theSquirrel; }

	bool Initialize(const SQInteger inStackSize = 1024);

	IScript* CreateScriptFromFile(LPCTSTR inFileName);
	inline IScript* CompileScriptFromFile(LPCTSTR inFileName)
	{ return CreateScriptFromFile(inFileName); }

	void ReleaseScript(IScript*& pScript);
	void ReleaseAllScripts();

	// ���\�[�X�v���e�N�g�ݒ�
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	std::list<IScript*>   m_ScriptList;
	std::set <void*   >   m_ProtectedResource;	// ���\�[�X�v���e�N�g�ݒ�

private:
	CSquirrel();
	void Release();

	CSquirrel(const CSquirrel&);
	CSquirrel& operator=(const CSquirrel&);
};

//------------------------------------------------------------------------------
//	�C�����C���֐���`
//------------------------------------------------------------------------------
inline CSquirrel& Squirrel() { return CSquirrel::GetInstance(); }

#endif /* _INC_SQUIRREL */