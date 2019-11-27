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
	 Network.cpp
				�l�b�g���[�N�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	���[�j���O�}��
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4996)

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <winsock2.h>
#include <lm.h>
#include "Network.hpp"

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "ws2_32.lib"  )
#pragma comment(lib, "netapi32.lib")

//------------------------------------------------------------------------------
//	�N���X�ϐ���`
//------------------------------------------------------------------------------
int     CNetworkSession::m_WSARef   = 0;
char*   CNetworkSession::SECURE_KEY = "ESNW_IWHBAWY_HTS";

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CNetworkSession::CNetworkSession()
	: m_ListenSock(INVALID_SOCKET), m_ComSock(INVALID_SOCKET), m_Dispose(true)
{
	m_WSARef++;
	if(m_WSARef == 1) {
		// WinSock2������
		WSADATA   wd;
		for(int minor = 2; minor >= 0; minor--) {
			if(::WSAStartup(MAKEWORD(2, minor), &wd) == 0)
				break;
		}
	}

	// �L�����Z���C�x���g����
	m_hCanselEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CNetworkSession::~CNetworkSession()
{
	// �l�b�g���[�N�Z�b�V�����I��
	Dispose();

	// �L�����Z���C�x���g�N���[�Y
	if(m_hCanselEvent != NULL) {
		::SetEvent(m_hCanselEvent);
		::CloseHandle(m_hCanselEvent);
		m_hCanselEvent = NULL;
	}

	m_WSARef--;
	if(m_WSARef == 0) {
		// WinSock2���
		::WSACleanup();
	}
}

//------------------------------------------------------------------------------
//	�l�b�g���[�N�Z�b�V��������
//------------------------------------------------------------------------------
bool CNetworkSession::Create(const u_short inPort, const DWORD inMSTimeout)
{
	if(m_Dispose == false)
		Dispose();

	::ResetEvent(m_hCanselEvent);

	// �ڑ��v����t�\�P�b�g����
	m_ListenSock = ::socket(AF_INET, SOCK_STREAM, 0);
	if(m_ListenSock == INVALID_SOCKET) {
		::OutputDebugString(TEXT("*** Error - �ڑ��v����t�\�P�b�g�������s(CNetworkSession_Create)\n"));
		return false;
	}

	// �\�P�b�g�I�v�V�����ݒ�
	int   opt = 1;
	::setsockopt(m_ListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	// �o�C���h
	sockaddr_in   server;
	server.sin_family           = AF_INET;
	server.sin_port             = ::htons(inPort);
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	if(::bind(m_ListenSock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		::OutputDebugString(TEXT("*** Error - �o�C���h���s(CNetworkSession_Create)\n"));
		Dispose();
		return false;
	}

	// �ڑ��v����t�J�n
	if(::listen(m_ListenSock, 5) == SOCKET_ERROR) {
		::OutputDebugString(TEXT("*** Error - �ڑ��v����t�J�n���s(CNetworkSession_Create)\n"));
		Dispose();
		return false;
	}

	// �ڑ��҂��C�x���g�ݒ�
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ListenSock, hEvents[0], FD_ACCEPT);

	const DWORD   BUF_SIZE = ::strlen(SECURE_KEY);
	char*         buf      = new char[BUF_SIZE];
	while(buf != NULL) {
		// �ڑ��܂��̓L�����Z���҂�
		if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, inMSTimeout, FALSE) != WSA_WAIT_EVENT_0)
			break;

		// �ڑ�
		m_ComSock = ::accept(m_ListenSock, NULL, NULL);
		if(m_ComSock == INVALID_SOCKET)
			break;

		// �F�؃L�[�����҂�(�ő�20�b)
		::ZeroMemory(buf, BUF_SIZE);
		ReceiveData (buf, BUF_SIZE, 20 * 1000);

		// �F�؃L�[�m�F
		if(::memcmp(SECURE_KEY, buf, BUF_SIZE) == 0)
			break;

		// �\�P�b�g�N���[�Y
		::shutdown(m_ComSock, SD_BOTH);
		::closesocket(m_ComSock);
		m_ComSock = INVALID_SOCKET;

		// �C�x���g�V�O�i��������
		::WSAResetEvent(hEvents[0]);
	}
	delete[] buf;

	// �C�x���g���
	::WSAEventSelect(m_ListenSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	if(m_ComSock == INVALID_SOCKET) {
		::OutputDebugString(TEXT("*** Error - �ڑ��󂯓��ꎸ�s(CNetworkSession_Create)\n"));
		Dispose();
		return false;
	}

	m_Dispose = false;

	return true;
}

//------------------------------------------------------------------------------
//	�l�b�g���[�N�Z�b�V�����ڑ�
//------------------------------------------------------------------------------
bool CNetworkSession::Join(const NetworkSessionProperties& inSession, const DWORD inMSTimeout)
{
	if(m_Dispose == false)
		Dispose();

	::ResetEvent(m_hCanselEvent);

	// �^�C���A�E�g�ݒ�(�ő�20�b)
	const DWORD   TIMEOUT = inMSTimeout <= 20 * 1000 ? inMSTimeout : 20 * 1000;

	// �ʐM�\�P�b�g����
	m_ComSock = ::socket(AF_INET, SOCK_STREAM, 0);
	if(m_ComSock == INVALID_SOCKET) {
		::OutputDebugString(TEXT("*** Error - �ʐM�\�P�b�g�������s(CNetworkSession_Join)\n"));
		return false;
	}

	// �ڑ���ݒ�
	sockaddr_in   server;
	server.sin_family           = AF_INET;
	server.sin_port             = ::htons(inSession.Port);
	server.sin_addr.S_un.S_addr = inSession.IP;

	// �ڑ��C�x���g�ݒ�
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_CONNECT);

	// �ڑ�
	BOOL   error = ::connect(m_ComSock, (sockaddr*)&server, sizeof(server));
	if(::WSAGetLastError() == WSAEWOULDBLOCK) {
		// �ڑ������܂��̓C�x���g�҂�
		switch(::WSAWaitForMultipleEvents(2, hEvents, FALSE, TIMEOUT, FALSE)) {
		  // ����I��
		  case WSA_WAIT_EVENT_0:
			error = 0;
			break;

		  // �L�����Z��
		  case WSA_WAIT_EVENT_0 + 1:
			break;

		  // �G���[
		  default:
			// �ڑ���A�h���X�ݒ�
			server.sin_addr.S_un.S_addr = ::inet_addr(inSession.IPAdress);
			if(server.sin_addr.S_un.S_addr == INADDR_NONE) {
				// �z�X�g������IP�擾
				hostent*   pHost = ::gethostbyname(inSession.HostName);
				if(pHost == NULL)
					break;
				// IP�ݒ�
				::CopyMemory(&server.sin_addr.S_un.S_addr, pHost->h_addr_list[0], 4);
			}

			// �Đڑ�
			::WSAResetEvent(hEvents[0]);

			error = ::connect(m_ComSock, (sockaddr*)&server, sizeof(server));
			if(::WSAGetLastError() != WSAEWOULDBLOCK)
				break;

			// �ڑ������܂��̓C�x���g�҂�
			if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, TIMEOUT, FALSE) == WSA_WAIT_EVENT_0)
				error = FALSE;

			break;
		}
	}

	// �C�x���g���
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	if(error != FALSE) {
		::OutputDebugString(TEXT("*** Error - �ڑ����s(CNetworkSession_Join)\n"));
		Dispose();
		return false;
	}

	// �F�؃L�[���M
	::send(m_ComSock, SECURE_KEY, ::strlen(SECURE_KEY), 0);

	m_Dispose = false;

	return true;
}

