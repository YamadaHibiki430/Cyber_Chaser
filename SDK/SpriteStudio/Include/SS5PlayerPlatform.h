/**
*  SS5PlayerPlatform.h
*/

#ifndef SS5PlayerPlatform_h
#define SS5PlayerPlatform_h

#include "SS5Player.h"
#include "common/loader/sstypes.h"
#include <stdio.h>
#include <string>
#include <set>

namespace ss
{
	struct State;
	struct UserData;
	class Player;

	extern unsigned char* SSFileOpen(const char* pszFileName, const char* pszMode, unsigned long * pSize);
	extern long SSTextureLoad(const char* pszFileName, SsTexWrapMode::_enum  wrapmode, SsTexFilterMode::_enum filtermode);
	extern bool SSTextureRelese(long handle);
	extern bool isAbsolutePath(const std::string& strPath);
	extern void SSDrawSprite(State state);
	extern void SSDrawSpriteEx(State state);
	extern bool SSGetTextureSize(long handle, int &w, int &h);
	extern void SSonUserData(Player *player, UserData *userData);
	extern void SSPlayEnd(Player *player);

	extern std::set<long>   release_handle_set;
	extern int              color_blend_func;
};	// namespace ss

#endif
