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
	 SceneShared.cpp
				シーン共有クラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SceneShared.hpp"

//------------------------------------------------------------------------------
//	整数値取得
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
//	単精度浮動小数値取得
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
//	倍精度浮動小数値取得
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
//	ブール値取得
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
//	データ取得
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
//	文字列取得
//------------------------------------------------------------------------------
std::basic_string<TCHAR> CSceneShared::GetStringForKey(const char* pKey)
{
	std::map< std::basic_string<char>, std::basic_string<TCHAR> >::iterator it;
	it = m_MapString.find(pKey);
	if(it == m_MapString.end())
		return std::basic_string<TCHAR>();

	return it->second;
}
