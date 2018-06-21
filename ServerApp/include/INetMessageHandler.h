#pragma once


namespace MatchlessServer
{
	class CClient;
}

class cIPacket;


class INetMessageHandler : public std::enable_shared_from_this< INetMessageHandler >
{

public:
	virtual int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) = 0;

};
