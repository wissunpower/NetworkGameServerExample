
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"MatchlessProtocol.h"

#include	"cNetMessageHandlerManager.h"
#include	"cNetMessageHandler_Impl.h"
#include	"cMatchlessDBServerException.h"


int cNetMessageHandlerManager::Initialize()
{
	m_vHandler.resize( Matchless::ENetMessageType::ENMT_TotalCount );

	for ( unsigned int index = Matchless::ENetMessageType::ENMT_Idle ; index < Matchless::ENetMessageType::ENMT_TotalCount ; ++index )
	{
		m_vHandler[ index ] = CreateHandler( static_cast<Matchless::ENetMessageType>( index ) );
	}

	return RETURN_SUCCEED;
}


int cNetMessageHandlerManager::OnProcess( const Matchless::ENetMessageType nMsgType, cConnection& connection, cIPacket& iPacket )
{
	if ( Matchless::ENetMessageType::ENMT_Idle > nMsgType || nMsgType >= Matchless::ENetMessageType::ENMT_TotalCount )
	{
		throw cMatchlessDBServerException_UnknownNetMessage { static_cast<unsigned int>( nMsgType ), __FILE__, __LINE__ };
	}

	if ( nullptr == m_vHandler[ nMsgType ] )
	{
		//throw cMatchlessServerException_NotFoundNetMessageHandler {};
		return RETURN_SUCCEED;
	}

	m_vHandler[ nMsgType ]->OnProcess( connection, iPacket );

	return RETURN_SUCCEED;
}

std::shared_ptr< INetMessageHandler< cConnection > > cNetMessageHandlerManager::CreateHandler( const Matchless::ENetMessageType type )
{
	std::shared_ptr< INetMessageHandler< cConnection > > pHandler{};

	switch ( type )
	{

	case Matchless::ENetMessageType::FSTD_LOGCOMMON_WRITE:
		pHandler.reset( new cLogCommonWriteHandler );
		break;

	}

	return pHandler;
}
