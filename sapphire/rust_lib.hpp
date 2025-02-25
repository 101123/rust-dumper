#pragma once

#include "unity_lib.hpp"

namespace rust {
	class base_entity {

	};

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

	class model_state {
	public:
		enum e_flag {
			ducked = 1,
			jumped = 2,
			on_ground = 4,
			sleeping = 8,
			sprinting = 16,
			on_ladder = 32,
			flying = 64,
			aiming = 128,
			prone = 256,
			mounted = 512,
			relaxed = 1024,
			on_phone = 2048,
			crawling = 4096,
			loading = 8192,
			head_look = 16384,
			has_parachute = 32768,
			blocking = 65536,
			ragdolling = 131072
		};
	};
}