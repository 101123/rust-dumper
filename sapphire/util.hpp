#pragma once

#include <vector>

#include "hde/hde64.h"

#define IsValidPtr( x ) !IsBadReadPtr( ( void* )x, 8 )

namespace util {
	struct function_attributes_t {
		std::vector<uint64_t> calls;
		std::vector<uint64_t> jmps;
		size_t length;

		bool transfers_control_to( void* address );
	};

	function_attributes_t get_function_attributes( void* address, size_t limit );
}
