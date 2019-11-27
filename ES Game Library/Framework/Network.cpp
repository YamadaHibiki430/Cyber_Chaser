/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft Windows 7以降

 【コンパイラ】
     Microsoft VisualC++ 2013

 【プログラム】
	 Network.cpp
				ネットワーククラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	ワーニング抑制
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4996)

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <winsock2.h>
#include <lm.h>
#include "Network.hpp"

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "ws2_32.lib"  )
#pragma comment(lib, "netapi32.lib")

//------------------------------------------------------------------------------
//	クラス変数定義
//------------------------------------------------------------------------------
int     CNetworkSession::m_WSARef   = 0;
char*   CNetworkSession::SECURE_KEY = "ESNW_IWHBAWY_HTS";

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CNetworkSession::CNetworkSession()
	: m_ListenSock(INVALID_SOCKET), m_ComSock(INVALID_SOCKET), m_Dispose(true)
{
	m_WSARef++;
	if(m_WSARef == 1) {
		// WinSock2初期化
		WSADATA   wd;
		for(int minor = 2; minor >= 0; minor--) {
			if(::WSAStartup(MAKEWORD(2, minor), &wd) == 0)
				break;
		}
	}

	// キャンセルイベント生成
	m_hCanselEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CNetworkSession::~CNetworkSession()
{
	// ネットワークセッション終了
	Dispose();

	// キャンセルイベントクローズ
	if(m_hCanselEvent != NULL) {
		::SetEvent(m_hCanselEvent);
		::CloseHandle(m_hCanselEvent);
		m_hCanselEvent = NULL;
	}

	m_WSARef--;
	if(m_WSARef == 0) {
		// WinSock2解放
		::WSACleanup();
	}
}

//------------------------------------------------------------------------------
//	ネットワークセッション生成
//------------------------------------------------------------------------------
bool CNetworkSession::Create(const u_short inPort, const DWORD inMSTimeout)
{
	if(m_Dispose == false)
		Dispose();

	::ResetEvent(m_hCanselEvent);

	// 接続要求受付ソケット生成
	m_ListenSock = ::socket(AF_INET, SOCK_STREAM, 0);
	if(m_ListenSock == INVALID_SOCKET) {
		::OutputDebugString(TEXT("*** Error - 接続要求受付ソケット生成失敗(CNetworkSession_Create)\n"));
		return false;
	}

	// ソケットオプション設定
	int   opt = 1;
	::setsockopt(m_ListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	// バインド
	sockaddr_in   server;
	server.sin_family           = AF_INET;
	server.sin_port             = ::htons(inPort);
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	if(::bind(m_ListenSock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		::OutputDebugString(TEXT("*** Error - バインド失敗(CNetworkSession_Create)\n"));
		Dispose();
		return false;
	}

	// 接続要求受付開始
	if(::listen(m_ListenSock, 5) == SOCKET_ERROR) {
		::OutputDebugString(TEXT("*** Error - 接続要求受付開始失敗(CNetworkSession_Create)\n"));
		Dispose();
		return false;
	}

	// 接続待ちイベント設定
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ListenSock, hEvents[0], FD_ACCEPT);

	const DWORD   BUF_SIZE = ::strlen(SECURE_KEY);
	char*         buf      = new char[BUF_SIZE];
	while(buf != NULL) {
		// 接続またはキャンセル待ち
		if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, inMSTimeout, FALSE) != WSA_WAIT_EVENT_0)
			break;

		// 接続
		m_ComSock = ::accept(m_ListenSock, NULL, NULL);
		if(m_ComSock == INVALID_SOCKET)
			break;

		// 認証キー到着待ち(最大20秒)
		::ZeroMemory(buf, BUF_SIZE);
		ReceiveData (buf, BUF_SIZE, 20 * 1000);

		// 認証キー確認
		if(::memcmp(SECURE_KEY, buf, BUF_SIZE) == 0)
			break;

		// ソケットクローズ
		::shutdown(m_ComSock, SD_BOTH);
		::closesocket(m_ComSock);
		m_ComSock = INVALID_SOCKET;

		// イベントシグナル初期化
		::WSAResetEvent(hEvents[0]);
	}
	delete[] buf;

	// イベント解放
	::WSAEventSelect(m_ListenSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	if(m_ComSock == INVALID_SOCKET) {
		::OutputDebugString(TEXT("*** Error - 接続受け入れ失敗(CNetworkSession_Create)\n"));
		Dispose();
		return false;
	}

	m_Dispose = false;

	return true;
}

//------------------------------------------------------------------------------
//	ネットワークセッション接続
//------------------------------------------------------------------------------
bool CNetworkSession::Join(const NetworkSessionProperties& inSession, const DWORD inMSTimeout)
{
	if(m_Dispose == false)
		Dispose();

	::ResetEvent(m_hCanselEvent);

	// タイムアウト設定(最大20秒)
	const DWORD   TIMEOUT = inMSTimeout <= 20 * 1000 ? inMSTimeout : 20 * 1000;

	// 通信ソケット生成
	m_ComSock = ::socket(AF_INET, SOCK_STREAM, 0);
	if(m_ComSock == INVALID_SOCKET) {
		::OutputDebugString(TEXT("*** Error - 通信ソケット生成失敗(CNetworkSession_Join)\n"));
		return false;
	}

	// 接続先設定
	sockaddr_in   server;
	server.sin_family           = AF_INET;
	server.sin_port             = ::htons(inSession.Port);
	server.sin_addr.S_un.S_addr = inSession.IP;

	// 接続イベント設定
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_CONNECT);

	// 接続
	BOOL   error = ::connect(m_ComSock, (sockaddr*)&server, sizeof(server));
	if(::WSAGetLastError() == WSAEWOULDBLOCK) {
		// 接続完了またはイベント待ち
		switch(::WSAWaitForMultipleEvents(2, hEvents, FALSE, TIMEOUT, FALSE)) {
		  // 正常終了
		  case WSA_WAIT_EVENT_0:
			error = 0;
			break;

		  // キャンセル
		  case WSA_WAIT_EVENT_0 + 1:
			break;

		  // エラー
		  default:
			// 接続先アドレス設定
			server.sin_addr.S_un.S_addr = ::inet_addr(inSession.IPAdress);
			if(server.sin_addr.S_un.S_addr == INADDR_NONE) {
				// ホスト名からIP取得
				hostent*   pHost = ::gethostbyname(inSession.HostName);
				if(pHost == NULL)
					break;
				// IP設定
				::CopyMemory(&server.sin_addr.S_un.S_addr, pHost->h_addr_list[0], 4);
			}

			// 再接続
			::WSAResetEvent(hEvents[0]);

			error = ::connect(m_ComSock, (sockaddr*)&server, sizeof(server));
			if(::WSAGetLastError() != WSAEWOULDBLOCK)
				break;

			// 接続完了またはイベント待ち
			if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, TIMEOUT, FALSE) == WSA_WAIT_EVENT_0)
				error = FALSE;

			break;
		}
	}

	// イベント解放
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	if(error != FALSE) {
		::OutputDebugString(TEXT("*** Error - 接続失敗(CNetworkSession_Join)\n"));
		Dispose();
		return false;
	}

	// 認証キー送信
	::send(m_ComSock, SECURE_KEY, ::strlen(SECURE_KEY), 0);

	m_Dispose = false;

	return true;
}

