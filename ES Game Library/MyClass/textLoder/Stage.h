/*
 * @file   Map.h
 * @brief  �}�b�v�N���X�w�b�_�[�t�@�C��
 * @author ���c�D��
 */
#pragma once
//Scenemanager
#include"../Scene/SceneManager.h"

#include"../GroundManeger/GroundManeger.h"
#include"../Player/Player.h"
#include"../Camera/3DCamera.h"
//�G�l�~�[
//#include"Enemy/EnemyFactory.h"
//�A�C�e���I�u�U�[�o�["
//�Q��
class BoxObserver;
/*
@enmu �n�R�̐F
*/
enum BoxCollar {
	RED,
	GREEN,
	BLUE,
	ENDNUM
};
const int MAX_STAGE = 2;
/*
*@class �X�e�[�W�N���X
*@brief �X�e�[�W�𐶐�����N���X
*/
class Stage  {
private:
	BoxObserver* m_pBoxObserver;
	//!�{�b�N�X�͍ŏ��ɐ������Ȃ��̂ō��W�����ۑ����Ă���
	Vector3 BoxPos[ENDNUM];
	//!���݂̃X�e�[�W
	int m_now_stage;
	//�X�e�[�W�𐶐�������
	bool m_is_cleate = false;
	//!�I���t���O
	bool m_is_clear_end=false;
	//!�t�F�[�h�A�E�g�̓����x
	float fadeout=0.f;
	//!���[�h���̕`��
	void LoadDraw();
	//!�N���A��̕`��
	void EndDraw();
	//�Q�[���I�[�o�[�̏���
	void GameOverUpdate();
	//�N���A�̏���
	void ClearUpdate();
public:
	void Init();
	void Update();
	void CleateStage(int stage_num);
	void Draw2D();
	void DrawAlpha2D();
	void CreateBox(BoxCollar collar);

	void SetStageNum(int stage) { m_now_stage = stage; };

};