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
				Squirrel�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Squirrel.hpp"
#include <windows.h>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "squirrel.lib")
#pragma comment(lib, "sqstdlib.lib")
#pragma comment(lib, "sqplus.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CSquirrel::CSquirrel()
{
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CSquirrel::~CSquirrel()
{
	Release();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CSquirrel::Initialize(const SQInteger inStackSize)
{
	Release();

	HSQUIRRELVM   vm = ::sq_open(inStackSize);
	if(vm == NULL) {
		::OutputDebugString(TEXT("*** Error - ���z�}�V�����������s(CSquirrel_Initialize)\n"));
		return false;
	}
	SquirrelVM::Init(vm);

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CSquirrel::Release()
{
	m_ProtectedResource.clear();

	ReleaseAllScripts();
	SquirrelVM::Shutdown();
}

//------------------------------------------------------------------------------
//	Script����
//------------------------------------------------------------------------------
IScript* CSquirrel::CreateScriptFromFile(LPCTSTR inFileName)
{
	IScript*   pScript;

	// �X�N���v�g�t�@�C���ǂݍ���
	SquirrelObject   sqobj;
	try {
		sqobj   = SquirrelVM::CompileScript(inFileName);
		pScript = new CScript(sqobj);
	} catch(...) {
		::OutputDebugString(TEXT("*** Error - �X�N���v�g�t�@�C���ǂݍ��ݎ��s(CSquirrel_CreateScriptFromFile)\n"));
		pScript = new CNullScript();
	}

	if(pScript == NULL) {
		::OutputDebugString(TEXT("*** Error - Script�������s(CSquirrel_CreateScriptFromFile)\n"));
		return NULL;
	}

	m_ScriptList.push_back(pScript);

	return pScript;
}

//------------------------------------------------------------------------------
//	Script���
//------------------------------------------------------------------------------
void CSquirrel::ReleaseScript(IScript*& pScript)
{
	m_ProtectedResource.erase(pScript);		// �v���e�N�g����

	m_ScriptList.remove(pScript);
	delete pScript;
	pScript = NULL;
}

//------------------------------------------------------------------------------
//	�SScript���
//------------------------------------------------------------------------------
void CSquirrel::ReleaseAllScripts()
{
	std::list<IScript*>::iterator   list_it = m_ScriptList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_ScriptList.end()) {
			delete *list_it;
			list_it++;
		}
		m_ScriptList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_ScriptList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_ScriptList.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	���\�[�X�v���e�N�g�ݒ�
//------------------------------------------------------------------------------
void CSquirrel::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_ProtectedResource.erase (pResource);		// �v���e�N�g����
	}
}
