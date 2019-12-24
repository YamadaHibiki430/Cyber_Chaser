#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../ESGLib.h"

#include <vector>

//------------------------------------------------------------------------------
//	SEManager�N���X��`
//------------------------------------------------------------------------------
class SoundManager {
public:
	virtual ~SoundManager() {}

	// �V���O���g���C���X�^���X�̎擾
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

	// �����o�ϐ�
	std::vector< std::vector<SOUND> > se;

	SoundManager(const SoundManager&);
	SoundManager& operator=(const SoundManager&);
};

//------------------------------------------------------------------------------
//	�C�����C���֐���`
//------------------------------------------------------------------------------
inline SoundManager& SNDMgr() { return SoundManager::GetInstance(); }
#define SNDMANAGER SoundManager::GetInstance()

