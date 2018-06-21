#pragma once


#include	"cException.h"


struct cMatchlessServerException_UnknownNetMessage : public cException
{
	cMatchlessServerException_UnknownNetMessage()
		: cException { "cMatchlessServerException_UnknownNetMessage" }
	{
	}
};


struct cMatchlessServerException_NotFoundNetMessageHandler : public cException
{
	cMatchlessServerException_NotFoundNetMessageHandler()
		: cException { "cMatchlessServerException_NotFoundNetMessageHandler" }
	{
	}
};
