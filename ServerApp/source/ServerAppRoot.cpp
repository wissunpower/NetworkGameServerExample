
#include	"stdafx.h"
#include	"ServerAppRoot.h"
#include	"LibraryDef.h"
#include	"cPacket.h"
#include	"LogUtil.h"



MatchlessServer::CTimer							g_Timer( 60 );

std::map< SOCKET, unsigned int >				g_mClientID;				// < SOCKET, Client ID >
cMonitor										g_csClientID;

std::list< unsigned int >						g_ReuseClientIDlist;
unsigned int									g_LargestClientID = 0;
std::map< unsigned int, std::shared_ptr< MatchlessServer::CClient > >	g_ClientList;			// < ID, SOCKET >
cMonitor										g_ClientListMonitor;
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

		cMonitor::Owner lock{ g_ClientListMonitor };
		g_Timer.Advance();
		currentTick = g_Timer.GetTick();
		isWakeUp = g_Timer.IsWakeUp();

		if( !isWakeUp )
		{
			continue;
		}

		if( 0 == g_Timer.GetTick() % 300 )
		{
			WriteLog( _T( "Timer Thread Run" ) );
		}

		while( !g_SkillMessageList.empty() )
		{
			smIter = g_SkillMessageList.begin();
			if( currentTick < smIter->first )
			{
				break;
			}

			HandleSkillRequest( false, static_cast<Matchless::ECharacterSkill>( smIter->second.m_SkillKind ), *g_ClientList[ smIter->second.m_Caster ], *g_ClientList[ smIter->second.m_Target ]  );
			g_SkillMessageList.erase( smIter );
		}
	}


	return	0;
}


