#include "Character.h"

ANIMATIONMODEL Character::loaded_anime[] = {};
MODEL Character::loaded_model[] = {};

Character::Character() {
	remove_flag = false;
}

/*!
@fn     �A�j���[�V�������f���Z�b�g�֐�
@brief  _animemodel�Ɏw�肳�ꂽloaded_anime���Z�b�g����
@param  file_name �Z�b�g����A�j���[�V�������f���̖��O
@return �Ȃ�
*/
void Character::SetAnimeModel(LPCTSTR file_name) {
	if (loaded_anime[GetType()] == NULL) {
		loaded_anime[GetType()] = GraphicsDevice.CreateAnimationModelFromFile(file_name);
	}
	_animemodel = loaded_anime[GetType()];
}
/*!
@fn     ���f���Z�b�g�֐�
@brief  _model�Ɏw�肳�ꂽloaded_model���Z�b�g����
@param  file_name �Z�b�g���郂�f���̖��O
@return �Ȃ�
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
	//�p�����[�^�ݒ�
	_width = width;
	_height = height;
	_depth = depth;
	//Model�ݒ�
	SimpleShape   shape;
	shape.Type = Shape_Box;
	shape.Width = _width;
	shape.Height = _height;
	shape.Depth = _depth;
	if (loaded_model[GetType()] == NULL) {
		loaded_model[GetType()] = GraphicsDevice.CreateModelFromSimpleShape(shape);
	}
	_model = loaded_model[GetType()];
	//Material�ݒ�
	Material _mtrl;
	_mtrl.Emissive = Color(color);
	_model->SetMaterial(_mtrl);
}
/*!
@fn  �@ �A�j���[�V�������f���|�W�V�����Z�b�g�֐�
@brief  _animemodel�̃|�W�V�������w�肷��
@param  character_pos �w�肷��Vector3�̐��l
@returm �Ȃ�
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
@fn  �@ �A�j���[�V�������f���X�P�[���Z�b�g�֐�
@brief  _animemodel�̃X�P�[�����w�肷��
@param  character_scale �w�肷��float�̐��l
@returm �Ȃ�
*/
void Character::SetAnimeScale(float character_scale) {
	_animemodel->SetScale(character_scale);
}
void Character::SetSimpleScale(float _scale) {
	_model->SetScale(_scale);
}
/*!
@fn  �@ �A�j���[�V�������f�����[�e�[�V�����Z�b�g�֐�
@brief  _animemodel�̃��[�e�[�V�������w�肷��
@param  character_rot �w�肷��Vector3�̐��l
@returm �Ȃ�
*/
void Character::SetAnimeRotation(Vector3 character_rot) {
	_animemodel->SetRotation(character_rot);
}
void Character::SetSimpleRotation(Vector3 _rot) {
	_model->SetRotation(_rot);
}
/*!
@fn  �@ �A�j���[�V�������f���̃g���b�N�Z�b�g�֐�
@brief  _animemodel�̃g���b�N���w�肷��
		_anime_track�ɍĐ��w�肵���g���b�N�̔ԍ���������
@param  track �Đ�����g���b�N�̃i���o�[
		max_track �Ō�̃g���b�N�̃i���o�[
@returm �Ȃ�
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
@fn  �@ �A�j���[�V�������f���̃A�j���[�V�����Đ��l�ݒ�֐�
@brief  _animemodel�̃A�j���[�V�����Đ��l��ݒ肷��
@param  time �w�肷��A�j���[�V�����Đ��l��float�̐��l
@returm �Ȃ�
*/
void Character::SetAnimeAdvanceTime(float time) {
	_advace_time = time;
}
/*!
@fn  �@ �A�j���[�V�������f���̃A�j���[�V�����Đ��֐�
@brief  _animemodel�̃A�j���[�V�����Đ�����
@param  time �w�肷��A�j���[�V�����̍Đ����x��float�̐��l
@returm �Ȃ�
*/
void Character::MoveAnimeAdvanceTime(float time) {
	_advace_time += time;
}
/*!
@fn  �@ �A�j���[�V�������f���̃A�j���[�V�����Đ��l�擾�֐�
@brief  _animemodel�̃A�j���[�V�����Đ��l���擾����
@returm ���݂�_animemodel�̃A�j���[�V�����Đ��l
*/
float Character::GetAdvanceTime() {
	return _advace_time;
}
/*!
@fn  �@ �A�j���[�V�������f���̃|�W�V�����擾�֐�
@brief  _animemodel�̃|�W�V�������擾����
@returm ���݂�_animemodel�̃|�W�V����
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
@fn     3D�`��֐�
@returm �Ȃ�
*/
void Character::Draw3D() {
	SetAnimeTrack(_anime_track, PLAYERANIMATIONTRACK);
	_animemodel->SetTrackPosition(_anime_track, _advace_time);
	_animemodel->Draw();
	_model->Draw();
}