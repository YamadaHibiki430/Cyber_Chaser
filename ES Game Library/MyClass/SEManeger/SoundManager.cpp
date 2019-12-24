#include "SoundManager.h"

bool SoundManager::Initialize()
{
//	se[0] = SoundDevice.CreateSoundFromFile(_T("se/se1.wav"));
//	se[1] = SoundDevice.CreateSoundFromFile(_T("se/se2.wav"));
//	se[2] = SoundDevice.CreateSoundFromFile(_T("se/se3.wav"));
	se.clear();
	se.reserve(4);

	bgm = SoundDevice.CreateMusicFromFile(_T("Sound/bgm_neorock82.wav"));

	return true;
}

int SoundManager::LoadSE(TCHAR* filename, const UINT multiple)
{
	std::vector<SOUND> se_vec;
	se_vec.assign(multiple, nullptr);

	SoundDevice.CreateSharedSoundFromFile(filename, &se_vec[0], multiple, false);
	se.push_back(se_vec);

	return se.size() - 1;


}

void SoundManager::PlaySE(int index)
{
	if (se[index].size() <= 1) {
		se[index][0]->Play();
		return;
	}

	for (UINT i = 0; i < se[index].size(); i++) {
		if (!se[index][i]->IsPlaying()) {
			se[index][i]->Play();
			return;
		}
	}

	se[index][0]->Play();
}

void SoundManager::PlayBGM()
{

	bgm->Play();

}
