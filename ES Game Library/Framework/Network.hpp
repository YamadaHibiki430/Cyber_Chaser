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
	 Network.hpp
				ネットワーククラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
// #include <winsock2.h>
#include <windows.h>
#include <list>

//------------------------------------------------------------------------------
//	ネットワークセッションプロパティ構造体
//------------------------------------------------------------------------------
struct NetworkSessionProperties {
	union {
		struct {
			BYTE   IP4;			// IPアドレス
			BYTE   IP3;
			BYTE   IP2;
			BYTE   IP1;
		};
		u_long     IP;			// IPアドレス
		char*      IPAdress;	// IPアドレス文字列
		char*      HostName;	// ホスト名
	};
	u_short        Port;		// ポート

	// ヘルパー関数
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
//	ローカルネットワークセッション型定義
//------------------------------------------------------------------------------
typedef std::list<NetworkSessionProperties> AvailableLocalNetworkSessions;

//------------------------------------------------------------------------------
//	ネットワークセッションクラス定義
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

	// クラス変数
	static int     m_WSARef;
	static char*   SECURE_KEY;

	CNetworkSession(const CNetworkSession&);
	CNetworkSession& operator=(const CNetworkSession&);
};
