
#include	"ServerAppRoot.h"

#include	<stdio.h>



CRITICAL_SECTION								g_CS;

MatchlessServer::CTimer							g_Timer( 60 );

std::list< unsigned int >						g_ReuseClientIDlist;
unsigned int									g_LargestClientID = 0;
std::map< unsigned int, Matchless::CClient >	g_ClientList;			// < ID, SOCKET >
std::map< unsigned short int, int >				g_TeamPlayerNumMap;
bool											g_IsGameStartable = false;
bool											g_IsAcceptable = true;
unsigned int									g_CurrentMapKind = 0;

std::map< unsigned int, MatchlessServer::SkillMessageInfo >		g_SkillMessageList;		// < Tick, SkillInfo >



DWORD WINAPI TimerThread( LPVOID arg )
{
	unsigned int	currentTick = 0;
	bool			isWakeUp = false;
	std::map< unsigned int, MatchlessServer::SkillMessageInfo >::iterator	smIter;


	while( true )
	{
		Sleep( 1 );

		EnterCriticalSection( &g_CS );
		g_Timer.Advance();
		currentTick = g_Timer.GetTick();
		isWakeUp = g_Timer.IsWakeUp();

		if( !isWakeUp )
		{
			LeaveCriticalSection( &g_CS );
			continue;
		}

		if( 0 == g_Timer.GetTick() % 300 )
		{
			printf( "Timer Thread Run\n" );
		}

		while( !g_SkillMessageList.empty() )
		{
			smIter = g_SkillMessageList.begin();
			if( currentTick < smIter->first )
			{
				break;
			}

			HandleSkillRequest(  false,  (Matchless::ECharacterSkill)smIter->second.m_SkillKind,
									g_ClientList[ smIter->second.m_Caster ],  g_ClientList[ smIter->second.m_Target ]  );
			g_SkillMessageList.erase( smIter );
		}
		LeaveCriticalSection( &g_CS );
	}


	return	0;
}


DWORD WINAPI GameProcessThread( LPVOID arg )
{
	unsigned int	currentTick = 0;
	bool			isWakeUp = false;

	std::map< unsigned int, Matchless::CClient >::iterator	cIter;
	std::list< Matchless::CState >::iterator				csIter;
	std::list< Matchless::CState >::iterator				currentCSIter;
	CNetMessage					tempMessage;
	char						buf[ BUFSIZE + 1 ];
	unsigned int				bufIndex;
	unsigned int				tempID;
	unsigned int				tempAmount;
	Matchless::ECharDataType	tempCDT = Matchless::ECDT_CurrentEnergy;


	while( true )
	{
		Sleep( 1 );

		EnterCriticalSection( &g_CS );
		currentTick = g_Timer.GetTick();
		isWakeUp = g_Timer.IsWakeUp();
		LeaveCriticalSection( &g_CS );

		if( g_IsAcceptable || !isWakeUp )
		{
			continue;
		}

		if( 0 == g_Timer.GetTick() % 300 )
		{
			printf( "Game Process Thread Run\n" );
		}

		EnterCriticalSection( &g_CS );
		for( cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
		{
			// remove expired effect
			csIter = cIter->second.m_PlayerInfo.GetCharacterInfo().GetStateList().begin();
			while( csIter != cIter->second.m_PlayerInfo.GetCharacterInfo().GetStateList().end() )
			{
				currentCSIter = csIter;
				++csIter;

				if( currentCSIter->GetRemoveTime() <= currentTick )
				{
					tempID = cIter->first;
					tempCDT = Matchless::ECDT_RemoveState;
					tempAmount = (unsigned int)currentCSIter->GetType();

					cIter->second.m_PlayerInfo.GetCharacterInfo().GetStateList().erase( currentCSIter );

					memcpy( buf, &tempID, sizeof( tempID ) );							bufIndex = sizeof( tempID );
					memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
					memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

					for( std::map< unsigned int, Matchless::CClient >::iterator	cIter1 = g_ClientList.begin()  ;
						cIter1 != g_ClientList.end()  ;  ++cIter1 )
					{

						SendDataFSV(  cIter1->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
							bufIndex,  buf  );
					}
				}
			}

			// energy recovery
			if( 0 == currentTick % 60 )
			{
				if( cIter->second.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy() >= cIter->second.m_PlayerInfo.GetCharacterInfo().GetMaxEnergy() )
				{
					continue;
				}

				tempID = cIter->first;
				tempCDT = Matchless::ECDT_CurrentEnergy;
				tempAmount = cIter->second.m_PlayerInfo.GetCharacterInfo().IncreaseCurrentEnergy( 1 );

				memcpy( buf, &tempID, sizeof( tempID ) );							bufIndex = sizeof( tempID );
				memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
				memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter1 = g_ClientList.begin()  ;
					cIter1 != g_ClientList.end()  ;  ++cIter1 )
				{

					SendDataFSV(  cIter1->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						bufIndex,  buf  );
				}
			}
		}
		LeaveCriticalSection( &g_CS );
	}


	return	0;
}




