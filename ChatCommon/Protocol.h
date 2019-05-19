#pragma once

//extern const int MAX_CHATMSG	= 300;
//extern const int MAX_IP			= 20;
#define		MAX_CHATMSG		300
#define		MAX_IP			20


enum ePacket
{
	PACKET_CHAT				/*= 1*/,
};

struct Packet_Chat
{
	unsigned int		s_nLength;
	unsigned int		s_sType;
	TCHAR				s_szIP[ MAX_IP ];
	TCHAR				s_szChatMsg[ MAX_CHATMSG ];
};