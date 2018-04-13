
#include	"stdafx.h"
#include	"ServerAppRoot.h"
#include	"LibraryDef.h"
#include	"cIocpMatchlessServer.h"
#include	"cConnectionManager.h"
#include	"cPacket.h"
#include	"cException.h"



DWORD WINAPI ProcessClient_Accept( LPVOID arg )
{
	cConnection* pConnection = reinterpret_cast<cConnection*>( arg );
	if ( nullptr == pConnection )
	{
		WriteLog( tstring{ _T( "[ Error ] : Invalied Connection Request in ProcessClient_Accept()" ) } );
	}

	SOCKADDR_IN				clientaddr;
	int						addrlen;
	Matchless::CClient		currentClient;
	const unsigned int		currentID = GetClientID();


	// Get client information
	currentClient.m_NetSystem.SetSocket( pConnection->GetSocket() );
	addrlen = sizeof( clientaddr );
	getpeername( currentClient.m_NetSystem.GetSocket(), (SOCKADDR*)&clientaddr, &addrlen );


	WriteLog( tstring{ _T( "[ Connect client ] : IP address = " ) } + pConnection->GetConnectionIp() );

	// Initialize server<->client connection
	currentClient.m_NetSystem.SetID( currentID );
	if( 0 == currentID )				// failed case
	{
		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_LOGIN_FAILED );
		oPacket.Send( currentClient.m_NetSystem.GetSocket() );
		WriteLog( _T( "[ Error ] : Failed get client ID number" ), { eLogInfoType::LOG_ERROR_HIGH } );
		closesocket( currentClient.m_NetSystem.GetSocket() );
		return 0;
	}
	else														// succeed case
	{
		currentClient.m_PlayerInfo.SetTeamNum( 1 );
		currentClient.m_PlayerInfo.SetbRoomMaster( DoNeedRoomMaster() );
		currentClient.m_PlayerInfo.GetCharacterInfo().SetClass( Matchless::ECC_Breaker );

		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER );
		oPacket.Encode4u( currentID );
		oPacket.Encode4u( currentClient.m_PlayerInfo.GetTeamNum() );
		oPacket.EncodeBool( currentClient.m_PlayerInfo.IsRoomMaster() );
		oPacket.Encode4u( currentClient.m_PlayerInfo.GetCharacterInfo().GetClass() );
		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
			{
				oPacket.Send( cIter->second.m_NetSystem.GetSocket() );
			}
			g_ClientList.insert( std::map< unsigned int, Matchless::CClient >::value_type( currentID, currentClient ) );
		}

		{
			cMonitor::Owner lock { g_csClientID };
			g_mClientID.emplace( currentClient.m_NetSystem.GetSocket(), currentID );
		}

		oPacket = cOPacket{};
		oPacket.Encode4u( Matchless::FSTC_LOGIN_SUCCEED );
		oPacket.Encode4u( currentID );
		oPacket.Encode4u( currentClient.m_PlayerInfo.GetTeamNum() );
		oPacket.EncodeBool( currentClient.m_PlayerInfo.IsRoomMaster() );
		oPacket.Encode4u( currentClient.m_PlayerInfo.GetCharacterInfo().GetClass() );
		oPacket.Send( currentClient.m_NetSystem.GetSocket() );

		oPacket = cOPacket{};
		oPacket.Encode4u( Matchless::FSTC_MAP_UPDATE );
		oPacket.Encode4u( g_CurrentMapKind );
		oPacket.Send( currentClient.m_NetSystem.GetSocket() );

		ChangeTeamPlayerNum( 0, 1 );
	}


	// Send another client information to current client.
	{
		cMonitor::Owner lock{ g_ClientListMonitor };
		for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
		{
			if ( cIter->second.m_NetSystem.GetID() == currentID )
			{
				continue;
			}

			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER );
			oPacket.Encode4u( cIter->second.m_NetSystem.GetID() );
			oPacket.Encode4u( cIter->second.m_PlayerInfo.GetTeamNum() );
			oPacket.EncodeBool( cIter->second.m_PlayerInfo.IsRoomMaster() );
			oPacket.Encode4u( cIter->second.m_PlayerInfo.GetCharacterInfo().GetClass() );

			oPacket.Send( currentClient.m_NetSystem.GetSocket() );
		}
	}

	return 0;
}

