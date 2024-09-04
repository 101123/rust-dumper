#pragma once

#include "unity_lib.hpp"

namespace rust {
	namespace console_system {
		class command {
		public:
			uint64_t get() {
				static uint32_t offset = il2cpp::get_field_by_name( il2cpp::get_class_by_name( "ConsoleSystem/Command" ), "GetOveride" )->offset();
				
				if ( !_this )
					return 0;

				uint64_t func = *( uint64_t* )( _this + offset );
				if ( !func )
					return 0;

				return *( uint64_t* )( func + 0x10 );
			}

			uint64_t set() {
				static uint32_t offset = il2cpp::get_field_by_name( il2cpp::get_class_by_name( "ConsoleSystem/Command" ), "SetOveride" )->offset();

				if ( !_this )
					return 0;

				uint64_t action = *( uint64_t* )( _this + offset );
				if ( !action )
					return 0;

				return *( uint64_t* )( action + 0x18 );
			}

			uint64_t call() {
				static uint32_t offset = il2cpp::get_field_by_name( il2cpp::get_class_by_name( "ConsoleSystem/Command" ), "Call" )->offset();

				if ( !_this )
					return 0;

				uint64_t action = *( uint64_t* )( _this + offset );
				if ( !action )
					return 0;

				return *( uint64_t* )( action + 0x18 );
			}
		};

		class client {
		public:
			static command* find( system_c::string_t* str ) {
				static command*( *console_system_index_client_find )( system_c::string_t* ) =
					( decltype( console_system_index_client_find ) )il2cpp::get_method_by_name( il2cpp::get_class_by_name( "ConsoleSystem/Index/Client" ), "Find" )->get_fn_ptr<void*>();

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