
#ifndef		__MATCHLESSCCHARACTER_H_f83re_9u324_fvj92__
#define		__MATCHLESSCCHARACTER_H_f83re_9u324_fvj92__



#include	"MatchlessRoot.h"
#include	"MatchlessCState.h"

#include	<list>



namespace	Matchless
{
	enum ECharDataType
	{
		ECDT_Idle,

		ECDT_MaxHealth,
		ECDT_CurrentHealth,
		ECDT_MaxEnergy,
		ECDT_CurrentEnergy,

		ECDT_PhysicalDamage,
		ECDT_PhysicalArmor,
		ECDT_MagicalDamage,
		ECDT_MagicalArmor,

		ECDT_InsertState,
		ECDT_RemoveState,

		ECDT_TotalCount
	};

	class CCharacter
	{

	public:

		CCharacter( void ) : m_Class( ECC_Idle ), m_MaxHealth( 0 ), m_CurrentHealth( 0 ), m_MaxEnergy( 0 ), m_CurrentEnergy( 0 ),
								m_PhysicalDamage( 0 ), m_PhysicalArmor( 0 ), m_MagicalDamage( 0 ), m_MagicalArmor( 0 )
		{ m_StateList.clear(); }
		CCharacter & operator=( const CCharacter & src );

		// Get Member Function
		const ECharacterClass GetClass( void ) const			{ return m_Class; }
		const unsigned int GetMaxHealth( void ) const			{ return m_MaxHealth; }
		const unsigned int GetCurrentHealth( void ) const		{ return m_CurrentHealth; }
		const unsigned int GetMaxEnergy( void ) const			{ return m_MaxEnergy; }
		const unsigned int GetCurrentEnergy( void ) const		{ return m_CurrentEnergy; }
		const unsigned int GetPhysicalDamage( void ) const
		{
			float	tempRate = 1.0f;

			if( FindState( EST_PhyDamInc ) )
			{
				tempRate += 0.5f;
			}

			if( FindState( EST_PhyDamDec ) )
			{
				tempRate += 0.5f;
			}

			return	(unsigned int)((float)m_PhysicalDamage * tempRate);
		}
		const unsigned int GetPhysicalArmor( void ) const
		{
			float	tempRate = 1.0f;

			if( FindState( EST_PhyArmInc ) )
			{
				tempRate += 0.5f;
			}

			if( FindState( EST_PhyArmDec ) )
			{
				tempRate += 0.5f;
			}

			return	(unsigned int)((float)m_PhysicalArmor * tempRate);
		}
		const unsigned int GetMagicalDamage( void ) const
		{
			float	tempRate = 1.0f;

			if( FindState( EST_MagDamInc ) )
			{
				tempRate += 0.5f;
			}

			if( FindState( EST_MagDamDec ) )
			{
				tempRate += 0.5f;
			}

			return	(unsigned int)((float)m_MagicalDamage * tempRate);
		}
		const unsigned int GetMagicalArmor( void ) const
		{
			float	tempRate = 1.0f;

			if( FindState( EST_MagArmInc ) )
			{
				tempRate += 0.5f;
			}

			if( FindState( EST_MagArmDec ) )
			{
				tempRate += 0.5f;
			}

			return	(unsigned int)((float)m_MagicalArmor * tempRate);
		}

		std::list< CState >& GetStateList( void )				{ return m_StateList; }
		const std::list< CState >& GetStateList( void ) const	{ return m_StateList; }

		// Set Member Function
		void SetClass( const ECharacterClass aType )			{ m_Class = aType; }
		void SetMaxHealth( const unsigned int aValue )			{ m_MaxHealth = aValue; }
		void SetCurrentHealth( const unsigned int aValue )		{ m_CurrentHealth = aValue; }
		void SetMaxEnergy( const unsigned int aValue )			{ m_MaxEnergy = aValue; }
		void SetCurrentEnergy( const unsigned int aValue )		{ m_CurrentEnergy = aValue; }
		void SetPhysicalDamage( const unsigned int aValue )		{ m_PhysicalDamage = aValue; }
		void SetPhysicalArmor( const unsigned int aValue )		{ m_PhysicalArmor = aValue; }
		void SetMagicalDamage( const unsigned int aValue )		{ m_MagicalDamage = aValue; }
		void SetMagicalArmor( const unsigned int aValue )		{ m_MagicalArmor = aValue; }

