
#ifndef		__MATCHLESSCSTATE_H_f92aw_93kqm_skc92__
#define		__MATCHLESSCSTATE_H_f92aw_93kqm_skc92__



class cOPacket;
class cIPacket;


namespace	Matchless
{
	enum	EStateType
	{
		EST_Idle,

		EST_Perfect,

		EST_PhyDamInc,
		EST_PhyDamDec,
		EST_PhyArmInc,
		EST_PhyArmDec,
		EST_MagDamInc,
		EST_MagDamDec,
		EST_MagArmInc,
		EST_MagArmDec,

		EST_Frozen,

		EST_Faint,

		EST_Silence,

		EST_Panic,

		EST_TotalCount
	};

	class CState
	{

	public:

		// Get Member Function
		const EStateType GetType( void ) const							{ return m_Type; }
		const unsigned int GetRemoveTime( void ) const					{ return m_RemoveTime; }
		const unsigned int GetExtraValue( void ) const					{ return m_ExtraValue; }

		// Set Member Function
		void SetType( const EStateType aType )							{ m_Type = aType; }
		void SetRemoveTime( const unsigned int aTime )					{ m_RemoveTime = aTime; }
		void SetExtraValue( const unsigned int aValue )					{ m_ExtraValue = aValue; }


	private:

		EStateType		m_Type;
		unsigned int	m_RemoveTime;
		unsigned int	m_ExtraValue;

	};

	void Encode( cOPacket& oPacket, const CState& info );
	void Decode( cIPacket& iPacket, CState& info );
}



#endif		// __MATCHLESSCSTATE_H_f92aw_93kqm_skc92__
