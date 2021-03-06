#pragma once

#include "resource.h"

#include "cMonitor.h"
#include "cSingleton.h"
#include "cThread.h"
#include "cLog.h"
#include "cVBuffer.h"
#include "cQueue.h"


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


class cUtilManager
{
public:
	int CalcSum( int a, int b );

};


class cTickThread : public cThread
{
public:
	virtual void OnProcess();
};
