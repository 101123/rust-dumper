#pragma once
#include "sapphire_crt.hpp"
 
class String
{
public:
	char zpad[ 0x10 ]{ };
	int size{ };
	wchar_t buffer[ 128 + 1 ];
public:
	String( const wchar_t* st )
	{
		size = min( utl::crt::string::wcslen( st ), 128 );
		for ( int idx = 0; idx < size; idx++ )
		{
			buffer[ idx ] = st[ idx ];
		}
		buffer[ size ] = 0;
	}
};

template<typename T>
class Array
{
public:
	uint32_t Count( )
	{
		return *( int* )( uint64_t( this ) + 0x18 );
	}

	T Get( uint32_t Index )
	{
		return *( T* )( uint64_t( this ) + 0x20 + Index * sizeof( uint64_t ) );
	}
};

template < typename T >
class List
{
private:
	char pad_00[ 0x10 ];
	DWORD64 base;
	UINT32 count;

public:
	UINT32 Count( )
	{
		if ( !this || !base )
			return 0;

		return count;
	}

	T Get( UINT32 Index )
	{
		if ( !this || !base )
			return T( );

		return *( T* )( base + 0x20 + Index * sizeof( DWORD64 ) );
	}
};

template<typename T>
class BufferList
{
	uint8_t pad_0x10[ 0x10 ]{ };
public:
	uint32_t Count;
	Array<T>* Buffer;

	T Get( uint32_t index )
	{
		return Buffer->Get( index );
	}
};

template<typename KeyType, typename ValueType>
class ListDictionary
{
	uint8_t pad_0x10[ 0x10 ]{ };
public:
	uint64_t Key2Idx;
	uint64_t Idx2Key;
	BufferList<KeyType>* Keys;
	BufferList<ValueType>* Vals;
};