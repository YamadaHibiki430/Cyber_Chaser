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
	 Squirrel.hpp
				Squirrelクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_SQUIRREL
#define _INC_SQUIRREL

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Script.hpp"

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	Squirrelクラス定義
//------------------------------------------------------------------------------
class CSquirrel {
public:
	virtual ~CSquirrel();

	// シングルトンインスタンスの取得
	static CSquirrel& GetInstance()
	{ static CSquirrel theSquirrel; return theSquirrel; }

	bool Initialize(const SQInteger inStackSize = 1024);

	IScript* CreateScriptFromFile(LPCTSTR inFileName);
	inline IScript* CompileScriptFromFile(LPCTSTR inFileName)
	{ return CreateScriptFromFile(inFileName); }

	void ReleaseScript(IScript*& pScript);
	void ReleaseAllScripts();

	// リソースプロテクト設定
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	std::list<IScript*>   m_ScriptList;
	std::set <void*   >   m_ProtectedResource;	// リソースプロテクト設定

private:
	CSquirrel();
	void Release();

	CSquirrel(const CSquirrel&);
	CSquirrel& operator=(const CSquirrel&);
};

//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
inline CSquirrel& Squirrel() { return CSquirrel::GetInstance(); }

#endif /* _INC_SQUIRREL */