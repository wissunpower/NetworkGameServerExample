
#include	"stdafx.h"
#include	"cMonitor.h"


cMonitor::cMonitor()
{
	InitializeCriticalSection( &m_csSyncObject );
}

cMonitor::~cMonitor()
{
	DeleteCriticalSection( &m_csSyncObject );
}

BOOL cMonitor::TryEnter()
{
	return TryEnterCriticalSection( &m_csSyncObject );
}

void cMonitor::Enter()
{
	::EnterCriticalSection( &m_csSyncObject );
}

void cMonitor::Leave()
{
	::LeaveCriticalSection( &m_csSyncObject );
}

cMonitor::Owner::Owner( cMonitor& crit )
	: m_csSyncObject( crit )
{
	m_csSyncObject.Enter();
}

cMonitor::Owner::~Owner()
{
	m_csSyncObject.Leave();
}