unsigned int GetClientID( void )
{
	unsigned int	returnValue;

	EnterCriticalSection( &g_CS );
	if( !g_ReuseClientIDlist.empty() )
	{
		returnValue = (*g_ReuseClientIDlist.begin());
		g_ReuseClientIDlist.pop_front();
	}
	else
	{
		returnValue = (++g_LargestClientID);
	}
	LeaveCriticalSection( &g_CS );

	return	returnValue;
}


int ReturnClientID( const unsigned int aID )
{
	if( 0 == aID )
	{
		return	(-1);
	}
	else
	{
		EnterCriticalSection( &g_CS );
		g_ReuseClientIDlist.push_back( aID );
		LeaveCriticalSection( &g_CS );
	}

	return 0;
}


int OutputServerInitialInfo( const SOCKADDR_IN & aAddrInfo, const SOCKET aListenSocket )
{
	int	addrlen;
	SOCKADDR_IN	tempAddr_in;
	//IN_ADDR		tempAddr;
	HOSTENT *	thisSystem = gethostbyaddr( (char*)&(aAddrInfo.sin_addr), 4, AF_INET );
	if( NULL == thisSystem )
	{
		printf( "-- Failed get server information.\n" );
		return -1;
	}

	addrlen = sizeof( tempAddr_in );
	if( SOCKET_ERROR == getsockname( aListenSocket, (SOCKADDR*)&tempAddr_in, &addrlen ) )
	{
		printf( "-- Failed get server information.\n" );
		return -1;
	}

	printf( "-- Succeed launch server.\n" );

	printf( "-- Server IP address : %s\n", inet_ntoa( tempAddr_in.sin_addr ) );
	//for( int i = 0 ; thisSystem->h_addr_list[ i ] != 0 ; ++i )
	//{
	//	tempAddr.s_addr = *(u_long*)thisSystem->h_addr_list[ i ];
	//	printf( "\t#%d\t%s\n", i, inet_ntoa( tempAddr ) );
	//}

	printf( "-- Server port number : %d\n", ntohs( aAddrInfo.sin_port ) );

	return 0;
}


bool DoNeedRoomMaster( void )
{
	bool		returnValue = true;
	std::map< unsigned int, Matchless::CClient >::iterator	cIt;

	EnterCriticalSection( &g_CS );
	for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
	{
		if( cIt->second.m_PlayerInfo.IsRoomMaster() )
		{
			returnValue = false;
			break;
		}
	}
	LeaveCriticalSection( &g_CS );

	return	returnValue;
}


