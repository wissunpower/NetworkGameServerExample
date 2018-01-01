
#include	"ServerAppRoot.h"



DWORD WINAPI ProcessClient( LPVOID arg )
{
	char					buf[ BUFSIZE + 1 ];
	unsigned int			bufIndex;
	SOCKADDR_IN				clientaddr;
	int						addrlen;
	int						retval;
	CNetMessage				tempMessage;
	CNetMessage				tempMessage2;
	Matchless::CClient		currentClient;
	std::map< unsigned int, Matchless::CClient >::iterator	cIt;
	const unsigned int		currentID = GetClientID();


	// Get client information
	currentClient.m_NetSystem.SetSocket( (SOCKET)arg );
	addrlen = sizeof( clientaddr );
	getpeername( currentClient.m_NetSystem.GetSocket(), (SOCKADDR*)&clientaddr, &addrlen );


	printf( "[ Connect client ] : IP address = %s, port number = %d\n", inet_ntoa( clientaddr.sin_addr ), ntohs( clientaddr.sin_port ) );

	// Initialize server<->client connection
	currentClient.m_NetSystem.SetID( currentID );
	if( 0 == currentID )				// failed case
	{
		SendDataFSV( currentClient.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_LOGIN_FAILED, 0, NULL );
		printf( "[ Error ] : Failed get client ID number\n" );
		closesocket( currentClient.m_NetSystem.GetSocket() );
		return 0;
	}
	else														// succeed case
	{
		currentClient.m_PlayerInfo.SetTeamNum( 1 );
		currentClient.m_PlayerInfo.SetbRoomMaster( DoNeedRoomMaster() );
		currentClient.m_PlayerInfo.GetCharacterInfo().SetClass( Matchless::ECC_Breaker );

		unsigned int				tempID = currentID;
		unsigned short int			tempTeamNo = currentClient.m_PlayerInfo.GetTeamNum();
		bool						tempbRoomMaster = currentClient.m_PlayerInfo.IsRoomMaster();
		Matchless::ECharacterClass	tempCC = currentClient.m_PlayerInfo.GetCharacterInfo().GetClass();

		memcpy( buf, &tempID, sizeof( tempID ) );									bufIndex = sizeof( tempID );
		memcpy( buf + bufIndex, &tempTeamNo, sizeof( tempTeamNo ) );				bufIndex += sizeof( tempTeamNo );
		memcpy( buf + bufIndex, &tempbRoomMaster, sizeof( tempbRoomMaster ) );		bufIndex += sizeof( tempbRoomMaster );
		memcpy( buf + bufIndex, &tempCC, sizeof( tempCC ) );						bufIndex += sizeof( tempCC );

		EnterCriticalSection( &g_CS );
		for( std::map< unsigned int, Matchless::CClient >::iterator cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
		{
			SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER,
									bufIndex,  buf  );
		}
		g_ClientList.insert( std::map< unsigned int, Matchless::CClient >::value_type( currentID, currentClient ) );
		LeaveCriticalSection( &g_CS );

		SendDataFSV(  currentClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_LOGIN_SUCCEED,
								bufIndex,  buf );

		SendDataFSV(  currentClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_MAP_UPDATE,
								sizeof( g_CurrentMapKind ),  (char*)&g_CurrentMapKind  );

		ChangeTeamPlayerNum( 0, 1 );
	}


	// Send another client information to current client.
	EnterCriticalSection( &g_CS );
	for( std::map< unsigned int, Matchless::CClient >::iterator cIter = g_ClientList.begin()  ;
			cIter != g_ClientList.end()  ;  ++cIter )
	{
		if( cIter->second.m_NetSystem.GetID() == currentID )
		{
			continue;
		}

		unsigned int				tempID = cIter->second.m_NetSystem.GetID();
		unsigned short int			tempTeamNo = cIter->second.m_PlayerInfo.GetTeamNum();
		bool						tempbRoomMaster = cIter->second.m_PlayerInfo.IsRoomMaster();
		Matchless::ECharacterClass	tempCC = cIter->second.m_PlayerInfo.GetCharacterInfo().GetClass();

		memcpy( buf, &tempID, sizeof( tempID ) );									bufIndex = sizeof( tempID );
		memcpy( buf + bufIndex, &tempTeamNo, sizeof( tempTeamNo ) );				bufIndex += sizeof( tempTeamNo );
		memcpy( buf + bufIndex, &tempbRoomMaster, sizeof( tempbRoomMaster ) );		bufIndex += sizeof( tempbRoomMaster );
		memcpy( buf + bufIndex, &tempCC, sizeof( tempCC ) );						bufIndex += sizeof( tempCC );

		SendDataFSV(  currentClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER,
								bufIndex,  buf  );
	}
	LeaveCriticalSection( &g_CS );


	Matchless::EMainStepState	tempMSS;
	Matchless::ECharacterClass	tempCC;
	Matchless::SMatrix4			tempMatrix;
	Matchless::ECharacterSkill	tempECS = Matchless::ECS_Idle;
	unsigned int				tempID;
	unsigned short int			tempTN;
	unsigned int				tempTargetID;


	// Handle receive data from client.
	while( true )
	{
		retval = tempMessage.ReceiveData( currentClient.m_NetSystem.GetSocket(), 0 );

		//PrintPacket( tempMessage );

		if( SOCKET_ERROR == retval )
		{
			err_display( TEXT( "tempMessage.ReceiveData()" ) );
			break;
		}
		else if( Matchless::FCTS_LOGOUT_INFORM == (Matchless::ENetMessageType)tempMessage.GetType() )
		{
			break;
		}
		else if( 0 == retval )
		{
			err_display( TEXT( "tempMessage.ReceiveData()" ) );
			break;
		}


		switch( (Matchless::ENetMessageType)tempMessage.GetType() )
		{

		case Matchless::FCTS_CHAT:
			break;

		case Matchless::FCTS_MSS_UPDATE:
			memcpy( &tempMSS, tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );
			//currentClient.m_PlayerInfo.SetMainStepState( tempMSS );

			EnterCriticalSection( &g_CS );
			if( Matchless::EMSS_Wait != currentClient.m_PlayerInfo.GetMainStepState()  &&
				Matchless::EMSS_Wait == tempMSS )							// another step -> wait step change case.
			{
				ChangeTeamPlayerNum( 0, currentClient.m_PlayerInfo.GetTeamNum() );
			}
			else if( Matchless::EMSS_Wait == currentClient.m_PlayerInfo.GetMainStepState()  &&
					Matchless::EMSS_Wait != tempMSS )						// wait step -> another step change case.
			{
				ChangeTeamPlayerNum( currentClient.m_PlayerInfo.GetTeamNum(), 0 );
			}
			g_ClientList[ currentID ].m_PlayerInfo.SetMainStepState( tempMSS );
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_CHARCLASS_UPDATE:
			tempID = currentID;
			// for server update
			memcpy( &tempCC, tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );
			currentClient.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
			// for reflect send
			memcpy( buf, (void*)(&tempID), sizeof( tempID ) );										bufIndex = sizeof( tempID );
			memcpy( buf + bufIndex, tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );		bufIndex += tempMessage.GetAddDataLen();
			EnterCriticalSection( &g_CS );
			g_ClientList[ currentID ].m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
			for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
			{
				if( cIt->first != currentID )
				{
					SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_CHARCLASS_UPDATE,
											bufIndex,  buf );
				}
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_TEAM_UPDATE:
			tempID = currentID;
			// for server update
			memcpy( &tempTN, tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );
			currentClient.m_PlayerInfo.SetTeamNum( tempTN );
			// for reflect send
			memcpy( buf, (void*)(&tempID), sizeof( tempID ) );										bufIndex = sizeof( tempID );
			memcpy( buf + bufIndex, tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );		bufIndex += tempMessage.GetAddDataLen();
			EnterCriticalSection( &g_CS );
			g_ClientList[ currentID ].m_PlayerInfo.SetTeamNum( tempTN );
			for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
			{
				if( cIt->first != currentID )
				{
					SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_TEAM_UPDATE,
											bufIndex,  buf );
				}
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_MAP_UPDATE:
			EnterCriticalSection( &g_CS );
			memcpy( &g_CurrentMapKind, tempMessage.GetpAddData(), sizeof( g_CurrentMapKind ) );
			for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
			{
				SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_MAP_UPDATE,
										tempMessage.GetAddDataLen(),  tempMessage.GetpAddData()  );
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_GAMESTART_REQUEST:
			EnterCriticalSection( &g_CS );
			if( g_IsGameStartable )
			{
				std::map< unsigned short int, int >	tempTeamPlayerNum;
				int						tempNum;
				Matchless::SMatrix4		tempMtx;

				g_IsAcceptable = false;

				// Set buf data
				Matchless::InitializeCharacterInfo( currentClient.m_PlayerInfo.GetCharacterInfo() );
				bufIndex = 0;
				for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
				{
					tempMtx.Reset();

					tempNum = tempTeamPlayerNum[ cIt->second.m_PlayerInfo.GetTeamNum() ]++;
					tempMtx._41 = (float)((tempNum % 3) * 300 - ((tempNum % 3 == 2 ) ? 600 : 0));
					tempMtx._43 = (float)(((tempNum % 3) ? 1500 : 1200) * ((cIt->second.m_PlayerInfo.GetTeamNum() % 2) ? 1 : -1));

					Matchless::InitializeCharacterInfo( cIt->second.m_PlayerInfo.GetCharacterInfo() );
					cIt->second.m_PlayerInfo.SetTransform( tempMtx );
					//memcpy( buf + bufIndex, &(cIt->second), sizeof( cIt->second ) );	bufIndex += sizeof( cIt->second );
				}
				// Send buf data each client
				for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
				{
					for( std::map< unsigned int, Matchless::CClient >::iterator cIter = g_ClientList.begin()  ;
						 cIter != g_ClientList.end()  ;  ++cIter )
					{
						SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_INFORM_CLIENTINFO,
										sizeof( cIter->second ),  (char*)&(cIter->second)  );
					}

					SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAMESTART_SUCCEED,
									0,  NULL  );
				}
			}
			else
			{
				SendDataFSV( currentClient.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMESTART_FAILED, 0, NULL );
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_GAME_MOVE_POSITION:
			tempID = currentID;
			// for server update
			memcpy( &tempMatrix, tempMessage.GetpAddData(), sizeof( tempMatrix ) );
			currentClient.m_PlayerInfo.SetTransform( tempMatrix );
			// for reflect send
			memcpy( buf, (void*)(&tempID), sizeof( tempID ) );
			memcpy( buf + sizeof( tempID ), tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );
			EnterCriticalSection( &g_CS );
			if( 0 >= g_ClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
			{
				LeaveCriticalSection( &g_CS );
				break;
			}
			if( IsNowCasting( tempID, true ) )
			{
				SendDataFSV(  g_ClientList[ tempID ].m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_CANCEL,
										0,  NULL  );
			}
			g_ClientList[ tempID ].m_PlayerInfo.SetTransform( tempMatrix );
			for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
			{
				if( cIt->first != currentID )
				{
					SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_MOVE_POSITION,
											sizeof( tempID ) + tempMessage.GetAddDataLen(),  buf );
				}
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_GAME_MOVE_ANIMATION:
			tempID = currentID;
			// for reflect send
			memcpy( buf, (void*)(&tempID), sizeof( tempID ) );
			memcpy( buf + sizeof( tempID ), tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );
			EnterCriticalSection( &g_CS );
			if( 0 >= g_ClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
			{
				LeaveCriticalSection( &g_CS );
				break;
			}
			for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
			{
				if( cIt->first != currentID )
				{
					SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_MOVE_ANIMATION,
											sizeof( tempID ) + tempMessage.GetAddDataLen(),  buf );
				}
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_GAME_MOVE_ALL_REQUEST:
			EnterCriticalSection( &g_CS );
			for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
			{
				if( cIt->first != currentID )
				{
					memcpy( buf, (void*)(&cIt->first), sizeof( cIt->first ) );
					memcpy( buf + sizeof( cIt->first ), &(cIt->second.m_PlayerInfo.GetTransform()), sizeof( cIt->second.m_PlayerInfo.GetTransform() ) );
					SendDataFSV(  currentClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_MOVE_POSITION,
											sizeof( cIt->first ) + sizeof( cIt->second.m_PlayerInfo.GetTransform() ),  buf );
				}
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_GAME_SKILL_REQUEST:
			memcpy( &tempECS, tempMessage.GetpAddData(), sizeof( tempECS ) );
			memcpy( &tempTargetID, tempMessage.GetpAddData() + sizeof( tempECS ), sizeof( tempTargetID ) );
			EnterCriticalSection( &g_CS );
			if( 0 >= g_ClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
			{
				LeaveCriticalSection( &g_CS );
				break;
			}
			if( g_ClientList.end() != g_ClientList.find( tempTargetID )  &&
				g_ClientList.end() != g_ClientList.find( currentID )  &&
				!IsNowCasting( tempID, false ) )
			{
				HandleSkillRequest( true, tempECS, g_ClientList[ currentID ], g_ClientList[ tempTargetID ] );
			}
			else
			{
				SendDataFSV( g_ClientList[ currentID ].m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
			}
			LeaveCriticalSection( &g_CS );
			break;

		case Matchless::FCTS_GAMEOUT_REQUEST:
			EnterCriticalSection( &g_CS );
			for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
			{
				SendDataFSV( cIt->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
			}
			g_IsAcceptable = true;
			LeaveCriticalSection( &g_CS );
			break;

		}
	}


	// Handle client disconnect.
	EnterCriticalSection( &g_CS );
	g_ClientList.erase( tempID = currentID );
	for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
	{
		if( currentClient.m_PlayerInfo.IsRoomMaster()  &&  !g_IsAcceptable )
		{
			SendDataFSV( cIt->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
		}

		SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_INFORM_ANOTHERCLIENT_LEAVE,
								sizeof( tempID ),  (char*)&tempID  );
	}
	if( currentClient.m_PlayerInfo.IsRoomMaster()  &&  !g_IsAcceptable )
	{
		g_IsAcceptable = true;
	}
	ReturnClientID( currentID );
	LeaveCriticalSection( &g_CS );

	printf( "[ Disconnect client ] : IP address = %s, port number = %d\n", inet_ntoa( clientaddr.sin_addr ), ntohs( clientaddr.sin_port ) );

	closesocket( currentClient.m_NetSystem.GetSocket() );


	return 0;
}


int main( int argc, char * argv[] )
{
	CMiniDump::Begin();


	WSADATA		wsa;
	if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &wsa ) )
		return -1;


	InitializeCriticalSection( &g_CS );


	// create timer thread step
	if( NULL == CreateThread( NULL, 0, TimerThread, (LPVOID)&g_Timer, 0, NULL ) )
	{
		printf( "[ Error ] : Failed create thread!\n" );
	}

	// create game process thread step
	if( NULL == CreateThread( NULL, 0, GameProcessThread, NULL, 0, NULL ) )
	{
		printf( "[ Error ] : Failed create thread!\n" );
	}


	// create socket
	SOCKET	listenSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( INVALID_SOCKET == listenSocket )
	{
		WSACleanup();
		err_quit( TEXT( "socket()" ) );
	}


	// bind step
	SOCKADDR_IN		serveraddr;
	ZeroMemory( &serveraddr, sizeof( serveraddr ) );
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons( MATCHLESS_SERVER_PORT );
	serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );
	if( SOCKET_ERROR == bind( listenSocket, (SOCKADDR*)&serveraddr, sizeof( serveraddr ) ) )
	{
		closesocket( listenSocket );
		WSACleanup();
		err_quit( TEXT( "bind()" ) );
	}


	// listne step
	if( SOCKET_ERROR == listen( listenSocket, SOMAXCONN ) )
	{
		closesocket( listenSocket );
		WSACleanup();
		err_quit( TEXT( "listen()" ) );
	}


	OutputServerInitialInfo( serveraddr, listenSocket );

	Matchless::SetSampleCharacterInfo();
	Matchless::SetSampleSkillInfo();


	SOCKET			clientSocket;
	SOCKADDR_IN		clientaddr;
	int				addrlen;
	DWORD			ThreadID;
	CNetMessage		tempMessage;

	while( true )
	{
		// accept step
		addrlen = sizeof( clientaddr );
		clientSocket = accept( listenSocket, (SOCKADDR*)&clientaddr, &addrlen );
		if( INVALID_SOCKET == clientSocket )
		{
			err_display( TEXT( "accept()" ) );
			continue;
		}

		EnterCriticalSection( &g_CS );
		if( !g_IsAcceptable )			// Already start the game.
		{
			LeaveCriticalSection( &g_CS );
			SendDataFSV( clientSocket, 0, (unsigned int)Matchless::FSTC_LOGIN_FAILED, 0, NULL );
			printf( "[ Error ] : Already start the game.\n" );
			closesocket( clientSocket );
			continue;
		}
		LeaveCriticalSection( &g_CS );

		// create thread step
		if( NULL == CreateThread( NULL, 0, ProcessClient, (LPVOID)clientSocket, 0, &ThreadID ) )
		{
			printf( "[ Error ] : Failed create thread!\n" );
			closesocket( clientSocket );
		}
	}


	closesocket( listenSocket );


	DeleteCriticalSection( &g_CS );


	WSACleanup();


	CMiniDump::End();


	return 0;
}
