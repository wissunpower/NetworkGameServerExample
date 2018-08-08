
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"MatchlessProtocol.h"

#include	"cNetMessageHandlerManager.h"
#include	"cNetMessageHandler_Impl.h"
#include	"cMatchlessServerException.h"


int cNetMessageHandlerManager::Initialize()
{
	m_vHandler.resize( Matchless::ENetMessageType::ENMT_TotalCount );

	for ( unsigned int index = Matchless::ENetMessageType::ENMT_Idle ; index < Matchless::ENetMessageType::ENMT_TotalCount ; ++index )
	{
		m_vHandler[ index ] = CreateHandler( static_cast<Matchless::ENetMessageType>( index ) );
	}

	return RETURN_SUCCEED;
}


int cNetMessageHandlerManager::OnProcess( const Matchless::ENetMessageType nMsgType, MatchlessServer::CClient& client, cIPacket& iPacket )
{
	if ( Matchless::ENetMessageType::ENMT_Idle > nMsgType || nMsgType >= Matchless::ENetMessageType::ENMT_TotalCount )
	{
		throw cMatchlessServerException_UnknownNetMessage {};
	}

	if ( nullptr == m_vHandler[ nMsgType ] )
	{
		//throw cMatchlessServerException_NotFoundNetMessageHandler {};
		return RETURN_SUCCEED;
	}

	m_vHandler[ nMsgType ]->OnProcess( client, iPacket );

	return RETURN_SUCCEED;
}

std::shared_ptr< INetMessageHandler< MatchlessServer::CClient > > cNetMessageHandlerManager::CreateHandler( const Matchless::ENetMessageType type )
{
	std::shared_ptr< INetMessageHandler< MatchlessServer::CClient > > pHandler{};

	switch ( type )
	{

	case Matchless::ENetMessageType::FCTS_MSS_UPDATE:
		pHandler.reset( new cMainStepStateUpdateHandler );
		break;

	case Matchless::ENetMessageType::FCTS_CHARCLASS_UPDATE:
		pHandler.reset( new cCharacterClassUpdateHandler );
		break;

	case Matchless::ENetMessageType::FCTS_TEAM_UPDATE:
		pHandler.reset( new cTeamUpdateHandler );
		break;

	case Matchless::ENetMessageType::FCTS_MAP_UPDATE:
		pHandler.reset( new cMapUpdateHandler );
		break;

	case Matchless::ENetMessageType::FCTS_GAMESTART_REQUEST:
		pHandler.reset( new cGameStartRequestHandler );
		break;

	case Matchless::ENetMessageType::FCTS_GAME_MOVE_POSITION:
		pHandler.reset( new cGameMovePositionHandler );
		break;

	case Matchless::ENetMessageType::FCTS_GAME_MOVE_ANIMATION:
		pHandler.reset( new cGameMoveAnimationHandler );
		break;

	case Matchless::ENetMessageType::FCTS_GAME_MOVE_ALL_REQUEST:
		pHandler.reset( new cGameMoveAllHandler );
		break;

	case Matchless::ENetMessageType::FCTS_GAME_SKILL_REQUEST:
		pHandler.reset( new cGameSkillRequestHandler );
		break;

	case Matchless::ENetMessageType::FCTS_CHAT:
		pHandler.reset( new cChatHandler );
		break;

	case Matchless::ENetMessageType::FCTS_GAMEOUT_REQUEST:
		pHandler.reset( new cGameOutRequestHandler );
		break;

	}

	return pHandler;
}
