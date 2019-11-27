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
#include <sqplus.h>
#include <windows.h>

//------------------------------------------------------------------------------
//	Script�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IScript {
public:
	virtual ~IScript() {}

	virtual void Run() = 0;

	// �I�u�W�F�N�g�擾
	virtual SquirrelObject GetScriptObject(const SQChar* inName) = 0;

	// �o�C���h
	virtual void Bind(const SQChar* inName, int&   inVariable) = 0;
	virtual void Bind(const SQChar* inName, float& inVariable) = 0;
	virtual void Bind(const SQChar* inName, bool&  inVariable) = 0;

	// �֐��o�C���h
	template<typename Func>
	inline void BindFunction(const SQChar* inName, Func inFunc)
	{ SqPlus::RegisterGlobal(SquirrelVM::GetVMPtr(), inFunc, inName); }

	// �z�񐶐�
	virtual void CreateArray(const SQChar* inName, const UINT inSize) = 0;
};

//------------------------------------------------------------------------------
//	Script�N���X��`
//------------------------------------------------------------------------------
class CScript : public IScript {
public:
	CScript(SquirrelObject& inSqObject) : m_SqObject(inSqObject) {}
	virtual ~CScript() {}

	virtual void Run();

	virtual SquirrelObject GetScriptObject(const SQChar* inName);

	virtual void Bind(const SQChar* inName, int&   inVariable);
	virtual void Bind(const SQChar* inName, float& inVariable);
	virtual void Bind(const SQChar* inName, bool&  inVariable);

	virtual void CreateArray(const SQChar* inName, const UINT inSize);

private:
	SquirrelObject   m_SqObject;
};

//------------------------------------------------------------------------------
//	NULLScript�N���X��`
//------------------------------------------------------------------------------
class CNullScript : public IScript {
public:
	CNullScript() {}
	virtual ~CNullScript() {}

	virtual void Run() {}

	virtual SquirrelObject GetScriptObject(const SQChar* inName)
	{ return SquirrelObject(); }

	virtual void Bind(const SQChar* inName, int&   inVariable) {}
	virtual void Bind(const SQChar* inName, float& inVariable) {}
	virtual void Bind(const SQChar* inName, bool&  inVariable) {}

	virtual void CreateArray(const SQChar* inName, const UINT inSize) {}
};