// if aBefore is 0, than occur insert player,
// if aAfter is 0, than occur remove player.
int ChangeTeamPlayerNum( const unsigned short int aBefore, const unsigned short int aAfter )
{
	std::map< unsigned int, Matchless::CClient >::iterator	cIt;
	std::map< unsigned short int, int >::iterator			tpIt;
	int				prevNum = 0;
	CNetMessage		tempMessage;


	EnterCriticalSection( &g_CS );
	
	// Update g_TeamPlayerNumMap
	--g_TeamPlayerNumMap[ aBefore ];
	++g_TeamPlayerNumMap[ aAfter ];

	// Update g_IsGameStartable
	g_IsGameStartable = true;
	if( g_TeamPlayerNumMap.empty() )
	{
		g_IsGameStartable = false;
	}
	else
	{
		prevNum = g_TeamPlayerNumMap.begin()->second;
		for( tpIt = g_TeamPlayerNumMap.begin() ; tpIt != g_TeamPlayerNumMap.end() ; ++tpIt )
		{
			if( prevNum != tpIt->second )
			{
				g_IsGameStartable = false;
				break;
			}

			prevNum = tpIt->second;
		}
	}

	// send startable update data to room master client
	for( cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
	{
		if( cIt->second.m_PlayerInfo.IsRoomMaster() )
		{
			break;
		}
	}
	if( g_ClientList.end() != cIt )
	{
		/* temporary code for dev. */g_IsGameStartable = true;
		SendDataFSV(  cIt->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_STARTABLE,
								sizeof( g_IsGameStartable ),  (char*)&g_IsGameStartable  );
	}

	LeaveCriticalSection( &g_CS );


	return 0;
}


bool IsGameFinish( void )
{
	std::map< unsigned int, Matchless::CClient >::iterator	cIter;
	std::map< unsigned short int, int >::iterator			tmIter;
	std::map< unsigned short int, bool >					tempRecord;			// < Team Number, IsAlive >
	std::map< unsigned short int, bool >::iterator			rIter;
	unsigned int											tempAliveTeamCount = 0;


	EnterCriticalSection( &g_CS );
	for( tmIter = g_TeamPlayerNumMap.begin() ; tmIter != g_TeamPlayerNumMap.end() ; ++tmIter )
	{
		tempRecord[ tmIter->first ] = false;
	}


	for( cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
	{
		if( true == tempRecord[ cIter->second.m_PlayerInfo.GetTeamNum() ] )
		{
			continue;
		}

		if( 0 < cIter->second.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
		{
			tempRecord[ cIter->second.m_PlayerInfo.GetTeamNum() ] = true;
		}
	}
	LeaveCriticalSection( &g_CS );


	for( rIter = tempRecord.begin() ; rIter != tempRecord.end() ; ++rIter )
	{
		if( true == rIter->second )
		{
			++tempAliveTeamCount;
		}
	}


	return	( ( 1 == tempAliveTeamCount ) ? true : false );
}


bool HandleSkillRequest( const bool aIsCastStart, const Matchless::ECharacterSkill aSkillKind, Matchless::CClient & aCaster, Matchless::CClient & aTarget )
{
	char						buf[ BUFSIZE + 1 ];
	char						buf1[ BUFSIZE + 1 ];
	unsigned int				bufIndex = 0;
	unsigned int				buf1Index = 0;
	CNetMessage					tempMessage;

	const unsigned int			casterID = aCaster.m_NetSystem.GetID();
	const unsigned int			targetID = aTarget.m_NetSystem.GetID();

	Matchless::ECharDataType	tempCDT;
	unsigned int				tempAmount;

	std::map< unsigned int, MatchlessServer::SkillMessageInfo >::iterator	smIter;
	std::map< unsigned int, MatchlessServer::SkillMessageInfo >::iterator	currentSMIter;


	if( 0 == casterID || 0 == targetID )
	{
		return	false;
	}


	switch( aSkillKind )
	{

	case Matchless::ECS_Breaker_NormalAttack:
	case Matchless::ECS_Defender_NormalAttack:
	case Matchless::ECS_Mage_NormalAttack:
	case Matchless::ECS_Healer_NormalAttack:
		if( Matchless::IsSkillPossible( Matchless::BreakerNormalAttack, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetPhysicalDamage() * (unsigned int)Matchless::BreakerNormalAttack.GetPhysicalDamageScale() );
			const bool		tempIsGameFinish = IsGameFinish();
			unsigned int	tempHealth = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
			memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
			memcpy( buf + bufIndex, &tempHealth, sizeof( tempHealth ) );		bufIndex += sizeof( tempHealth );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					bufIndex,  buf  );
				if( tempIsGameFinish )
				{
					SendDataFSV( cIter->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
				}
			}
			if( tempIsGameFinish )
			{
				g_IsAcceptable = true;
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Breaker_Raid:
		if( Matchless::IsSkillPossible( Matchless::BreakerRaid, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerRaid.GetEnergyCost() );
			aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetPhysicalDamage() * (unsigned int)Matchless::BreakerRaid.GetPhysicalDamageScale() );
			const bool		tempIsGameFinish = IsGameFinish();

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
			memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
			memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
			memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

			tempCDT = Matchless::ECDT_CurrentEnergy;
			tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					bufIndex,  buf  );
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );

				if( tempIsGameFinish )
				{
					SendDataFSV( cIter->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
				}
			}
			if( tempIsGameFinish )
			{
				g_IsAcceptable = true;
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Breaker_Stun:
		if( Matchless::IsSkillPossible( Matchless::BreakerStun, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerStun.GetEnergyCost() );
			aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( (unsigned int)((float)aCaster.m_PlayerInfo.GetCharacterInfo().GetPhysicalDamage() * Matchless::BreakerStun.GetPhysicalDamageScale()) );
			const bool		tempIsGameFinish = IsGameFinish();

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_Faint, g_Timer.GetTick() + g_Timer.GetFPS() * 3, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}

			// cancel target's casting
			SendDataFSV(  aTarget.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_CANCEL, 0, NULL );
			smIter = g_SkillMessageList.begin();
			while( smIter != g_SkillMessageList.end() )
			{
				currentSMIter = smIter;
				++smIter;

				if( targetID == currentSMIter->second.m_Caster )
				{
					g_SkillMessageList.erase( currentSMIter );
				}
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
			memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
			memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
			memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_Faint;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					bufIndex,  buf  );
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );

				if( tempIsGameFinish )
				{
					SendDataFSV( cIter->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
				}
			}
			if( tempIsGameFinish )
			{
				g_IsAcceptable = true;
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Breaker_ArmorBreak:
		if( Matchless::IsSkillPossible( Matchless::BreakerArmorBreak, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerArmorBreak.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyArmDec, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyArmDec;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Breaker_PowerBreak:
		if( Matchless::IsSkillPossible( Matchless::BreakerPowerBreak, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerPowerBreak.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyDamDec, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyDamDec;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Breaker_Silent:
		if( Matchless::IsSkillPossible( Matchless::BreakerSilent, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerSilent.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_Silence, g_Timer.GetTick() + g_Timer.GetFPS() * 8, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_Silence;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Defender_StoneSkin:
		if( Matchless::IsSkillPossible( Matchless::DefenderStoneSkin, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderStoneSkin.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyArmInc, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyArmInc;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Defender_Natural:
		if( Matchless::IsSkillPossible( Matchless::DefenderNatural, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderNatural.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_MagArmInc, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_MagArmInc;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Defender_AttackBreak:
		if( Matchless::IsSkillPossible( Matchless::DefenderAttackBreak, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderAttackBreak.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyDamDec, g_Timer.GetTick() + g_Timer.GetFPS() * 20, 0 );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_MagDamDec, g_Timer.GetTick() + g_Timer.GetFPS() * 20, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyDamDec;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_MagDamDec;
			memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
			memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
			memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );

				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Defender_HeavyBlow:
		if( Matchless::IsSkillPossible( Matchless::DefenderHeavyBlow, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderHeavyBlow.GetEnergyCost() );
			aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetPhysicalDamage() * (unsigned int)Matchless::DefenderHeavyBlow.GetPhysicalDamageScale() );
			const bool		tempIsGameFinish = IsGameFinish();

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
			memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
			memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
			memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

			tempCDT = Matchless::ECDT_CurrentEnergy;
			tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					bufIndex,  buf  );
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );

				if( tempIsGameFinish )
				{
					SendDataFSV( cIter->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
				}
			}
			if( tempIsGameFinish )
			{
				g_IsAcceptable = true;
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Mage_IceBolt:
		if( Matchless::IsSkillPossible( Matchless::MageIceBolt, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			if( aIsCastStart )
			{
				unsigned int						tempCastTime = g_Timer.GetFPS() * Matchless::MageIceBolt.GetCastingTime();
				MatchlessServer::SkillMessageInfo	tempSMInfo;

				tempSMInfo.m_Caster = aCaster.m_NetSystem.GetID();
				tempSMInfo.m_Target = aTarget.m_NetSystem.GetID();
				tempSMInfo.m_SkillKind = (unsigned int)aSkillKind;

				SendDataFSV(  aCaster.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_CASTSTART,
										sizeof( tempCastTime ),  (char*)&tempCastTime  );

				EnterCriticalSection( &g_CS );
				g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
				LeaveCriticalSection( &g_CS );
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::MageIceBolt.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::MageIceBolt.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
				const bool		tempIsGameFinish = IsGameFinish();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
				memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
				memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
						bufIndex,  buf  );
				}
				LeaveCriticalSection( &g_CS );

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
				memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
				memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
				memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
				memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						bufIndex,  buf  );

					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						buf1Index,  buf1  );

					if( tempIsGameFinish )
					{
						SendDataFSV( cIter->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
					}
				}
				if( tempIsGameFinish )
				{
					g_IsAcceptable = true;
				}
				LeaveCriticalSection( &g_CS );
			}
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Mage_FireBall:
		if( Matchless::IsSkillPossible( Matchless::MageFireBall, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			if( aIsCastStart )
			{
				unsigned int						tempCastTime = g_Timer.GetFPS() * Matchless::MageFireBall.GetCastingTime();
				MatchlessServer::SkillMessageInfo	tempSMInfo;

				tempSMInfo.m_Caster = aCaster.m_NetSystem.GetID();
				tempSMInfo.m_Target = aTarget.m_NetSystem.GetID();
				tempSMInfo.m_SkillKind = (unsigned int)aSkillKind;

				SendDataFSV(  aCaster.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_CASTSTART,
										sizeof( tempCastTime ),  (char*)&tempCastTime  );

				EnterCriticalSection( &g_CS );
				g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
				LeaveCriticalSection( &g_CS );
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::MageIceBolt.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::MageFireBall.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
				const bool		tempIsGameFinish = IsGameFinish();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
				memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
				memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
						bufIndex,  buf  );
				}
				LeaveCriticalSection( &g_CS );

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
				memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
				memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
				memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
				memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						bufIndex,  buf  );

					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						buf1Index,  buf1  );

					if( tempIsGameFinish )
					{
						SendDataFSV( cIter->second.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAMEOUT, 0, NULL );
					}
				}
				if( tempIsGameFinish )
				{
					g_IsAcceptable = true;
				}
				LeaveCriticalSection( &g_CS );
			}
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Healer_Perfect:
		if( Matchless::IsSkillPossible( Matchless::HealerPerfect, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::HealerPerfect.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
			memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
			memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

			EnterCriticalSection( &g_CS );
			aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_Perfect, g_Timer.GetTick() + g_Timer.GetFPS() * 8, 0 );

			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
					bufIndex,  buf  );
			}
			LeaveCriticalSection( &g_CS );

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_Perfect;
			memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
			memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
			memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

			EnterCriticalSection( &g_CS );
			for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
				cIter != g_ClientList.end()  ;  ++cIter )
			{
				SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
					buf1Index,  buf1  );
			}
			LeaveCriticalSection( &g_CS );
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Healer_QuickHeal:
		if( Matchless::IsSkillPossible( Matchless::HealerQuickHeal, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			if( aIsCastStart )
			{
				unsigned int						tempCastTime = g_Timer.GetFPS() * Matchless::HealerQuickHeal.GetCastingTime();
				MatchlessServer::SkillMessageInfo	tempSMInfo;

				tempSMInfo.m_Caster = aCaster.m_NetSystem.GetID();
				tempSMInfo.m_Target = aTarget.m_NetSystem.GetID();
				tempSMInfo.m_SkillKind = (unsigned int)aSkillKind;

				SendDataFSV(  aCaster.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_CASTSTART,
										sizeof( tempCastTime ),  (char*)&tempCastTime  );

				EnterCriticalSection( &g_CS );
				g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
				LeaveCriticalSection( &g_CS );
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::HealerQuickHeal.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().IncreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::HealerQuickHeal.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
				memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
				memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
						bufIndex,  buf  );
				}
				LeaveCriticalSection( &g_CS );

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
				memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
				memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
				memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
				memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						bufIndex,  buf  );

					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						buf1Index,  buf1  );
				}
				LeaveCriticalSection( &g_CS );
			}
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	case Matchless::ECS_Healer_NormalHeal:
		if( Matchless::IsSkillPossible( Matchless::HealerNormalHeal, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			if( aIsCastStart )
			{
				unsigned int						tempCastTime = g_Timer.GetFPS() * Matchless::HealerNormalHeal.GetCastingTime();
				MatchlessServer::SkillMessageInfo	tempSMInfo;

				tempSMInfo.m_Caster = aCaster.m_NetSystem.GetID();
				tempSMInfo.m_Target = aTarget.m_NetSystem.GetID();
				tempSMInfo.m_SkillKind = (unsigned int)aSkillKind;

				SendDataFSV(  aCaster.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_CASTSTART,
										sizeof( tempCastTime ),  (char*)&tempCastTime  );

				EnterCriticalSection( &g_CS );
				g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
				LeaveCriticalSection( &g_CS );
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::HealerNormalHeal.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().IncreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::HealerNormalHeal.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				memcpy( buf, &casterID, sizeof( casterID ) );						bufIndex = sizeof( casterID );
				memcpy( buf + bufIndex, &targetID, sizeof( targetID ) );			bufIndex += sizeof( targetID );
				memcpy( buf + bufIndex, &aSkillKind, sizeof( aSkillKind ) );		bufIndex += sizeof( aSkillKind );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_SKILL_APPLY,
						bufIndex,  buf  );
				}
				LeaveCriticalSection( &g_CS );

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				memcpy( buf, &targetID, sizeof( targetID ) );						bufIndex = sizeof( targetID );
				memcpy( buf + bufIndex, &tempCDT, sizeof( tempCDT ) );				bufIndex += sizeof( tempCDT );
				memcpy( buf + bufIndex, &tempAmount, sizeof( tempAmount ) );		bufIndex += sizeof( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				memcpy( buf1, &targetID, sizeof( targetID ) );						buf1Index = sizeof( targetID );
				memcpy( buf1 + buf1Index, &tempCDT, sizeof( tempCDT ) );			buf1Index += sizeof( tempCDT );
				memcpy( buf1 + buf1Index, &tempAmount, sizeof( tempAmount ) );		buf1Index += sizeof( tempAmount );

				EnterCriticalSection( &g_CS );
				for( std::map< unsigned int, Matchless::CClient >::iterator	cIter = g_ClientList.begin()  ;
					cIter != g_ClientList.end()  ;  ++cIter )
				{
					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						bufIndex,  buf  );

					SendDataFSV(  cIter->second.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FSTC_GAME_CHAR_UPDATE,
						buf1Index,  buf1  );
				}
				LeaveCriticalSection( &g_CS );
			}
		}
		else
		{
			SendDataFSV( aCaster.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FSTC_GAME_SKILL_FAILED, 0, NULL );
		}
		break;

	}


	return	true;
}


