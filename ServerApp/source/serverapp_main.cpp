
#include	"stdafx.h"
#include	"ServerAppRoot.h"
#include	"LibraryDef.h"
#include	"cIocpMatchlessServer.h"
#include	"cConnectionManager.h"
#include	"cPacket.h"
#include	"cException.h"
#include	"cNetMessageHandlerManager.h"
#include	"cDBServer.h"


DWORD WINAPI ProcessClient_Accept( const cConnection& connection )
{
	SOCKADDR_IN				clientaddr;
	int						addrlen;
	const unsigned int		currentID = GetClientID();


	WriteLog( tstring{ _T( "[ Connect client ] : IP address = " ) } + connection.GetConnectionIp() );

	// Initialize server<->client connection
	if( 0 == currentID )				// failed case
	{
		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_LOGIN_FAILED );
		oPacket.Send( connection.GetSocket() );
		WriteLog( _T( "[ Error ] : Failed get client ID number" ), { eLogInfoType::LOG_ERROR_HIGH } );
		closesocket( connection.GetSocket() );
		return 0;
	}
	else														// succeed case
	{
		std::shared_ptr< MatchlessServer::CClient > pCurrentClient { new MatchlessServer::CClient };

		// Get client information
		pCurrentClient->m_NetSystem.SetSocket( connection.GetSocket() );
		addrlen = sizeof( clientaddr );
		getpeername( pCurrentClient->m_NetSystem.GetSocket(), (SOCKADDR*)&clientaddr, &addrlen );

		pCurrentClient->m_NetSystem.SetID( currentID );

		pCurrentClient->m_PlayerInfo.SetTeamNum( 1 );
		pCurrentClient->m_PlayerInfo.SetbRoomMaster( DoNeedRoomMaster() );
		pCurrentClient->m_PlayerInfo.GetCharacterInfo().SetClass( Matchless::ECC_Breaker );

		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER );
		oPacket.Encode4u( currentID );
		oPacket.Encode4u( pCurrentClient->m_PlayerInfo.GetTeamNum() );
		oPacket.EncodeBool( pCurrentClient->m_PlayerInfo.IsRoomMaster() );
		oPacket.Encode4u( pCurrentClient->m_PlayerInfo.GetCharacterInfo().GetClass() );
		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
			{
				oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
			}
			g_ClientList.insert( std::map< unsigned int, std::shared_ptr< MatchlessServer::CClient > >::value_type( currentID, pCurrentClient ) );
		}

		{
			cMonitor::Owner lock { g_csClientID };
			g_mClientID.emplace( pCurrentClient->m_NetSystem.GetSocket(), currentID );
		}

		oPacket = cOPacket{};
		oPacket.Encode4u( Matchless::FSTC_LOGIN_SUCCEED );
		oPacket.Encode4u( currentID );
		oPacket.Encode4u( pCurrentClient->m_PlayerInfo.GetTeamNum() );
		oPacket.EncodeBool( pCurrentClient->m_PlayerInfo.IsRoomMaster() );
		oPacket.Encode4u( pCurrentClient->m_PlayerInfo.GetCharacterInfo().GetClass() );
		oPacket.Send( pCurrentClient->m_NetSystem.GetSocket() );

		oPacket = cOPacket{};
		oPacket.Encode4u( Matchless::FSTC_MAP_UPDATE );
		oPacket.Encode4u( g_CurrentMapKind );
		oPacket.Send( pCurrentClient->m_NetSystem.GetSocket() );

		ChangeTeamPlayerNum( 0, 1 );
	}


	// Send another client information to current client.
	{
		cMonitor::Owner lock{ g_ClientListMonitor };
		for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
		{
			if ( cIter->second->m_NetSystem.GetID() == currentID )
			{
				continue;
			}

			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER );
			oPacket.Encode4u( cIter->second->m_NetSystem.GetID() );
			oPacket.Encode4u( cIter->second->m_PlayerInfo.GetTeamNum() );
			oPacket.EncodeBool( cIter->second->m_PlayerInfo.IsRoomMaster() );
			oPacket.Encode4u( cIter->second->m_PlayerInfo.GetCharacterInfo().GetClass() );

			oPacket.Send( connection.GetSocket() );
		}
	}

	return 0;
}

