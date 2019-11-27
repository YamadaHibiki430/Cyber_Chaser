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
	 Network.hpp
				�l�b�g���[�N�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
// #include <winsock2.h>
#include <windows.h>
#include <list>

//------------------------------------------------------------------------------
//	�l�b�g���[�N�Z�b�V�����v���p�e�B�\����
//------------------------------------------------------------------------------
struct NetworkSessionProperties {
	union {
		struct {
			BYTE   IP4;			// IP�A�h���X
			BYTE   IP3;
			BYTE   IP2;
			BYTE   IP1;
		};
		u_long     IP;			// IP�A�h���X
		char*      IPAdress;	// IP�A�h���X������
		char*      HostName;	// �z�X�g��
	};
	u_short        Port;		// �|�[�g

	// �w���p�[�֐�
	inline void SetIP(const BYTE ip1, const BYTE ip2, const BYTE ip3, const BYTE ip4)
	{	IP1 = ip1;	IP2 = ip2;	IP3 = ip3;	IP4 = ip4;	}

	void SetIP(const char* adress)
	{
		IP = ::inet_addr(adress);
		if(IP == INADDR_NONE) {
			hostent*   pHost = ::gethostbyname(adress);
			if(pHost != NULL)
				::CopyMemory(&IP, pHost->h_addr_list[0], 4);
		}
	}
};

//------------------------------------------------------------------------------
//	���[�J���l�b�g���[�N�Z�b�V�����^��`
//------------------------------------------------------------------------------
typedef std::list<NetworkSessionProperties> AvailableLocalNetworkSessions;

//------------------------------------------------------------------------------
//	�l�b�g���[�N�Z�b�V�����N���X��`
//------------------------------------------------------------------------------
class CNetworkSession {
public:
	CNetworkSession();
	virtual ~CNetworkSession();

	bool Create(const u_short inPort, const DWORD inMSTimeout = INFINITE);
	bool Join(const NetworkSessionProperties& inSession, const DWORD inMSTimeout = 20 * 1000);

	int SendData(const void* pData, const int inBytes, const DWORD inMSTimeout = INFINITE);
	int ReceiveData(void* pBuffer , const int inBytes, const DWORD inMSTimeout = INFINITE);

	int Find(AvailableLocalNetworkSessions& outSessions,
			 const u_short inPort, const DWORD inMSTimeout = 20 * 1000);

	void Dispose();

	inline void SetCanselEvent() { ::SetEvent(m_hCanselEvent); }

private:
	SOCKET         m_ListenSock;
	SOCKET         m_ComSock;

	HANDLE         m_hCanselEvent;

	bool           m_Dispose;

	// �N���X�ϐ�
	static int     m_WSARef;
	static char*   SECURE_KEY;

	CNetworkSession(const CNetworkSession&);
	CNetworkSession& operator=(const CNetworkSession&);
};
