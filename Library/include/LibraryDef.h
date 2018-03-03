#pragma once



static const int PACKET_SIZE_LENGTH = 4;


enum class eOperationType
{
	OP_CLOSE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND,
	OP_RECVPACKET,
	OP_SYSTEM,
};


typedef struct _INITCONFIG
{
	int		nIndex;
	SOCKET	sockListener;
	int		nRecvBufCnt;
	int		nSendBufCnt;
	int		nRecvBufSize;
	int		nSendBufSize;
	int		nProcessPacketCnt;
	int		nServerPort;
	int		nWokerThreadCnt;
	int		nProcessThreadCnt;
} INITCONFIG;


typedef struct _OVERLAPPED_EX
{
	WSAOVERLAPPED	s_Overlapped;
	WSABUF			s_WsaBuf;
	int				s_nTotalBytes;
	DWORD			s_dwRemain;
	char*			s_lpSocketMsg;
	eOperationType	s_eOperation;
	void*			s_lpConnection;
	_OVERLAPPED_EX( void* lpConnection )
		: _OVERLAPPED_EX {}
	{
		s_lpConnection = lpConnection;
	}
} OVERLAPPED_EX, *LPOVERLAPPED_EX;


typedef struct _PROCESSPACKET
{
	eOperationType	s_eOperationType;
	WPARAM			s_wParam;
	LPARAM			s_lParam;
} PROCESSPACKET, *LPPROCESSPACKET;
