#pragma once
#include "../../ESGLib.h"

class GoalTime {
private:


	float movetime = 1.f;

	int in = 0;
	int h = 0, m = 0, s = 0;
	FONT def;

	bool is_move = true;
	bool is_goal = false;

	int sc =0;

	int dh = 0, dm = 0, ds = 0;
public:
	void Initialize();
	void Update();
	void Draw2D();

	void TimeSecond();
	void AddMoveTime();
	void LowerMoveTime();

	float GetMyTime();

	bool IsGoal();

};