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
				Squirrelクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Squirrel.hpp"
#include <windows.h>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "squirrel.lib")
#pragma comment(lib, "sqstdlib.lib")
#pragma comment(lib, "sqplus.lib")

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CSquirrel::CSquirrel()
{
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CSquirrel::~CSquirrel()
{
	Release();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CSquirrel::Initialize(const SQInteger inStackSize)
{
	Release();

	HSQUIRRELVM   vm = ::sq_open(inStackSize);
	if(vm == NULL) {
		::OutputDebugString(TEXT("*** Error - 仮想マシン初期化失敗(CSquirrel_Initialize)\n"));
		return false;
	}
	SquirrelVM::Init(vm);

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CSquirrel::Release()
{
	m_ProtectedResource.clear();

	ReleaseAllScripts();
	SquirrelVM::Shutdown();
}

//------------------------------------------------------------------------------
//	Script生成
//------------------------------------------------------------------------------
IScript* CSquirrel::CreateScriptFromFile(LPCTSTR inFileName)
{
	IScript*   pScript;

	// スクリプトファイル読み込み
	SquirrelObject   sqobj;
	try {
		sqobj   = SquirrelVM::CompileScript(inFileName);
		pScript = new CScript(sqobj);
	} catch(...) {
		::OutputDebugString(TEXT("*** Error - スクリプトファイル読み込み失敗(CSquirrel_CreateScriptFromFile)\n"));
		pScript = new CNullScript();
	}

	if(pScript == NULL) {
		::OutputDebugString(TEXT("*** Error - Script生成失敗(CSquirrel_CreateScriptFromFile)\n"));
		return NULL;
	}

	m_ScriptList.push_back(pScript);

	return pScript;
}

//------------------------------------------------------------------------------
//	Script解放
//------------------------------------------------------------------------------
void CSquirrel::ReleaseScript(IScript*& pScript)
{
	m_ProtectedResource.erase(pScript);		// プロテクト解除

	m_ScriptList.remove(pScript);
	delete pScript;
	pScript = NULL;
}

//------------------------------------------------------------------------------
//	全Script解放
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
				// プロテクト対象外
				delete *list_it;
				list_it = m_ScriptList.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	リソースプロテクト設定
//------------------------------------------------------------------------------
void CSquirrel::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}
