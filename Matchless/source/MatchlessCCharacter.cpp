
#include	"stdafx.h"
#include	"MatchlessCCharacter.h"
#include	"cPacket.h"


Matchless::CCharacter & Matchless::CCharacter::operator=( const CCharacter & src )
{
	if( this != (&src) )
	{
		m_Class = src.m_Class;

		m_MaxHealth = src.m_MaxHealth;
		m_CurrentHealth = src.m_CurrentHealth;
		m_MaxEnergy = src.m_MaxEnergy;
		m_CurrentEnergy = src.m_CurrentEnergy;

		m_PhysicalDamage = src.m_PhysicalDamage;
		m_PhysicalArmor = src.m_PhysicalArmor;
		m_MagicalDamage = src.m_MagicalDamage;
		m_MagicalArmor = src.m_MagicalArmor;

		//m_StateList.clear();
		//for( std::list< CState >::const_iterator sIter = src.m_StateList.begin() ; sIter != src.m_StateList.end() ; ++sIter )
		//{
		//	m_StateList.push_back( *sIter );
		//}
	}

	return	(*this);
}


void Matchless::CCharacter::SetData( const Matchless::ECharDataType aType, const unsigned int aAmount, const unsigned int aTime, unsigned int aExtraValue )
{
	switch( aType )
	{
		
	case ECDT_MaxHealth:
		( m_MaxHealth < aAmount ) ? IncreaseMaxHealth( aAmount - m_MaxHealth ) : DecreaseMaxHealth( m_MaxHealth - aAmount );
		break;
		
	case ECDT_CurrentHealth:
		( m_CurrentHealth < aAmount ) ? IncreaseCurrentHealth( aAmount - m_CurrentHealth ) : DecreaseCurrentHealth( m_CurrentHealth - aAmount );
		break;
		
	case ECDT_MaxEnergy:
		( m_MaxHealth < aAmount ) ? IncreaseMaxHealth( aAmount - m_MaxHealth ) : DecreaseMaxHealth( m_MaxHealth - aAmount );
		break;
		
	case ECDT_CurrentEnergy:
		( m_CurrentEnergy < aAmount ) ? IncreaseCurrentEnergy( aAmount - m_CurrentEnergy ) : DecreaseCurrentEnergy( m_CurrentEnergy - aAmount );
		break;

	case ECDT_PhysicalDamage:
		( m_PhysicalDamage < aAmount ) ? IncreasePhysicalDamage( aAmount - m_PhysicalDamage ) : DecreasePhysicalDamage( m_PhysicalDamage - aAmount );
		break;
		
	case ECDT_PhysicalArmor:
		( m_PhysicalArmor < aAmount ) ? IncreasePhysicalArmor( aAmount - m_PhysicalArmor ) : DecreasePhysicalArmor( m_PhysicalArmor - aAmount );
		break;
		
	case ECDT_MagicalDamage:
		( m_MagicalDamage < aAmount ) ? IncreaseMagicalDamage( aAmount - m_MagicalDamage ) : DecreaseMagicalDamage( m_MagicalDamage - aAmount );
		break;
		
	case ECDT_MagicalArmor:
		( m_MagicalArmor < aAmount ) ? IncreaseMagicalArmor( aAmount - m_MagicalArmor ) : DecreaseMagicalArmor( m_MagicalArmor - aAmount );
		break;

	case ECDT_InsertState:
		AddState( (EStateType)aAmount, aTime, aExtraValue );
		break;

	case ECDT_RemoveState:
		RemoveState( (EStateType)aAmount, aTime );
		break;

	}
}


const unsigned int Matchless::CCharacter::IncreaseMaxHealth( const unsigned int aValue )
{
	return	( m_MaxHealth += aValue );
}


const unsigned int Matchless::CCharacter::IncreaseCurrentHealth( const unsigned int aValue )
{
	return	( m_CurrentHealth = ( m_CurrentHealth > m_MaxHealth - aValue ) ? m_MaxHealth : m_CurrentHealth + aValue );
}


