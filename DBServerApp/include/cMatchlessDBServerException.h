#pragma once


#include	"cException.h"


struct cMatchlessDBServerException_FailedSQLHandleAllocate : public cException
{
	cMatchlessDBServerException_FailedSQLHandleAllocate()
		: cException { "cMatchlessDBServerException_FailedSQLHandleAllocate" }
	{
	}
	cMatchlessDBServerException_FailedSQLHandleAllocate( const char* objectName, const char* whereFile, const int whereLine )
		: cException { std::string{ std::string{ "cException_FailedToCreateObject" }
				+ " : " + objectName
			}.c_str(), whereFile, whereLine }
	{
	}
};
