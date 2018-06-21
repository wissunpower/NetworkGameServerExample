#pragma once


namespace MatchlessServer
{
	class CClient;
}

class cIPacket;
class INetMessageHandler;


class cNetMessageHandlerManager
{

public:
	int Initialize();

	int OnProcess( const Matchless::ENetMessageType nMsgType, MatchlessServer::CClient& client, cIPacket& iPacket );

private:
	std::shared_ptr< INetMessageHandler > CreateHandler( const Matchless::ENetMessageType type );

private:
	std::vector< std::shared_ptr< INetMessageHandler > >	m_vHandler;

};
