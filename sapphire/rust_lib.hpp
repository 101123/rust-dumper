#pragma once

#include "unity_lib.hpp"

namespace rust {
	namespace console_system {
		inline size_t get_override_offset = 0;
		inline size_t set_override_offset = 0;
		inline size_t call_offset = 0;

		class command {
		public:
			uint64_t get() {
				if ( !_this )
					return 0;

				uint64_t func = *( uint64_t* )( _this + get_override_offset );
				if ( !func )
					return 0;

				return *( uint64_t* )( func + 0x10 );
			}

			uint64_t set() {
				if ( !_this )
					return 0;

				uint64_t action = *( uint64_t* )( _this + set_override_offset );
				if ( !action )
					return 0;

				return *( uint64_t* )( action + 0x18 );
			}

			uint64_t call() {
				if ( !_this )
					return 0;

				uint64_t action = *( uint64_t* )( _this + call_offset );
				if ( !action )
					return 0;

				return *( uint64_t* )( action + 0x18 );
			}
		};

		inline command*( *console_system_index_client_find )( system_c::string_t* ) = nullptr;

		class client {
		public:
			static command* find( system_c::string_t* str ) {
				return console_system_index_client_find( str );
			}
		};
	}

	class item_container {
	public:
		enum e_item_container_flag {
			is_player = 1,
			clothing = 2,
			belt = 4,
			single_type = 8,
			is_locked = 16,
			show_slots_on_icon = 32,
			no_broken_items = 64,
			no_item_input = 128,
			contents_hidden = 256
		};
	};
}