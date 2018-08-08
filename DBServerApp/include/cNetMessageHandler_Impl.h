#pragma once


#include	"INetMessageHandler.h"


class cConnection;


class cLogCommonWriteHandler : public INetMessageHandler< cConnection >
{

public:
	int OnProcess( cConnection& connection, cIPacket& iPacket ) override;

};