		void SetData( const ECharDataType aType, const unsigned int aAmount, const unsigned int aTime, unsigned int aExtraValue );

		const unsigned int IncreaseMaxHealth( const unsigned int aValue );
		const unsigned int IncreaseCurrentHealth( const unsigned int aValue );
		const unsigned int IncreaseMaxEnergy( const unsigned int aValue );
		const unsigned int IncreaseCurrentEnergy( const unsigned int aValue );

		const unsigned int IncreasePhysicalDamage( const unsigned int aValue );
		const unsigned int IncreasePhysicalArmor( const unsigned int aValue );
		const unsigned int IncreaseMagicalDamage( const unsigned int aValue );
		const unsigned int IncreaseMagicalArmor( const unsigned int aValue );

		const unsigned int DecreaseMaxHealth( const unsigned int aValue );
		const unsigned int DecreaseCurrentHealth( const unsigned int aValue );
		const unsigned int DecreaseMaxEnergy( const unsigned int aValue );
		const unsigned int DecreaseCurrentEnergy( const unsigned int aValue );

		const unsigned int DecreasePhysicalDamage( const unsigned int aValue );
		const unsigned int DecreasePhysicalArmor( const unsigned int aValue );
		const unsigned int DecreaseMagicalDamage( const unsigned int aValue );
		const unsigned int DecreaseMagicalArmor( const unsigned int aValue );

		bool AddState( const EStateType aType, unsigned int aRemoveTime, unsigned int aExtraValue )
		{
			CState	tempState;
			tempState.SetType( aType );
			tempState.SetRemoveTime( aRemoveTime );
			tempState.SetExtraValue( aExtraValue );

			return	( AddState( tempState ) );
		}

		bool AddState( const CState & aState )
		{
			std::list< CState >::iterator	sIter;

			for( sIter = m_StateList.begin() ; sIter != m_StateList.end() ; ++sIter )
			{
				if( sIter->GetType() == aState.GetType() )
				{
					sIter->SetRemoveTime( aState.GetRemoveTime() );
					sIter->SetExtraValue( aState.GetExtraValue() );
					return	true;
				}
			}

			if( sIter == m_StateList.end() )
			{
				m_StateList.push_back( aState );
			}
			
			return	true;
		}

		bool RemoveState( const EStateType aType, unsigned int aTime )
		{
			std::list< CState >::iterator	sIter;
			for( sIter = m_StateList.begin() ; sIter != m_StateList.end() ; ++sIter )
			{
				if( sIter->GetType() == aType )
				{
					break;
				}
			}

			if( m_StateList.end() != sIter  &&  (0 == aTime || sIter->GetRemoveTime() <= aTime ) )
			{
				m_StateList.erase( sIter );
				return	true;
			}

			return	false;
		}

		bool FindState( const EStateType aType ) const
		{
			for( auto sIter = m_StateList.begin() ; sIter != m_StateList.end() ; ++sIter )
			{
				if( sIter->GetType() == aType )
				{
					return	true;
				}
			}
			return	false;
		}


	private:

		ECharacterClass		m_Class;

		unsigned int		m_MaxHealth;
		unsigned int		m_CurrentHealth;
		unsigned int		m_MaxEnergy;
		unsigned int		m_CurrentEnergy;

		unsigned int		m_PhysicalDamage;
		unsigned int		m_PhysicalArmor;
		unsigned int		m_MagicalDamage;
		unsigned int		m_MagicalArmor;

		std::list< CState >		m_StateList;

	};

	void Encode( cOPacket& oPacket, const CCharacter& info );
	void Decode( cIPacket& iPacket, CCharacter& info );


	extern CCharacter	DefaultCharacter;
	extern CCharacter	BreakerCharacter;
	extern CCharacter	DefenderCharacter;
	extern CCharacter	MageCharacter;
	extern CCharacter	HealerCharacter;


	bool SetSampleCharacterInfo( void );
	bool InitializeCharacterInfo( CCharacter & aCharacter );
}



#endif		// __MATCHLESSCCHARACTER_H_f83re_9u324_fvj92__
