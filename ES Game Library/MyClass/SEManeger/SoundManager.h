#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "../../ESGLib.h"

#include <vector>

//------------------------------------------------------------------------------
//	SEManagerクラス定義
//------------------------------------------------------------------------------
class SoundManager {
public:
	virtual ~SoundManager() {}

	// シングルトンインスタンスの取得
	static SoundManager& GetInstance()
	{ 
		static SoundManager theSEManager;
		return theSEManager;
	}

	bool Initialize();

	int LoadSE(TCHAR* filename, const UINT multiple = 1);

	void PlaySE(int index);
	
	void PlayBGM();

private:
	SoundManager() {}
	
	MUSIC bgm;

	// メンバ変数
	std::vector< std::vector<SOUND> > se;

	SoundManager(const SoundManager&);
	SoundManager& operator=(const SoundManager&);
};

//------------------------------------------------------------------------------
//	インライン関数定義
//------------------------------------------------------------------------------
inline SoundManager& SNDMgr() { return SoundManager::GetInstance(); }
#define SNDMANAGER SoundManager::GetInstance()

