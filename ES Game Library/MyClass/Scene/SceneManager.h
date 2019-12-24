

#pragma once
#include"BaseScreen.h"
#include"MainScene.h"
#include"TitleScene.h"
#include"ResultScene.h"
#include"GameOver.h"

/*
*�V�[���Ǘ��N���X
*/
class SceneManager {
public:
	enum SCENE {
		TITLE,
		MAIN,
		GAMEOVER,
		RESULT
	};
	static void ChangeScene(SCENE scene);//�V�[���������ɂ���ĕς���֐�
	static void Update();//���݂̃V�[���̍X�V�֐�
	static void Draw3D();//���݂̃V�[���̕`��֐�
	static void Draw2D();//���݂̃V�[���̕`��֐�
	static void DrawAlpha3D();//���݂̃V�[���̕`��֐�

	static BaseScene *m_pScene;           //���݂̃V�[��

private:

};

