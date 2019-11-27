#pragma once

#include"../../ESGLib.h"

class CameraManager {
private:
	//�J����
     CAMERA _camera;

     //�f���^�[�Q�b�g�̃��X�g
	std::list<Vector3*> _tagets;

	//FoV�L���p
	float _nowFoV = 0.0f;

	Vector3 pos;

	void CameraManager::Zoom();
	void CameraManager::Move();

public:
   //���C���J����
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