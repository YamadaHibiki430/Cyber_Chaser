#pragma once
#include "../../ESGLib.h"


class Ground {
private:
	MODEL ground;
	Vector3 color = Vector3(0.0f, 0.0f, 1.0f);
	float time;
	int time_count;
public:

	void SetColor(Vector3 colors);
	void Init();
	void Update();
	void Draw3D();


};
