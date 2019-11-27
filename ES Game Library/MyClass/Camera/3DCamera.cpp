#include "3DCamera.h"

void CameraManejar::Init() {
	// ビューポート情報取得
	Viewport   view = GraphicsDevice.GetViewport();

	// カメラ設定
	camera->SetView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	camera->SetPerspectiveFieldOfView(45.0f, (float)view.Width, (float)view.Height, 1.0f, 10000.0f);
	GraphicsDevice.SetCamera(camera);
	SimpleShape   shape;
	shape.Type = Shape_Box;
	shape.Width = 1.f;
	shape.Height = 1.f;
	shape.Depth = 1.f;
	target = GraphicsDevice.CreateModelFromSimpleShape(shape);
	targetpos = Vector3_Zero;
	targetrote = Vector3_Zero;
}

void CameraManejar::Move() {

	// キーボード入力を取得
	KeyboardState key = Keyboard->GetState(); //キーボードの状態を取

    //カメラの操作-----------------------------

	if (key.IsKeyDown(Keys_Right)) { // もし、'→'キーが押されていたら… 
		camera->Rotation(0.0f, 1.0f, 0.0f); // カメラを1.0度回転させる 
	}
	if (key.IsKeyDown(Keys_Left)) { // もし、'→'キーが押されていたら… 
		camera->Rotation(0.0f, -1.0f, 0.0f); // カメラを1.0度回転させる 
	}
	if (key.IsKeyDown(Keys_Z)) {
		camera->Rotation(1.0f, 0.0f, 0.0f);
	}
	if (key.IsKeyDown(Keys_X)) {
		camera->Rotation(-1.0f, 0.0f, 0.0f);
	}
	if (key.IsKeyDown(Keys_C)) {
		camera->Rotation(0.0f, 0.0f, -1.0f);
	}
	if (key.IsKeyDown(Keys_V)) {
		camera->Rotation(0.0f, 0.0f, 1.0f);
	}
	if (key.IsKeyDown(Keys_Up)) {
		camera->Move(0.0f, 0.0f, 0.25f);
	}
	if (key.IsKeyDown(Keys_Down)) {
		camera->Move(0.0f, 0.0f, -0.25f);
	}
	if (key.IsKeyDown(Keys_Space)) {
		camera->Move(0.0f, 0.25f, 0.0f);
	}
	if (key.IsKeyDown(Keys_Enter)) {
		camera->Move(0.0f, -0.25f, 0.0f);
	}
	
	if (abs(targetrote.z) >= 360) { targetrote.z = 0; }
	GraphicsDevice.SetCamera(camera); // 変更したカメラをデバイスに適用する
}

void CameraManejar::Draw() {
	target->SetPosition(targetpos);
	target->SetRotation(targetrote);

}

Vector3 CameraManejar::GetCameraVector() {
	Vector3 result;

	if (target->GetFrontVector().z > 0.5f) { result = Vector3_Forward; }
	if (target->GetFrontVector().z < -0.5f) { result = Vector3_Backward; }
	if (target->GetFrontVector().x < -0.5f) { result = Vector3_Right; }
	if (target->GetFrontVector().x > 0.5f) { result = Vector3_Left; }
		
	
	return result;
}

void CameraManejar::SetLockAtPosCamera(Vector3 atpos,int id) {
	//if (Input::GetRightStickVector(id) == Vector3_Right) {
	//	targetrote.y += 1.f;
	//}
	//if (Input::GetRightStickVector(id) == Vector3_Left) {
	//	targetrote.y -= 1.f;
	//}
	targetpos = atpos + Vector3(0.0f, 10.0f, -10.0f);
	
	camera->SetLookAt(target->GetPosition() + Vector3(0.0f, 20.0f, 30.0f) + target->GetFrontVector() * 20, target->GetPosition(), Vector3_Up);
	//camera->SetRotation(camera->GetRotation().x +20.f, camera->GetRotation().y+25.f, camera->GetRotation().z+25.f);
	GraphicsDevice.SetCamera(camera);
}