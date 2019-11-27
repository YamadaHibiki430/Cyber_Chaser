#include "GoalTime.h"



void GoalTime::Initialize() {
	def = GraphicsDevice.CreateDefaultFont();
}

void GoalTime::Update() {
	TimeSecond();



	if (movetime > 2.25f) {
		movetime = 2.25f;
	}
	if (movetime < 0.5) {
		movetime = 0.5f;
	}	

	if (is_move == true) {
		ds = s;
		dm = m;
		dh = h;
	}

}

void GoalTime::Draw2D() {
	SpriteBatch.DrawString(def, Vector2(0.f, 15.f), Color(1.f, 1.f, 1.f), _T("%d:%d.%d"), dm,ds,dh);
}

bool GoalTime::IsGoal() {
	if (sc >= 70) {
		is_goal = false;
	}
	else
	{
		is_goal = true;
	}
	is_move = false;

	return is_goal;
}

float GoalTime::GetMyTime() {
	float result = (float)sc * movetime;
	return result;
}

void GoalTime::TimeSecond(){
	in += 1;
	h = (in / 6) % 10;
	s = (in / 60) % 60;
	m = (in / 3600) % 60;
	sc = in / 60;
}

void GoalTime::AddMoveTime() {
	movetime += 0.25f;
}
void GoalTime::LowerMoveTime() {
	movetime -= 0.25f;
}
