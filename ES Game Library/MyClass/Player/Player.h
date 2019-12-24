#pragma once
#include "../Character/Character.h"
#include "../Input/Input.h"
#include "../State/State.h"
#include "../HitBox/HitBox.h"
#include "../Camera/3DCamera.h"

class Player;


class PlayerStateProcessor :public StateProcessor {
public:
	Player* player;
};

class Player :public Character {
private:
	PlayerStateProcessor _stateprocessor;
	HitBox* hitbox;
	FONT def;
protected:
	int gamepad_id = 0;//ゲームパッドのID
	float anime_speed;
	float player_scale;
	Vector3 player_position;
	Vector3 player_rotation;
	Vector3 cameravector;
	float player_speed;
	bool is_just = false;
	bool is_damage = false;
	int damage_count = 0;
	int justmovecount;
	bool damage_efect = false;
	bool IsHitObjects(std::string tags);
	void IsHitObjectsRemove(std::string tags);

public:
	Player();
	~Player();
	void Update();
	void Draw3D();
	void Draw2D();
 	void DrawAlph(){}
	void SetPadID(int pad_id);

	bool AxisDown();
	CHARACTER_TYPE GetType() { return PLAYER_TYPE; }
	bool isPadtach();
	int GetPadID() { return gamepad_id; }
	double AxisRotatoryPower();
	void SetCameraVector(Vector3 vec) { cameravector = vec; }
	int GetPlayerJust();
	void Remove();
	bool GetIsJust() { return is_just; }
	bool GetIsDamage() { return is_damage; }
	void SetTarget(Vector3 _target) {}
	int GetDamageCount() { return damage_count; }
	Vector3 GetPlayerPosition() { return player_position; }

	//-----STATE---------------


	//待機
	class Idle : public State
	{
	private:
		//状態の所有者
		PlayerStateProcessor* _owner;
	public:
		Idle(PlayerStateProcessor* owner) : _owner(owner) {}
		virtual ~Idle() {}

		virtual void Update();
	};
	//移動
	class Move : public State
	{
	private:
		PlayerStateProcessor* _owner;

	public:
	    Move(PlayerStateProcessor* owner) : _owner(owner) {}
		virtual ~Move() {}

		virtual void Update();
	};
	//スライド回避
	class Sliding : public State
	{
	private:
		PlayerStateProcessor* _owner;
		float time =0.0f;
		bool one_trigger = true;
	public:
		Sliding(PlayerStateProcessor* owner) : _owner(owner) { time = 0; }
		virtual ~Sliding() {}

		virtual void Update();
	};
	//ダメージ
	class Down : public State
	{
	private:
		PlayerStateProcessor* _owner;
		float time = 0.0f;
		
	public:
		Down(PlayerStateProcessor* owner) : _owner(owner) { time = 0; }
		virtual ~Down() {}

		virtual void Update();
	};
	//右ステップ
	class RightStep : public State
	{
	private:
		PlayerStateProcessor* _owner;
		float time = 0.0f;
		bool one_trigger = true;
	public:
		RightStep(PlayerStateProcessor* owner) : _owner(owner) { time = 0; }
		virtual ~RightStep() {}

		virtual void Update();
	};
	//右ステップ
	class LeftStep : public State
	{
	private:
		PlayerStateProcessor* _owner;
		float time = 0.0f;
		bool one_trigger = true;
	public:
		LeftStep(PlayerStateProcessor* owner) : _owner(owner) { time = 0; }
		virtual ~LeftStep() {}

		virtual void Update();
	};
	class Jump :public State
	{
	private:
		PlayerStateProcessor* _owner;
		float time = 0.0f;
		float jump_power = 0.f;
		bool one_trigger = true;
	public:
		Jump(PlayerStateProcessor* owner) :_owner(owner){}
		virtual ~Jump(){}

		virtual void Update();
	};
	class RightMove :public State
	{
	private:
		PlayerStateProcessor* _owner;

	public:
		RightMove(PlayerStateProcessor* owner) :_owner(owner) {}
		virtual ~RightMove(){}

		virtual void Update();

	};
	class LeftMove :public State
	{
	private:
		PlayerStateProcessor* _owner;

	public:
		LeftMove(PlayerStateProcessor* owner) :_owner(owner) {}
		virtual ~LeftMove() {}

		virtual void Update();

	};
	class JumpLong :public State
	{
	private:
		PlayerStateProcessor* _owner;
		float time = 0.0f;
		float jump_power = 0.f;
		bool one_trigger = true;
	public:
		JumpLong(PlayerStateProcessor* owner) :_owner(owner) {}
		virtual ~JumpLong() {}

		virtual void Update();
	};
};