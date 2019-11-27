/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft WindowsXP以降

 【コンパイラ】
     Microsoft VisualC++ 2008

 【プログラム】
	 XAduio2.hpp
				XAudio2クラスヘッダ

 【バージョン】
	 * Version    2014.03.00      2014/03/14  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SoundVoice.hpp"
#include <list>

//------------------------------------------------------------------------------
//	XAudio2クラス定義
//------------------------------------------------------------------------------
class CXAudio2 {
public:
	virtual ~CXAudio2();

	// シングルトンインスタンスの取得
	static CXAudio2& GetInstance()
	{ static CXAudio2 theXAudio; return theXAudio; }

	// 初期化・解放
	bool Initialize();
	void Release();

	// マスターボリューム
	void  SetMasterVolume(const float inVolume);
	float GetMasterVolume() const;

	// ソースボイス生成
	ISoundVoice* CreateVoiceFromFile(LPTSTR inFileName);

private:
	IXAudio2*                 m_pXAudio;
	IXAudio2MasteringVoice*   m_pMasterVoice;

//	std::list<ISoundVoice*>   m_SoundVoiceList;		// サウンドボイスリスト

private:
	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	CXAudio2();
	CXAudio2(const CXAudio2&);
	CXAudio2& operator=(const CXAudio2&);
};

//------------------------------------------------------------------------------
//	アクセス関数
//------------------------------------------------------------------------------
inline CXAudio2& XAudio() { return CXAudio2::GetInstance(); }
