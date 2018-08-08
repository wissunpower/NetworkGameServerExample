#pragma once


#include	"cLog.h"


int InitLog();
int CloseLog();

int WriteLog( const std::initializer_list< tstring >& sl, const std::initializer_list< eLogInfoType >& tl = { eLogInfoType::LOG_INFO_LOW } );
int WriteLog( const tstring& s, const std::initializer_list< eLogInfoType >& tl = { eLogInfoType::LOG_INFO_LOW } );
