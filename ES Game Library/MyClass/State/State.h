/**
* @file   State.h
* @brief  �X�e�[�g�Ǘ��N���X
* @author ���I���Y
* @date   2019/7/5
*/
#pragma once

// �C���^�[�t�F�[�X�N���X
class State {
private:
   //�o�߃t���[��
   int _timef = 0;
   //���Ɉڍs������
   State* _nextState = nullptr;
public:
   // ���z�f�X�g���N�^
   virtual ~State() {}    
   // �X�V
   virtual void Update() {}; 

   //�L�����Z�����x
   virtual int  State::CancelLv() {
	return 0;
   }
   //�L�����Z���\�t���[��
   virtual int  State::CancelOkF() {
	return 0;
   }
   //�ŏI�t���[��
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
//�X�e�[�g�Ǘ��N���X
class StateProcessor {
private:
   State* _state=nullptr; //���݂̏��
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