//------------------------------------------------------------------------------
//	�f�[�^���M
//------------------------------------------------------------------------------
int CNetworkSession::SendData(const void* pData, const int inBytes, const DWORD inMSTimeout)
{
	if(m_Dispose)
		return 0;

	::ResetEvent(m_hCanselEvent);

	if(m_ComSock == NULL) {
		::OutputDebugString(TEXT("*** Error - �ʐM�\�P�b�g������(CNetworkSession_SendDate)\n"));
		return 0;
	}

	// �ڑ��C�x���g�ݒ�
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_WRITE | FD_CLOSE);

	// ���M���[�v
	char*   buf       = (char*)pData;
	int     send_byte = 0;
	while(send_byte < inBytes) {
		// ���M
		const int   RET = ::send(m_ComSock, buf + send_byte, inBytes - send_byte, 0);
		if(RET != SOCKET_ERROR) {
			send_byte += RET;
			continue;
		}

		// �G���[����
		if(::WSAGetLastError() != WSAEWOULDBLOCK) {
			::OutputDebugString(TEXT("*** Error - ���M���s(CNetworkSession_SendData)\n"));
			break;
		}

		// �C�x���g�ҋ@
		if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, inMSTimeout, FALSE) != WSA_WAIT_EVENT_0) {
			::OutputDebugString(TEXT("*** Error - ���M���s(CNetworkSession_SendData)\n"));
			break;
		}

		// �C�x���g����
		WSANETWORKEVENTS   net_event;
		if(::WSAEnumNetworkEvents(m_ComSock, hEvents[0], &net_event) != 0) {
			::OutputDebugString(TEXT("*** Error - �C�x���g�������s(CNetworkSession_SendData)\n"));
			break;
		}
		if(net_event.lNetworkEvents & FD_WRITE)
			continue;
		if(net_event.lNetworkEvents & FD_CLOSE) {
			::OutputDebugString(TEXT("*** Error - �Z�b�V�����ؒf(CNetworkSession_SendData)\n"));
			break;
		}

		// �C�x���g�V�O�i��������
		::WSAResetEvent(hEvents[0]);
	}

	// �C�x���g���
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	return send_byte;
}

