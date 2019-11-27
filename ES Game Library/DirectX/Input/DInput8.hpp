/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 DInput8.hpp
				DirectInput8�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�o�[�W�����錾
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "InputDevice.hpp"
#include <vector>

//------------------------------------------------------------------------------
//	DirectInput8�N���X��`
//------------------------------------------------------------------------------
class CDInput8 {
public:
	virtual ~CDInput8();

	// �V���O���g���C���X�^���X�̎擾
	static CDInput8& GetInstance() { static CDInput8 theDInput; return theDInput; }

	bool Initialize(const HWND hWnd);
	void Release();

	bool CreateKeyboard();
	bool CreateMouse();

	void ReleaseKeyboard() { ReleaseDevice(m_pKeyboard); }
	void ReleaseMouse()    { ReleaseDevice(m_pMouse);    }

	IInputDevice* GetKeyboard() const { return m_pKeyboard; }
	IInputDevice* GetMouse()    const { return m_pMouse;    }

	DWORD CreateGamePad(const DWORD inCount);
	void ReleaseGamePad();
	IInputDevice* GetGamePad(int i) const { return m_GamePad.at(i); }

	void Acquire();
	void Unacquire();

	inline void ResetDevice()   { Acquire();   }
	inline void SuspendDevice() { Unacquire(); }

	inline HWND GetHWnd() const { return m_hWnd; }

private:
	CDInput8();

	void ReleaseDevice(IInputDevice* pDevice);
	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}

	// �Q�[���p�b�h�񋓃R�[���o�b�N
	static BOOL CALLBACK EnumGamePadProc(LPDIDEVICEINSTANCE pDevInst, LPVOID pRef);
	// �Q�[���p�b�h������
	BOOL InitGamePad(const LPDIDEVICEINSTANCE pDevInst, LPVOID pRef);

	// �Q�[���p�b�h���񋓃R�[���o�b�N
	static BOOL CALLBACK EnumGamePadAxesProc(LPDIDEVICEOBJECTINSTANCE pDevObjInst, LPVOID pRef);
	// �Q�[���p�b�h���ݒ�
	BOOL SetAxesProp(const LPDIDEVICEOBJECTINSTANCE pDevObjInst, LPVOID pRef);

	// �����o�ϐ�
	IDirectInput8*               m_pDInput;			// DirectInput8�C���^�t�F�[�X
	HWND                         m_hWnd;			// ���C���E�B���h�E�n���h��

	IInputDevice*                m_pKeyboard;		// �L�[�{�[�h
	IInputDevice*                m_pMouse;			// �}�E�X

	std::vector<IInputDevice*>   m_GamePad;			// �Q�[���p�b�h

	// �Q�[���p�b�h���񋓈����\����
	struct DIENUMAXESARGS {
		CDInput8*              pDInput;
		IDirectInputDevice8*   pDIDevice;
	};

	CDInput8(const CDInput8&);
	CDInput8& operator=(const CDInput8&);
};

//------------------------------------------------------------------------------
//	�C�����C���֐���`
//------------------------------------------------------------------------------
inline CDInput8& DInput() { return CDInput8::GetInstance(); }

inline IInputDevice* DIKeyboard()     { return DInput().GetKeyboard(); }
inline IInputDevice* DIMouse()        { return DInput().GetMouse();    }
inline IInputDevice* DIGamePad(int i) { return DInput().GetGamePad(i); }
