#pragma once


#include	"INetMessageHandler.h"


class cConnection;
class cIPacket;


class cNetMessageHandlerManager
{

public:
	int Initialize();

	int OnProcess( const Matchless::ENetMessageType nMsgType, cConnection& connection, cIPacket& iPacket );

private:
	std::shared_ptr< INetMessageHandler< cConnection > > CreateHandler( const Matchless::ENetMessageType type );

private:
	std::vector< std::shared_ptr< INetMessageHandler< cConnection > > >	m_vHandler;

};
