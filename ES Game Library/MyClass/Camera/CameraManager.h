#pragma once

#include"../../ESGLib.h"

class CameraManager {
private:
	//カメラ
     CAMERA _camera;

     //映すターゲットのリスト
	std::list<Vector3*> _tagets;

	//FoV記憶用
	float _nowFoV = 0.0f;

	Vector3 pos;

	void CameraManager::Zoom();
	void CameraManager::Move();

public:
   //メインカメラ
   static CameraManager* CameraManager::mainCamera;

      CameraManager::CameraManager() {};
	virtual CameraManager::~CameraManager() {};
	
	Vector3 Vector3_Lerp(Vector3 A, Vector3 B, float t);

	void CameraManager::Init();
	void CameraManager::Update();
	
	void CameraManager::GetMinMaxPosition(Vector3& minpos, Vector3& maxpos);
	float CameraManager::GetGreatestDistance();
	Vector3 CameraManager::GetCenterPoint();

	void CameraManager::SetMainCamera(CameraManager* camera) {
	   mainCamera = camera;
	}
	void CameraManager::AddTagets(Vector3* addtaget);
};