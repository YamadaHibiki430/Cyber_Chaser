/*
 * @file   Map.h
 * @brief  マップクラスヘッダーファイル
 * @author 松田颯太
 */
#pragma once
//Scenemanager
#include"../Scene/SceneManager.h"

#include"../GroundManeger/GroundManeger.h"
#include"../Player/Player.h"
#include"../Camera/3DCamera.h"
//エネミー
//#include"Enemy/EnemyFactory.h"
//アイテムオブザーバー"
//参照
class BoxObserver;
/*
@enmu ハコの色
*/
enum BoxCollar {
	RED,
	GREEN,
	BLUE,
	ENDNUM
};
const int MAX_STAGE = 2;
/*
*@class ステージクラス
*@brief ステージを生成するクラス
*/
class Stage  {
private:
	BoxObserver* m_pBoxObserver;
	//!ボックスは最初に生成しないので座標だけ保存しておく
	Vector3 BoxPos[ENDNUM];
	//!現在のステージ
	int m_now_stage;
	//ステージを生成したか
	bool m_is_cleate = false;
	//!終了フラグ
	bool m_is_clear_end=false;
	//!フェードアウトの透明度
	float fadeout=0.f;
	//!ロード中の描画
	void LoadDraw();
	//!クリア後の描画
	void EndDraw();
	//ゲームオーバーの処理
	void GameOverUpdate();
	//クリアの処理
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