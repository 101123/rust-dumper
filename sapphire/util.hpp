#pragma once

#include <iostream>
#include <vector>

#include "hde/hde64.h"

#define is_valid_ptr( x ) !IsBadReadPtr( ( void* )x, 8 )
#define is_valid_read( x, s ) !IsBadReadPtr( ( void* )x, s )

namespace util {
	struct function_attributes_t {
		std::vector<uint64_t> calls;
		std::vector<uint64_t> jmps;
		size_t length;

		bool transfers_control_to( void* address );
	};

	function_attributes_t get_function_attributes( void* address, size_t limit );
}
