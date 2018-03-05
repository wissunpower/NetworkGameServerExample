
#include	"stdafx.h"
#include	"MatchlessServerCTimer.h"



MatchlessServer::CTimer::CTimer( void ) : m_FPS( 60 ), m_Tick( 0 ), m_Second( 0 ), m_IsWakeUp( false )
{
	QueryPerformanceFrequency( &m_Frequency );
	m_fTimeScale = 1.0f / m_Frequency.QuadPart;
}


MatchlessServer::CTimer::CTimer( unsigned int aFPS ) : m_FPS( aFPS ), m_Tick( 0 ), m_Second( 0 ), m_IsWakeUp( false )
{
	QueryPerformanceFrequency( &m_Frequency );
	m_fTimeScale = 1.0f / m_Frequency.QuadPart;
}


MatchlessServer::CTimer::~CTimer( void )
{
}


MatchlessServer::CTimer::CTimer( const MatchlessServer::CTimer & src ) : m_FPS( src.m_FPS ), m_Frequency( src.m_Frequency ),
																			m_fTimeScale( src.m_fTimeScale ),
																			m_Tick( src.m_Tick ), m_Second( src.m_Second ),
																			m_IsWakeUp( src.m_IsWakeUp )
{
}


MatchlessServer::CTimer & MatchlessServer::CTimer::operator=( const MatchlessServer::CTimer & src )
{
	if( this != (&src) )
	{
		m_FPS = src.m_FPS;
		m_Frequency = src.m_Frequency;

		m_Tick = src.m_Tick;
		m_Second = src.m_Second;

		m_IsWakeUp = src.m_IsWakeUp;
	}

	return	(*this);
}


int MatchlessServer::CTimer::Advance( void )
{
	static TimeUnit		lastCount;
	static TimeUnit		currentCount;

	static float	fTimeElapsed = 0.0f;


	QueryPerformanceCounter( &currentCount );
	if( 0 == lastCount.QuadPart )
	{
		lastCount.QuadPart = currentCount.QuadPart;
	}
	fTimeElapsed += (currentCount.QuadPart - lastCount.QuadPart) * m_fTimeScale;

	lastCount.QuadPart = currentCount.QuadPart;

	if( (1.0f / m_FPS) <= fTimeElapsed )
	{
		fTimeElapsed -= (1.0f / m_FPS);
		++m_Tick;
		m_IsWakeUp = true;
	}
	else
	{
		m_IsWakeUp = false;
		return	(-1);
	}

	if( (m_Second + 1) * m_FPS <= m_Tick )
	{
		++m_Second;
	}


	return	0;
}
