#include "Player.h"



Player::Player() {
	SetAnimeModel(_T("Model/test.X"));
	SetAnimeAdvanceTime(0.0f);
	//ステート
	_stateprocessor.player = this;
	_stateprocessor.ChangeState(new Player::Idle(&_stateprocessor));

	hitbox = new HitBox;
	hitbox->Init();
	hitbox->Settags("player");
	hitbox->SetHitBoxScale(5.f);

	justmovecount = 0;
	anime_speed = 0.08f;
	player_speed = -1.f;
	player_scale = 0.5f; 
	player_rotation = Vector3(0.0f, 180.0f, 0.0f);
	player_position = Vector3(0.0f, 0.0f, 10.0f);

	def = GraphicsDevice.CreateDefaultFont();
}

Player::~Player() {
	hitbox->OnReMove();
	remove();
}

void Player::Update() {

	if (player_speed <= -3.f) {
		player_speed = -3.f;
	}


	if (player_speed >= -1.f) {
		player_speed = -1.f;
	}

	player_position.y -= 3.5f;
	//ステート更新
	_stateprocessor.Update();
	if (player_position.y < 0) {
		player_position.y = 0;
	}
	
	//player_position = GetAnimePosition();
}

void Player::Draw3D() {
	hitbox->SetHitBoxPosition(GetAnimePosition() + Vector3(0.f, 10.f, 0.f));
	hitbox->Draw3D();
	MoveAnimeAdvanceTime(anime_speed);
	SetAnimeTrack(_anime_track, PLAYERANIMATIONTRACK);
	SetAnimePosition(player_position);
	SetAnimeRotation(player_rotation);
	SetAnimeScale(player_scale);

	_animemodel->SetTrackPosition(_anime_track, _advace_time);
	_animemodel->Draw();

	
}

void Player::Draw2D() {
	SpriteBatch.DrawString(def, Vector2(0.f, 0.f), Color(1.f, 1.f, 1.f), _T("%f"), AxisRotatoryPower());
}

/*!
@fn     ゲームパッドIDセット関数
@brief  Playerを操作するゲームパッドを設定
@parm　 pad_id ゲームパッドのIDの番号をintで指定
@returm なし
*/
void Player::SetPadID(int pad_id) {
	gamepad_id = pad_id;
}
/*!
@fn     ゲームパッドタッチ関数
@brief  ゲームパッドに触れているかを取る
@returm ゲームパッドのボタンを入力していたらtrue,入力していなかったらfalse
*/
bool Player::isPadtach() {
	bool result = false;
	if (Input::GetStickandArrowkeyVector(GetPadID()) != Vector3_Zero || Input::GetPadandKeybordInputDown(GetPadID(), 0)) {
		result = true;
	}
	return result;
}

double Player::AxisRotatoryPower() {
	GamePadState _padstate = GamePad(GetPadID())->GetState();
	double result = 0;

	//result = atan2f(double(_padstate.X - (double)Axis_Center) / double(Axis_Max - Axis_Center), double(-_padstate.Y - (double)Axis_Center) / double(Axis_Max + Axis_Center));
	if (cameravector == Vector3_Forward){
		result = atan2f(double(_padstate.X - Axis_Center) / double(Axis_Max - Axis_Center), double(-_padstate.Y - Axis_Center) / double(Axis_Max - Axis_Center));
	}
	if (cameravector == Vector3_Backward) {
		result = atan2f(double(-_padstate.X - Axis_Center) / double(Axis_Max - Axis_Center), double(_padstate.Y - Axis_Center) / double(Axis_Max - Axis_Center));
	}
	if (cameravector == Vector3_Right) {
		result = atan2f(double(_padstate.Y - Axis_Center) / double(Axis_Max - Axis_Center), double(_padstate.X - Axis_Center) / double(Axis_Max - Axis_Center));
	}
	if (cameravector == Vector3_Left) {
		result = atan2f(double(-_padstate.Y - Axis_Center) / double(Axis_Max - Axis_Center), double(-_padstate.X - Axis_Center) / double(Axis_Max - Axis_Center));
	}

	return MathHelper_ToDegrees(result);
}

bool Player::AxisDown() {
	GamePadState _padstate = GamePad(GetPadID())->GetState();
	bool result = false;
	if (Axis_Center == _padstate.X && Axis_Center == _padstate.Y) { result = true; }
	return result;
}

bool Player::IsHitObjects(std::string tags) {
	bool result = false;
	std::list<HitBox*> HitList = hitbox->HitHitBoxlist();
	for (auto&& other : HitList) {
		int a = 1;
		if (other->tag == tags) {

			result = true;
		}
	}
	return result;
}

