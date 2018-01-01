
#ifndef		__MATCHLESSTIMER_H_f93jw_f02k3_u93rw__
#define		__MATCHLESSTIMER_H_f93jw_f02k3_u93rw__



namespace	Matchless
{
	class Timer
	{

	private:

		Timer( void );


	public:

		Timer( unsigned int aFPS );
		~Timer( void );
		Timer( const Timer & src );
		Timer & operator=( const Timer & src );

		// Get Member Function
		const unsigned int GetFPS( void ) const					{ return m_FPS; }
		const double GetFramePeriod( void ) const				{ return m_FramePeriod; }
		const bool IsWakeUp( void ) const						{ return m_IsWakeUp; }
		const unsigned int GetTick( void ) const				{ return m_Tick; }
		const unsigned int GetSecond( void ) const				{ return m_Second; }

		// Set Member Function


	public:

		int Advance( float aElapsedTime );


	private:

		unsigned int	m_FPS;					// frames per second.
		double			m_FramePeriod;

		double			m_RemainElapsedTime;
		unsigned int	m_Tick;
		unsigned int	m_Second;

		bool			m_IsWakeUp;

	};
}



#endif		// __MATCHLESSTIMER_H_f93jw_f02k3_u93rw__
