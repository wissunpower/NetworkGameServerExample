
#ifndef		__MATCHLESSPROTOCOL_H_83hs9_f98h2_99h23__
#define		__MATCHLESSPROTOCOL_H_83hs9_f98h2_99h23__



#include	"CNetMessage.h"



#define		MATCHLESS_SERVER_PORT		0xF12E
#define		MATCHLESS_DBSERVER_PORT		0xF32E



namespace	Matchless
{
	enum ENetMessageType
	{
		ENMT_Idle,


		// FSTC : from server to client
		// FCTS : from client to server

		// message type							// additional data
		FSTC_LOGIN_SUCCEED,						// unsigned int ID, bool RoomMaster, unsigned short int Team No, ECharacterClass CC.
		FSTC_LOGIN_FAILED,						// No exist.

		FSTC_INFORM_ANOTHERCLIENT_ENTER,		// unsigned int ID, bool RoomMaster, unsigned short int Team No, ECharacterClass CC.
		FSTC_INFORM_ANOTHERCLIENT_LEAVE,		// unsigned int ID

		FCTS_MSS_UPDATE,						// unsigned int MSS

		FCTS_CHARCLASS_UPDATE,					// (Matchless::ECharacterClass)(unsigned int CharacterClass)
		FSTC_CHARCLASS_UPDATE,					// unsigned int ID, (Matchless::ECharacterClass)(unsigned int CharacterClass)

		FCTS_TEAM_UPDATE,						// unsigned short int TeamNum
		FSTC_TEAM_UPDATE,						// unsigned int ID, unsigned short int TeamNum ( if it is 0, than mean request is failed. )

		FCTS_MAP_UPDATE,						// unsigned short int map ID
		FSTC_MAP_UPDATE,						// unsigned int ID, unsigned short int map ID ( if it is 0, than mean request is failed. )

		FSTC_STARTABLE,							// bool bOnOff

		FCTS_GAMESTART_REQUEST,					// No exist.

		FSTC_GAMESTART_SUCCEED,					// Connected client list
		FSTC_GAMESTART_FAILED,					// No exist.

		FSTC_INFORM_CLIENTINFO,					// Matchless::CClient client information.

		FCTS_GAME_MOVE_POSITION,				// float [16] 4 by 4 matrix
		FSTC_GAME_MOVE_POSITION,				// unsigned int ID, float [16] 4 by 4 matrix
		FCTS_GAME_MOVE_ANIMATION,				// < UINT, SAniTrackInfo > type list
		FSTC_GAME_MOVE_ANIMATION,				// unsigned int ID, < UINT, SAniTrackInfo > type list
		FCTS_GAME_MOVE_ALL_REQUEST,

		FCTS_GAME_SKILL_REQUEST,				// unsigned int Skill ID, target list
		FSTC_GAME_SKILL_FAILED,					// No exist.
		FSTC_GAME_SKILL_CASTSTART,				// unsigned int cast time
		FSTC_GAME_SKILL_APPLY,					// unsigned int caster ID, unsigned int target ID, unsigned int Skill ID
		FSTC_GAME_SKILL_CANCEL,

		FSTC_GAME_CHAR_UPDATE,					// unsigned int ID, Character data type, Character data amount

		FCTS_CHAT,								// char msg[ length ]
		FSTC_CHAT,								// unsigned int ID, char msg[ length ]

		FCTS_GAMEOUT_REQUEST,					// No exist.
		FSTC_GAMEOUT,							// bool bWon

		FCTS_LOGOUT_INFORM,						// No exist.


		ENMT_TotalCount
	};
}



#endif		// __MATCHLESSPROTOCOL_H_83hs9_f98h2_99h23__
