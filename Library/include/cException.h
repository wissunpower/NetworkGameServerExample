#pragma once


#include "linkopt.h"


struct NETLIB_API cException : public std::runtime_error
{
	cException( const char* msg ) : runtime_error { msg }
	{}
};


struct NETLIB_API cException_PacketDecodeUnderflow : public cException
{
	cException_PacketDecodeUnderflow()
		: cException { "cException_PacketDecodeUnderflow" }
	{
	}
};


struct NETLIB_API cException_PacketNotEnoughData : public cException
{
	cException_PacketNotEnoughData()
		: cException { "cException_PacketNotEnoughData" }
	{
	}
};
