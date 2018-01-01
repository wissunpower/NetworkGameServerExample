
#ifndef		__MATCHLESSSEFFECT_H_f92lq_qoe92_5472j__
#define		__MATCHLESSSEFFECT_H_f92lq_qoe92_5472j__



#include	"MatchlessRoot.h"



namespace	Matchless
{
	struct SEffect
	{

	public:

		enum EPositionType
		{
			EPT_Idle,

			EPT_Active,
			EPT_Character,
			EPT_Area,

			EPT_TotalCount
		};


	public:

		unsigned int		m_Type1;
		unsigned int		m_Type2;

		unsigned int		m_CharacterID;

		unsigned int		m_StartTick;
		unsigned int		m_EndTick;
		unsigned int		m_CurrentTick;

		EPositionType		m_PosType;

		float				m_StartPosX;
		float				m_StartPosY;
		float				m_StartPosZ;

		float				m_EndPosX;
		float				m_EndPosY;
		float				m_EndPosZ;

	};
}





#endif		// __MATCHLESSCEFFECT_H_f92lq_qoe92_5472j__