DWORD WINAPI GameProcessThread( LPVOID arg )
{
	unsigned int	currentTick = 0;
	bool			isWakeUp = false;

	CNetMessage					tempMessage;
	unsigned int				tempID;
	unsigned int				tempAmount;
	Matchless::ECharDataType	tempCDT = Matchless::ECDT_CurrentEnergy;


	while( true )
	{
		Sleep( 1 );

		{
			cMonitor::Owner lock{ g_ClientListMonitor };
			currentTick = g_Timer.GetTick();
			isWakeUp = g_Timer.IsWakeUp();
		}

		if( g_IsAcceptable || !isWakeUp )
		{
			continue;
		}

		if( 0 == g_Timer.GetTick() % 300 )
		{
			WriteLog( _T( "Game Process Thread Run" ), { eLogInfoType::LOG_INFO_LOW } );
		}

		cMonitor::Owner lock{ g_ClientListMonitor };
		for( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
		{
			// remove expired effect
			auto csIter = cIter->second->m_PlayerInfo.GetCharacterInfo().GetStateList().begin();
			while( csIter != cIter->second->m_PlayerInfo.GetCharacterInfo().GetStateList().end() )
			{
				auto currentCSIter = csIter;
				++csIter;

				if( currentCSIter->GetRemoveTime() <= currentTick )
				{
					tempID = cIter->first;
					tempCDT = Matchless::ECDT_RemoveState;
					tempAmount = (unsigned int)currentCSIter->GetType();

					cIter->second->m_PlayerInfo.GetCharacterInfo().GetStateList().erase( currentCSIter );

					cOPacket oPacket;
					oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
					oPacket.Encode4u( tempID );
					oPacket.Encode4u( tempCDT );
					oPacket.Encode4u( tempAmount );

					for( auto cIter1 = g_ClientList.begin() ; cIter1 != g_ClientList.end() ; ++cIter1 )
					{
						oPacket.Send( cIter1->second->m_NetSystem.GetSocket() );
					}
				}
			}

			// energy recovery
			if( 0 == currentTick % 60 )
			{
				if( cIter->second->m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy() >= cIter->second->m_PlayerInfo.GetCharacterInfo().GetMaxEnergy() )
				{
					continue;
				}

				tempID = cIter->first;
				tempCDT = Matchless::ECDT_CurrentEnergy;
				tempAmount = cIter->second->m_PlayerInfo.GetCharacterInfo().IncreaseCurrentEnergy( 1 );

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket.Encode4u( tempID );
				oPacket.Encode4u( tempCDT );
				oPacket.Encode4u( tempAmount );

				for( auto cIter1 = g_ClientList.begin() ; cIter1 != g_ClientList.end() ; ++cIter1 )
				{
					oPacket.Send( cIter1->second->m_NetSystem.GetSocket() );
				}
			}
		}
	}


	return	0;
}




unsigned int GetClientID( void )
{
	unsigned int	returnValue;

	cMonitor::Owner lock{ g_ClientListMonitor };
	if( !g_ReuseClientIDlist.empty() )
	{
		returnValue = (*g_ReuseClientIDlist.begin());
		g_ReuseClientIDlist.pop_front();
	}
	else
	{
		returnValue = (++g_LargestClientID);
	}

	return	returnValue;
}


int ReturnClientID( const unsigned int aID )
{
	if( 0 == aID )
	{
		return ( -1 );
	}
	else
	{
		cMonitor::Owner lock{ g_ClientListMonitor };
		g_ReuseClientIDlist.push_back( aID );
	}

	return 0;
}


int OutputServerInitialInfo( const SOCKADDR_IN & aAddrInfo, const SOCKET aListenSocket )
{
	std::vector< tchar > vHostName( NI_MAXHOST );
	std::vector< tchar > vServInfo( NI_MAXSERV );
	int	addrlen;
	SOCKADDR_IN	tempAddr_in;

	DWORD dwRet = GetNameInfo( (SOCKADDR*)&aAddrInfo, sizeof( SOCKADDR ), vHostName.data(), NI_MAXHOST, vServInfo.data(), NI_MAXSERV, NI_NUMERICSERV );
	if( 0 != dwRet )
	{
		WriteLog( _T( "-- Failed get server information." ) );
		return -1;
	}

	addrlen = sizeof( tempAddr_in );
	if( SOCKET_ERROR == getsockname( aListenSocket, (SOCKADDR*)&tempAddr_in, &addrlen ) )
	{
		WriteLog( _T( "-- Failed get server information." ) );
		return -1;
	}

	WriteLog( _T( "-- Succeed launch server." ) );

	WriteLog( tstring{ _T( "-- Server IP address : " ) } + Inet_Ntop( tempAddr_in ) + _T( ", " ) + tstring{ vHostName.data() } );

	WriteLog( tstring{ _T( "-- Server port number : " ) } + wsp::to( ntohs( aAddrInfo.sin_port ) ) );

	return 0;
}


bool DoNeedRoomMaster( void )
{
	bool returnValue = true;

	cMonitor::Owner lock{ g_ClientListMonitor };
	for( auto cIt = g_ClientList.begin() ; cIt != g_ClientList.end() ; ++cIt )
	{
		if( cIt->second->m_PlayerInfo.IsRoomMaster() )
		{
			returnValue = false;
			break;
		}
	}

	return	returnValue;
}


// if aBefore is 0, than occur insert player,
// if aAfter is 0, than occur remove player.
int ChangeTeamPlayerNum( const unsigned short int aBefore, const unsigned short int aAfter )
{
	int				prevNum = 0;
	CNetMessage		tempMessage;


	cMonitor::Owner lock{ g_ClientListMonitor };
	
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
		for( auto tpIt = g_TeamPlayerNumMap.begin() ; tpIt != g_TeamPlayerNumMap.end() ; ++tpIt )
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
	auto cIt = g_ClientList.begin();
	for( ; cIt != g_ClientList.end() ; ++cIt )
	{
		if( cIt->second->m_PlayerInfo.IsRoomMaster() )
		{
			break;
		}
	}
	if( g_ClientList.end() != cIt )
	{
		/* temporary code for dev. */g_IsGameStartable = true;

		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_STARTABLE );
		oPacket.EncodeBool( g_IsGameStartable );
		oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
	}

	return 0;
}


