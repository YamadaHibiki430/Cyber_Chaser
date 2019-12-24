#pragma once
#include "../../ESGLib.h"

class TimeManeger {
private:
	int in = 0;
	int h = 0, m = 0, s = 0;
	FONT def;

	int sc =0;

	int dh = 0, dm = 0, ds = 0;
public:
	void Initialize();
	void Update();
	void Draw2D();

	void TimeSecond();

	float GetMyTime();


};