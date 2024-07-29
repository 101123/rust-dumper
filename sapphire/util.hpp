#pragma once

#include <vector>

namespace util {
	struct function_attributes_t {
		std::vector<uint64_t> calls;
		std::vector<uint64_t> jmps;
		size_t length;

		bool transfers_control_to( void* address );
	};

	function_attributes_t get_function_attributes( void* address, size_t limit );
}