//------------------------------------------------------------------------------
//	データ送信
//------------------------------------------------------------------------------
int CNetworkSession::SendData(const void* pData, const int inBytes, const DWORD inMSTimeout)
{
	if(m_Dispose)
		return 0;

	::ResetEvent(m_hCanselEvent);

	if(m_ComSock == NULL) {
		::OutputDebugString(TEXT("*** Error - 通信ソケット未生成(CNetworkSession_SendDate)\n"));
		return 0;
	}

	// 接続イベント設定
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_WRITE | FD_CLOSE);

	// 送信ループ
	char*   buf       = (char*)pData;
	int     send_byte = 0;
	while(send_byte < inBytes) {
		// 送信
		const int   RET = ::send(m_ComSock, buf + send_byte, inBytes - send_byte, 0);
		if(RET != SOCKET_ERROR) {
			send_byte += RET;
			continue;
		}

		// エラー処理
		if(::WSAGetLastError() != WSAEWOULDBLOCK) {
			::OutputDebugString(TEXT("*** Error - 送信失敗(CNetworkSession_SendData)\n"));
			break;
		}

		// イベント待機
		if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, inMSTimeout, FALSE) != WSA_WAIT_EVENT_0) {
			::OutputDebugString(TEXT("*** Error - 送信失敗(CNetworkSession_SendData)\n"));
			break;
		}

		// イベント処理
		WSANETWORKEVENTS   net_event;
		if(::WSAEnumNetworkEvents(m_ComSock, hEvents[0], &net_event) != 0) {
			::OutputDebugString(TEXT("*** Error - イベント処理失敗(CNetworkSession_SendData)\n"));
			break;
		}
		if(net_event.lNetworkEvents & FD_WRITE)
			continue;
		if(net_event.lNetworkEvents & FD_CLOSE) {
			::OutputDebugString(TEXT("*** Error - セッション切断(CNetworkSession_SendData)\n"));
			break;
		}

		// イベントシグナル初期化
		::WSAResetEvent(hEvents[0]);
	}

	// イベント解放
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	return send_byte;
}

