#include "Character.h"

ANIMATIONMODEL Character::loaded_anime[] = {};
MODEL Character::loaded_model[] = {};

Character::Character() {
	remove_flag = false;
}

/*!
@fn     アニメーションモデルセット関数
@brief  _animemodelに指定されたloaded_animeをセットする
@param  file_name セットするアニメーションモデルの名前
@return なし
*/
void Character::SetAnimeModel(LPCTSTR file_name) {
	if (loaded_anime[GetType()] == NULL) {
		loaded_anime[GetType()] = GraphicsDevice.CreateAnimationModelFromFile(file_name);
	}
	_animemodel = loaded_anime[GetType()];
}
/*!
@fn     モデルセット関数
@brief  _modelに指定されたloaded_modelをセットする
@param  file_name セットするモデルの名前
@return なし
*/
void Character::SetModel(LPCTSTR file_name) {
	if (loaded_model[GetType()] == NULL) {
		loaded_model[GetType()] = GraphicsDevice.CreateModelFromFile(file_name);
	}
	_model = loaded_model[GetType()];
	Material mtrl2(_model);
	mtrl2.Emissive = Color(1.06f, 1.06f, 1.06f);
	_model->SetMaterial(mtrl2);
}
void Character::SetSimpleModel(float width, float height, float depth) {
	//パラメータ設定
	_width = width;
	_height = height;
	_depth = depth;
	//Model設定
	SimpleShape   shape;
	shape.Type = Shape_Box;
	shape.Width = _width;
	shape.Height = _height;
	shape.Depth = _depth;
	if (loaded_model[GetType()] == NULL) {
		loaded_model[GetType()] = GraphicsDevice.CreateModelFromSimpleShape(shape);
	}
	_model = loaded_model[GetType()];
	//Material設定
	Material _mtrl;
	_mtrl.Emissive = Color(color);
	_model->SetMaterial(_mtrl);
}
/*!
@fn  　 アニメーションモデルポジションセット関数
@brief  _animemodelのポジションを指定する
@param  character_pos 指定するVector3の数値
@returm なし
*/
void Character::SetAnimePosition(Vector3 character_pos) {
	_animemodel->SetPosition(character_pos);
}
void Character::SetSimplePosition(Vector3 _pos) {
	_model->SetPosition(_pos);
}

void Character::SetAnimeMove(Vector3 character_vector) {
	_animemodel->Move(character_vector);
}

/*!
@fn  　 アニメーションモデルスケールセット関数
@brief  _animemodelのスケールを指定する
@param  character_scale 指定するfloatの数値
@returm なし
*/
void Character::SetAnimeScale(float character_scale) {
	_animemodel->SetScale(character_scale);
}
void Character::SetSimpleScale(float _scale) {
	_model->SetScale(_scale);
}
/*!
@fn  　 アニメーションモデルローテーションセット関数
@brief  _animemodelのローテーションを指定する
@param  character_rot 指定するVector3の数値
@returm なし
*/
void Character::SetAnimeRotation(Vector3 character_rot) {
	_animemodel->SetRotation(character_rot);
}
void Character::SetSimpleRotation(Vector3 _rot) {
	_model->SetRotation(_rot);
}
/*!
@fn  　 アニメーションモデルのトラックセット関数
@brief  _animemodelのトラックを指定する
		_anime_trackに再生指定したトラックの番号を代入する
@param  track 再生するトラックのナンバー
		max_track 最後のトラックのナンバー
@returm なし
*/
void Character::SetAnimeTrack(int track, int max_track) {
	_animemodel->SetTrackEnable(track, true);
	for (int i = 0; i < max_track; i++) {
		if (i != track) {
			_animemodel->SetTrackEnable(i, false);
		}
	}
	_anime_track = track;
}
/*!
@fn  　 アニメーションモデルのアニメーション再生値設定関数
@brief  _animemodelのアニメーション再生値を設定する
@param  time 指定するアニメーション再生値のfloatの数値
@returm なし
*/
void Character::SetAnimeAdvanceTime(float time) {
	_advace_time = time;
}
/*!
@fn  　 アニメーションモデルのアニメーション再生関数
@brief  _animemodelのアニメーション再生する
@param  time 指定するアニメーションの再生速度のfloatの数値
@returm なし
*/
void Character::MoveAnimeAdvanceTime(float time) {
	_advace_time += time;
}
/*!
@fn  　 アニメーションモデルのアニメーション再生値取得関数
@brief  _animemodelのアニメーション再生値を取得する
@returm 現在の_animemodelのアニメーション再生値
*/
float Character::GetAdvanceTime() {
	return _advace_time;
}
/*!
@fn  　 アニメーションモデルのポジション取得関数
@brief  _animemodelのポジションを取得する
@returm 現在の_animemodelのポジション
*/
Vector3 Character::GetAnimePosition() {
	return _animemodel->GetPosition();
}
Vector3 Character::GetAnimeFrontVector() {
	return _animemodel->GetFrontVector();
}

void Character::SetSimpleColor(Vector3 _color) {
	color = _color;
}




ANIMATIONMODEL Character::GetAnimeModel() {
	return _animemodel;
}
/*!
@fn     3D描画関数
@returm なし
*/
void Character::Draw3D() {
	SetAnimeTrack(_anime_track, PLAYERANIMATIONTRACK);
	_animemodel->SetTrackPosition(_anime_track, _advace_time);
	_animemodel->Draw();
	_model->Draw();
}