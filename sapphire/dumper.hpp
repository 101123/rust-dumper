#pragma once

#include "il2cpp_lib.hpp"
#include "unity_lib.hpp"
#include "rust_lib.hpp"

#include <windows.h>

namespace dumper
{
	void produce( );
	void write_game_assembly();

	extern FILE* outfile_handle;
	extern uint64_t game_base;

	char* clean_klass_name( const char* klass_name );
	void write_to_file( const char* format, ... );

	__forceinline uint32_t get_image_size( uint8_t* image ) {
		PIMAGE_DOS_HEADER dos_header = ( PIMAGE_DOS_HEADER ) ( image );
		PIMAGE_NT_HEADERS nt_headers = ( PIMAGE_NT_HEADERS ) ( image + dos_header->e_lfanew );
		return nt_headers->OptionalHeader.SizeOfImage;
	}

	__forceinline BYTE* relative_32( BYTE* inst, uint32_t offset ) {
		int32_t rel_offset = *( int32_t* ) ( inst + offset );
		return ( inst + rel_offset + offset + sizeof( int32_t ) );
	}

	__forceinline bool compare_pattern( uint8_t* base, uint8_t* pattern, size_t mask ) {
		for ( ; mask; ++base, ++pattern, mask-- ) {
			if ( *pattern != 0xCC && *base != *pattern ) {
				return false;
			}
		}

		return true;
	}

	__forceinline uint8_t* find_pattern( uint8_t* base, size_t size, uint8_t* pattern, size_t mask ) {
		size -= mask;

		for ( SIZE_T i = 0; i <= size; ++i ) {
			uint8_t* addr = &base[ i ];

			if ( compare_pattern( addr, pattern, mask ) ) {
				return addr;
			}
		}

		return nullptr;
	}

	__forceinline uint8_t* find_pattern_image( uint8_t* image, uint8_t* pattern, size_t mask ) {
		PIMAGE_DOS_HEADER dosHeader = ( PIMAGE_DOS_HEADER ) ( image );
		PIMAGE_NT_HEADERS ntHeaders = ( PIMAGE_NT_HEADERS ) ( image + dosHeader->e_lfanew );

		uint8_t* sectionBase = ( uint8_t* ) &ntHeaders->OptionalHeader + ntHeaders->FileHeader.SizeOfOptionalHeader;

		for ( uint32_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++ ) {
			PIMAGE_SECTION_HEADER section =
				( PIMAGE_SECTION_HEADER ) ( sectionBase + ( i * sizeof( IMAGE_SECTION_HEADER ) ) );

			if ( ( section->Characteristics & IMAGE_SCN_MEM_EXECUTE ) == 0 ||
				( section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE ) != 0 ) {
				continue;
			}

			uint32_t len = max( section->SizeOfRawData, section->Misc.VirtualSize );
			uint8_t* result = find_pattern( image + section->VirtualAddress, len, pattern, mask );

			if ( result ) {
				return result;
			}
		}

		return nullptr;
	}
}

#define FIND_PATTERN( base, size, sig ) dumper::find_pattern( ( uint8_t* ) base, size, ( uint8_t* ) sig, sizeof( sig ) - 1 )
#define FIND_PATTERN_IMAGE( base, sig ) dumper::find_pattern_image( ( uint8_t* ) base, ( uint8_t* ) sig, sizeof( sig ) - 1 )