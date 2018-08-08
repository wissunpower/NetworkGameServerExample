#pragma once


class cIPacket;


template< typename T >
class INetMessageHandler : public std::enable_shared_from_this< INetMessageHandler< T > >
{

public:
	virtual int OnProcess( T& requester, cIPacket& iPacket ) = 0;

};
