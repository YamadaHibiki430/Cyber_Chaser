/**
* @file   CameraManager.cpp
* @brief  カメラ管理クラス.cpp
* @author 岡悠太郎
* @date   2019/5/30
*/

#include"CameraManager.h"

//定数
const float MIN_FOV = 15.0f;
const float MAX_FOV = 75.0f;

//static
CameraManager* CameraManager::mainCamera=nullptr;

/*
	* @fn     　　Vector3 リープ補完関数
	* @brief      リープ補完関数を利用してVector3でリープ補完を行う
	* @param[in]  Vector3 始点となるベクトル位置
	* @param[in]  Vector3 終点となるベクトル位置
	* @param[in]  float   両端の距離を1とした時の割合
	* @return     Vector3 リープ補完結果   　　　　　　
*/
Vector3 CameraManager::Vector3_Lerp(Vector3 A, Vector3 B, float t) {

   Vector3 Lerp = Vector3_Zero;

   Lerp.x = MathHelper_Lerp(A.x, B.x, t);
   Lerp.y = MathHelper_Lerp(A.y, B.y, t);
   Lerp.z = MathHelper_Lerp(A.z, B.z, t);

   return Lerp;
}

	void CameraManager::Init() {   
		Viewport   view = GraphicsDevice.GetViewport();
		_camera->SetView(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f));
		_camera->SetPerspectiveFieldOfView(MAX_FOV, (float)view.Width, (float)view.Height, 1.0f, 10000.f);
		_nowFoV = MAX_FOV;
		GraphicsDevice.SetCamera(_camera);
		//メインカメラが設定されていない場合自分を設定
		if (mainCamera == nullptr) mainCamera = this;
	}

	void CameraManager::Update() {
		Zoom();
		Move();

		_camera->SetPosition(Vector3_Lerp(_camera->GetPosition(), pos, 0.05f));
		GraphicsDevice.SetCamera(_camera);
	}

/*
* @fn     　　ズーム処理
*/
	void CameraManager::Zoom() {
	   _nowFoV = MathHelper_Lerp(_nowFoV,
		MathHelper_Lerp(MIN_FOV,MAX_FOV,GetGreatestDistance() / 10.0f), 0.05f);
		_camera->SetFieldOfViewY(_nowFoV);
	}

/*
* @fn     　　カメラの移動処理
*/
	void CameraManager::Move() {

		pos = GetCenterPoint();
	}

	void CameraManager::GetMinMaxPosition(Vector3& minpos, Vector3& maxpos) {
	  minpos = Vector3(99999.f, 99999.f, 99999.f);
	  maxpos = Vector3(-99999.f, -99999.f, -99999.f);
	   for (auto&& t : _tagets) {
		minpos = Vector3_Min(minpos, Vector3(t->x, t->y, t->z));
		maxpos = Vector3_Max(maxpos, Vector3(t->x, t->y, t->z));
	   }
	}

	float CameraManager::GetGreatestDistance()
	{
	   Vector3 _minpos,_maxpos;
	   GetMinMaxPosition(_minpos, _maxpos);

		float _m_size_x = (_maxpos.x - _minpos.x);
		float _m_size_y = (_maxpos.y - _minpos.y) / 2.f;

		return _m_size_x >= _m_size_y ? _m_size_x : _m_size_y;
	}

	Vector3 CameraManager::GetCenterPoint()
	{
	   Vector3 _minpos, _maxpos;
	   GetMinMaxPosition(_minpos, _maxpos);

		Vector3 m_centerpos = Vector3_Lerp(_minpos, _maxpos, 0.5f);

		return Vector3(m_centerpos.x, m_centerpos.y, -1000.0f);
	}

	void CameraManager::AddTagets(Vector3* addtaget) {
		_tagets.push_back(addtaget);
	}
