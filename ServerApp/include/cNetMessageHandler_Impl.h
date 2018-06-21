#pragma once



#include	"INetMessageHandler.h"



class cMainStepStateUpdateHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cCharacterClassUpdateHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cTeamUpdateHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cMapUpdateHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameStartRequestHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameMovePositionHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameMoveAnimationHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameMoveAllHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameSkillRequestHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cChatHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};


class cGameOutRequestHandler : public INetMessageHandler
{

public:
	int OnProcess( MatchlessServer::CClient& client, cIPacket& iPacket ) override;

};