//------------------------------------------------------------------------------
//	データ受信
//------------------------------------------------------------------------------
int CNetworkSession::ReceiveData(void* pBuffer, const int inBytes, const DWORD inMSTimeout)
{
	if(m_Dispose)
		return 0;

	::ResetEvent(m_hCanselEvent);

	if(m_ComSock == NULL) {
		::OutputDebugString(TEXT("*** Error - 通信ソケット未生成(CNetworkSession_ReceiveData)\n"));
		return 0;
	}

	// 接続イベント設定
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_READ | FD_CLOSE);

	// 受信ループ
	char*   buf       = (char*)pBuffer;
	int     recv_byte = 0;
	while(recv_byte < inBytes) {
		// 受信
		const int   RET = ::recv(m_ComSock, buf + recv_byte, inBytes - recv_byte, 0);
		if(RET != SOCKET_ERROR) {
			recv_byte += RET;
			continue;
		}

		// エラー処理
		if(::WSAGetLastError() != WSAEWOULDBLOCK) {
			::OutputDebugString(TEXT("*** Error - 受信失敗(CNetworkSession_ReceiveData)\n"));
			break;
		}

		// イベント待機
		if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, inMSTimeout, FALSE) != WSA_WAIT_EVENT_0) {
			::OutputDebugString(TEXT("*** Error - 受信失敗(CNetworkSession_ReceiveData)\n"));
			break;
		}

		// イベント取得
		WSANETWORKEVENTS   net_event;
		if(::WSAEnumNetworkEvents(m_ComSock, hEvents[0], &net_event) != 0) {
			::OutputDebugString(TEXT("*** Error - イベント取得失敗(CNetworkSession_ReceiveData)\n"));
			break;
		}
		if(net_event.lNetworkEvents & FD_READ )
			continue;
		if(net_event.lNetworkEvents & FD_CLOSE) {
			::OutputDebugString(TEXT("*** Error - セッション切断(CNetworkSession_ReceiveData)\n"));
			break;
		}

		// イベントシグナル初期化
		::WSAResetEvent(hEvents[0]);
	}

	// イベント解放
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	return recv_byte;
}

