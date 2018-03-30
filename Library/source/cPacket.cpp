
#include "stdafx.h"
#include "cPacket.h"
#include "LibraryDef.h"
#include "cException.h"


static int recvn( SOCKET s, char * buf, int len, int flags )
{
	int received;
	char * ptr = buf;
	int left = len;


	while( left > 0 )
	{
		received = recv( s, ptr, left, flags );

		if( received == SOCKET_ERROR )
			return SOCKET_ERROR;
		else if( received == 0 )
			break;

		left -= received;
		ptr += received;
	}

	return (len - left);
}


cOPacket::cOPacket( const unsigned int bufSize /*= PACKET_INIT_LENGTH*/ )
	: m_nIndex { PACKET_SIZE_LENGTH }
{
	m_vBuffer.resize( bufSize );
}

void cOPacket::EncodeBool( const bool b )
{
	Encode( 1, &b );
}

void cOPacket::Encode1( const char c )
{
	Encode( 1, &c );
}

void cOPacket::Encode4( const int n )
{
	Encode( 4, &n );
}

void cOPacket::Encode4u( const unsigned int n )
{
	Encode( 4, &n );
}

void cOPacket::Encode4f( const float f )
{
	Encode( 4, &f );
}

void cOPacket::Encode8d( const double d )
{
	Encode( 8, &d );
}

void cOPacket::EncodeStr( const std::string& s )
{
	Encode4u( s.length() );
	Encode( sizeof( std::string::value_type ) * s.length(), s.c_str() );
}

void cOPacket::EncodeStr( const std::wstring& s )
{
	Encode4u( s.length() );
	Encode( sizeof( std::wstring::value_type ) * s.length(), s.c_str() );
}

int cOPacket::Send( const SOCKET socket, const int flags /*= 0*/ )
{
	memcpy_s( m_vBuffer.data(), PACKET_SIZE_LENGTH, &m_nIndex, PACKET_SIZE_LENGTH );
	return send( socket, m_vBuffer.data(), m_nIndex, flags );
}

void cOPacket::Encode( const unsigned int size, const void* pData )
{
	if ( size + m_nIndex > m_vBuffer.size() )
	{
		std::vector< char > temp( size );
		memcpy_s( temp.data(), size, pData, size );
		m_vBuffer.insert( m_vBuffer.end(), temp.begin(), temp.end() );
	}
	else
	{
		memcpy_s( m_vBuffer.data() + m_nIndex, size, pData, size );
	}

	m_nIndex += size;
}


bool cIPacket::DecodeBool()
{
	unsigned int nDecodeSize = 1;
	void* pData = Decode( nDecodeSize );

	bool b;
	memcpy_s( &b, nDecodeSize, pData, nDecodeSize );
	return b;
}

char cIPacket::Decode1()
{
	unsigned int nDecodeSize = 1;
	void* pData = Decode( nDecodeSize );

	char c;
	memcpy_s( &c, nDecodeSize, pData, nDecodeSize );
	return c;
}

int cIPacket::Decode4()
{
	unsigned int nDecodeSize = 4;
	void* pData = Decode( nDecodeSize );

	int i;
	memcpy_s( &i, nDecodeSize, pData, nDecodeSize );
	return i;
}

unsigned int cIPacket::Decode4u()
{
	unsigned int nDecodeSize = 4;
	void* pData = Decode( nDecodeSize );

	unsigned int u;
	memcpy_s( &u, nDecodeSize, pData, nDecodeSize );
	return u;
}

float cIPacket::Decode4f()
{
	unsigned int nDecodeSize = 4;
	void* pData = Decode( nDecodeSize );

	float f;
	memcpy_s( &f, nDecodeSize, pData, nDecodeSize );
	return f;
}

double cIPacket::Decode8d()
{
	unsigned int nDecodeSize = 8;
	void* pData = Decode( nDecodeSize );

	double d;
	memcpy_s( &d, nDecodeSize, pData, nDecodeSize );
	return d;
}

std::string cIPacket::DecodeStr( const std::string& s /*= std::string{}*/ )
{
	unsigned int nStrLen = Decode4u();
	void* pData = Decode( sizeof( std::string::value_type ) * nStrLen );

	return std::string { reinterpret_cast<std::string::pointer>( pData ), nStrLen };
}

std::wstring cIPacket::DecodeStr( const std::wstring s /*= std::wstring{}*/ )
{
	unsigned int nStrLen = Decode4u();
	void* pData = Decode( sizeof( std::wstring::value_type ) * nStrLen );

	return std::wstring { reinterpret_cast<std::wstring::pointer>( pData ), nStrLen };
}

int cIPacket::Recv( const SOCKET socket, const int flags /*= 0*/ )
{
	unsigned int nPacketSize = 0;

	int ret = recvn( socket, reinterpret_cast<char*>( &nPacketSize ), PACKET_SIZE_LENGTH, flags );
	if ( SOCKET_ERROR == ret || PACKET_SIZE_LENGTH > nPacketSize )
	{
		return SOCKET_ERROR;
	}

	m_nIndex = 0;
	m_vBuffer.resize( nPacketSize );

	memcpy_s( m_vBuffer.data() + m_nIndex, PACKET_SIZE_LENGTH, &nPacketSize, PACKET_SIZE_LENGTH );
	m_nIndex += PACKET_SIZE_LENGTH;

	ret = recvn( socket, m_vBuffer.data() + m_nIndex, m_vBuffer.size() - m_nIndex, flags );
	if ( SOCKET_ERROR == ret )
	{
		return SOCKET_ERROR;
	}

	return nPacketSize;
}

void cIPacket::ResetIndex()
{
	m_nIndex = PACKET_SIZE_LENGTH;
}

void cIPacket::ResetIndex( const unsigned int index )
{
	m_nIndex = index;
}

void cIPacket::SetBuffer( const unsigned int size, const char* pData )
{
	if ( size < PACKET_SIZE_LENGTH )
	{
		throw cException_PacketNotEnoughData {};
	}

	m_nIndex = 0;
	m_vBuffer.resize( size );

	unsigned int nCopySize = PACKET_SIZE_LENGTH;
	memcpy_s( m_vBuffer.data() + m_nIndex, nCopySize, pData + m_nIndex, nCopySize );
	m_nIndex += nCopySize;

	nCopySize = size - PACKET_SIZE_LENGTH;
	memcpy_s( m_vBuffer.data() + m_nIndex, nCopySize, pData + m_nIndex, nCopySize );
	m_nIndex += nCopySize;
}

bool cIPacket::IsRemain() const
{
	return ( m_nIndex < m_vBuffer.size() );
}

void* cIPacket::Decode( const unsigned int size )
{
	if ( size + m_nIndex > m_vBuffer.size() )
	{
		// throw exception ...
		throw cException_PacketDecodeUnderflow {};
		//return nullptr;
	}

	const unsigned int nOldIndex = m_nIndex;

	m_nIndex += size;

	return m_vBuffer.data() + nOldIndex;
}
