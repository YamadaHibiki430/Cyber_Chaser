#pragma once
#include "../../ESGLib.h"

enum ROAD_TYPE {
	NONE_TYPE,
	STRAIGHT_TYPE,
	RIGHT_TYPE,
	LEFT_TYPE,
	ROAD_NUM_TYPE
};

class Road;

typedef std::shared_ptr<Road> Road_Ref;

class GroundManeger {
private:
	void JointRoteto();
	
	std::vector<Road_Ref> road_list;
	void AddList(Road_Ref& road);

	int create_count;
	float time;
	float back_time;
public:
	GroundManeger() {};
	~GroundManeger() {};
	void Initialize();
	void Update();
	void Draw3D();
	int GetCreate_Count() { return create_count; }
};


class Road {
private:
	static MODEL loaded_model[ROAD_NUM_TYPE];
	bool remove_flag = false;
public:
	Road();
	virtual void Update() = 0;
	virtual void Draw3D();

	virtual ROAD_TYPE GetType() = 0;
	void SetModel(LPCTSTR file_name);
	void SetPosition(Vector3 road_pos);
	void SetRotation(Vector3 road_rot);
	void SetScale(float road_scale);
	void SetSpeed(float _speed);

	bool IsRemove() { return remove_flag; }

protected:
	void remove() { remove_flag = true; }
	MODEL  _model;
	float move_speed;
};


class RoadStraight :public Road {
private:
	float time;
	
protected:
	Vector3 position;
	Vector3 rotation;
public:
	RoadStraight();
	~RoadStraight();

	void Update();
	void Draw3D();


	ROAD_TYPE GetType() { return STRAIGHT_TYPE; }
	void Remove();
};

class RoadBack :public Road {
private:
	float time;
protected:
	Vector3 position;
	Vector3 rotation;
public:
	RoadBack();
	~RoadBack();

	void Update();
	void Draw3D();


	ROAD_TYPE GetType() { return RIGHT_TYPE; }
	void Remove();

};

class RoadWall :public Road {
private:
	float time;
protected:
	Vector3 position;
	Vector3 rotation;
public:
	RoadWall();
	~RoadWall();

	void Update();
	void Draw3D();


	ROAD_TYPE GetType() { return LEFT_TYPE; }
	void Remove();
};