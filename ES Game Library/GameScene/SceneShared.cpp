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
	 SceneShared.cpp
				�V�[�����L�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "SceneShared.hpp"

//------------------------------------------------------------------------------
//	�����l�擾
//------------------------------------------------------------------------------
int CSceneShared::GetIntegerForKey(const char* pKey)
{
	std::map<std::basic_string<char>, int>::iterator it;
	it = m_MapInt.find(pKey);
	if(it == m_MapInt.end())
		return 0;

	return it->second;
}

//------------------------------------------------------------------------------
//	�P���x���������l�擾
//------------------------------------------------------------------------------
float CSceneShared::GetFloatForKey(const char* pKey)
{
	std::map<std::basic_string<char>, float>::iterator it;
	it = m_MapFloat.find(pKey);
	if(it == m_MapFloat.end())
		return 0.0f;

	return it->second;
}

//------------------------------------------------------------------------------
//	�{���x���������l�擾
//------------------------------------------------------------------------------
double CSceneShared::GetDoubleForKey(const char* pKey)
{
	std::map<std::basic_string<char>, double>::iterator it;
	it = m_MapDouble.find(pKey);
	if(it == m_MapDouble.end())
		return 0.0;

	return it->second;
}

//------------------------------------------------------------------------------
//	�u�[���l�擾
//------------------------------------------------------------------------------
bool CSceneShared::GetBoolForKey(const char* pKey)
{
	std::map<std::basic_string<char>, bool>::iterator it;
	it = m_MapBool.find(pKey);
	if(it == m_MapBool.end())
		return false;

	return it->second;
}

//------------------------------------------------------------------------------
//	�f�[�^�擾
//------------------------------------------------------------------------------
void* CSceneShared::GetDataForKey(const char* pKey)
{
	std::map<std::basic_string<char>, void*>::iterator it;
	it = m_MapVoidPtr.find(pKey);
	if(it == m_MapVoidPtr.end())
		return NULL;

	return it->second;
}

//------------------------------------------------------------------------------
//	������擾
//------------------------------------------------------------------------------
std::basic_string<TCHAR> CSceneShared::GetStringForKey(const char* pKey)
{
	std::map< std::basic_string<char>, std::basic_string<TCHAR> >::iterator it;
	it = m_MapString.find(pKey);
	if(it == m_MapString.end())
		return std::basic_string<TCHAR>();

	return it->second;
}
