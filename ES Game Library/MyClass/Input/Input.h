#pragma once
/**
* @file   Input.h
* @brief  キーボード、ゲームパッド両対応入力クラス
* @author 岡悠太郎
* @date   2019/6/10
*/

#include "../../ESGLib.h"

//◆ボタンの最大数
int const BUTTON_MAX=31;
//◆ゲームパッドの最大数
int const PAD_MAX = 4;

class  Input {
private:
     static int PadPadID[PAD_MAX];
     static int KeyBordPlayerID;
	
     static BYTE buttonID[PAD_MAX][BUTTON_MAX];
     static int       keybordID[BUTTON_MAX];

	static GamePadBuffer _padbuf[PAD_MAX];
	static KeyboardBuffer _keybuf;
	static GamePadState _padstate[PAD_MAX];
	static KeyboardState _keystate;

	Input::Input() {};

public:
  
      virtual Input::~Input() {};
	static void Input::Init();
	static void Input::Update();

	static bool Input::GetPadInputDown(int PadID, int KeyID);
	static bool Input::GetPadInput(int PadID, int KeyID);
	
	static bool Input::GetPadandKeybordInputDown(int PadID, int KeyID);
	static bool Input::GetPadandKeybordInput(int PadID, int KeyID);

	static Vector3 Input::GetStickVector(int PadID);
	static Vector3 Input::GetRightStickVector(int PadID);
	static Vector3 Input::GetArrowkeyVector();
	static Vector3 Input::GetStickandArrowkeyVector(int PadID);

	static void Input::SetKeybordID(int PadID);
	static void Input::SetPadID(int PadID, int newPadID);
	static void Input::SetConfigButtonID(int PadID, int button, int buttonid);
	static void Input::SetConfigKeyID(int key, int keyid);

	static void Input::ResetConfigButton(int PadID);
	static void Input::ResetConfigKey();
};