//------------------------------------------------------------------------------
//	�f�[�^��M
//------------------------------------------------------------------------------
int CNetworkSession::ReceiveData(void* pBuffer, const int inBytes, const DWORD inMSTimeout)
{
	if(m_Dispose)
		return 0;

	::ResetEvent(m_hCanselEvent);

	if(m_ComSock == NULL) {
		::OutputDebugString(TEXT("*** Error - �ʐM�\�P�b�g������(CNetworkSession_ReceiveData)\n"));
		return 0;
	}

	// �ڑ��C�x���g�ݒ�
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_READ | FD_CLOSE);

	// ��M���[�v
	char*   buf       = (char*)pBuffer;
	int     recv_byte = 0;
	while(recv_byte < inBytes) {
		// ��M
		const int   RET = ::recv(m_ComSock, buf + recv_byte, inBytes - recv_byte, 0);
		if(RET != SOCKET_ERROR) {
			recv_byte += RET;
			continue;
		}

		// �G���[����
		if(::WSAGetLastError() != WSAEWOULDBLOCK) {
			::OutputDebugString(TEXT("*** Error - ��M���s(CNetworkSession_ReceiveData)\n"));
			break;
		}

		// �C�x���g�ҋ@
		if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, inMSTimeout, FALSE) != WSA_WAIT_EVENT_0) {
			::OutputDebugString(TEXT("*** Error - ��M���s(CNetworkSession_ReceiveData)\n"));
			break;
		}

		// �C�x���g�擾
		WSANETWORKEVENTS   net_event;
		if(::WSAEnumNetworkEvents(m_ComSock, hEvents[0], &net_event) != 0) {
			::OutputDebugString(TEXT("*** Error - �C�x���g�擾���s(CNetworkSession_ReceiveData)\n"));
			break;
		}
		if(net_event.lNetworkEvents & FD_READ )
			continue;
		if(net_event.lNetworkEvents & FD_CLOSE) {
			::OutputDebugString(TEXT("*** Error - �Z�b�V�����ؒf(CNetworkSession_ReceiveData)\n"));
			break;
		}

		// �C�x���g�V�O�i��������
		::WSAResetEvent(hEvents[0]);
	}

	// �C�x���g���
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	return recv_byte;
}

