
#include"SceneManager.h"
BaseScene *SceneManager::m_pScene = nullptr;       //�V�[������ɂ���/**
/*
*@fn �V�[���̃`�F���W
* @brief �����̃V�[���𐶐�����
* @param ��������V�[��
* @return 
*/
void SceneManager::ChangeScene(SCENE scene) {

	if (m_pScene != nullptr) {
		delete m_pScene;
	}
	int i = 0;
	switch (scene) {          
	case SCENE::TITLE:
		m_pScene = new TitleScene(); 
		break;
	case SCENE::MAIN:
		m_pScene = new MainScene();      
		break;
	case SCENE::RESULT:
		m_pScene = new ResultScene();     
		break;
	case SCENE::GAMEOVER:
		m_pScene = new GameOver();
		break;
	}
	m_pScene->Initialize();
	int a = 1;
}
/*
*@fn ���݂̃V�[���̍X�V�֐�
* @brief ���ݓǂݍ���ł���V�[���̕`��֐����Ăяo��
* @param 
* @return
*/
void SceneManager::Update() {
	m_pScene->Update();         //���݂̃V�[���̍X�V�֐�
}
/*
*@fn ���݂̃V�[����3D�`��֐�
* @brief ���ݓǂݍ���ł���V�[����3D�`��֐����Ăяo��
* @param 
* @return 
*/
void SceneManager::Draw3D() {
	m_pScene->Draw3D();           //���݂̃V�[���̕`��֐�
}
/*
*@fn ���݂̃V�[���̃A���t�@3D�`��֐�
* @brief ���ݓǂݍ���ł���V�[����3D�`��֐����Ăяo��
* @param
* @return
*/

void SceneManager::DrawAlpha3D() {
	m_pScene->DrawAlpha3D();
}
/*
*@fn ���݂̃V�[����2D�`��֐�
* @brief ���ݓǂݍ���ł���V�[����2D�`��֐����Ăяo��
* @param
* @return
*/
void SceneManager::Draw2D() {
	m_pScene->Draw2D();
}