void Player::IsHitObjectsRemove(std::string tags) {
	
	std::list<HitBox*> HitList = hitbox->HitHitBoxlist();
	for (auto&& other : HitList) {
		int a = 1;
		if (other->tag == tags) {
			other->OnReMove();
		}
	}
	
}

int Player::GetPlayerJust() {
	if (justmovecount >= 30) {justmovecount = 20;}
	if (justmovecount <= -5) { justmovecount = -5; }
	return justmovecount;
}

void Player::Remove() {
	
	justmovecount = 0;
	anime_speed = 0.08f;
	player_speed = -1.f;
	player_scale = 0.5f;
	player_rotation = Vector3(0.0f, 180.0f, 0.0f);
	player_position = Vector3(0.0f, 0.0f, 10.0f);
	SetAnimePosition(player_position);
	_stateprocessor.ChangeState(new Player::Idle(&_stateprocessor));
}


void Player::Idle::Update() {
	_owner->player->player_speed -= 0.0001f;
	if (_owner->player->IsHitObjects("enemy") == true) {
		_owner->player->is_damage = true;
		_owner->player->_stateprocessor.ChangeState(new Player::Down(&_owner->player->_stateprocessor));
		return;
	}

	//if (Input::GetPadInputDown(_owner->player->GetPadID(), 2) && Input::GetStickandArrowkeyVector(_owner->player->GetPadID()).x == Vector3_Left.x) {
	//	_owner->player->_stateprocessor.ChangeState(new Player::LeftStep(&_owner->player->_stateprocessor));
	//	return;
	//}
	//if (Input::GetPadInputDown(_owner->player->GetPadID(), 2) && Input::GetStickandArrowkeyVector(_owner->player->GetPadID()).x == Vector3_Right.x) {
	//	_owner->player->_stateprocessor.ChangeState(new Player::RightStep(&_owner->player->_stateprocessor));
	//	return;
	//}

	if (Input::GetPadInputDown(_owner->player->GetPadID(), 0) && _owner->player->player_position.y <= 0) {
		_owner->player->_stateprocessor.ChangeState(new Player::Jump(&_owner->player->_stateprocessor));
		return;
	}

	if (_owner->player->AxisDown() == false) {
		_owner->player->_stateprocessor.ChangeState(new Player::Move(&_owner->player->_stateprocessor));
		return;
	}

	_owner->player->anime_speed = 0.08f;
	_owner->player->SetAnimeTrack(2, PLAYERANIMATIONTRACK);


}

void Player::Move::Update() {
	_owner->player->player_speed -= 0.0001f;

	if (_owner->player->IsHitObjects("enemy") == true){
		_owner->player->is_damage = true;
		_owner->player->_stateprocessor.ChangeState(new Player::Down(&_owner->player->_stateprocessor)); 
		return;
	}
	//if (Input::GetPadInputDown(_owner->player->GetPadID(), 2) && Input::GetStickandArrowkeyVector(_owner->player->GetPadID()).x == Vector3_Left.x) {
	//	_owner->player->_stateprocessor.ChangeState(new Player::LeftStep(&_owner->player->_stateprocessor));
	//	return;
	//}
	//if (Input::GetPadInputDown(_owner->player->GetPadID(), 2) && Input::GetStickandArrowkeyVector(_owner->player->GetPadID()).x == Vector3_Right.x) {
	//	_owner->player->_stateprocessor.ChangeState(new Player::RightStep(&_owner->player->_stateprocessor));
	//	return;
	//}
	if (_owner->player->AxisDown() == true) {
		_owner->player->_stateprocessor.ChangeState(new Player::Idle(&_owner->player->_stateprocessor));
		return;
	}

	if (Input::GetPadInputDown(_owner->player->GetPadID(), 0) && _owner->player->player_position.y <=0) {
		_owner->player->_stateprocessor.ChangeState(new Player::Jump(&_owner->player->_stateprocessor));
		return;
	}

	if (Input::GetStickandArrowkeyVector(_owner->player->GetPadID()).x == Vector3_Left.x) {
		_owner->player->player_position.x -= _owner->player->player_speed;
	}

	if (Input::GetStickandArrowkeyVector(_owner->player->GetPadID()).x == Vector3_Right.x) {
		_owner->player->player_position.x += _owner->player->player_speed;
	}

	_owner->player->anime_speed = 0.08f;

	_owner->player->SetAnimeTrack(2, PLAYERANIMATIONTRACK);

}