const unsigned int Matchless::CCharacter::IncreaseMaxEnergy( const unsigned int aValue )
{
	return	( m_MaxEnergy += aValue );
}


const unsigned int Matchless::CCharacter::IncreaseCurrentEnergy( const unsigned int aValue )
{
	return	( m_CurrentEnergy = ( m_CurrentEnergy > m_MaxEnergy - aValue ) ? m_MaxEnergy : m_CurrentEnergy + aValue );
}


const unsigned int Matchless::CCharacter::IncreasePhysicalDamage( const unsigned int aValue )
{
	return	( m_PhysicalDamage += aValue );
}


const unsigned int Matchless::CCharacter::IncreasePhysicalArmor( const unsigned int aValue )
{
	return	( m_PhysicalArmor += aValue );
}


const unsigned int Matchless::CCharacter::IncreaseMagicalDamage( const unsigned int aValue )
{
	return	( m_MagicalDamage += aValue );
}


const unsigned int Matchless::CCharacter::IncreaseMagicalArmor( const unsigned int aValue )
{
	return	( m_MagicalArmor += aValue );
}


const unsigned int Matchless::CCharacter::DecreaseMaxHealth( const unsigned int aValue )
{
	return	( m_MaxHealth = ( aValue > m_MaxHealth ) ? 0 : m_MaxHealth - aValue );
}


const unsigned int Matchless::CCharacter::DecreaseCurrentHealth( const unsigned int aValue )
{
	return	( m_CurrentHealth = ( aValue > m_CurrentHealth ) ? 0 : m_CurrentHealth - aValue );
}


const unsigned int Matchless::CCharacter::DecreaseMaxEnergy( const unsigned int aValue )
{
	return	( m_MaxEnergy = ( aValue > m_MaxEnergy ) ? 0 : m_MaxEnergy - aValue );
}


const unsigned int Matchless::CCharacter::DecreaseCurrentEnergy( const unsigned int aValue )
{
	return	( m_CurrentEnergy = ( aValue > m_CurrentEnergy ) ? 0 : m_CurrentEnergy - aValue );
}


const unsigned int Matchless::CCharacter::DecreasePhysicalDamage( const unsigned int aValue )
{
	return	( m_PhysicalDamage = ( aValue > m_PhysicalDamage ) ? 0 : m_PhysicalDamage - aValue );
}


const unsigned int Matchless::CCharacter::DecreasePhysicalArmor( const unsigned int aValue )
{
	return	( m_PhysicalArmor = ( aValue > m_PhysicalArmor ) ? 0 : m_PhysicalArmor - aValue );
}


const unsigned int Matchless::CCharacter::DecreaseMagicalDamage( const unsigned int aValue )
{
	return	( m_MagicalDamage = ( aValue > m_MagicalDamage ) ? 0 : m_MagicalDamage - aValue );
}


const unsigned int Matchless::CCharacter::DecreaseMagicalArmor( const unsigned int aValue )
{
	return	( m_MagicalArmor = ( aValue > m_MagicalArmor ) ? 0 : m_MagicalArmor - aValue );
}


void Matchless::Encode( cOPacket& oPacket, const CCharacter& info )
{
	oPacket.Encode4u( info.GetClass() );
	oPacket.Encode4u( info.GetMaxHealth() );
	oPacket.Encode4u( info.GetCurrentHealth() );
	oPacket.Encode4u( info.GetMaxEnergy() );
	oPacket.Encode4u( info.GetCurrentEnergy() );
	oPacket.Encode4u( info.GetPhysicalDamage() );
	oPacket.Encode4u( info.GetPhysicalArmor() );
	oPacket.Encode4u( info.GetMagicalDamage() );
	oPacket.Encode4u( info.GetMagicalArmor() );

	oPacket.Encode4u( static_cast<unsigned int>( info.GetStateList().size() ) );
	for ( auto state : info.GetStateList() )
	{
		Encode( oPacket, state );
	}
}

