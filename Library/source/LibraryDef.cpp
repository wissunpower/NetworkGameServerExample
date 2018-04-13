
#include	"stdafx.h"
#include	"LibraryDef.h"


tstring Inet_Ntop( SOCKADDR_IN& addr )
{
	std::vector< tchar > buf( 32 );
	InetNtop( addr.sin_family, &addr.sin_addr, buf.data(), buf.size() );
	tstring str { buf.data() };
	return str;
}
