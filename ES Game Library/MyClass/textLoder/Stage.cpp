/**
 * @file Map.cpp
 * @brief マップクラスソースファイル
 * @author 松田颯太  
 */
#include"Stage.h"
const float MAP_CHIP_SIZE = 64;
/*
*@fn 初期化関数
*/
void Stage::Init() {

}
/*
@fn 更新関数
*/
void Stage::Update() {
	//if (GetJust()) return;
	//if (!m_is_cleate) {
	//	CleateStage(m_now_stage);
	//}
	//if (m_pBoxObserver->IsClear()) {
	//		m_is_clear_end=true;
	//}
	////クリア処理
	ClearUpdate();
	//ゲームオーバー処理
	GameOverUpdate();
}
/*
* @fn　ゲームオーバー処理
*/
void Stage::GameOverUpdate() {
	if (!m_is_cleate)return;
	//if (IsFindTag("Player"))return;
	SceneManager::ChangeScene(SceneManager::RESULT);

}
/*
* @fn クリアの処理
*/
void Stage::ClearUpdate() {
	if (!m_is_clear_end) return;
	fadeout += 0.005;
	/*if (fadeout >= 1) ReMove();*/
}
/*
*@fn 2D描画
*/
void Stage::Draw2D() {

}
/*
*@fn 2D透過描画
*/
void Stage::DrawAlpha2D() {
	
	LoadDraw();
	EndDraw();

}
/*
* @fn ロード画面中の描画
*/
void Stage::LoadDraw(){
	if (m_is_cleate)return;
		switch (m_now_stage) {
		case 0:
			//SpriteBatch.Draw(*SpriteStorage::GetSprite(SPRITE_NUM::GAME_START_1), Vector3(0, 0, 0));
			break;
		case 1:
			//SpriteBatch.Draw(*SpriteStorage::GetSprite(SPRITE_NUM::GAME_START_2), Vector3(0, 0, 0));
			break;
		}

	
}
/*
* @fn クリア後の描画
*/
void Stage::EndDraw() {
	//if (!m_pBoxObserver->IsClear()) return;
	//SpriteBatch.Draw(*SpriteStorage::GetSprite(SPRITE_NUM::STAGE_CLEAL), Vector3(0, 0, 0));
}
/*
 * @fn         マップ取得関数
 * @brief      テキストからマップデータをインポートする
 * @param[in]  int ステージ番号
 * @param[out] std::vector< std::vector<TCHAR> > 取得したマップを収納する変数
 * @return     なし
*/
void InportMapDate(int stage_num, std::vector< std::vector<TCHAR> >& map_data) {
	StreamReader map_csv;
	switch (stage_num) {
	case 0:
		map_csv.Open(_T("map/stage01.txt"));
		break;
	case 1:
		map_csv.Open(_T("map/stage02.txt"));
		break;
	}
	TCHAR buffer[500 + 1];

	while (true) {
		map_csv.ReadLine(buffer);//一行読む
		if (map_csv.IsEOF())
			break;//最後まで読んだらブレイク

		std::vector<TCHAR>  data;

		for (int m = 0; buffer[m] != '\0'; m++) {
			if (buffer[m] == ',')
				continue;//コンマだったら次の文字に進む

			data.push_back(buffer[m]);
		}
		map_data.push_back(data);
	}

}
/*
 * @fn         コンフィグ取得関数
 * @brief      テキストからコンフィグをインポートする
 * @param[out] std::vector< std::vector<int*>> >　保存する変数
 * @return     なし
*/
void InputConfig(std::vector< std::vector<int> >& config_deta) {

	FILE* file;
	file = fopen("Config/config.csv", "r");

	char str[512];
	char* s;
	int mozi_count = 0;

	do {
		mozi_count = 0;
		std::vector<int> data;
		s = fgets(str, 512, file);

		for (int i = 0; ; i++) {

			if (i == 0) {

				//初回
				//tp =_tcstok_s(buffer, TEXT(","), &next);
				s = strtok(str, ",");


			}
			else {
				//２回目以降
				//tp = _tcstok_s(NULL, TEXT(","), &next);
				s = strtok(NULL, ",");
			}

			//取り出し終了
			if (s == NULL)
				break;
			
			//文字列をintに変換捨てリストに格納
			data.push_back(atoi(s));
			mozi_count++;
		}

		config_deta.push_back(data);
		int a = 0;
	} while (mozi_count != 1);
	fclose(file);
}

