#pragma once


#define		WM_SOCKETMSG	( WM_USER + 1 )


struct CChatClientDlg
{

public:
	void OutputMsg( const char* szOutputString, ... ) {}

};


class cAsyncSocket
{

public:
	cAsyncSocket();
	~cAsyncSocket();

	bool InitSocket( HWND hWnd );
	void CloseSocket( SOCKET socketClose, bool bIsForce = false );

	bool ConnectTo( std::wstring szIP, int nPort );
	int SendMsg( std::vector<char> msg, int nLen );
	int SendMsg( char*, int nLen );

	void SetMainDlg( CChatClientDlg* pMainDlg ) { m_pMainDlg = pMainDlg; }

private:
	SOCKET	m_socket;
	HWND	m_hWnd;
	CChatClientDlg*	m_pMainDlg;
	std::vector<char>	m_vSocketBuf;

};