void Matchless::Decode( cIPacket& iPacket, CCharacter& info )
{
	info.SetClass( static_cast<ECharacterClass>( iPacket.Decode4u() ) );
	info.SetMaxHealth( iPacket.Decode4u() );
	info.SetCurrentHealth( iPacket.Decode4u() );
	info.SetMaxEnergy( iPacket.Decode4u() );
	info.SetCurrentEnergy( iPacket.Decode4u() );
	info.SetPhysicalDamage( iPacket.Decode4u() );
	info.SetPhysicalArmor( iPacket.Decode4u() );
	info.SetMagicalDamage( iPacket.Decode4u() );
	info.SetMagicalArmor( iPacket.Decode4u() );

	auto& stateList = info.GetStateList();
	stateList.clear();
	size_t nStateCount = iPacket.Decode4u();
	for( auto i = 0 ; i < nStateCount ; ++i )
	{
		CState state;
		Decode( iPacket, state );
		stateList.push_back( state );;
	}
}



Matchless::CCharacter	Matchless::DefaultCharacter;
Matchless::CCharacter	Matchless::BreakerCharacter;
Matchless::CCharacter	Matchless::DefenderCharacter;
Matchless::CCharacter	Matchless::MageCharacter;
Matchless::CCharacter	Matchless::HealerCharacter;



bool Matchless::SetSampleCharacterInfo( void )
{
	DefaultCharacter = CCharacter();

	BreakerCharacter.SetClass( ECC_Breaker );
	BreakerCharacter.SetMaxHealth( 100 );
	BreakerCharacter.SetCurrentHealth( 100 );
	BreakerCharacter.SetMaxEnergy( 300 );
	BreakerCharacter.SetCurrentEnergy( 300 );
	BreakerCharacter.SetPhysicalDamage( 5 );
	BreakerCharacter.SetPhysicalArmor( 2 );
	BreakerCharacter.SetMagicalDamage( 1 );
	BreakerCharacter.SetMagicalArmor( 2 );


	DefenderCharacter.SetClass( ECC_Defender );
	DefenderCharacter.SetMaxHealth( 100 );
	DefenderCharacter.SetCurrentHealth( 100 );
	DefenderCharacter.SetMaxEnergy( 300 );
	DefenderCharacter.SetCurrentEnergy( 300 );
	DefenderCharacter.SetPhysicalDamage( 3 );
	DefenderCharacter.SetPhysicalArmor( 3 );
	DefenderCharacter.SetMagicalDamage( 2 );
	DefenderCharacter.SetMagicalArmor( 2 );


	MageCharacter.SetClass( ECC_Mage );
	MageCharacter.SetMaxHealth( 100 );
	MageCharacter.SetCurrentHealth( 100 );
	MageCharacter.SetMaxEnergy( 300 );
	MageCharacter.SetCurrentEnergy( 300 );
	MageCharacter.SetPhysicalDamage( 1 );
	MageCharacter.SetPhysicalArmor( 1 );
	MageCharacter.SetMagicalDamage( 5 );
	MageCharacter.SetMagicalArmor( 2 );


	HealerCharacter.SetClass( ECC_Healer );
	HealerCharacter.SetMaxHealth( 100 );
	HealerCharacter.SetCurrentHealth( 100 );
	HealerCharacter.SetMaxEnergy( 300 );
	HealerCharacter.SetCurrentEnergy( 300 );
	HealerCharacter.SetPhysicalDamage( 1 );
	HealerCharacter.SetPhysicalArmor( 2 );
	HealerCharacter.SetMagicalDamage( 3 );
	HealerCharacter.SetMagicalArmor( 3 );


	return	true;
}


bool Matchless::InitializeCharacterInfo( Matchless::CCharacter & aCharacter )
{
	switch( aCharacter.GetClass() )
	{

	case Matchless::ECC_Breaker:
		aCharacter = BreakerCharacter;
		break;

	case Matchless::ECC_Defender:
		aCharacter = DefenderCharacter;
		break;

	case Matchless::ECC_Mage:
		aCharacter = MageCharacter;
		break;

	case Matchless::ECC_Healer:
		aCharacter = HealerCharacter;
		break;

	default:
		aCharacter = DefaultCharacter;
		break;

	}

	return	true;
}
