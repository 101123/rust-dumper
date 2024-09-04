#pragma once

#define _this ( uint64_t )this

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

		static string_t* create_string( const wchar_t* str ) {
			static string_t* ( *fast_allocate_string_f )( uint32_t ) = ( decltype( fast_allocate_string_f ) )il2cpp::get_method_by_name( il2cpp::get_class_by_name( "String", "System" ), "FastAllocateString" )->get_fn_ptr<void*>();

			int length = wcslen( str );
			string_t* string = fast_allocate_string_f( length );
			string->size = length;
			wcscpy( string->str, str );

			return string;
		}
	};

	template <typename t>
	class list {
	public:
		int size() {
			uint64_t buffer = get_buffer();
			if ( !buffer )
				return 0;

			return *( int* )( buffer + 0x18 );
		}

		t at( int index ) {
			uint64_t buffer = get_buffer();
			if ( !buffer )
				return t();

			return *( t* )( buffer + 0x20 + ( sizeof( t ) * index ) );
		}

	private:
		uint64_t get_buffer() {
			return *( uint64_t* )( _this + 0x10 );
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
	
	struct vector2_t {
		float x, y;

		vector2_t() : x( 0.f ), y( 0.f ) {};
		vector2_t( float _x, float _y ) : x( _x ), y( _y ) {};

		bool operator==( vector2_t other ) {
			return this->x == other.x && this->y == other.y;
		}
	};

	struct vector3_t {
		float x, y, z;

		vector3_t() : x( 0.f ), y( 0.f ), z( 0.f ) {};
		vector3_t( float _x, float _y, float _z ) : x( _x ), y( _y ), z( _z ) {};

		bool operator==( vector3_t other ) {
			return this->x == other.x && this->y == other.y && this->z == other.z;
		}

		float magnitude() {
			return sqrtf( this->x * this->x + this->y * this->y + this->z * this->z );
		}
	};
}