//------------------------------------------------------------------------------
//	ローカルネットワークセッション検索
//------------------------------------------------------------------------------
int CNetworkSession::Find(AvailableLocalNetworkSessions& outSessions,
						  const u_short inPort, const DWORD inMSTimeout)
{
	::ResetEvent(m_hCanselEvent);

	outSessions.clear();

	// LAN端末検索
	LPSERVER_INFO_101   pTermInfo = NULL;
	DWORD               entries, total, resume = 0;
	::NetServerEnum(NULL, 101, (LPBYTE*)&pTermInfo,
					MAX_PREFERRED_LENGTH, &entries, &total,
					SV_TYPE_WORKSTATION | SV_TYPE_NT | SV_TYPE_WINDOWS,
					NULL, &resume);

	// 変数設定
	sockaddr_in   terminal;
	terminal.sin_family   = AF_INET;
	terminal.sin_port     = ::htons(inPort);

	// タイムアウト設定(最大20秒)
	const DWORD   TIMEOUT = inMSTimeout <= 20 * 1000 ? inMSTimeout : 20 * 1000;

	// 確認キー設定
	const char*   KEY     = "ESNW_LAE_HTS";

	// 接続イベント設定
	WSAEVENT   hEvents[2];
	hEvents[0] = ::WSACreateEvent();
	hEvents[1] = m_hCanselEvent;

	::WSAEventSelect(m_ComSock, hEvents[0], FD_CONNECT);

	// 端末確認
	for(DWORD i = 0; i < entries; i++) {
		// ワイド文字列をマルチバイト文字列に変換
		char   name[63 + 1];
		::WideCharToMultiByte(CP_ACP, 0, pTermInfo[i].sv101_name, -1, name, 64, NULL, FALSE);

		// 接続先アドレス設定
		hostent*   pHost = ::gethostbyname(name);
		if(pHost == NULL)
			continue;
		::CopyMemory(&terminal.sin_addr.S_un.S_addr, pHost->h_addr_list[0], 4);

		// 指定ポートに接続
		SOCKET   sock;
		sock = ::connect(m_ComSock, (sockaddr*)&terminal, sizeof(terminal));
		if(::WSAGetLastError() == WSAEWOULDBLOCK) {
			// 接続完了またはキャンセル待ち
			if(::WSAWaitForMultipleEvents(2, hEvents, FALSE, TIMEOUT, FALSE) == WSA_WAIT_EVENT_0 + 1)
				break;	// キャンセル
		}
		if(sock == INVALID_SOCKET)
			continue;

		// 確認キー送信
		::send(sock, KEY, ::strlen(KEY), 0);

		// セッション切断
		::shutdown(sock, SD_BOTH);
		::closesocket(sock);

		// リストへ登録
		NetworkSessionProperties   session;
		session.IP   = terminal.sin_addr.S_un.S_addr;
		session.Port = inPort;
		outSessions.push_back(session);
	}

	// イベント解放
	::WSAEventSelect(m_ComSock, hEvents[0], 0);
	::WSACloseEvent(hEvents[0]);

	// ネットワーク管理関数メモリ解放
	::NetApiBufferFree(pTermInfo);

	return outSessions.size();
}

//------------------------------------------------------------------------------
//	ネットワークセッション終了
//------------------------------------------------------------------------------
void CNetworkSession::Dispose()
{
	m_Dispose = true;

	// 待機キャンセル
	if(m_hCanselEvent != NULL)
		::SetEvent(m_hCanselEvent);

	// 通信ソケットクローズ
	if(m_ComSock != INVALID_SOCKET) {
		// ポート解放のため、半接続にして残データを受信
		::shutdown(m_ComSock, SD_SEND);

		// ブロッキングモードにする
		u_long   cmd = 0;
		::ioctlsocket(m_ComSock, FIONBIO, &cmd);

		// 受信ループ
		char   buf[256];
		while(::recv(m_ComSock, buf, 256, 0) > 0)
			;

		// ソケットクローズ
		::shutdown(m_ComSock, SD_BOTH);
		::closesocket(m_ComSock);
		m_ComSock = INVALID_SOCKET;
	}

	// 接続要求受付ソケットクローズ
	if(m_ListenSock != INVALID_SOCKET) {
		::closesocket(m_ListenSock);
		m_ListenSock = INVALID_SOCKET;
	}
}

#pragma warning(pop)
