#pragma once


#include	"INetMessageHandler.h"


namespace MatchlessServer
{
	class CClient;
}

class cIPacket;


class cNetMessageHandlerManager
{

public:
	int Initialize();

	int OnProcess( const Matchless::ENetMessageType nMsgType, MatchlessServer::CClient& client, cIPacket& iPacket );

private:
	std::shared_ptr< INetMessageHandler< MatchlessServer::CClient > > CreateHandler( const Matchless::ENetMessageType type );

private:
	std::vector< std::shared_ptr< INetMessageHandler< MatchlessServer::CClient > > >	m_vHandler;

};
