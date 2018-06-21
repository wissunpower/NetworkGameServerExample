
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"cNetMessageHandler_Impl.h"
#include	"cMonitor.h"
#include	"cPacket.h"
#include	"MatchlessRoot.h"
#include	"ServerAppRoot.h"



int cMainStepStateUpdateHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	Matchless::EMainStepState	tempMSS;

	tempMSS = static_cast<Matchless::EMainStepState>( iPacket.Decode4u() );

	{
		cMonitor::Owner lock{ g_ClientListMonitor };
		if ( Matchless::EMSS_Wait != client.m_PlayerInfo.GetMainStepState() &&
			Matchless::EMSS_Wait == tempMSS )							// another step -> wait step change case.
		{
			ChangeTeamPlayerNum( 0, client.m_PlayerInfo.GetTeamNum() );
		}
		else if ( Matchless::EMSS_Wait == client.m_PlayerInfo.GetMainStepState() &&
			Matchless::EMSS_Wait != tempMSS )						// wait step -> another step change case.
		{
			ChangeTeamPlayerNum( client.m_PlayerInfo.GetTeamNum(), 0 );
		}
		client.m_PlayerInfo.SetMainStepState( tempMSS );
	}

	return RETURN_SUCCEED;
}


int cCharacterClassUpdateHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	Matchless::ECharacterClass	tempCC;
	auto tempID = client.m_NetSystem.GetID();

	// for server update
	tempCC = static_cast<Matchless::ECharacterClass>( iPacket.Decode4u() );
	client.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );

	// for reflect send
	cOPacket oPacket;
	oPacket.Encode4u( Matchless::FSTC_CHARCLASS_UPDATE );
	oPacket.Encode4u( tempID );
	oPacket.Encode4u( tempCC );
	{
		cMonitor::Owner lock{ g_ClientListMonitor };
		client.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
		for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
		{
			if ( cIt->first != tempID )
			{
				oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
			}
		}
	}

	return RETURN_SUCCEED;
}


int cTeamUpdateHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	auto tempID = client.m_NetSystem.GetID();

	// for server update
	auto tempTN = static_cast<unsigned short>( iPacket.Decode4u() );
	client.m_PlayerInfo.SetTeamNum( tempTN );

	// for reflect send
	cOPacket oPacket;
	oPacket.Encode4u( Matchless::FSTC_TEAM_UPDATE );
	oPacket.Encode4u( tempID );
	oPacket.Encode4u( tempTN );
	{
		cMonitor::Owner lock{ g_ClientListMonitor };
		client.m_PlayerInfo.SetTeamNum( tempTN );
		for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
		{
			if ( cIt->first != tempID )
			{
				oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
			}
		}
	}

	return RETURN_SUCCEED;
}


int cMapUpdateHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	cOPacket oPacket;
	oPacket.Encode4u( Matchless::FSTC_MAP_UPDATE );

	cMonitor::Owner lock{ g_ClientListMonitor };
	g_CurrentMapKind = iPacket.Decode4u();

	oPacket.Encode4u( g_CurrentMapKind );

	for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
	{
		oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
	}

	return RETURN_SUCCEED;
}


int cGameStartRequestHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	cMonitor::Owner lock{ g_ClientListMonitor };
	if ( g_IsGameStartable )
	{
		std::map< unsigned short int, int >	tempTeamPlayerNum;
		int						tempNum;
		Matchless::SMatrix4		tempMtx;

		g_IsAcceptable = false;

		// Set buf data
		Matchless::InitializeCharacterInfo( client.m_PlayerInfo.GetCharacterInfo() );
		for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
		{
			tempMtx.Reset();

			tempNum = tempTeamPlayerNum[ cIt->second->m_PlayerInfo.GetTeamNum() ]++;
			tempMtx._41 = (float)( ( tempNum % 3 ) * 300 - ( ( tempNum % 3 == 2 ) ? 600 : 0 ) );
			tempMtx._43 = (float)( ( ( tempNum % 3 ) ? 1500 : 1200 ) * ( ( cIt->second->m_PlayerInfo.GetTeamNum() % 2 ) ? 1 : -1 ) );

			Matchless::InitializeCharacterInfo( cIt->second->m_PlayerInfo.GetCharacterInfo() );
			cIt->second->m_PlayerInfo.SetTransform( tempMtx );
		}
		// Send buf data each client
		for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
		{
			for ( auto cIter = g_ClientList.begin(); cIter != g_ClientList.end(); ++cIter )
			{
				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FSTC_INFORM_CLIENTINFO );
				Matchless::Encode( oPacket, *cIter->second );
				oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
			}

			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAMESTART_SUCCEED );
			oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
		}
	}
	else
	{
		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_GAMESTART_FAILED );
		oPacket.Send( client.m_NetSystem.GetSocket() );
	}

	return RETURN_SUCCEED;
}


int cGameMovePositionHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	Matchless::SMatrix4			tempMatrix;
	auto tempID = client.m_NetSystem.GetID();

	// for server update
	Matchless::Decode( iPacket, tempMatrix );
	client.m_PlayerInfo.SetTransform( tempMatrix );

	// for reflect send
	{
		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_GAME_MOVE_POSITION );
		oPacket.Encode4u( tempID );
		Matchless::Encode( oPacket, tempMatrix );

		cMonitor::Owner lock{ g_ClientListMonitor };
		if ( 0 >= g_ClientList[ tempID ]->m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
		{
			return RETURN_SUCCEED;
		}
		if ( IsNowCasting( tempID, true ) )
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_CANCEL );
			oPacket.Send( g_ClientList[ tempID ]->m_NetSystem.GetSocket() );
		}
		g_ClientList[ tempID ]->m_PlayerInfo.SetTransform( tempMatrix );
		for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
		{
			if ( cIt->first != tempID )
			{
				oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
			}
		}
	}

	return RETURN_SUCCEED;
}


int cGameMoveAnimationHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	auto tempID = client.m_NetSystem.GetID();

	// for reflect send
	cAniTrackInfo aniInfo;
	cOPacket oPacket;
	oPacket.Encode4u( Matchless::FSTC_GAME_MOVE_ANIMATION );
	oPacket.Encode4u( tempID );

	auto nAniCount = iPacket.Decode4u();
	oPacket.Encode4u( nAniCount );
	for ( unsigned int i = 0; i < nAniCount; ++i )
	{
		oPacket.Encode4u( iPacket.Decode4u() );
		Decode( iPacket, aniInfo );
		Encode( oPacket, aniInfo );
	}

	cMonitor::Owner lock{ g_ClientListMonitor };
	if ( 0 >= g_ClientList[ tempID ]->m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
	{
		return RETURN_SUCCEED;
	}
	for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
	{
		if ( cIt->first != tempID )
		{
			oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
		}
	}

	return RETURN_SUCCEED;
}


int cGameMoveAllHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	auto tempID = client.m_NetSystem.GetID();

	cMonitor::Owner lock{ g_ClientListMonitor };
	for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
	{
		if ( cIt->first != tempID )
		{
			cOPacket oPacket;
			oPacket.Encode4u( Matchless::FSTC_GAME_MOVE_POSITION );
			oPacket.Encode4u( cIt->first );
			Matchless::Encode( oPacket, cIt->second->m_PlayerInfo.GetTransform() );
			oPacket.Send( client.m_NetSystem.GetSocket() );
		}
	}

	return RETURN_SUCCEED;
}


int cGameSkillRequestHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	auto tempID = client.m_NetSystem.GetID();
	auto tempECS = static_cast<Matchless::ECharacterSkill>( iPacket.Decode4u() );
	auto tempTargetID = iPacket.Decode4u();

	cMonitor::Owner lock{ g_ClientListMonitor };
	if ( 0 >= g_ClientList[ tempID ]->m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() )
	{
		return RETURN_SUCCEED;
	}
	if ( g_ClientList.end() != g_ClientList.find( tempTargetID ) &&
		g_ClientList.end() != g_ClientList.find( tempID ) &&
		!IsNowCasting( tempID, false ) )
	{
		HandleSkillRequest( true, tempECS, *g_ClientList[ tempID ], *g_ClientList[ tempTargetID ] );
	}
	else
	{
		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_GAME_SKILL_FAILED );
		oPacket.Send( g_ClientList[ tempID ]->m_NetSystem.GetSocket() );
	}

	return RETURN_SUCCEED;
}


int cChatHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	return RETURN_SUCCEED;
}


int cGameOutRequestHandler::OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket )
{
	cMonitor::Owner lock{ g_ClientListMonitor };
	for ( auto cIt = g_ClientList.begin(); cIt != g_ClientList.end(); ++cIt )
	{
		cOPacket oPacket;
		oPacket.Encode4u( Matchless::FSTC_GAMEOUT );
		oPacket.Send( cIt->second->m_NetSystem.GetSocket() );
	}
	g_IsAcceptable = true;

	return RETURN_SUCCEED;
}
