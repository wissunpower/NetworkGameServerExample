// ChatClientDemo.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ChatClientDemo.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

const size_t	CHATSTR_MAX = 200;
TCHAR			g_ChatStr[ CHATSTR_MAX ];
HWND			g_ChatInputEdit;
HWND			g_ChatOutputEdit;
cAsyncSocket	g_AsyncSocket;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT				OnSocketMsg( WPARAM wParam, LPARAM lParam );
void				ProcessPacket( char* pRecvBuf, int nRecvLen );
void				OutputMsg( LPCTSTR szOutputString, ... );

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHATCLIENTDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENTDEMO));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENTDEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHATCLIENTDEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			g_ChatInputEdit = CreateWindow( _T( "edit" ), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 10, 400, 25, hWnd, (HMENU)ID_CHATINPUTEDIT, hInst, NULL );
			g_ChatOutputEdit = CreateWindow( _T( "edit" ), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY, 10, 80, 400, 25, hWnd, (HMENU)ID_CHATINPUTEDIT, hInst, NULL );
			g_AsyncSocket.InitSocket( hWnd );
			g_AsyncSocket.ConnectTo( std::wstring( L"127.0.0.1" ), 8080 );
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

			case ID_CHATINPUTEDIT:
				switch ( HIWORD( wParam ) )
				{
				case EN_CHANGE:
					GetWindowText( g_ChatInputEdit, g_ChatStr, CHATSTR_MAX );
					SetWindowText( hWnd, g_ChatStr );
				}
				break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_RETURN:
			{
				Packet_Chat		Chat;
				Chat.s_nLength = sizeof( Packet_Chat );
				Chat.s_sType = PACKET_CHAT;
				_tcscpy_s( Chat.s_szIP, _T( "127.0.0.1" ) );
				_tcsncpy_s( Chat.s_szChatMsg, g_ChatStr, CHATSTR_MAX );
				g_AsyncSocket.SendMsg( reinterpret_cast<char*>( &Chat ), sizeof( Chat ) );
			}
			break;
		}
		InvalidateRect( hWnd, NULL, TRUE );
		break;
	case WM_SOCKETMSG:
		OnSocketMsg( wParam, lParam );
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT OnSocketMsg( WPARAM wParam, LPARAM lParam )
{
	const int	MAX_SOCKBUF = 1024;
	SOCKET		sock = static_cast<SOCKET>( wParam );
	char		szSocketBuf[ MAX_SOCKBUF * 4 ];
	static int	nRestRecvLen = 0;

	int nError = WSAGETSELECTERROR( lParam );
	if ( 0 != nError )
	{
		OutputMsg( _T( "[에러] WSAGETSELECTERROR : %d " ), nError );
		g_AsyncSocket.CloseSocket( sock );
		return false;
	}

	int nEvent = WSAGETSELECTEVENT( lParam );
	switch ( nEvent )
	{
	case FD_READ:
		{
			int nRecvLen = recv( sock, szSocketBuf + nRestRecvLen, MAX_SOCKBUF, 0 );
			if ( 0 == nRecvLen )
			{
				OutputMsg( _T( "[CLOSE] 클라이언트와 연결이 종료 되었습니다." ) );
				g_AsyncSocket.CloseSocket( sock );
				return false;
			}
			else if ( -1 == nRecvLen )
			{
				OutputMsg( _T( "[ERROR] recv 실패 : %d " ), WSAGetLastError() );
				g_AsyncSocket.CloseSocket( sock );
			}

			nRestRecvLen += nRecvLen;

			if ( nRestRecvLen < 4 )
			{
				return true;
			}

			int nMsgLen = 0;
			char* pCurrent = szSocketBuf;
			CopyMemory( &nMsgLen, pCurrent, sizeof( int ) );
			while ( nMsgLen <= nRestRecvLen )
			{
				ProcessPacket( szSocketBuf, nMsgLen );
				nRestRecvLen -= nMsgLen;
				if ( nRestRecvLen <= 0 )
				{
					break;
				}

				pCurrent += nMsgLen;
				CopyMemory( &nMsgLen, pCurrent, sizeof( int ) );
			}
			MoveMemory( szSocketBuf, pCurrent, nRestRecvLen );
		}
		break;

	case FD_CLOSE:
		{
			OutputMsg( _T( "[CLOSE] 클라이언트 접속 종료 : SOCKET(%d)" ), sock );
			g_AsyncSocket.CloseSocket( sock );
		}
		break;
	}

	return true;
}

void ProcessPacket( char* pRecvBuf, int nRecvLen )
{
	unsigned int uType;

	CopyMemory( &uType, pRecvBuf + sizeof( int ), sizeof( unsigned int ) );

	switch ( uType )
	{
	case PACKET_CHAT:
		{
			Packet_Chat* pChat = reinterpret_cast<Packet_Chat*>( pRecvBuf );
			OutputMsg( _T( "[%s] : %s" ), pChat->s_szIP, pChat->s_szChatMsg );
		}
		break;

	default:
		OutputMsg( _T( "[ERROR] 정의되지 않은 메시지 도착" ) );
		break;
	}
}

void OutputMsg( LPCTSTR szOutputString, ... )
{
	TCHAR szOutStr[ 1024 ];
	va_list argptr;
	va_start( argptr, szOutputString );
	_vstprintf_s( szOutStr, szOutputString, argptr );
	va_end( argptr );

	SetWindowText( g_ChatOutputEdit, szOutStr );
}
