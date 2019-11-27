/**
* @file   Input.cpp
* @brief  キーボード、ゲームパッド両対応入力クラス
* @author 岡悠太郎
* @date   2019/6/10
*/
#include "Input.h"

//static
int Input::PadPadID[PAD_MAX];
BYTE  Input::buttonID[PAD_MAX][BUTTON_MAX];

int  Input::keybordID[BUTTON_MAX];
int Input::KeyBordPlayerID = 0;

GamePadBuffer Input::_padbuf[PAD_MAX];
KeyboardBuffer Input::_keybuf;
GamePadState Input::_padstate[PAD_MAX];
KeyboardState Input::_keystate;

void Input::Init() {
   //ゲームパッド使用宣言
	InputDevice.CreateGamePad(PAD_MAX);
	//キーボード初期化
	SetKeybordID(0);
	ResetConfigKey();
	//ゲームパッド初期化
	for (int i = 0; i < PAD_MAX; i++){
	   //PadID指定
	   SetPadID(i, i);
	   //コンフィグ情報初期化
	   ResetConfigButton(i);
	}
}
void Input::Update() {
   _keybuf = Keyboard->GetBuffer();
   _keystate = Keyboard->GetState();

   for (int i = 0; i < PAD_MAX; i++){
	_padbuf[i] = GamePad(PadPadID[i])->GetBuffer();
	_padstate[i] = GamePad(PadPadID[i])->GetState();
   }
}

bool Input::GetPadInputDown(int PadID, int KeyID) {
	return (BOOL)  _padbuf[PadID].IsPressed(buttonID[PadPadID[PadID]][KeyID]);
	}
bool Input::GetPadInput(int PadID, int KeyID) {	
	return _padstate[PadID].Buttons[KeyID]!=0;
	}

bool Input::GetPadandKeybordInputDown(int PadID, int KeyID) {
   if (PadPadID[PadID] == KeyBordPlayerID) {
	
	if (_keybuf.IsPressed(keybordID[KeyID]))
		return true;
	
   }
	return GetPadInputDown(PadID, KeyID);
}
bool Input::GetPadandKeybordInput(int PadID, int KeyID) {
   if (PadPadID[PadID] == KeyBordPlayerID) {
	
	if (_keystate.IsKeyDown(keybordID[KeyID])) return true;
   }
	return  GetPadInput(PadID, KeyID);
}

Vector3 Input::GetStickVector(int PadID) {
	Vector3 result=Vector3_Zero;


	if (_padstate[PadID].X < (Axis_Min/4)) {
	   result += Vector3_Left;
	}
	else {
	   if (_padstate[PadID].X > (Axis_Max / 4)) {
		result += Vector3_Right;
	   }
	}

	if ((_padstate[PadID].Y)  < (Axis_Min / 2))
	{
	   result += Vector3_Up;
	}
	else {
	   if (_padstate[PadID].Y  > (Axis_Max / 2)) {
		result += Vector3_Down;
	   }
	}
	return result;
}
Vector3 Input::GetRightStickVector(int PadID) {
	Vector3 result = Vector3_Zero;


	if (_padstate[PadID].Y2 < (Axis_Min / 4)) {
		result += Vector3_Left;
	}
	else {
		if (_padstate[PadID].Y2 > (Axis_Max / 4)) {
			result += Vector3_Right;
		}
	}

	if ((_padstate[PadID].X3) < (Axis_Min / 2))
	{
		result += Vector3_Up;
	}
	else {
		if (_padstate[PadID].X3 > (Axis_Max / 2)) {
			result += Vector3_Down;
		}
	}
	return result;
}

Vector3 Input::GetArrowkeyVector() {
	Vector3 result = Vector3_Zero;

	if (_keystate.IsKeyDown(Keys_Left)) {
	   result += Vector3_Left;
	}
	else {
	   if (_keystate.IsKeyDown(Keys_Right)) {
		result += Vector3_Right;
	   }
	}

	if (_keystate.IsKeyDown(Keys_Up)) { 
	   result += Vector3_Up; 
	}
	else {
	   if (_keystate.IsKeyDown(Keys_Down)) {
		result += Vector3_Down;
	   }
	}
	return result;
}
Vector3 Input::GetStickandArrowkeyVector(int PadID) {
   if (PadPadID[PadID] == KeyBordPlayerID) {
	if (GetArrowkeyVector() != Vector3_Zero) return GetArrowkeyVector();
   }
	return GetStickVector(PadID);
}

//コンフィグ用
void Input::SetKeybordID(int PadID) {
   Input::KeyBordPlayerID = PadPadID[PadID];
}
void Input::SetPadID(int PadID, int newPadID) {
   PadPadID[PadID] = newPadID;
}
void Input::SetConfigButtonID(int PadID, int button, int buttonid) {
	buttonID[PadPadID[PadID]][button]=buttonid;
}
void Input::SetConfigKeyID(int key, int keyid) {
	keybordID[key] = keyid;
}

//コンフィグリセット用
void Input::ResetConfigButton(int PadID) {
   SetConfigButtonID(PadID, 0, GamePad_Button1);
   SetConfigButtonID(PadID, 1, GamePad_Button2);
   SetConfigButtonID(PadID, 2, GamePad_Button3);
   SetConfigButtonID(PadID, 3, GamePad_Button4);
   SetConfigButtonID(PadID, 4, GamePad_Button5);
   SetConfigButtonID(PadID, 5, GamePad_Button6);
   SetConfigButtonID(PadID, 6, GamePad_Button7);
   SetConfigButtonID(PadID, 7, GamePad_Button8);
   SetConfigButtonID(PadID, 8, GamePad_Button9);
   SetConfigButtonID(PadID, 9, GamePad_Button10);
   SetConfigButtonID(PadID, 10, GamePad_Button11);
   SetConfigButtonID(PadID, 11, GamePad_Button12);
   SetConfigButtonID(PadID, 12, GamePad_Button13);
   SetConfigButtonID(PadID, 13, GamePad_Button14);
   SetConfigButtonID(PadID, 14, GamePad_Button15);
   SetConfigButtonID(PadID, 15, GamePad_Button16);
   SetConfigButtonID(PadID, 16, GamePad_Button17);
   SetConfigButtonID(PadID, 17, GamePad_Button18);
   SetConfigButtonID(PadID, 18, GamePad_Button19);
   SetConfigButtonID(PadID, 19, GamePad_Button20);
   SetConfigButtonID(PadID, 20, GamePad_Button21);
   SetConfigButtonID(PadID, 21, GamePad_Button22);
   SetConfigButtonID(PadID, 22, GamePad_Button23);
   SetConfigButtonID(PadID, 23, GamePad_Button24);
   SetConfigButtonID(PadID, 24, GamePad_Button25);
   SetConfigButtonID(PadID, 25, GamePad_Button26);
   SetConfigButtonID(PadID, 26, GamePad_Button27);
   SetConfigButtonID(PadID, 27, GamePad_Button28);
   SetConfigButtonID(PadID, 28, GamePad_Button29);
   SetConfigButtonID(PadID, 29, GamePad_Button30);
   SetConfigButtonID(PadID, 30, GamePad_Button31);
}
void Input::ResetConfigKey() {
   SetConfigKeyID(0, Keys_Z);
   SetConfigKeyID(1, Keys_X);
   SetConfigKeyID(2, Keys_C);
   SetConfigKeyID(7, Keys_Enter);
}