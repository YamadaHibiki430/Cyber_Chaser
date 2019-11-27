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
	 Script.hpp
				Script�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Script.hpp"

//------------------------------------------------------------------------------
//	���s
//------------------------------------------------------------------------------
void CScript::Run()
{ 
	SquirrelVM::RunScript(m_SqObject);
}

//------------------------------------------------------------------------------
//	�X�N���v�g�I�u�W�F�N�g�擾
//------------------------------------------------------------------------------
SquirrelObject CScript::GetScriptObject(const SQChar* inName)
{
	SquirrelObject   root = SquirrelVM::GetRootTable();
	return root.GetValue(inName);
}

//------------------------------------------------------------------------------
//	�o�C���h
//------------------------------------------------------------------------------
void CScript::Bind(const SQChar* inName, int& inVariable)
{
	SqPlus::BindVariable(&inVariable, inName);
}

//------------------------------------------------------------------------------
//	�o�C���h
//------------------------------------------------------------------------------
void CScript::Bind(const SQChar* inName, float& inVariable)
{
	SqPlus::BindVariable(&inVariable, inName);
}

//------------------------------------------------------------------------------
//	�o�C���h
//------------------------------------------------------------------------------
void CScript::Bind(const SQChar* inName, bool& inVariable)
{
	SqPlus::BindVariable(&inVariable, inName);
}

//------------------------------------------------------------------------------
//	�z�񐶐�
//------------------------------------------------------------------------------
void CScript::CreateArray(const SQChar* inName, const UINT inSize)
{
	SquirrelObject   arr  = SquirrelVM::CreateArray(inSize);
	SquirrelObject   root = SquirrelVM::GetRootTable();
	root.SetValue(inName, arr);
}
