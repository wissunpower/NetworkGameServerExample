
#ifndef		__MATCHLESSCPLAYERINFO_H_93hs9_vj29t_fj923__
#define		__MATCHLESSCPLAYERINFO_H_93hs9_vj29t_fj923__



#include	"MatchlessRoot.h"
#include	"MatchlessCCharacter.h"



namespace	Matchless
{
	class CPlayerInfo
	{

	public:

		// Get Member Function
		const EMainStepState & GetMainStepState( void ) const		{ return m_MainStepState; }
		const bool IsRoomMaster( void ) const						{ return m_bRoomMaster; }
		const unsigned short int GetTeamNum( void ) const			{ return m_TeamNum; }
		SMatrix4 & GetTransform( void )								{ return m_Transform; }
		CCharacter & GetCharacterInfo( void )						{ return m_CharacterInfo; }
		const unsigned int GetCurrentCastStartTick( void ) const	{ return m_CurrentCastStartTick; }

		// Set Member Function
		void SetMainStepState( const EMainStepState & aValue )		{ m_MainStepState = aValue; }
		void SetbRoomMaster( const bool aBool )						{ m_bRoomMaster = aBool; }
		void SetTeamNum( const unsigned short int aValue )			{ m_TeamNum = aValue; }
		void SetTransform( const SMatrix4 & aMatrix )				{ m_Transform = aMatrix; }
		void SetCharacterInfo( const CCharacter & aValue )			{ m_CharacterInfo = aValue; }
		void SetCurrentCastStartTick( const unsigned int aTick )	{ m_CurrentCastStartTick = aTick; }


		const unsigned int DecreaseHealth( const unsigned int aValue )
		{
			if( aValue > m_CharacterInfo.GetCurrentHealth() )
				m_CharacterInfo.SetCurrentHealth( 0 );
			else
				m_CharacterInfo.SetCurrentHealth( m_CharacterInfo.GetCurrentHealth() - aValue );

			return	m_CharacterInfo.GetCurrentHealth();
		}


	private:

		EMainStepState		m_MainStepState;
		bool				m_bRoomMaster;
		unsigned short int	m_TeamNum;

		SMatrix4			m_Transform;

		CCharacter			m_CharacterInfo;

		unsigned int		m_CurrentCastStartTick;

	};
}



#endif		// __MATCHLESSCPLAYERINFO_H_93hs9_vj29t_fj923__