//------------------------------------------------------------------------------
//	���[�J���l�b�g���[�N�Z�b�V��������
//------------------------------------------------------------------------------
int CNetworkSession::Find(AvailableLocalNetworkSessions& outSessions,
						  const u_short inPort, const DWORD inMSTimeout)
{
	::ResetEvent(m_hCanselEvent);

	outSessions.clear();

	// LAN�[������
	LPSERVER_INFO_101   pTermInfo = NULL;
	DWORD               entries, total, resume = 0;
	::NetServerEnum(NULL, 101, (LPBYTE*)&pTermInfo,
					MAX_PREFERRED_LENGTH, &entries, &total,
					SV_TYPE_WORKSTATION | SV_TYPE_NT | SV_TYPE_WINDOWS,
					NULL, &resume);

	// �ϐ��ݒ�
	sockaddr_in   terminal;
	terminal.sin_family   = AF_INET;
	terminal.sin_port     = ::htons(inPort);

	// �^�C���A�E�g�ݒ�(�ő�20�b)
	const DWORD   TIMEOUT = inMSTimeout <= 20 * 1000 ? inMSTimeout : 20 * 1000;

	// �m�F�L�[�ݒ�
	const char*   KEY     = "ESNW_LAE_HTS";

	// �ڑ��C�x���g�ݒ�
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_CONNECT);

	// �[���m�F
	for(DWORD i = 0; i < entries; i++) {
		// ���C�h��������}���`�o�C�g������ɕϊ�
		char   name[63 + 1];
		::WideCharToMultiByte(CP_ACP, 0, pTermInfo[i].sv101_name, -1, name, 64, NULL, FALSE);

		// �ڑ���A�h���X�ݒ�
		hostent*   pHost = ::gethostbyname(name);
		if(pHost == NULL)
			continue;
		::CopyMemory(&terminal.sin_addr.S_un.S_addr, pHost->h_addr_list[0], 4);

		// �w��|�[�g�ɐڑ�
		SOCKET   sock;
		sock = ::connect(m_ComSock, (sockaddr*)&terminal, sizeof(terminal));
		if(::WSAGetLastError() == WSAEWOULDBLOCK) {
			// �ڑ������܂��̓L�����Z���҂�
			if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, TIMEOUT, FALSE) == WSA_WAIT_EVENT_0 + 1)
				break;	// �L�����Z��
		}
		if(sock == INVALID_SOCKET)
			continue;

		// �m�F�L�[���M
		::send(sock, KEY, ::strlen(KEY), 0);

		// �Z�b�V�����ؒf
		::shutdown(sock, SD_BOTH);
		::closesocket(sock);

		// ���X�g�֓o�^
		NetworkSessionProperties   session;
		session.IP   = terminal.sin_addr.S_un.S_addr;
		session.Port = inPort;
		outSessions.push_back(session);
	}

	// �C�x���g���
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	// �l�b�g���[�N�Ǘ��֐����������
	::NetApiBufferFree(pTermInfo);

	return outSessions.size();
}

//------------------------------------------------------------------------------
//	�l�b�g���[�N�Z�b�V�����I��
//------------------------------------------------------------------------------
void CNetworkSession::Dispose()
{
	m_Dispose = true;

	// �ҋ@�L�����Z��
	if(m_hCanselEvent != NULL)
		::SetEvent(m_hCanselEvent);

	// �ʐM�\�P�b�g�N���[�Y
	if(m_ComSock != INVALID_SOCKET) {
		// �|�[�g����̂��߁A���ڑ��ɂ��Ďc�f�[�^����M
		::shutdown(m_ComSock, SD_SEND);

		// �u���b�L���O���[�h�ɂ���
		u_long   cmd = 0;
		::ioctlsocket(m_ComSock, FIONBIO, &cmd);

		// ��M���[�v
		char   buf[256];
		while(::recv(m_ComSock, buf, 256, 0) > 0)
			;

		// �\�P�b�g�N���[�Y
		::shutdown(m_ComSock, SD_BOTH);
		::closesocket(m_ComSock);
		m_ComSock = INVALID_SOCKET;
	}

	// �ڑ��v����t�\�P�b�g�N���[�Y
	if(m_ListenSock != INVALID_SOCKET) {
		::closesocket(m_ListenSock);
		m_ListenSock = INVALID_SOCKET;
	}
}

#pragma warning(pop)
