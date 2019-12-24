#include "GoalTime.h"



void TimeManeger::Initialize() {
	def = GraphicsDevice.CreateDefaultFont();
}

void TimeManeger::Update() {
	TimeSecond();

	
		ds = s;
		dm = m;
		dh = h;
	

}

void TimeManeger::Draw2D() {
	SpriteBatch.DrawString(def, Vector2(0.f, 15.f), Color(1.f, 1.f, 1.f), _T("%d:%d.%d"), dm,ds,dh);
}

float TimeManeger::GetMyTime() {
	float result = (float)sc;
	return result;
}

void TimeManeger::TimeSecond(){
	in += 1;
	h = (in / 6) % 10;
	s = (in / 60) % 60;
	m = (in / 3600) % 60;
	sc = in / 60;
}


