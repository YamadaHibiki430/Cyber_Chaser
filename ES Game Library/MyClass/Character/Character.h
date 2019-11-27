#pragma once
#include "../../ESGLib.h"

int const PLAYERANIMATIONTRACK = 10;

enum CHARACTER_TYPE {
	NULL_TYPE,
	PLAYER_TYPE,
	ENEMY_TYPE,
	BULLET_ENEMY_TYPE,
	SHORT_WALL_TYPE,
	LONG_WALL_TYPE,
	CHARA_NUM_TYPE
};

class Character;

typedef std::shared_ptr<Character> Character_Ref;

class Character {
private:
	static ANIMATIONMODEL loaded_anime[CHARA_NUM_TYPE];//アニメーションモデルの保存配列
	static MODEL loaded_model[CHARA_NUM_TYPE];//アニメーションモデルの保存配列
	bool remove_flag = false;
	//パラメータ
	float _width = 1.0f;
	float _height = 1.0f;
	float _depth = 1.0f;
	Vector3 color = Vector3(1.0f, 0.0f, 0.0f);
public:

	Character();
	virtual void Update() = 0;
	virtual void Draw3D();

	virtual CHARACTER_TYPE GetType() = 0;//自分自身のキャラクタータイプを返す
	void SetAnimeModel(LPCTSTR file_name);
	void SetAnimeMove(Vector3 character_vector);
	void SetAnimePosition(Vector3 character_pos);
	void SetAnimeRotation(Vector3 character_rot);
	void SetAnimeScale(float character_scale);
	void SetAnimeTrack(int track, int max_track);
	void SetAnimeAdvanceTime(float time);
	virtual void SetTarget(Vector3 _target) = 0;

	void MoveAnimeAdvanceTime(float time);


	void SetModel(LPCTSTR file_name);
	void SetSimpleModel(float width, float height, float depth);
	void SetSimpleColor(Vector3 _color);
	void SetSimplePosition(Vector3 _pos);
	void SetSimpleScale(float _scale);
	void SetSimpleRotation(Vector3 _rot);

	bool IsRemove() { return remove_flag; }
	Vector3 GetAnimePosition();
	Vector3 GetAnimeFrontVector();
	ANIMATIONMODEL GetAnimeModel();
	float GetAdvanceTime();
	void Oll_Remove() { remove_flag = true; }
	
protected:
	void remove() { remove_flag = true; }
	ANIMATIONMODEL _animemodel;
	MODEL  _model;
	double _advace_time = 0;//アニメーションモデルの再生数値
	int _anime_track;//アニメーションモデルのトラック情報
};