#pragma once


#include "linkopt.h"


struct NETLIB_API cException : public std::runtime_error
{
	cException( const char* msg ) : runtime_error { msg }
	{}
	cException( const char* msg, const char* whereFile, const int whereLine )
		: runtime_error { std::string{ std::string{ msg }
				+ " in " + whereFile
				+ ", " + std::to_string( whereLine ).c_str() + "line"
			}.c_str() }
	{
	}
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


struct NETLIB_API cException_FailedToCreateObject : public cException
{
	cException_FailedToCreateObject()
		: cException { "cException_FailedToCreateObject" }
	{
	}
	cException_FailedToCreateObject( const char* objectName, const char* whereFile, const int whereLine )
		: cException { std::string{ std::string{ "cException_FailedToCreateObject" }
				+ " : " + objectName
			}.c_str(), whereFile, whereLine }
	{
	}
};
