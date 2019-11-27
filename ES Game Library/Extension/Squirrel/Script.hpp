/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft Windows 7以降

 【コンパイラ】
     Microsoft VisualC++ 2013

 【プログラム】
	 Script.hpp
				Scriptクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <sqplus.h>
#include <windows.h>

//------------------------------------------------------------------------------
//	Scriptインタフェース定義
//------------------------------------------------------------------------------
class IScript {
public:
	virtual ~IScript() {}

	virtual void Run() = 0;

	// オブジェクト取得
	virtual SquirrelObject GetScriptObject(const SQChar* inName) = 0;

	// バインド
	virtual void Bind(const SQChar* inName, int&   inVariable) = 0;
	virtual void Bind(const SQChar* inName, float& inVariable) = 0;
	virtual void Bind(const SQChar* inName, bool&  inVariable) = 0;

	// 関数バインド
	template<typename Func>
	inline void BindFunction(const SQChar* inName, Func inFunc)
	{ SqPlus::RegisterGlobal(SquirrelVM::GetVMPtr(), inFunc, inName); }

	// 配列生成
	virtual void CreateArray(const SQChar* inName, const UINT inSize) = 0;
};

//------------------------------------------------------------------------------
//	Scriptクラス定義
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
//	NULLScriptクラス定義
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
