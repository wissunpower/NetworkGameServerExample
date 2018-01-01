
#ifndef		__SERVERAPPROOT_H_92mxm_vmd9w_82kd9__
#define		__SERVERAPPROOT_H_92mxm_vmd9w_82kd9__



#include	<list>
#include	<map>

#include	<winsock2.h>

#include	"ErrorHandler.h"
#include	"CMiniDump.h"

#include	"MatchlessProtocol.h"
#include	"MatchlessCClient.h"
#include	"MatchlessCSkill.h"
#include	"MatchlessCState.h"

#include	"MatchlessServerCTimer.h"



#define		BUFSIZE		512



namespace	MatchlessServer
{
	struct SkillMessageInfo
	{
		unsigned int	m_Caster;
		unsigned int	m_Target;
		unsigned int	m_SkillKind;
	};
}



extern CRITICAL_SECTION									g_CS;

extern MatchlessServer::CTimer							g_Timer;

extern std::list< unsigned int >						g_ReuseClientIDlist;
extern unsigned int										g_LargestClientID;
extern std::map< unsigned int, Matchless::CClient >		g_ClientList;			// < ID, SOCKET >
extern std::map< unsigned short int, int >				g_TeamPlayerNumMap;
extern bool												g_IsGameStartable;
extern bool												g_IsAcceptable;
extern unsigned int										g_CurrentMapKind;

extern std::map< unsigned int, MatchlessServer::SkillMessageInfo >		g_SkillMessageList;		// < Tick, SkillInfo >



DWORD WINAPI TimerThread( LPVOID arg );
DWORD WINAPI GameProcessThread( LPVOID arg );

unsigned int GetClientID( void );
int ReturnClientID( const unsigned int aID );

int OutputServerInitialInfo( const SOCKADDR_IN & aAddrInfo, const SOCKET aListenSocket );
bool DoNeedRoomMaster( void );
int ChangeTeamPlayerNum( const unsigned short int aBefore, const unsigned short int aAfter );
//bool IsGameStartable( void );
bool IsGameFinish( void );
bool HandleSkillRequest( const bool aIsCastStart, const Matchless::ECharacterSkill aSkillKind, Matchless::CClient & aCaster, Matchless::CClient & aTarget );
bool IsNowCasting( const unsigned int aID, const bool aIsCancel );

int SendDataFSV( SOCKET socket, int aFlags, unsigned int aType, unsigned int aAddDataLen, const char * const apAddData );

bool PrintPacket( const CNetMessage & msg );




#endif		// __SERVERAPPROOT_H_92mxm_vmd9w_82kd9__
