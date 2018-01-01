
#ifndef		__MATCHLESSSERVERCTIMER_H_f91i2_qowe9_382hf__
#define		__MATCHLESSSERVERCTIMER_H_f91i2_qowe9_382hf__



#include	<winsock2.h>



typedef LARGE_INTEGER	TimeUnit;



namespace	MatchlessServer
{
	class CTimer
	{

	private:

		CTimer( void );


	public:

		CTimer( unsigned int aFPS );
		~CTimer( void );
		CTimer( const CTimer & src );
		CTimer & operator=( const CTimer & src );

		// Get Member Function
		const unsigned int GetFPS( void ) const					{ return m_FPS; }
		const TimeUnit GetFrequency( void ) const				{ return m_Frequency; }
		const bool IsWakeUp( void ) const						{ return m_IsWakeUp; }
		const unsigned int GetTick( void ) const				{ return m_Tick; }
		const unsigned int GetSecond( void ) const				{ return m_Second; }

		// Set Member Function


	public:

		int Advance( void );


	private:

		unsigned int	m_FPS;					// frames per second.
		TimeUnit		m_Frequency;

		float			m_fTimeScale;

		unsigned int	m_Tick;
		unsigned int	m_Second;

		bool			m_IsWakeUp;

	};
}



#endif		// __MATCHLESSSERVERCTIMER_H_f91i2_qowe9_382hf__