/*
 * @fn         マップ生成関数
 * @brief      ステージ番号をもとにステージを生成する
 * @param[in]  int ステージ番号
 * @return     なし
*/
void Stage::CleateStage(int stage_num) {

	std::vector< std::vector<TCHAR> > map_date;
	InportMapDate(stage_num, map_date);
	std::vector< std::vector<int> > config_date;
	InputConfig(config_date);
	//地形生成
	//Ground* ground;
	//ground = new Ground;
	//ground->CreateModel(stage_num);
	//プレイヤー生成
	//Player* player;
	//player = new Player;
	//player->SetGround(ground);
	//ChildObj_AddList((ChildObjRef)player);
	//カメラ生成
	//CameraManager *camera;
	//camera = new CameraManager;
	//camera->SetTargetObject(player);
	//camera->SetCameraZoom(config_date[2][stage_num+1]);
	//ChildObj_AddList((ChildObjRef)camera);

	//ヒューマンオブザーバー生成
	//ItemObserver* human_observer=new HumanObserver;
	//ChildObj_AddList((ChildObjRef)human_observer);
	//コインオブザーバー
	//SignalObserver* signal_observer = new SignalObserver;
	//ChildObj_AddList((ChildObjRef)signal_observer);
	//signal_observer->SetStagePointa(this);

	//レーダー生成
	//m_pRadar = new Radar;
	//ChildObj_AddList((ChildObjRef)m_pRadar);
	//m_pRadar->SetPivotPos(&player->transform);
	//ライトの生成
	//CreateLight(config_date[1][stage_num + 1],player);
	for (int y = 0; y < map_date.size(); y++) {
		for (int x = 0; x < map_date[y].size(); x++) {
			Vector3 CHIP_POS = Vector3(x * MAP_CHIP_SIZE, -y * MAP_CHIP_SIZE, 0.0f);

			switch (map_date[y][x]) {
			case'p':

				break;
			case'1':
				break;
			case'2':
				/*ground->transform.position = CHIP_POS;
				player->transform.position = CHIP_POS;*/
				break;
			case'3':
				//ItemBase* human;
				//human = ItemFactory::CreateItem(ItemNum::HUMAN);
				//ChildObj_AddList((ChildObjRef)human);
				//human->transform.position = CHIP_POS;
				//human->OvserverAddList(human_observer);
				break;
			case'4':
				//ItemBase* redsignal;
				//redsignal = ItemFactory::CreateItem(ItemNum::SIGNAL_RED);
				//ChildObj_AddList((ChildObjRef)redsignal);
				//redsignal->transform.position = CHIP_POS;
				//redsignal->OvserverAddList(signal_observer);
				break;
			case'5':
				//ItemBase* greensignal;
				//greensignal= ItemFactory::CreateItem(ItemNum::SIGNAL_GREEN);
				//ChildObj_AddList((ChildObjRef)greensignal);
				//greensignal->transform.position = CHIP_POS;
				//greensignal->OvserverAddList(signal_observer);
				break;
			case'6':
				//ItemBase* bluesignal;
				//bluesignal = ItemFactory::CreateItem(ItemNum::SIGNAL_BLUE);
				//ChildObj_AddList((ChildObjRef)bluesignal);
				//bluesignal->transform.position = CHIP_POS;
				//bluesignal->OvserverAddList(signal_observer);
				break;
			case'7':
				BoxPos[RED] = CHIP_POS;
				break;
			case'8':
				BoxPos[GREEN] = CHIP_POS;
				break;
			case'9':
				BoxPos[BLUE] = CHIP_POS;
				break;

			case'a':
				//EnemyBase* enemy;
				//enemy = EnemyFactory::CreateEnemy(Enemy_Num::HOMING_ENEMY);
				//ChildObj_AddList((ChildObjRef)enemy);
				//enemy->transform.position = CHIP_POS;
				//enemy->SetTraget(player);
				break;
			}
		}
	}
	m_is_cleate = true;
}
/*
* @fn         ボックス生成関数
* @brief      あらかじめ保存してた座標の場所に生成する
* @param[in]  列挙型　生成する色
*/

