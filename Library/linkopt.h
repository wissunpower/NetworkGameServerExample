#pragma once


#ifdef NGS_LIBRARY_EXPORTS
#define NETLIB_API __declspec( dllexport )
#else	// NGS_LIBRARY_EXPORTS
#define NETLIB_API __declspec( dllimport )
#endif	// NGS_LIBRARY_EXPORTS