DWORD WINAPI ProcessClient_Recv( const cConnection& connection, cIPacket& iPacket )
try {
	Matchless::EMainStepState	tempMSS;
	Matchless::ECharacterClass	tempCC;
	Matchless::SMatrix4			tempMatrix;
	Matchless::ECharacterSkill	tempECS = Matchless::ECS_Idle;
	unsigned int				tempID = 0;
	unsigned short int			tempTN;
	unsigned int				tempTargetID;

	Matchless::CClient*		pClient { nullptr };

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
			pClient = &it->second;
		}
	}

	if ( nullptr == pClient )
	{
		return 0;
	}


	const Matchless::ENetMessageType nMsgType = static_cast<Matchless::ENetMessageType>( iPacket.Decode4u() );

	switch ( nMsgType )
	{

	case Matchless::FCTS_CHAT:
		break;

	case Matchless::FCTS_MSS_UPDATE:
		tempMSS = static_cast<Matchless::EMainStepState>( iPacket.Decode4u() );
		//currentClient.m_PlayerInfo.SetMainStepState( tempMSS );

		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			if ( Matchless::EMSS_Wait != pClient->m_PlayerInfo.GetMainStepState() &&
				Matchless::EMSS_Wait == tempMSS )							// another step -> wait step change case.
			{
				ChangeTeamPlayerNum( 0, pClient->m_PlayerInfo.GetTeamNum() );
			}
			else if ( Matchless::EMSS_Wait == pClient->m_PlayerInfo.GetMainStepState() &&
				Matchless::EMSS_Wait != tempMSS )						// wait step -> another step change case.
			{
				ChangeTeamPlayerNum( pClient->m_PlayerInfo.GetTeamNum(), 0 );
			}
			pClient->m_PlayerInfo.SetMainStepState( tempMSS );
		}
		break;

	case Matchless::FCTS_CHARCLASS_UPDATE:
		{
			// for server update
			tempCC = static_cast<Matchless::ECharacterClass>( iPacket.Decode4u() );
			pClient->m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );

			// for reflect send
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_CHARCLASS_UPDATE );
			oPacket.Encode4u( tempID );
			oPacket.Encode4u( tempCC );
			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				pClient->m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
				for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
				{
					if ( cIt->first != tempID )
					{
						oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
					}
				}
			}
		}
		break;

	case Matchless::FCTS_TEAM_UPDATE:
		{
			// for server update
			tempTN = static_cast<unsigned short>( iPacket.Decode4u() );
			pClient->m_PlayerInfo.SetTeamNum( tempTN );

			// for reflect send
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_TEAM_UPDATE );
			oPacket.Encode4u( tempID );
			oPacket.Encode4u( tempTN );
			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				pClient->m_PlayerInfo.SetTeamNum( tempTN );
				for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
				{
					if ( cIt->first != tempID )
					{
						oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
					}
				}
			}
		}
		break;

	case Matchless::FCTS_MAP_UPDATE:
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_MAP_UPDATE );

			cMonitor::Owner lock{ g_ClientListMonitor };
			g_CurrentMapKind = iPacket.Decode4u();

			oPacket.Encode4u( g_CurrentMapKind );

			for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
			{
				oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
			}
		}
		break;

	case Matchless::FCTS_GAMESTART_REQUEST:
		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			if ( g_IsGameStartable )
			{
				std::map< unsigned short int, int >	tempTeamPlayerNum;
				int						tempNum;
				Matchless::SMatrix4		tempMtx;

				g_IsAcceptable = false;

				// Set buf data
				Matchless::InitializeCharacterInfo( pClient->m_PlayerInfo.GetCharacterInfo() );
				for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
				{
					tempMtx.Reset();

					tempNum = tempTeamPlayerNum[ cIt->second.m_PlayerInfo.GetTeamNum() ]++;
					tempMtx._41 = (float)( ( tempNum % 3 ) * 300 - ( ( tempNum % 3 == 2 ) ? 600 : 0 ) );
					tempMtx._43 = (float)( ( ( tempNum % 3 ) ? 1500 : 1200 ) * ( ( cIt->second.m_PlayerInfo.GetTeamNum() % 2 ) ? 1 : -1 ) );

					Matchless::InitializeCharacterInfo( cIt->second.m_PlayerInfo.GetCharacterInfo() );
					cIt->second.m_PlayerInfo.SetTransform( tempMtx );
					//memcpy( buf + bufIndex, &(cIt->second), sizeof( cIt->second ) );	bufIndex += sizeof( cIt->second );
				}
				// Send buf data each client
				for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
				{
					for ( auto cIter = g_ClientList.begin(); cIter != g_ClientList.end(); ++cIter )
					{
						cOPacket oPacket;
						oPacket.Encode4u( Matchless::FSTC_INFORM_CLIENTINFO );
						Matchless::Encode( oPacket, cIter->second );
						oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
					}

					cOPacket oPacket;
					oPacket.Encode4u( Matchless::FSTC_GAMESTART_SUCCEED );
					oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
				}
			}
			else
			{
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAMESTART_FAILED );
				oPacket.Send( pClient->m_NetSystem.GetSocket() );
			}
		}
		break;

	case Matchless::FCTS_GAME_MOVE_POSITION:
		// for server update
		Matchless::Decode( iPacket, tempMatrix );
		pClient->m_PlayerInfo.SetTransform( tempMatrix );

		// for reflect send
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_MOVE_POSITION );
			oPacket.Encode4u( tempID );
			Matchless::Encode( oPacket, tempMatrix );

			cMonitor::Owner lock{ g_ClientListMonitor };
			if ( 0 >= g_ClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
			{
				break;
			}
			if ( IsNowCasting( tempID, true ) )
			{
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_CANCEL );
				oPacket.Send( g_ClientList[ tempID ].m_NetSystem.GetSocket() );
			}
			g_ClientList[ tempID ].m_PlayerInfo.SetTransform( tempMatrix );
			for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
			{
				if ( cIt->first != tempID )
				{
					oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
				}
			}
		}
		break;

	case Matchless::FCTS_GAME_MOVE_ANIMATION:
		{
			// for reflect send
			cAniTrackInfo aniInfo;
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_MOVE_ANIMATION );
			oPacket.Encode4u( tempID );

			auto nAniCount = iPacket.Decode4u();
			oPacket.Encode4u( nAniCount );
			for ( unsigned int i = 0 ; i < nAniCount ; ++i )
			{
				oPacket.Encode4u( iPacket.Decode4u() );
				Decode( iPacket, aniInfo );
				Encode( oPacket, aniInfo );
			}

			cMonitor::Owner lock{ g_ClientListMonitor };
			if ( 0 >= g_ClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
			{
				break;
			}
			for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
			{
				if ( cIt->first != tempID )
				{
					oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
				}
			}
		}
		break;

	case Matchless::FCTS_GAME_MOVE_ALL_REQUEST:
		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
			{
				if ( cIt->first != tempID )
				{
					cOPacket oPacket;
					oPacket.Encode4u( Matchless::FSTC_GAME_MOVE_POSITION );
					oPacket.Encode4u( cIt->first );
					Matchless::Encode( oPacket, cIt->second.m_PlayerInfo.GetTransform() );
					oPacket.Send( pClient->m_NetSystem.GetSocket() );
				}
			}
		}
		break;

	case Matchless::FCTS_GAME_SKILL_REQUEST:
		tempECS = static_cast<Matchless::ECharacterSkill>( iPacket.Decode4u() );
		tempTargetID = iPacket.Decode4u();
		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			if ( 0 >= g_ClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
			{
				break;
			}
			if ( g_ClientList.end() != g_ClientList.find( tempTargetID ) &&
				g_ClientList.end() != g_ClientList.find( tempID ) &&
				!IsNowCasting( tempID, false ) )
			{
				HandleSkillRequest( true, tempECS, g_ClientList[ tempID ], g_ClientList[ tempTargetID ] );
			}
			else
			{
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
				oPacket.Send( g_ClientList[ tempID ].m_NetSystem.GetSocket() );
			}
		}
		break;

	case Matchless::FCTS_GAMEOUT_REQUEST:
		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
			{
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
				oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
			}
			g_IsAcceptable = true;
		}
		break;

	}


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
					oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
				}

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_INFORM_ANOTHERCLIENT_LEAVE );
				oPacket.Encode4u( tempID );
				oPacket.Send( cIt->second.m_NetSystem.GetSocket() );
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
	TCHAR notice[ 512 ];

#if defined( UNICODE ) | defined( _UNICODE )
	MultiByteToWideChar( CP_ACP, 0, e.what(), -1, notice, 512 );
#else
	_tcsncpy( g_Notice, e.what(), 512 );
#endif

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000003;
}
catch ( const std::runtime_error& e ) {
	TCHAR notice[ 512 ];

#if defined( UNICODE ) | defined( _UNICODE )
	MultiByteToWideChar( CP_ACP, 0, e.what(), -1, notice, 512 );
#else
	_tcsncpy( g_Notice, e.what(), 512 );
#endif

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000002;
}
catch ( const std::exception& e ) {
	TCHAR notice[ 512 ];

#if defined( UNICODE ) | defined( _UNICODE )
	MultiByteToWideChar( CP_ACP, 0, e.what(), -1, notice, 512 );
#else
	_tcsncpy( g_Notice, e.what(), 512 );
#endif

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

	while ( true )
	{
	}


	WSACleanup();

	CloseLog();

	CMiniDump::End();


	return 0;
}