DWORD WINAPI ProcessClient_Recv( const cConnection& connection, cIPacket& iPacket )
try {
	unsigned int				tempID = 0;

	std::shared_ptr< MatchlessServer::CClient > pClient { nullptr };

	{
		cMonitor::Owner lock { g_csClientID };
		auto it = g_mClientID.find( connection.GetSocket() );
		if ( it != g_mClientID.end() )
		{
			tempID = it->second;
		}
	}

	if ( 0 != tempID )
	{
		cMonitor::Owner lock { g_ClientListMonitor };
		auto it = g_ClientList.find( tempID );
		if ( it != g_ClientList.end() )
		{
			pClient = it->second;
		}
	}

	if ( nullptr == pClient )
	{
		return 0;
	}


	const Matchless::ENetMessageType nMsgType = static_cast<Matchless::ENetMessageType>( iPacket.Decode4u() );

	cSingleton< cNetMessageHandlerManager >::Get()->OnProcess( nMsgType, *pClient, iPacket );


	// Handle client disconnect.
	if( Matchless::FCTS_LOGOUT_INFORM == nMsgType )
	{
		{
			cMonitor::Owner lock{ g_csClientID };
			g_mClientID.erase( tempID );
		}

		{
			cMonitor::Owner lock{ g_ClientListMonitor };

			g_ClientList.erase( tempID );
			for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
			{
				if ( pClient->m_PlayerInfo.IsRoomMaster() && !g_IsAcceptable )
				{
					cOPacket oPacket;
					oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
					oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
				}

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_INFORM_ANOTHERCLIENT_LEAVE );
				oPacket.Encode4u( tempID );
				oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
			}
			if ( pClient->m_PlayerInfo.IsRoomMaster() && !g_IsAcceptable )
			{
				g_IsAcceptable = true;
			}
			ReturnClientID( tempID );
		}

		WriteLog( tstring{ _T( "[ Disconnect client ] : IP address = " ) } + connection.GetConnectionIp() );
	}

	//closesocket( currentClient.m_NetSystem.GetSocket() );


	return 0;
}
catch ( const cException& e ) {
	tstring notice;

	notice = wsp::to( e.what() );

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000003;
}
catch ( const std::runtime_error& e ) {
	tstring notice;

	notice = wsp::to( e.what() );

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000002;
}
catch ( const std::exception& e ) {
	tstring notice;

	notice = wsp::to( e.what() );

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000001;
}
catch ( ... ) {
	WriteLog( _T( "Unknown Exception" ), { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000000;
}


int main( int argc, char * argv[] )
{
	CMiniDump::Begin();

	InitLog();

	//WSADATA		wsa;
	//if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &wsa ) )
	//	return -1;


	Matchless::SetSampleCharacterInfo();
	Matchless::SetSampleSkillInfo();

	cSingleton< cNetMessageHandlerManager >::Get()->Initialize();


	SYSTEM_INFO si;
	GetSystemInfo( &si );

	INITCONFIG initConfig {};
	initConfig.nServerPort = MATCHLESS_SERVER_PORT;
	initConfig.nRecvBufCnt = 10;
	initConfig.nRecvBufSize = 1024;
	initConfig.nProcessPacketCnt = 100;
	initConfig.nSendBufCnt = 10;
	initConfig.nSendBufSize = 1024;
	initConfig.nWokerThreadCnt = si.dwNumberOfProcessors;
	initConfig.nProcessThreadCnt = 1;

	cIocpMatchlessServer* pServer = cSingleton< cIocpMatchlessServer >::Get();
	pServer->ServerStart( initConfig );

	OutputServerInitialInfo( pServer->GetSockAddr(), pServer->GetListenSocket() );

	cSingleton< cConnectionManager >::Get()->CreateConnection( initConfig, 20 );

	// create timer thread step
	if( NULL == CreateThread( NULL, 0, TimerThread, (LPVOID)&g_Timer, 0, NULL ) )
	{
		WriteLog( _T( "[ Error ] : Failed create thread!" ), { eLogInfoType::LOG_ERROR_HIGH } );
	}

	// create game process thread step
	if( NULL == CreateThread( NULL, 0, GameProcessThread, NULL, 0, NULL ) )
	{
		WriteLog( _T( "[ Error ] : Failed create thread!" ), { eLogInfoType::LOG_ERROR_HIGH } );
	}


	// Try DB Server Connect.
	tstring		sDBServerIP { _T( "127.0.0.1" ) };
	cSingleton< Matchless::cDBServer >::Get()->SetSocket( socket( AF_INET, SOCK_STREAM, 0 ) );
	auto connectRet = cSingleton< Matchless::cDBServer >::Get()->Connect( AF_INET, MATCHLESS_DBSERVER_PORT, sDBServerIP );


	bool bContinue = true;
	while ( bContinue )
	{
	}


	WSACleanup();

	CloseLog();

	CMiniDump::End();


	return 0;
}