bool IsGameFinish( void )
{
	std::map< unsigned short int, bool >	tempRecord;			// < Team Number, IsAlive >
	unsigned int							tempAliveTeamCount = 0;


	{
		cMonitor::Owner lock{ g_ClientListMonitor };
		for ( auto tmIter = g_TeamPlayerNumMap.begin() ; tmIter != g_TeamPlayerNumMap.end() ; ++tmIter )
		{
			tempRecord[ tmIter->first ] = false;
		}


		for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
		{
			if ( true == tempRecord[ cIter->second->m_PlayerInfo.GetTeamNum() ] )
			{
				continue;
			}

			if ( 0 < cIter->second->m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
			{
				tempRecord[ cIter->second->m_PlayerInfo.GetTeamNum() ] = true;
			}
		}
	}


	for( auto rIter = tempRecord.begin() ; rIter != tempRecord.end() ; ++rIter )
	{
		if( true == rIter->second )
		{
			++tempAliveTeamCount;
		}
	}


	return	( ( 1 == tempAliveTeamCount ) ? true : false );
}


bool HandleSkillRequest( const bool aIsCastStart, const Matchless::ECharacterSkill aSkillKind, MatchlessServer::CClient& aCaster, MatchlessServer::CClient& aTarget )
{
	unsigned int				bufIndex = 0;
	unsigned int				buf1Index = 0;
	CNetMessage					tempMessage;

	const unsigned int			casterID = aCaster.m_NetSystem.GetID();
	const unsigned int			targetID = aTarget.m_NetSystem.GetID();

	Matchless::ECharDataType	tempCDT;
	unsigned int				tempAmount;


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
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			oPacket = cOPacket{};
			oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( tempCDT );
			oPacket.Encode4u( tempHealth );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					if ( tempIsGameFinish )
					{
						cOPacket oPacket;
						oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}
				if ( tempIsGameFinish )
				{
					g_IsAcceptable = true;
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Breaker_Raid:
		if( Matchless::IsSkillPossible( Matchless::BreakerRaid, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerRaid.GetEnergyCost() );
			aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetPhysicalDamage() * (unsigned int)Matchless::BreakerRaid.GetPhysicalDamageScale() );
			const bool		tempIsGameFinish = IsGameFinish();

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
			oPacket = cOPacket{};
			oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( tempCDT );
			oPacket.Encode4u( tempAmount );

			tempCDT = Matchless::ECDT_CurrentEnergy;
			tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( casterID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );

					if ( tempIsGameFinish )
					{
						cOPacket oPacket;
						oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}
				if ( tempIsGameFinish )
				{
					g_IsAcceptable = true;
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Breaker_Stun:
		if( Matchless::IsSkillPossible( Matchless::BreakerStun, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerStun.GetEnergyCost() );
			aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( (unsigned int)((float)aCaster.m_PlayerInfo.GetCharacterInfo().GetPhysicalDamage() * Matchless::BreakerStun.GetPhysicalDamageScale()) );
			const bool		tempIsGameFinish = IsGameFinish();

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_Faint, g_Timer.GetTick() + g_Timer.GetFPS() * 3, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}

				// cancel target's casting
				cOPacket oPacket1;
				oPacket1.Encode4u( Matchless::FSTC_GAME_SKILL_CANCEL );
				oPacket1.Send( aTarget.m_NetSystem.GetSocket() );
				auto smIter = g_SkillMessageList.begin();
				while ( smIter != g_SkillMessageList.end() )
				{
					auto currentSMIter = smIter;
					++smIter;

					if ( targetID == currentSMIter->second.m_Caster )
					{
						g_SkillMessageList.erase( currentSMIter );
					}
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
			oPacket = cOPacket{};
			oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( tempCDT );
			oPacket.Encode4u( tempAmount );

			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_Faint;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );

					if ( tempIsGameFinish )
					{
						cOPacket oPacket;
						oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}
				if ( tempIsGameFinish )
				{
					g_IsAcceptable = true;
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Breaker_ArmorBreak:
		if( Matchless::IsSkillPossible( Matchless::BreakerArmorBreak, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerArmorBreak.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyArmDec, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyArmDec;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Breaker_PowerBreak:
		if( Matchless::IsSkillPossible( Matchless::BreakerPowerBreak, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerPowerBreak.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyDamDec, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyDamDec;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Breaker_Silent:
		if( Matchless::IsSkillPossible( Matchless::BreakerSilent, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::BreakerSilent.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_Silence, g_Timer.GetTick() + g_Timer.GetFPS() * 8, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_Silence;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Defender_StoneSkin:
		if( Matchless::IsSkillPossible( Matchless::DefenderStoneSkin, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderStoneSkin.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyArmInc, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyArmInc;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Defender_Natural:
		if( Matchless::IsSkillPossible( Matchless::DefenderNatural, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderNatural.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_MagArmInc, g_Timer.GetTick() + g_Timer.GetFPS() * 30, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_MagArmInc;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Defender_AttackBreak:
		if( Matchless::IsSkillPossible( Matchless::DefenderAttackBreak, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderAttackBreak.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_PhyDamDec, g_Timer.GetTick() + g_Timer.GetFPS() * 20, 0 );
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_MagDamDec, g_Timer.GetTick() + g_Timer.GetFPS() * 20, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_PhyDamDec;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_MagDamDec;
			oPacket = cOPacket{};
			oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( tempCDT );
			oPacket.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Defender_HeavyBlow:
		if( Matchless::IsSkillPossible( Matchless::DefenderHeavyBlow, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::DefenderHeavyBlow.GetEnergyCost() );
			aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetPhysicalDamage() * (unsigned int)Matchless::DefenderHeavyBlow.GetPhysicalDamageScale() );
			const bool		tempIsGameFinish = IsGameFinish();

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_CurrentHealth;
			tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
			oPacket = cOPacket{};
			oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( tempCDT );
			oPacket.Encode4u( tempAmount );

			tempCDT = Matchless::ECDT_CurrentEnergy;
			tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( casterID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );

					if ( tempIsGameFinish )
					{
						cOPacket oPacket;
						oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}
				if ( tempIsGameFinish )
				{
					g_IsAcceptable = true;
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
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

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_CASTSTART );
				oPacket.Encode4u( tempCastTime );
				oPacket.Send( aCaster.m_NetSystem.GetSocket() );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
				}
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::MageIceBolt.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::MageIceBolt.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
				const bool		tempIsGameFinish = IsGameFinish();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
				oPacket.Encode4u( casterID );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( aSkillKind );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				oPacket = cOPacket{};
				oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( tempCDT );
				oPacket.Encode4u( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				cOPacket oPacket1;
				oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket1.Encode4u( casterID );
				oPacket1.Encode4u( tempCDT );
				oPacket1.Encode4u( tempAmount );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
						oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );

						if ( tempIsGameFinish )
						{
							cOPacket oPacket;
							oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
							oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
						}
					}
					if ( tempIsGameFinish )
					{
						g_IsAcceptable = true;
					}
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
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

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_CASTSTART );
				oPacket.Encode4u( tempCastTime );
				oPacket.Send( aCaster.m_NetSystem.GetSocket() );

				cMonitor::Owner lock{ g_ClientListMonitor };
				g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::MageIceBolt.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::MageFireBall.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();
				const bool		tempIsGameFinish = IsGameFinish();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
				oPacket.Encode4u( casterID );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( aSkillKind );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				oPacket = cOPacket{};
				oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( tempCDT );
				oPacket.Encode4u( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				cOPacket oPacket1;
				oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket1.Encode4u( casterID );
				oPacket1.Encode4u( tempCDT );
				oPacket1.Encode4u( tempAmount );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
						oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );

						if ( tempIsGameFinish )
						{
							cOPacket oPacket;
							oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
							oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
						}
					}
					if ( tempIsGameFinish )
					{
						g_IsAcceptable = true;
					}
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	case Matchless::ECS_Healer_Perfect:
		if( Matchless::IsSkillPossible( Matchless::HealerPerfect, aCaster.m_PlayerInfo, aTarget.m_PlayerInfo ) )
		{
			aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::HealerPerfect.GetEnergyCost() );

			// send Matchless::FSTC_GAME_SKILL_APPLY message to client
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
			oPacket.Encode4u( casterID );
			oPacket.Encode4u( targetID );
			oPacket.Encode4u( aSkillKind );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				aTarget.m_PlayerInfo.GetCharacterInfo().AddState( Matchless::EST_Perfect, g_Timer.GetTick() + g_Timer.GetFPS() * 8, 0 );

				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}

			// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
			tempCDT = Matchless::ECDT_InsertState;
			tempAmount = (unsigned int)Matchless::EST_Perfect;
			cOPacket oPacket1;
			oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
			oPacket1.Encode4u( targetID );
			oPacket1.Encode4u( tempCDT );
			oPacket1.Encode4u( tempAmount );

			{
				cMonitor::Owner lock{ g_ClientListMonitor };
				for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
				{
					oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
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

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_CASTSTART );
				oPacket.Encode4u( tempCastTime );
				oPacket.Send( aCaster.m_NetSystem.GetSocket() );

				cMonitor::Owner lock{ g_ClientListMonitor };
				g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::HealerQuickHeal.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().IncreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::HealerQuickHeal.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
				oPacket.Encode4u( casterID );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( aSkillKind );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				oPacket = cOPacket{};
				oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( tempCDT );
				oPacket.Encode4u( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				cOPacket oPacket1;
				oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket1.Encode4u( casterID );
				oPacket1.Encode4u( tempCDT );
				oPacket1.Encode4u( tempAmount );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
						oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
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

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_CASTSTART );
				oPacket.Encode4u( tempCastTime );
				oPacket.Send( aCaster.m_NetSystem.GetSocket() );

				cMonitor::Owner lock{ g_ClientListMonitor };
				g_SkillMessageList.insert( std::map< unsigned int, MatchlessServer::SkillMessageInfo >::value_type( g_Timer.GetTick() + tempCastTime, tempSMInfo ) );
			}
			else
			{
				aCaster.m_PlayerInfo.GetCharacterInfo().DecreaseCurrentEnergy( Matchless::HealerNormalHeal.GetEnergyCost() );
				aTarget.m_PlayerInfo.GetCharacterInfo().IncreaseCurrentHealth( aCaster.m_PlayerInfo.GetCharacterInfo().GetMagicalDamage() * (unsigned int)Matchless::HealerNormalHeal.GetMagicalDamageScale() );
				unsigned int	tempAmount = aTarget.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth();

				// send Matchless::FSTC_GAME_SKILL_APPLY message to client
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_APPLY );
				oPacket.Encode4u( casterID );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( aSkillKind );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}

				// send Matchless::FSTC_GAME_CHAR_UPDATE message to client
				tempCDT = Matchless::ECDT_CurrentHealth;
				oPacket = cOPacket{};
				oPacket.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket.Encode4u( targetID );
				oPacket.Encode4u( tempCDT );
				oPacket.Encode4u( tempAmount );

				tempAmount = aCaster.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy();
				tempCDT = Matchless::ECDT_CurrentEnergy;
				cOPacket oPacket1;
				oPacket1.Encode4u( Matchless::FSTC_GAME_CHAR_UPDATE );
				oPacket1.Encode4u( casterID );
				oPacket1.Encode4u( tempCDT );
				oPacket1.Encode4u( tempAmount );

				{
					cMonitor::Owner lock{ g_ClientListMonitor };
					for ( auto cIter = g_ClientList.begin() ; cIter != g_ClientList.end() ; ++cIter )
					{
						oPacket.Send( cIter->second->m_NetSystem.GetSocket() );
						oPacket1.Send( cIter->second->m_NetSystem.GetSocket() );
					}
				}
			}
		}
		else
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
			oPacket.Send( aCaster.m_NetSystem.GetSocket() );
		}
		break;

	}


	return	true;
}


bool IsNowCasting( const unsigned int aID, const bool aIsCancel )
{
	bool	returnValue = false;

	cMonitor::Owner lock{ g_ClientListMonitor };

	auto smIter = g_SkillMessageList.begin();
	for( ; smIter != g_SkillMessageList.end() ; ++smIter )
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


	return	returnValue;
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
