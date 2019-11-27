/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 GameScene.hpp
				�Q�[���V�[���N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../DirectX/Graphics/DXGraphics9.hpp"
#include "../DirectX/Input/DInput8.hpp"
#include "../DirectX/Audio/DXAudio8.hpp"
#include "../DirectX/Show/DShow.hpp"
#include "../Framework/FixTimer.hpp"

//------------------------------------------------------------------------------
//	�Q�[���V�[�������}�N��
//------------------------------------------------------------------------------
#define GAME_SCENE(scene) reinterpret_cast<int>(scene)

//------------------------------------------------------------------------------
// �Q�[���V�[����{�N���X��`
//------------------------------------------------------------------------------
class CGameScene {
public:
	CGameScene() {}
	virtual ~CGameScene() {}

	virtual int ActiveProc()
	{
/*
		if(FPSTimer().IsSkip() == false) {
			Draw();
			DXGraphics().UpdateFrame();
		}
		FPSTimer().Wait();
*/
		Draw();
		DXGraphics().UpdateFrame();
		FixTimer().Wait();
		return Update(); 
	}

	virtual int NonActiveProc() 
	{ 
		::WaitMessage();
		return 0;
	}

public:
	virtual bool Initialize () { return true; }
	virtual void LoadContent() {}

	virtual int  Update() { return 0; }
	virtual void Draw  () {}

private:
	CGameScene(const CGameScene&);
	CGameScene& operator=(const CGameScene&);
};

//------------------------------------------------------------------------------
//	�Q�[���V�[���C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../GameMain.h"