#pragma once


#include "linkopt.h"
#include <vector>


const unsigned int PACKET_INIT_LENGTH = 1024;


class NETLIB_API cOPacket
{
public:
	cOPacket( const unsigned int bufSize = PACKET_INIT_LENGTH );

	void EncodeBool( const bool b );
	void Encode1( const char c );
	void Encode4( const int n );
	void Encode4u( const unsigned int n );
	void Encode4f( const float f );
	void Encode8d( const double d );
	void EncodeStr( const std::string& s );
	void EncodeStr( const std::wstring& s );
	void EncodeTM( const tm& data );

	int Send( const SOCKET socket, const int flags = 0 );

private:
	void Encode( const unsigned int size, const void* pData );

private:
	unsigned int			m_nIndex;
	std::vector< char >		m_vBuffer;
};


class NETLIB_API cIPacket
{
public:
	//cIPacket( const unsigned int bufSize = PACKET_INIT_LENGTH );

	bool DecodeBool();
	char Decode1();
	int Decode4();
	unsigned int Decode4u();
	float Decode4f();
	double Decode8d();
	std::string DecodeStr( const std::string& s = std::string{} );
	std::wstring DecodeStr( const std::wstring s = std::wstring{} );
	tm DecodeTM();

	int Recv( const SOCKET socket, const int flags = 0 );

	void ResetIndex();
	void ResetIndex( const unsigned int index );
	void SetBuffer( const unsigned int size, const char* pData );

	bool IsRemain() const;

private:
	void* Decode( const unsigned int size );

private:
	unsigned int			m_nIndex = 0;
	std::vector< char >		m_vBuffer;
};
