
#include "stdafx.h"
#include "cIocpChatServer.h"
#include "cConnectionManager.h"
#include "cSingleton.h"
#include "cLog.h"
#include "../ChatCommon/Protocol.h"


bool cIocpChatServer::OnAccept( const std::shared_ptr< cConnection >& lpConnection )
{
	cSingleton< cConnectionManager >::Get()->AddConnection( lpConnection );
	LOG( eLogInfoType::LOG_INFO_LOW, tstring{ _T( "SYSTEM | cIocpChatServer::OnAccept() | IP[ " ) } + lpConnection->GetConnectionIp() + _T( " ] Socket[ " ) + wsp::to( lpConnection->GetSocket() ) + _T( " ] 접속 UserCnt[ " ) + wsp::to( cSingleton< cConnectionManager >::Get()->GetConnectionCnt() ) + _T( " ]" ) );
	return true;
}

bool cIocpChatServer::OnRecv( const std::shared_ptr< cConnection >& lpConnection, DWORD dwSize, char* pRecvedMsg )
{
	unsigned int usType;
	CopyMemory( &usType, pRecvedMsg + 4, PACKET_SIZE_LENGTH );

	switch( usType )
	{

	case PACKET_CHAT:
		{
			Packet_Chat* pChat = reinterpret_cast<Packet_Chat*>( pRecvedMsg );
			cSingleton< cConnectionManager >::Get()->BroadCast_Chat( pChat->s_szIP, pChat->s_szChatMsg );
			LOG( eLogInfoType::LOG_INFO_LOW, tstring{ _T( "[메세지] IP : " ) } + pChat->s_szIP + _T( ", Msg : " ) + pChat->s_szChatMsg );
		}
		break;

	default:
		LOG( eLogInfoType::LOG_ERROR_NORMAL, tstring{ _T( "SYSTEM | cIocpChatServer::OnRecv() | 정의되지 않은 패킷( " ) } + wsp::to( usType ) + _T( " )" ) );
		break;

	}

	return true;
}

bool cIocpChatServer::OnRecvImmediately( const std::shared_ptr< cConnection >& lpConnection, DWORD dwSize, char* pRecvedMsg )
{
	return false;
}

void cIocpChatServer::OnClose( const std::shared_ptr< cConnection >& lpConnection )
{
	cSingleton< cConnectionManager >::Get()->RemoveConnection( lpConnection );
	LOG( eLogInfoType::LOG_INFO_LOW, tstring{ _T( "SYSTEM | cIocpChatServer::OnClose() | IP[ " ) } + lpConnection->GetConnectionIp() + _T( " ] Socket[ " ) + wsp::to( lpConnection->GetSocket() ) + _T( " ] 종료 UserCnt[ " ) + wsp::to( cSingleton< cConnectionManager >::Get()->GetConnectionCnt() ) + _T( " ]" ) );
}

bool cIocpChatServer::OnSystemMsg( const std::shared_ptr< cConnection >& lpConnection, DWORD dwMsgType, LPARAM lParam )
{
	return true;
}

