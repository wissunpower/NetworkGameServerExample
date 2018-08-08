#pragma once


#include	"INetMessageHandler.h"


namespace MatchlessServer
{
	class CClient;
}


class cMainStepStateUpdateHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cCharacterClassUpdateHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cTeamUpdateHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cMapUpdateHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameStartRequestHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameMovePositionHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameMoveAnimationHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameMoveAllHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameSkillRequestHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cChatHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameOutRequestHandler : public INetMessageHandler< MatchlessServer::CClient >
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};
