#pragma once

#define _this ( uint64_t )this

namespace system_c {
	class string_t {
		char zpad[ 0x10 ];
	public:
		int size;
		wchar_t str[ 256 + 1 ];
		string_t() {};
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

	template <typename T>
	class list {
	public:
		uint8_t pad[ 0x10 ];
		uint64_t _items;
		int _size;

		T at( int index ) {
			if ( !_items )
				return T();

			return *( T* )( _items + 0x20 + ( index * sizeof( T ) ) );
		}
	};
}

namespace unity {
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

		float distance( vector3_t other ) {
			return sqrtf( 
				( ( this->x - other.x ) * ( this->x - other.x ) ) + 
				( ( this->y - other.y ) * ( this->y - other.y ) ) + 
				( ( this->z - other.z ) * ( this->z - other.z ) ) 
			);
		}
	};

	class game_object_t;
	class component_t;
	class transform_t;
	class camera_t;

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

		transform_t* get_transform() {
			static transform_t* ( *get_transform_f )( game_object_t* ) = ( decltype( get_transform_f ) )il2cpp::resolve_icall( "UnityEngine.GameObject::get_transform()" );
			return get_transform_f( this );
		}

		void dont_destroy_on_load() {
			static void( *dont_destroy_on_load_f )( game_object_t* ) = ( decltype( dont_destroy_on_load_f ) )il2cpp::resolve_icall( "UnityEngine.Object::DontDestroyOnLoad(UnityEngine.Object)" );
			return dont_destroy_on_load_f( this );
		}
	};

	class component_t {
	public:
		transform_t* get_transform() {
			static transform_t*( *get_transform_f )( component_t* ) = ( decltype( get_transform_f ) )il2cpp::resolve_icall( "UnityEngine.Component::get_transform()" );
			return get_transform_f( this );
		}
	};

	class transform_t {
	public:
		vector3_t get_position() {
			static void( *get_position_injected_f )( transform_t*, vector3_t* ) = ( decltype( get_position_injected_f ) )il2cpp::resolve_icall( "UnityEngine.Transform::get_position_Injected(UnityEngine.Vector3&)" );

			vector3_t result;
			get_position_injected_f( this, &result );
			return result;
		}

		void set_position( vector3_t position ) {
			static void( *set_position_injected_f )( transform_t*, vector3_t* ) = ( decltype( set_position_injected_f ) )il2cpp::resolve_icall( "UnityEngine.Transform::set_position_Injected(UnityEngine.Vector3&)" );
			set_position_injected_f( this, &position );
		}
	};

	class camera_t : public component_t {
	public:
		static camera_t* get_main() {
			static camera_t*( *get_main_f )() = ( decltype( get_main_f ) )il2cpp::resolve_icall( "UnityEngine.Camera::get_main()" );
			return get_main_f();
		}
	};

	class time {
	public:
		static float get_fixed_time() {
			static float( *get_fixed_time_f )( ) = ( decltype( get_fixed_time_f ) )il2cpp::resolve_icall( "UnityEngine.Time::get_fixedTime()" );
			return get_fixed_time_f();
		}
	};
}