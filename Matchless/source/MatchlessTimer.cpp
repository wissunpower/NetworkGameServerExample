
#include	"MatchlessTimer.h"



Matchless::Timer::Timer( void ) : m_FPS( 60 ), m_FramePeriod( 1.0 / 60.0 ),
									m_RemainElapsedTime( 0.0 ), m_Tick( 0 ), m_Second( 0 ), m_IsWakeUp( false )
{
}


Matchless::Timer::Timer( unsigned int aFPS ) : m_FPS( aFPS ), m_FramePeriod( 1.0 / ((double)aFPS) ),
												m_RemainElapsedTime( 0.0 ), m_Tick( 0 ), m_Second( 0 ), m_IsWakeUp( false )
{
}


Matchless::Timer::~Timer( void )
{
}


Matchless::Timer::Timer( const Matchless::Timer & src ) : m_FPS( src.m_FPS ), m_FramePeriod( src.m_FramePeriod ),
															m_RemainElapsedTime( src.m_RemainElapsedTime ), m_Tick( src.m_Tick ),
															m_Second( src.m_Second ), m_IsWakeUp( src.m_IsWakeUp )
{
}


Matchless::Timer & Matchless::Timer::operator=( const Matchless::Timer & src )
{
	if( this != (&src) )
	{
		m_FPS = src.m_FPS;
		m_FramePeriod = src.m_FramePeriod;

		m_RemainElapsedTime = src.m_RemainElapsedTime;
		m_Tick = src.m_Tick;
		m_Second = src.m_Second;

		m_IsWakeUp = src.m_IsWakeUp;
	}

	return	(*this);
}


int Matchless::Timer::Advance( float aElapsedTime )
{
	aElapsedTime -= (float)((int)aElapsedTime);

	m_RemainElapsedTime += ((double)aElapsedTime);

	if( m_FramePeriod <= m_RemainElapsedTime )
	{
		m_RemainElapsedTime -= m_FramePeriod;
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
