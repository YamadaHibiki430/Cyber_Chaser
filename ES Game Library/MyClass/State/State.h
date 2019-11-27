/**
* @file   State.h
* @brief  ステート管理クラス
* @author 岡悠太郎
* @date   2019/7/5
*/
#pragma once

// インターフェースクラス
class State {
private:
   //経過フレーム
   int _timef = 0;
   //次に移行する状態
   State* _nextState = nullptr;
public:
   // 仮想デストラクタ
   virtual ~State() {}    
   // 更新
   virtual void Update() {}; 

   //キャンセル強度
   virtual int  State::CancelLv() {
	return 0;
   }
   //キャンセル可能フレーム
   virtual int  State::CancelOkF() {
	return 0;
   }
   //最終フレーム
   virtual int  State::ExitTime() {
	return 0;
   }
 
  void      State::SetNextState(State* next) { 
     delete _nextState;
     _nextState = next;
  }
  State*  State::GetNextState()                          { return _nextState; }

  void  State::AddTime() { _timef++; };
  int      State::GetTime() { return _timef; };

};
//ステート管理クラス
class StateProcessor {
private:
   State* _state=nullptr; //現在の状態
public:
   StateProcessor::StateProcessor() : _state(nullptr) {

   }
   StateProcessor::~StateProcessor() {
	delete _state;
   }

   void StateProcessor::Update() {
	_state->AddTime();
	_state->Update();

	if (_state->GetNextState() != nullptr) {
	   if (_state->GetNextState()->CancelLv() > _state->CancelLv() && _state->GetTime() >=_state->CancelOkF())  this->ChangeState(_state->GetNextState());
	   if (_state->GetTime() == _state->ExitTime())                                           this->ChangeState(_state->GetNextState());
	}
   }

   void     StateProcessor::ChangeState(State* state) {
	delete _state;
	_state = state;
   }
   State* StateProcessor::NowState() {
	return _state;
   }  
};
