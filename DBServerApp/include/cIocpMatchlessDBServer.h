#pragma once


#include "cIocpServer.h"


class cIocpMatchlessDBServer : public cIocpServer
{
public:
	bool OnAccept( const std::shared_ptr< cConnection >& lpConnection ) override;
	bool OnRecv( const std::shared_ptr< cConnection >& lpConnection, DWORD dwSize, char* pRecvedMsg ) override;
	bool OnRecvImmediately( const std::shared_ptr< cConnection >& lpConnection, DWORD dwSize, char* pRecvedMsg ) override;
	void OnClose( const std::shared_ptr< cConnection >& lpConnection ) override;
	bool OnSystemMsg( const std::shared_ptr< cConnection >& lpConnection, DWORD dwMsgType, LPARAM lParam ) override;
};
