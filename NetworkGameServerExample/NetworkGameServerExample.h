#pragma once

#include "resource.h"

#include "cMonitor.h"


class cSyncClass : public cMonitor
{
public:
	cSyncClass();
	~cSyncClass();

	void IncrementInteger();

private:
	int			m_nInteger;
	cMonitor	m_csInteger;
};
