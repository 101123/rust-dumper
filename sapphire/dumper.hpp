#pragma once
#include "il2cpp_lib.hpp"

namespace dumper
{
	void produce( );

	extern FILE* outfile_handle;
	extern uint64_t game_base;

	char* clean_klass_name( const char* klass_name );
	void write_to_file( const char* format, ... );

	namespace components
	{
		void dump_typeinfo( uint64_t base_addr, il2cpp::Il2CppClass* klass );
	}
}