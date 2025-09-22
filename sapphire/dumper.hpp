#pragma once

#include "il2cpp_lib.hpp"
#include "unity_lib.hpp"
#include "rust_lib.hpp"
#include "hook_lib.hpp"

#include <windows.h>

namespace dumper
{
	void produce();
	void produce_unity();
	void write_game_assembly();

	void dump_call( uint64_t function, uint32_t limit, uint32_t depth = 0 );

	extern FILE* outfile_handle;
	extern FILE* outfile_log_handle;
	extern uint64_t game_base;
	extern uint64_t unity_base;
	extern il2cpp::il2cpp_class_t** type_info_definition_table;

	extern bool resolve_type_info_definition_table();
	extern int get_class_type_definition_index( il2cpp::il2cpp_class_t* klass );

	char* clean_klass_name( const char* klass_name );
	char* clean_inner_klass_name( il2cpp::il2cpp_class_t* klass );
	void write_to_file( const char* format, ... );
	void write_to_log( const char* format, ... );
	void flush();

	extern uint64_t start_write_value;

	void dump_protobuf_methods( il2cpp::il2cpp_class_t* klass );

	void dump_projectile_shoot( il2cpp::il2cpp_object_t* object );
	void dump_player_projectile_update( il2cpp::il2cpp_object_t* object );
	void dump_player_projectile_attack( il2cpp::il2cpp_object_t* object );

	long exception_handler( _EXCEPTION_POINTERS* exception_info );

	__forceinline uint32_t get_image_size( uint8_t* image ) {
		PIMAGE_DOS_HEADER dos_header = ( PIMAGE_DOS_HEADER ) ( image );
		PIMAGE_NT_HEADERS nt_headers = ( PIMAGE_NT_HEADERS ) ( image + dos_header->e_lfanew );
		return nt_headers->OptionalHeader.SizeOfImage;
	}

	__forceinline uint8_t* relative_32( uint8_t* inst, uint32_t offset ) {
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

	struct image_section_info_t {
		image_section_info_t( uint64_t _address, uint32_t _size ) :
			address( _address ), size( _size ) {};

		uint64_t address;
		uint32_t size;
	};

	__forceinline image_section_info_t get_section_info( uint8_t* image, const char* name ) {
		PIMAGE_DOS_HEADER dosHeader = ( PIMAGE_DOS_HEADER )( image );
		PIMAGE_NT_HEADERS ntHeaders = ( PIMAGE_NT_HEADERS )( image + dosHeader->e_lfanew );

		uint8_t* sectionBase = ( uint8_t* )&ntHeaders->OptionalHeader + ntHeaders->FileHeader.SizeOfOptionalHeader;

		for ( uint32_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++ ) {
			PIMAGE_SECTION_HEADER section =
				( PIMAGE_SECTION_HEADER )( sectionBase + ( i * sizeof( IMAGE_SECTION_HEADER ) ) );

			char* section_name = ( char* )section->Name;
			if ( !strcmp( section_name, name ) ) {
				uint64_t address = ( uint64_t )image + section->VirtualAddress;
				uint32_t size = section->Misc.VirtualSize;

				return image_section_info_t( address, size );
			}
		}

		return image_section_info_t( 0, 0 );
	}

	template <typename T>
	__forceinline uint64_t find_value_in_data_section( T value ) {
		image_section_info_t data_section = get_section_info( ( uint8_t* )game_base, ".data" );
		if ( !data_section.address || !data_section.size )
			return 0;

		uint64_t current_address = data_section.address;
		uint64_t end_address = data_section.address + data_section.size;

		while ( current_address < end_address ) {
			uint64_t _value = *( uint64_t* )( current_address );
			if ( _value == ( uint64_t )value ) {
				return current_address;
			}

			current_address += 8;
		}

		return 0;
	}
}

#define FIND_PATTERN( base, size, sig ) dumper::find_pattern( ( uint8_t* ) base, size, ( uint8_t* ) sig, sizeof( sig ) - 1 )
#define FIND_PATTERN_IMAGE( base, sig ) dumper::find_pattern_image( ( uint8_t* ) base, ( uint8_t* ) sig, sizeof( sig ) - 1 )