bool IsNowCasting( const unsigned int aID, const bool aIsCancel )
{
	bool	returnValue = false;
	std::map< unsigned int, MatchlessServer::SkillMessageInfo >::iterator	smIter;


	EnterCriticalSection( &g_CS );
	for( smIter = g_SkillMessageList.begin() ; smIter != g_SkillMessageList.end() ; ++smIter )
	{
		if( aID == smIter->second.m_Caster )
		{
			returnValue = true;
			break;
		}
	}

	if( aIsCancel && returnValue )
	{
		g_SkillMessageList.erase( smIter );
	}
	LeaveCriticalSection( &g_CS );


	return	returnValue;
}


int SendDataFSV( SOCKET socket, int aFlags, unsigned int aType, unsigned int aAddDataLen, const char * const apAddData )
{
	int				rv;
	CNetMessage		tempMessage;


	rv = tempMessage.SendData( socket, aFlags, aType, ( aAddDataLen < BUFSIZE ) ? aAddDataLen : BUFSIZE, apAddData );

	//PrintPacket( tempMessage );

	return	( rv );
}


bool PrintPacket( const CNetMessage & msg )
{
	switch( (Matchless::ENetMessageType)msg.GetType() )
	{

	case Matchless::FSTC_LOGIN_SUCCEED:
		printf( "FSTC_LOGIN_SUCCEED\t" );
		break;

	case Matchless::FSTC_LOGIN_FAILED:
		printf( "FSTC_LOGIN_FAILED\t" );
		break;

	case Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER:
		printf( "FSTC_INFORM_ANOTHERCLIENT_ENTER\t" );
		break;

	case Matchless::FSTC_INFORM_ANOTHERCLIENT_LEAVE:
		printf( "FSTC_INFORM_ANOTHERCLIENT_LEAVE\t" );
		break;

	case Matchless::FCTS_MSS_UPDATE:
		printf( "FCTS_MSS_UPDATE\t" );
		break;

	case Matchless::FCTS_CHARCLASS_UPDATE:
		printf( "FCTS_CHARCLASS_UPDATE\t" );
		break;

	case Matchless::FSTC_CHARCLASS_UPDATE:
		printf( "FSTC_CHARCLASS_UPDATE\t" );
		break;

	case Matchless::FCTS_TEAM_UPDATE:
		printf( "FCTS_TEAM_UPDATE\t" );
		break;

	case Matchless::FSTC_TEAM_UPDATE:
		printf( "FSTC_TEAM_UPDATE\t" );
		break;

	case Matchless::FCTS_MAP_UPDATE:
		printf( "FCTS_MAP_UPDATE\t" );
		break;

	case Matchless::FSTC_MAP_UPDATE:
		printf( "FSTC_MAP_UPDATE\t" );
		break;

	case Matchless::FSTC_STARTABLE:
		printf( "FSTC_STARTABLE\t" );
		break;

	case Matchless::FCTS_GAMESTART_REQUEST:
		printf( "FCTS_GAMESTART_REQUEST\t" );
		break;

	case Matchless::FSTC_GAMESTART_SUCCEED:
		printf( "FSTC_GAMESTART_SUCCEED\t" );
		break;

	case Matchless::FSTC_GAMESTART_FAILED:
		printf( "FSTC_GAMESTART_FAILED\t" );
		break;

	case Matchless::FSTC_INFORM_CLIENTINFO:
		printf( "FSTC_INFORM_CLIENTINFO\t" );
		break;

	case Matchless::FCTS_GAME_MOVE_POSITION:
		printf( "FCTS_GAME_MOVE_POSITION\t" );
		break;

	case Matchless::FSTC_GAME_MOVE_POSITION:
		printf( "FSTC_GAME_MOVE_POSITION\t" );
		break;

	case Matchless::FCTS_GAME_MOVE_ANIMATION:
		printf( "FCTS_GAME_MOVE_ANIMATION\t" );
		break;

	case Matchless::FSTC_GAME_MOVE_ANIMATION:
		printf( "FSTC_GAME_MOVE_ANIMATION\t" );
		break;

	case Matchless::FCTS_GAME_MOVE_ALL_REQUEST:
		printf( "FCTS_GAME_MOVE_ALL_REQUEST\t" );
		break;

	case Matchless::FCTS_GAME_SKILL_REQUEST:
		printf( "FCTS_GAME_SKILL_REQUEST\t" );
		break;

	case Matchless::FSTC_GAME_SKILL_FAILED:
		printf( "FSTC_GAME_SKILL_FAILED\t" );
		break;

	case Matchless::FSTC_GAME_SKILL_CASTSTART:
		printf( "FSTC_GAME_SKILL_CASTSTART\t" );
		break;

	case Matchless::FSTC_GAME_SKILL_APPLY:
		printf( "FSTC_GAME_SKILL_APPLY\t" );
		break;

	case Matchless::FSTC_GAME_SKILL_CANCEL:
		printf( "FSTC_GAME_SKILL_CANCEL\t" );
		break;

	case Matchless::FSTC_GAME_CHAR_UPDATE:
		printf( "FSTC_GAME_CHAR_UPDATE\t" );
		break;

	case Matchless::FCTS_CHAT:
		printf( "FCTS_CHAT\t" );
		break;

	case Matchless::FSTC_CHAT:
		printf( "FSTC_CHAT\t" );
		break;

	case Matchless::FCTS_GAMEOUT_REQUEST:
		printf( "FCTS_GAMEOUT_REQUEST\t" );
		break;

	case Matchless::FSTC_GAMEOUT:
		printf( "FSTC_GAMEOUT\t" );
		break;

	case Matchless::FCTS_LOGOUT_INFORM:
		printf( "FCTS_LOGOUT_INFORM\t" );
		break;

	}

	printf( "%u\n", msg.GetAddDataLen() );


	return	true;
}
