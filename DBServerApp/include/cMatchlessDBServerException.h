#pragma once


#include	"cException.h"


struct cMatchlessDBServerException_FailedSQLHandleAllocate : public cException
{
	cMatchlessDBServerException_FailedSQLHandleAllocate()
		: cException { "cMatchlessDBServerException_FailedSQLHandleAllocate" }
	{
	}
	cMatchlessDBServerException_FailedSQLHandleAllocate( const char* objectName, const char* whereFile, const int whereLine )
		: cException { std::string{ std::string{ "cMatchlessDBServerException_FailedSQLHandleAllocate" }
				+ " : " + objectName
			}.c_str(), whereFile, whereLine }
	{
	}
};


struct cMatchlessDBServerException_UnknownNetMessage : public cException
{
	cMatchlessDBServerException_UnknownNetMessage()
		: cException { "cMatchlessDBServerException_UnknownNetMessage" }
	{
	}
	cMatchlessDBServerException_UnknownNetMessage( const unsigned int nMessageType, const char* whereFile, const int whereLine )
		: cException { std::string{ std::string{ "cMatchlessDBServerException_UnknownNetMessage" }
				+ " ( message type : " + std::to_string( nMessageType ) + " ) "
			}.c_str(), whereFile, whereLine }
	{
	}
};


struct cMatchlessDBServerException_NotFoundNetMessageHandler : public cException
{
	cMatchlessDBServerException_NotFoundNetMessageHandler()
		: cException { "cMatchlessDBServerException_NotFoundNetMessageHandler" }
	{
	}
	cMatchlessDBServerException_NotFoundNetMessageHandler( const char* whereFile, const int whereLine )
		: cException { std::string{ std::string{ "cMatchlessDBServerException_NotFoundNetMessageHandler" }
			}.c_str(), whereFile, whereLine }
	{
	}
};
