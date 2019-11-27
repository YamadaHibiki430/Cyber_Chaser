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
	 SceneShared.hpp
				シーン共有ヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <map>
#include <string>
#include <tchar.h>

//------------------------------------------------------------------------------
//	シーン共有クラス定義
//------------------------------------------------------------------------------
class CSceneShared {
public:
	virtual ~CSceneShared() {}

	// シングルトンインスタンス取得
	static CSceneShared& GetInstance()
	{
		static CSceneShared   theSceneShared;
		return theSceneShared;
	}

	// 整数
	inline void SetIntegerForKey   (const char* pKey, const int    inValue) { m_MapInt[pKey] = inValue;    }
	inline void RemoveIntegerForKey(const char* pKey)						{ m_MapInt.erase(pKey);        }
	int GetIntegerForKey(const char* pKey);

	// 単精度浮動小数
	inline void SetFloatForKey     (const char* pKey, const float  inValue) { m_MapFloat[pKey] = inValue;  }
	inline void RemoveFloatForKey  (const char* pKey)						{ m_MapFloat.erase(pKey);      }
	float GetFloatForKey(const char* pKey);

	// 倍精度浮動小数
	inline void SetDoubleForKey    (const char* pKey, const double inValue) { m_MapDouble[pKey] = inValue; }
	inline void RemoveDoubleForKey (const char* pKey)						{ m_MapDouble.erase(pKey);     }
	double GetDoubleForKey(const char* pKey);

	// ブール
	inline void SetBoolForKey      (const char* pKey, const bool   inValue)	{ m_MapBool[pKey] = inValue;   }
	inline void RemoveBoolForKey   (const char* pKey)						{ m_MapBool.erase(pKey);       }
	bool GetBoolForKey(const char* pKey);

	// データ
	inline void SetDataForKey      (const char* pKey,       void*  pPtr   )	{ m_MapVoidPtr[pKey] = pPtr;   }
	inline void RemoveDataForKey   (const char* pKey)						{ m_MapVoidPtr.erase(pKey);    }
	void* GetDataForKey(const char* pKey);

	// 文字列
	inline void SetStringForKey(const char* pKey, const std::basic_string<TCHAR> inString)
	{ m_MapString[pKey] = inString; }
	inline void RemoveStringForKey(const char* pKey)
	{ m_MapString.erase(pKey);      }
	std::basic_string<TCHAR> GetStringForKey(const char* pKey);

private:
	CSceneShared() {}

	std::map<std::basic_string<char>, int   >   m_MapInt;
	std::map<std::basic_string<char>, float >   m_MapFloat;
	std::map<std::basic_string<char>, double>   m_MapDouble;
	std::map<std::basic_string<char>, bool  >   m_MapBool;
	std::map<std::basic_string<char>, void* >   m_MapVoidPtr;
	std::map<std::basic_string<char>,
				   std::basic_string<TCHAR> >   m_MapString;
};

//------------------------------------------------------------------------------
//	マクロ定義
//------------------------------------------------------------------------------
inline CSceneShared& SceneShared()
{ return CSceneShared::GetInstance(); }