void Player::Sliding::Update() {
	//1
	time += 1.f;
	if (_owner->player->IsHitObjects("enemy") == true && one_trigger == true && time < 15) {
		_owner->player->player_speed -= 0.05f;
		_owner->player->justmovecount += 1;
		_owner->player->hitbox->Settags("just");
		_owner->player->hitbox->SetColor(Vector3(1.f, 1.f, 0.f));
		one_trigger = false;
	}
	
	
	
	if (time >= 30) {
		if (_owner->player->AxisDown() == false) {
			time = 0.f;
			_owner->player->hitbox->Settags("player");
			one_trigger = true;
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Move(&_owner->player->_stateprocessor));
			return;
		}
		if (_owner->player->AxisDown() == true) {
			time = 0.f;
			_owner->player->hitbox->Settags("player");
			one_trigger = true;
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Idle(&_owner->player->_stateprocessor));
			
			return;
		}
	}
	_owner->player->SetAnimeTrack(8, PLAYERANIMATIONTRACK);
}

void Player::Down::Update() {
	_owner->player->is_damage = false;


	if (time >= 30) {
		if (_owner->player->AxisDown() == false) {
			_owner->player->player_speed += 0.05f;
			_owner->player->justmovecount -= 1;
			time = 0.f;
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Move(&_owner->player->_stateprocessor));
			return;
		}
		if (_owner->player->AxisDown() == true) {
			_owner->player->player_speed += 0.05f;
			_owner->player->justmovecount -= 1;
			time = 0.f;
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Idle(&_owner->player->_stateprocessor));

			return;
		}
	}

	_owner->player->anime_speed = 0.016f;
	_owner->player->SetAnimeTrack(5, PLAYERANIMATIONTRACK);

	if (_owner->player->player_speed >= -0.5f) {
		_owner->player->player_speed = -0.5f;
	}
	time += 1.f;
}

void Player::LeftStep::Update() {
	_owner->player->is_just = false;

	if (time >= 40) {
		if (_owner->player->AxisDown() == false) {
			_owner->player->hitbox->Settags("player");
			time = 0.f;
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Move(&_owner->player->_stateprocessor));
			return;
		}
		if (_owner->player->AxisDown() == true) {
			time = 0.f;
			_owner->player->hitbox->Settags("player");
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Idle(&_owner->player->_stateprocessor));

			return;
		}
	}


	if (_owner->player->IsHitObjects("enemy") == true && one_trigger == true && time < 25) {
		_owner->player->SetAnimeAdvanceTime(0.0f);
		_owner->player->player_speed -= 0.05f;
		_owner->player->hitbox->Settags("just");
		_owner->player->justmovecount += 1;
		_owner->player->hitbox->SetColor(Vector3(1.f, 1.f, 0.f));
		_owner->player->is_just = true;
		one_trigger = false;
	}


	_owner->player->anime_speed = 0.016f;
	_owner->player->SetAnimeTrack(1, PLAYERANIMATIONTRACK);
	time += 1.f;
	_owner->player->SetAnimeMove(Vector3(_owner->player->player_speed *1.2, 0.f, 0.f));
}

void Player::RightStep::Update() {
	_owner->player->is_just = false;


	if (time >= 40) {
		if (_owner->player->AxisDown() == false) {
			time = 0.f;
			_owner->player->hitbox->Settags("player");
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Move(&_owner->player->_stateprocessor));
			return;
		}
		if (_owner->player->AxisDown() == true) {
			time = 0.f;
			_owner->player->hitbox->Settags("player");
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Idle(&_owner->player->_stateprocessor));

			return;
		}
	}

	if (_owner->player->IsHitObjects("enemy") == true && one_trigger == true && time < 25) {
		_owner->player->SetAnimeAdvanceTime(0.0f);
		_owner->player->player_speed -= 0.05f;
		_owner->player->hitbox->Settags("just");
		_owner->player->justmovecount += 1;
		_owner->player->hitbox->SetColor(Vector3(1.f, 1.f, 0.f));
		_owner->player->is_just = true;
		one_trigger = false;
	}

	
	_owner->player->anime_speed = 0.016f;
	_owner->player->SetAnimeTrack(0, PLAYERANIMATIONTRACK);
	time += 1.f;
	_owner->player->SetAnimeMove(Vector3(-_owner->player->player_speed * 1.2, 0.f, 0.f));
}

void Player::Jump::Update() {

	if (time >= 10) {
		if (_owner->player->AxisDown() == false) {
			_owner->player->hitbox->Settags("player");
			time = 0.f;
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Move(&_owner->player->_stateprocessor));
			return;
		}
		if (_owner->player->AxisDown() == true) {
			time = 0.f;
			_owner->player->hitbox->Settags("player");
			_owner->player->hitbox->SetColor(Vector3(1.f, 0.f, 0.f));
			_owner->player->_stateprocessor.ChangeState(new Player::Idle(&_owner->player->_stateprocessor));

			return;
		}
	}


	_owner->player->anime_speed = 0.016f;
	time += 1.f;
	//_owner->player->SetAnimeTrack(1, PLAYERANIMATIONTRACK);
	_owner->player->player_position.y += 5.f;
}
