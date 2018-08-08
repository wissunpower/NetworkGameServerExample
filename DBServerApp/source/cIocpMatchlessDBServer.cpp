
#include	"stdafx.h"
#include	"cIocpMatchlessDBServer.h"
#include	"cSingleton.h"
#include	"cLog.h"
#include	"cServerConnectionManager.h"
#include	"LogUtil.h"
#include	"MatchlessProtocol.h"
#include	"cPacket.h"
#include	"cNetMessageHandlerManager.h"


bool cIocpMatchlessDBServer::OnAccept( const std::shared_ptr< cConnection >& lpConnection )
{
	if ( nullptr == lpConnection )
	{
		WriteLog( tstring{ _T( "[ Error ] : Invalied Connection Request in OnAccept()" ) } );
	}

	cSingleton< cServerConnectionManager >::Get()->AddConnection( lpConnection );
	LOG( eLogInfoType::LOG_INFO_LOW, tstring{ _T( "SYSTEM | cIocpMatchlessDBServer::OnAccept() | IP[ " ) } + lpConnection->GetConnectionIp() + _T( " ] Socket[ " ) + wsp::to( lpConnection->GetSocket() ) + _T( " ] 立加 UserCnt[ " ) + wsp::to( cSingleton< cServerConnectionManager >::Get()->GetConnectionCnt() ) + _T( " ]" ) );

	//ProcessClient_Accept( *lpConnection );

	return true;
}
	
bool cIocpMatchlessDBServer::OnRecv( const std::shared_ptr< cConnection >& lpConnection, DWORD dwSize, char* pRecvedMsg )
{
	return true;
}

bool cIocpMatchlessDBServer::OnRecvImmediately( const std::shared_ptr< cConnection >& lpConnection, DWORD dwSize, char* pRecvedMsg )
{
	if ( nullptr == lpConnection )
	{
		WriteLog( tstring{ _T( "[ Error ] : Invalied Connection Request in OnRecvImmediately()" ) } );
	}

	cIPacket iPacket;
	iPacket.SetBuffer( dwSize, pRecvedMsg );
	iPacket.ResetIndex();

	const Matchless::ENetMessageType nMsgType = static_cast<Matchless::ENetMessageType>( iPacket.Decode4u() );

	cSingleton< cNetMessageHandlerManager >::Get()->OnProcess( nMsgType, *lpConnection, iPacket );

	return true;
}

void cIocpMatchlessDBServer::OnClose( const std::shared_ptr< cConnection >& lpConnection )
{
	if ( nullptr == lpConnection )
	{
		WriteLog( tstring{ _T( "[ Error ] : Invalied Connection Request in OnClose()" ) } );
	}

	cSingleton< cServerConnectionManager >::Get()->RemoveConnection( lpConnection );
	LOG( eLogInfoType::LOG_INFO_LOW, tstring{ _T( "SYSTEM | cIocpMatchlessDBServer::OnClose() | IP[ " ) } + lpConnection->GetConnectionIp() + _T( " ] Socket[ " ) + wsp::to( lpConnection->GetSocket() ) + _T( " ] 立加 辆丰 UserCnt [ " ) + wsp::to( cSingleton< cServerConnectionManager >::Get()->GetConnectionCnt() ) + _T( " ]" ) );
}

bool cIocpMatchlessDBServer::OnSystemMsg( const std::shared_ptr< cConnection >& lpConnection, DWORD dwMsgType, LPARAM lParam )
{
	return true;
}
