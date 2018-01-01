
#ifndef		__NETWORKHANDLER_H_fj923_03293_Fj932__
#define		__NETWORKHANDLER_H_fj923_03293_Fj932__



#include		"ClientAppRoot.h"



DWORD WINAPI NetReceiveProcess( LPVOID arg );
int ChangeAndInformMainStepState( Matchless::CClient & aDestModule, const Matchless::EMainStepState & aSrc );
bool HandleSkillWork( const Matchless::ECharacterSkill aSkillKind, const unsigned int aCasterID, const unsigned int aTargetID );



#endif		// __NETWORKHANDLER_H_fj923_03293_Fj932__
