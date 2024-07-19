#pragma once

namespace system_c {
	class string_t {
		char zpad[ 0x10 ];
	public:
		int size;
		wchar_t str[ 256 + 1 ];
		string_t( const wchar_t* st ) {
			size = min( ( int )wcslen( ( wchar_t* )st ), 256 );
			for ( int i = 0; i < size; i++ ) {
				str[ i ] = st[ i ];
			}
			str[ size ] = 0;
		}
	};
}

namespace unity {
	class game_object_t {
	public:
		static game_object_t* create( system_c::string_t name ) {
			static void( *create_f )( uint64_t, system_c::string_t ) = ( decltype( create_f ) )il2cpp::resolve_icall( "UnityEngine.GameObject::Internal_CreateGameObject(UnityEngine.GameObject,System.String)" );

			uint64_t game_object = il2cpp::object_new( il2cpp::get_class_by_name( "GameObject", "UnityEngine" ) );
			create_f( game_object, name );

			return ( game_object_t* )game_object;
		}

		void add_component( il2cpp::il2cpp_type_t* type ) {
			static void( *add_component_f )( game_object_t*, il2cpp::il2cpp_object_t* ) = ( decltype( add_component_f ) )il2cpp::resolve_icall( "UnityEngine.GameObject::Internal_AddComponentWithType(System.Type)" );
			return add_component_f( this, il2cpp::type_get_object( type ) );
		}

		uint64_t get_component( il2cpp::il2cpp_type_t* type ) {
			static uint64_t( *get_component_f )( game_object_t*, void* ) = ( decltype( get_component_f ) )il2cpp::resolve_icall( "UnityEngine.GameObject::GetComponent(System.Type)" );
			return get_component_f( this, il2cpp::type_get_object( type ) );
		}

		void dont_destroy_on_load() {
			static void( *dont_destroy_on_load_f )( game_object_t* ) = ( decltype( dont_destroy_on_load_f ) )il2cpp::resolve_icall( "UnityEngine.Object::DontDestroyOnLoad(UnityEngine.Object)" );
			return dont_destroy_on_load_f( this );
		}
	};

	class component_t {

	};
}