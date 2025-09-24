#pragma once

#include <windows.h>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

#include "util.hpp"
#include "il2cpp/il2cpp-class-internals.h"
#include "il2cpp/il2cpp-tabledefs.h"

#define STR( x ) #x
#define CREATE_TYPE( name, args ) using il2cpp_##name = args; inline il2cpp_##name name;
#define ASSIGN_TYPE( name ) name = ( decltype( name ) ) GetProcAddress( GetModuleHandleA( "GameAssembly.dll" ), STR( il2cpp_##name ) );

#define NO_FILT il2cpp::method_filter_t()
#define FILT_I( x, i, n ) il2cpp::method_filter_t( x, i, n )
#define FILT_N( x, n ) il2cpp::method_filter_t( x, 0, n )
#define FILT( x ) il2cpp::method_filter_t( x, 0, 1 )

#define WILDCARD_VALUE( t ) ( t )0x1337DEADBEEF7331 

namespace il2cpp {
	using call_set_t = std::set<uint64_t>;
	using call_map_t = std::map<uint64_t, call_set_t>;
	using size_map_t = std::map<uint64_t, uint64_t>;

	inline call_map_t call_cache;
	inline call_map_t inverse_call_cache;
	inline size_map_t function_size_cache;

	inline call_set_t* get_calls( uint64_t function ) {
		if ( call_cache.find( function ) != call_cache.end() ) {
			return &call_cache[ function ];
		}

		return nullptr;
	}

	inline call_set_t* get_inverse_calls( uint64_t function ) {
		if ( inverse_call_cache.find( function ) != inverse_call_cache.end() ) {
			return &inverse_call_cache[ function ];
		}

		return nullptr;
	}

	inline size_t get_function_size( uint64_t function ) {
		if ( function_size_cache.find( function ) != function_size_cache.end() ) {
			return function_size_cache[ function ];
		}

		return 0;
	}

	inline bool has_call_in_tree( uint64_t function, uint64_t target, uint32_t depth ) {
		// return if the depth is zero
		if ( depth == 0 )
			return false;

		// find all calling functions
		call_set_t* calls = get_calls( function );

		if ( calls ) {
			// enum each caller and look for target
			for ( uint64_t call : *calls ) {
				// if there is a call from our target
				if ( call == target )
					return true;

				// continue traversing parents
				if ( has_call_in_tree( call, target, depth - 1 ) )
					return true;
			}
		}

		return false;
	}

	typedef struct _UNWIND_CODE {
		union {
			struct {
				BYTE CodeOffset;
				BYTE UnwindOp : 4;
				BYTE OpInfo : 4;

			} s;

			WORD FrameOffset;
			WORD Value;
		} u;
	} UNWIND_CODE, *PUNWIND_CODE;

	typedef struct _UNWIND_INFO {
		BYTE Version : 3;
		BYTE Flags : 5;
		BYTE PrologSize;
		BYTE CountOfCodes;
		BYTE FrameRegister : 4;
		BYTE FrameOffset : 4;
		UNWIND_CODE UnwindCode[ 1 ];
	} UNWIND_INFO, *PUNWIND_INFO;

	inline PRUNTIME_FUNCTION resolve_runtime_func( uint64_t game_base, PRUNTIME_FUNCTION func ) {
		// resolve the chained func
		PUNWIND_INFO unwind_info = ( PUNWIND_INFO )( func->UnwindData + game_base );

		if ( ( unwind_info->Flags & UNW_FLAG_CHAININFO ) != 0 ) {
			uint32_t index = unwind_info->CountOfCodes;
			if ( ( index & 1 ) != 0 )
				index += 1;

			PRUNTIME_FUNCTION chain_func = ( PRUNTIME_FUNCTION ) ( &unwind_info->UnwindCode[ index ] );

			return resolve_runtime_func( game_base, chain_func );
		}

		return func;
	}

	inline void build_call_cache() {
		uint64_t game_base = ( uint64_t ) GetModuleHandleA( "GameAssembly.dll" );
		if ( !game_base )
			return;

		PIMAGE_DOS_HEADER dos_header = ( PIMAGE_DOS_HEADER ) ( game_base );
		PIMAGE_NT_HEADERS nt_headers = ( PIMAGE_NT_HEADERS ) ( game_base + dos_header->e_lfanew );

		uint32_t game_size = nt_headers->OptionalHeader.SizeOfImage;

		IMAGE_DATA_DIRECTORY* data_dir =
		    &nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXCEPTION ];

		if ( !data_dir->VirtualAddress || !data_dir->Size )
			return;

		PRUNTIME_FUNCTION func_table = ( PRUNTIME_FUNCTION ) ( game_base + data_dir->VirtualAddress );
		uint32_t func_count = ( data_dir->Size / sizeof( RUNTIME_FUNCTION ) ) - 1;

		for ( uint32_t i = 0; i < func_count; i++ ) {
			PRUNTIME_FUNCTION func = &func_table[ i ];
			uint32_t length = func->EndAddress - func->BeginAddress;
			uint64_t start = game_base + func->BeginAddress;

			util::function_attributes_t attrs = util::get_function_attributes( ( void* ) start, length );

			// resolve all chained functions to final parent
			PRUNTIME_FUNCTION final_func = resolve_runtime_func( game_base, func );
			start = game_base + final_func->BeginAddress;

			for ( uint64_t target : attrs.calls ) {
				// skip self referencing calls
				if ( target == start )
					continue;

				// insert call into the call cache
				call_cache[ target ].insert( start );
				inverse_call_cache[ start ].insert( target );
				function_size_cache[ start ] = length;
			}

			for ( uint64_t target : attrs.jmps ) {
				// insert jump into the call cache
				call_cache[ target ].insert( start );
				inverse_call_cache[ start ].insert( target );
				function_size_cache[ start ] = length;
			}
		}
	}

	struct method_filter_t {
		inline method_filter_t() : target( 0 ), ignore( 0 ), max_depth( 0 ) {}

		inline method_filter_t( const method_filter_t& filter )
		    : target( filter.target ), ignore( filter.ignore ), max_depth( filter.max_depth ) {}

		inline method_filter_t( uint64_t _target, uint64_t _ignore, uint32_t _max_depth )
		    : target( _target ), ignore( _ignore ), max_depth( _max_depth ) {}

		uint64_t target;
		uint64_t ignore;
		uint32_t max_depth;
	};

	// get_method_by_return_type_attrs_method_attr
	enum e_attr_search : uint8_t {
		attr_search_want = 0,
		attr_search_ignore
	};

	struct method_info_t;
	struct field_info_t;
	struct il2cpp_type_t;
	struct il2cpp_class_t;
	struct il2cpp_image_t;
	struct il2cpp_assembly_t;
	struct il2cpp_domain_t;
	struct il2cpp_object_t;

	CREATE_TYPE( domain_get, il2cpp_domain_t* ( * )( ) );
	CREATE_TYPE( domain_get_assemblies, il2cpp_assembly_t** ( * )( void*, size_t* ) );
	CREATE_TYPE( domain_assembly_open, il2cpp_assembly_t* ( * )( void*, const char* ) );

	CREATE_TYPE( assembly_get_image, il2cpp_image_t* ( * )( void* ) );

	CREATE_TYPE( class_from_name, il2cpp_class_t* ( * )( void*, const char*, const char* ) );
	CREATE_TYPE( image_get_class_count, size_t( * )( il2cpp_image_t* ) );
	CREATE_TYPE( image_get_class, il2cpp_class_t* ( * )( il2cpp_image_t*, size_t ) );

	CREATE_TYPE( type_get_object, il2cpp_object_t* ( * )( void* ) );
	CREATE_TYPE( type_get_class_or_element_class, il2cpp_class_t* ( * )( void* ) );
	CREATE_TYPE( type_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( type_get_attrs, uint32_t( * )( il2cpp_type_t* ) );

	CREATE_TYPE( class_get_methods, method_info_t* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_nested_types, il2cpp_class_t* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_fields, field_info_t* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_type, il2cpp_type_t* ( * )( void* ) );
	CREATE_TYPE( class_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( class_get_namespace, const char* ( * )( void* ) );
	CREATE_TYPE( class_from_il2cpp_type, il2cpp_class_t* ( * )( il2cpp_type_t* ) );
	CREATE_TYPE( class_get_static_field_data, void*( * )( il2cpp_class_t* ) );
	CREATE_TYPE( class_get_parent, il2cpp_class_t* ( * )( il2cpp_class_t* ) );
	CREATE_TYPE( class_get_interfaces, il2cpp_class_t* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_image, il2cpp_image_t* ( * )( void* ) );
	CREATE_TYPE( class_get_flags, uint32_t( * )( void* ) );

	CREATE_TYPE( method_get_param_count, uint32_t( * )( void* ) );
	CREATE_TYPE( method_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( method_get_param_name, const char* ( * )( method_info_t*, uint32_t ) );
	CREATE_TYPE( method_get_param, il2cpp_type_t* ( * )( method_info_t*, uint32_t ) );
	CREATE_TYPE( method_get_return_type, il2cpp_type_t* ( * )( void* ) );
	CREATE_TYPE( method_get_class, il2cpp_class_t* ( * )( method_info_t* ) );
	CREATE_TYPE( method_get_flags, uint32_t ( * )( method_info_t*, uint32_t* ) );
	CREATE_TYPE( method_has_attribute, bool( * )( method_info_t*, il2cpp_class_t* ) );

	CREATE_TYPE( field_get_offset, size_t( * )( void* ) );
	CREATE_TYPE( field_get_type, il2cpp_type_t* ( * )( void* ) );
	CREATE_TYPE( field_get_parent, il2cpp_class_t* ( * )( void* ) );
	CREATE_TYPE( field_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( field_static_get_value, void( * )( void*, void* ) );
	CREATE_TYPE( field_get_flags, int( * )( void* ) );
	CREATE_TYPE( field_has_attribute, bool( * )( void*, il2cpp_class_t* ) );

	CREATE_TYPE( object_get_class, il2cpp_class_t* ( * )( void* ) );
	CREATE_TYPE( object_new, uint64_t( * )( il2cpp_class_t* ) );

	CREATE_TYPE( resolve_icall, std::uintptr_t( * )( const char* ) );

	struct il2cpp_type_t {
		il2cpp_class_t* klass() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return type_get_class_or_element_class( this );
		}

		const char* name() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return type_get_name( this );
		}

		uint32_t attributes() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			return type_get_attrs( this );
		}
	};

	struct field_info_t {
		il2cpp_type_t* type() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return field_get_type( this );
		}

		const char* name() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return field_get_name( this );
		}

		size_t offset() {
			if ( !is_valid_ptr( this ) )
				return 0ull;

			return field_get_offset( this );
		}

		size_t offset( il2cpp_class_t* klass ) {
			if ( !is_valid_ptr( this ) )
				return 0ull;

			Il2CppClass* native_klass = ( Il2CppClass* )klass;
			if ( !is_valid_ptr( klass ) )
				return 0ull;

			bool is_struct = native_klass->byval_arg.type == IL2CPP_TYPE_VALUETYPE;
			size_t offset = field_get_offset( this ) - ( is_struct ? 0x10 : 0 );

			return offset;
		}

		template <typename T>
		T static_get_value() {
			if ( !is_valid_ptr( this ) )
				return T();

			T value;
			field_static_get_value( this, &value );
			return value;
		}

		int flags() {
			if ( !is_valid_ptr( this ) )
				return 0;

			return field_get_flags( this );
		}
	};

	struct virtual_invoke_data_t {
		void* method_ptr;
		method_info_t* method;
	};

	struct il2cpp_class_t {
		const char* name() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_name( this );
		}

		const char* namespaze() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_namespace( this );
		}

		il2cpp_type_t* type() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_type( this );
		}

		field_info_t* fields( void** iter ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_fields( this, iter );
		}

		std::vector<field_info_t*> get_fields( bool include_static = false ) {
			std::vector<field_info_t*> ret;

			void* iter = nullptr;
			while ( field_info_t* field_info = class_get_fields( this, &iter ) ) {
				if ( !include_static && ( field_info->flags() & FIELD_ATTRIBUTE_STATIC ) )
					continue;

				ret.push_back( field_info );
			}

			return ret;
		}

		uint32_t field_count() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			void* iter = nullptr;
			uint32_t count = 0u;
			while ( field_info_t* field = fields( &iter ) ) {
				// ignore static fields
				if ( !( field->flags() & FIELD_ATTRIBUTE_STATIC ) ) {
					count++;
				}
			}

			return count;
		}

		method_info_t* methods( void** iter ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_methods( this, iter );
		}

		uint32_t method_count() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			void* iter = nullptr;
			uint32_t count = 0u;
			while ( method_info_t* method = methods( &iter ) ) {
				count++;
			}

			return count;
		}

		method_info_t* method_from_addr( uint64_t address ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			void* iter = nullptr;
			while ( method_info_t* method = this->methods( &iter ) ) {
				if ( !is_valid_ptr( method ) )
					continue;

				if ( *( uint64_t* )( method ) == address )
					return method;
			}

			return nullptr;
		}

		std::vector<method_info_t*> get_methods() {
			std::vector<method_info_t*> ret;

			void* iter = nullptr;
			while ( method_info_t* method = class_get_methods( this, &iter ) ) {
				ret.push_back( method );
			}

			return ret;
		}

		il2cpp_class_t* nested_types( void** iter ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_nested_types( this, iter );
		}

		uint32_t nested_type_count() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			void* iter = nullptr;
			uint32_t count = 0;
			while ( il2cpp_class_t* klass = nested_types( &iter ) ) {
				count++;
			}

			return count;
		}

		void* static_field_data() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_static_field_data( this );
		}

		il2cpp_class_t* parent() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_parent( this );
		}

		// All methods that take in an iterator should eventually be transformed into something like this
		std::vector<il2cpp_class_t*> get_interfaces() {
			std::vector<il2cpp_class_t*> ret;

			void* iter = nullptr;
			while ( il2cpp_class_t* interface_ = class_get_interfaces( this, &iter ) ) {
				ret.push_back( interface_ );
			}

			return ret;
		}

		il2cpp_image_t* image( ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_get_image( this );
		}

		il2cpp_class_t* get_generic_argument_at( uint32_t index ) {
			Il2CppClass* klass = ( Il2CppClass* )this;
			if ( !is_valid_ptr( klass ) )
				return nullptr;

			Il2CppGenericClass* generic_class = klass->generic_class;
			if ( !is_valid_ptr( generic_class ) )
				return nullptr;

			const Il2CppGenericInst* instance = generic_class->context.class_inst;
			if ( !is_valid_ptr( instance ) )
				return nullptr;

			if ( index > instance->type_argc )
				return nullptr;

			const Il2CppType** types = instance->type_argv;
			if ( !is_valid_ptr( types ) )
				return nullptr;

			const Il2CppType* type = types[ index ];
			if ( !is_valid_ptr( type ) )
				return nullptr;

			return class_from_il2cpp_type( ( il2cpp_type_t* )type );
		}

		uint16_t vtable_count() {
			Il2CppClass* klass = ( Il2CppClass* )this;
			if ( !is_valid_ptr( klass ) )
				return 0;
			
			return klass->vtable_count;
		}

		uint16_t total_vtable_count() {
			if ( !is_valid_ptr( this ) )
				return 0;

			uint16_t count = 0;
			il2cpp_class_t* current = this;

			while ( current ) {
				count += current->vtable_count();
				current = current->parent();
			}

			return count;
		}

		virtual_invoke_data_t* get_vtable_entry( uint32_t index ) {
			Il2CppClass* klass = ( Il2CppClass* )this;
			if ( !is_valid_ptr( klass ) )
				return nullptr;

			return ( virtual_invoke_data_t* )&klass->vtable[ index ];
		}

		uint32_t flags() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			return class_get_flags( this );
		}
	};

	struct image_global_metadata_t {
		int32_t m_type_start;
		int32_t m_exported_type_start;
		int32_t m_custom_attribute_start;
		int32_t m_entry_point_index;
		il2cpp_image_t* image;
	};

	struct il2cpp_image_t {
		uint32_t type_count() {
			Il2CppImage* image = ( Il2CppImage* )this;
			if ( !is_valid_ptr( image ) )
				return 0u;

			return image->typeCount;
		}

		size_t class_count() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			return image_get_class_count( this );
		}

		il2cpp_class_t* get_class( size_t index ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return image_get_class( this, index );
		}

		il2cpp_class_t* get_class_from_name( const char* namespaze, const char* klass ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return class_from_name( this, namespaze, klass );
		}
	};

	struct il2cpp_object_t {
		il2cpp_class_t* get_class() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return object_get_class( this );
		}
	};

	struct il2cpp_assembly_t {
		il2cpp_image_t* image() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return assembly_get_image( this );
		}
	};

	struct il2cpp_domain_t {
		static il2cpp_domain_t* get() {
			return domain_get();
		}

		il2cpp_assembly_t** get_assemblies( size_t* size ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return domain_get_assemblies( this, size );
		}

		il2cpp_assembly_t* get_assembly_from_name( const char* name ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return domain_assembly_open( this, name );
		}
	};

	struct method_info_t {
		static method_info_t* from_addr( uint64_t addr ) {
			il2cpp::il2cpp_domain_t* domain = il2cpp::il2cpp_domain_t::get();
			if ( !is_valid_ptr( domain ) )
				return nullptr;

			size_t assembly_ct = 0;
			il2cpp::il2cpp_assembly_t** assemblies = domain->get_assemblies( &assembly_ct );
			if ( !is_valid_ptr( assemblies ) )
				return nullptr;

			for ( size_t i = 0; i < assembly_ct; i++ ) {
				il2cpp::il2cpp_assembly_t* assembly = assemblies[ i ];
				if ( !is_valid_ptr( assembly ) )
					continue;

				il2cpp::il2cpp_image_t* image = assembly->image();
				if ( !is_valid_ptr( image ) )
					continue;

				size_t class_ct = image->class_count();
				for ( size_t j = 0; j < class_ct; j++ ) {
					il2cpp::il2cpp_class_t* klass = image->get_class( j );
					if ( !is_valid_ptr( klass ) )
						continue;

					void* iter = nullptr;
					while ( il2cpp::method_info_t* method = klass->methods( &iter ) ) {
						if ( method->get_fn_ptr<uint64_t>() == addr ) {
							return method;
						}
					}
				}
			}

			return nullptr;
		}

		bool should_filter( method_filter_t filter ) {
			if ( !this )
				return true;

			if ( !filter.max_depth )
				return false;

			uint64_t address = this->get_fn_ptr<uint64_t>();
			if ( address == filter.ignore )
				return true;

			bool match = has_call_in_tree( address, filter.target, filter.max_depth );
			return !match;
		}

		const char* name() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return method_get_name( this );
		}

		il2cpp_type_t* get_param( uint32_t index ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return method_get_param( this, index );
		}

		const char* get_param_name( uint32_t index ) {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return method_get_param_name( this, index );
		}

		uint32_t param_count() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			return method_get_param_count( this );
		}

		il2cpp_type_t* return_type() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return method_get_return_type( this );
		}

		il2cpp_class_t* klass() {
			if ( !is_valid_ptr( this ) )
				return nullptr;

			return method_get_class( this );
		}

		uint32_t flags() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			return method_get_flags( this, nullptr );
		}

		template<typename T>
		T get_fn_ptr() {
			if ( !is_valid_ptr( this ) )
				return T();

			return *( T* )( this );
		}

		uint32_t get_vtable_offset() {
			if ( !is_valid_ptr( this ) )
				return 0u;

			il2cpp_class_t* klass = this->klass();
			if ( !is_valid_ptr( klass ) )
				return 0u;

			for ( uint16_t i = 0, n = klass->total_vtable_count(); i < n; i++ ) {
				virtual_invoke_data_t* virtual_invoke_data = klass->get_vtable_entry( i );
				if ( virtual_invoke_data->method != this )
					continue;

				return ( uint64_t )virtual_invoke_data - ( uint64_t )klass;
			}

			return 0u;
		}
	};

	struct method_search_flags_t {
		il2cpp_type_t* m_ret_type;

		int m_wanted_vis;
		int m_wanted_attrs;
		int m_param_ct;

		std::vector<const char*> m_params_strs;
	};

	inline il2cpp_assembly_t* get_assembly_by_name( const char* name ) {
		il2cpp_domain_t* domain = il2cpp_domain_t::get( );
		if ( !is_valid_ptr( domain ) )
			return nullptr;

		return domain->get_assembly_from_name( name );
	}

	template<typename T>
	inline il2cpp_class_t* get_class_from_class_field_type( il2cpp_class_t* klass, T comparator ) {
		void* iter = nullptr;
		while ( field_info_t* field = klass->fields( &iter ) ) {
			if ( comparator( field ) ) {
				return class_from_il2cpp_type( field->type() );
			}
		}

		return nullptr;
	}

	inline il2cpp_class_t* get_class_from_field_type_by_name( il2cpp_class_t* klass, const char* field_name )
	{
		return get_class_from_class_field_type( klass, [ = ] ( field_info_t* field ) -> bool { return strcmp( field->name( ), field_name ) == 0; } );
	}

	inline il2cpp_class_t* get_class_from_field_type_by_offset( il2cpp_class_t* klass, uint64_t offset )
	{
		return get_class_from_class_field_type( klass, [ = ] ( field_info_t* field ) -> bool { return field->offset( ) == offset; } );
	}

	inline il2cpp_class_t* get_class_from_field_type_by_type( il2cpp_class_t* klass, il2cpp_type_t* wanted_type )
	{
		return get_class_from_class_field_type( klass, [ = ] ( field_info_t* field ) -> bool {
			const char* name = field->type( )->name( );
			return name && strcmp( name, wanted_type->name( ) ) == 0;
			} );
	}

	inline il2cpp_class_t* get_class_from_field_type_by_type_contains( il2cpp_class_t* klass, const char* search ) {
		return get_class_from_class_field_type( klass, [ = ] ( field_info_t* field ) -> bool {
			const char* name = field->type( )->name( );
			return name && strstr( name, search ) != nullptr;
			} );
	}

	inline il2cpp_class_t* get_class_from_field_type_by_class( il2cpp_class_t* klass, il2cpp_class_t* wanted_klass )
	{
		return get_class_from_field_type_by_type( klass, wanted_klass->type( ) );
	}

	inline il2cpp_class_t* get_class_from_method( il2cpp_class_t* klass, const char* method_name, const char* param_name, int param_ct = -1 )
	{
		bool checkMethodName = method_name != nullptr;
		bool checkParamName = param_name != nullptr;
		bool checkParamCount = param_ct != -1;

		void* iter = nullptr;
		while ( method_info_t* method = klass->methods( &iter ) )
		{
			int found = 0;

			if ( checkMethodName )
			{
				if ( strcmp( method->name( ), method_name ) == 0 )
					found++;
			}

			if ( checkParamName || checkParamCount )
			{
				uint32_t numParams = method->param_count( );

				if ( checkParamCount && numParams != param_ct )
					found--;

				if ( checkParamName )
				{
					for ( uint32_t i = 0; i < numParams; i++ )
					{
						const char* name = method->get_param_name( i );
						if ( !name )
							continue;

						if ( strcmp( name, param_name ) == 0 )
							found++;
					}
				}
			}

			if ( found )
				return klass;
		}

		return nullptr;
	}

	inline il2cpp_class_t* get_class_by_name_from_assembly( il2cpp_assembly_t* assembly, const char* klass_name, const char* namespaze = "" ) {
		il2cpp_image_t* image = assembly->image( );
		if ( !image )
			return nullptr;

		return image->get_class_from_name( namespaze, klass_name );
	}

	inline il2cpp_class_t* get_class_by_name( const char* klass, const char* namespaze = "" )
	{
		il2cpp_domain_t* domain = il2cpp_domain_t::get( );
		if ( !domain )
			return nullptr;

		size_t assemblyCount{};
		il2cpp_assembly_t** assemblies = domain->get_assemblies( &assemblyCount );
		if ( !assemblies )
			return nullptr;

		for ( size_t i = 0; i < assemblyCount; i++ )
		{
			il2cpp_assembly_t* assembly = assemblies[ i ];
			if ( !assembly )
				continue;

			if ( il2cpp_class_t* kl = get_class_by_name_from_assembly( assembly, klass, namespaze ) )
				return kl;
		}

		return nullptr;
	}

	template<typename T>
	inline il2cpp_class_t* search_for_class_in_image( il2cpp_image_t* image, T comparator ) {
		if ( !image )
			return nullptr;

		size_t classCount = image->class_count( );
		for ( size_t i = 0; i < classCount; i++ )
		{
			il2cpp_class_t* klass = image->get_class( i );
			if ( !klass )
				continue;

			if ( comparator( klass ) )
				return klass;
		}

		return nullptr;
	}

	template<typename T>
	inline void populate_classes_in_image( il2cpp_image_t* image, std::vector<il2cpp_class_t*>* classes, T comparator ) {
		if ( !image )
			return;

		size_t class_ct = image->class_count();
		for ( size_t i = 0; i < class_ct; i++ ) {
			il2cpp_class_t* klass = image->get_class( i );
			if ( !klass )
				continue;

			if ( comparator( klass ) )
				classes->push_back( klass );
		}

		return;
	}

	inline il2cpp_class_t* search_for_class_by_method_in_assembly( const char* assembly_name, const char* method_name, const char* param_name, int param_ct = -1 ) {
		il2cpp_assembly_t* assembly = get_assembly_by_name( assembly_name );
		if ( !assembly )
			return nullptr;

		const auto search_for_class_by_method_in_assembly = [ = ] ( il2cpp_class_t* klass ) -> bool {
			return get_class_from_method( klass, method_name, param_name, param_ct ) != nullptr;
		};

		return search_for_class_in_image( assembly->image( ), search_for_class_by_method_in_assembly );
	}

	template<typename Comparator>
	inline il2cpp_class_t* search_for_class( Comparator comparator ) {
		il2cpp_domain_t* domain = il2cpp_domain_t::get( );
		if ( !domain )
			return nullptr;

		size_t assemblyCount{};
		il2cpp_assembly_t** assemblies = domain->get_assemblies( &assemblyCount );
		if ( !assemblies )
			return nullptr;

		for ( size_t i = 0; i < assemblyCount; i++ )
		{
			il2cpp_assembly_t* assembly = assemblies[ i ];
			if ( !assembly )
				continue;

			il2cpp_image_t* image = assembly->image( );
			if ( !image )
				continue;

			il2cpp_class_t* found = search_for_class_in_image( image, comparator );
			if ( found )
				return found;
		}

		return nullptr;
	}

	template<typename Comparator>
	inline std::vector<il2cpp_class_t*> search_for_classes( Comparator comparator ) {
		std::vector<il2cpp_class_t*> classes;

		il2cpp_domain_t* domain = il2cpp_domain_t::get();
		if ( !domain )
			return classes;

		size_t assembly_ct = 0;
		il2cpp_assembly_t** assemblies = domain->get_assemblies( &assembly_ct );
		if ( !assemblies )
			return classes;

		for ( size_t i = 0; i < assembly_ct; i++ ) {
			il2cpp_assembly_t* assembly = assemblies[ i ];
			if ( !assembly )
				continue;

			il2cpp_image_t* image = assembly->image();
			if ( !image )
				continue;

			populate_classes_in_image( image, &classes, comparator );
		}

		return classes;
	}

	inline il2cpp_class_t* search_for_class_by_field_count( il2cpp_class_t** match_klasses, int match_klasses_ct, int field_count, uint8_t equality )
	{
		const auto search_by_field_count = [ = ] ( il2cpp_class_t* klass ) -> bool {
			uint32_t count = klass->field_count( );

			if ( equality == 0 && count != field_count )
				return false;
			else if ( equality == 1 && count >= field_count )
				return false;
			else if ( equality == 2 && count <= field_count )
				return false;

			int correctFields = 0;
			for ( size_t k = 0; k < match_klasses_ct; k++ )
			{
				il2cpp_class_t* matchClass = match_klasses[ k ];
				if ( !matchClass )
					continue;

				void* iter2 = nullptr;
				while ( field_info_t* field = klass->fields( &iter2 ) )
				{
					if ( field->type( )->klass( ) == matchClass )
					{
						correctFields++;
						break;
					}
				}
			}

			if ( correctFields != match_klasses_ct )
				return false;

			return true;
		};

		return search_for_class( search_by_field_count );
	}

	inline il2cpp_class_t* search_for_class_by_method_return_type_name( const char* ret_type_name, uint32_t wanted_vis, uint32_t wanted_flags )
	{
		const auto search_by_return_type_name = [ = ] ( il2cpp_class_t* klass ) -> bool {
			void* iter = nullptr;
			while ( method_info_t* method = klass->methods( &iter ) )
			{
				const char* name = method->return_type( )->name( );
				if ( !name )
					continue;

				if ( strcmp( name, ret_type_name ) == 0 )
				{
					uint32_t vis = method->flags( ) & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
					if ( wanted_vis && ( vis != wanted_vis ) )
						continue;

					if ( wanted_flags && !( method->flags( ) & wanted_flags ) )
						continue;

					return true;
				}
			}

			return false;
		};

		return search_for_class( search_by_return_type_name );
	}

	inline std::vector<il2cpp_class_t*> get_inheriting_classes( il2cpp_class_t* target ) {
		const auto get_inheriting_classes = [=]( il2cpp_class_t* klass ) -> bool {
			il2cpp_class_t* parent = klass->parent();
			if ( !parent )
				return false;

			return strcmp( parent->type()->name(), target->type()->name() ) == 0;
		};

		return search_for_classes( get_inheriting_classes );
	}

	inline il2cpp_class_t* search_for_class_by_interfaces_contain( const char* search ) {
		const auto search_for_class_by_interfaces_contain = [ = ] ( il2cpp_class_t* klass ) {
			for ( il2cpp_class_t* interface_ : klass->get_interfaces() ) {
				if ( strstr( interface_->name(), search ) ) {
					return true;
				}
			}

			return false;
		};

		return search_for_class( search_for_class_by_interfaces_contain );
	}

	inline il2cpp_class_t* search_for_class_by_field_types( il2cpp_type_t* field_type, int field_type_ct, int wanted_vis, int wanted_flags, il2cpp_class_t* ignore = nullptr ) {
		const auto search_for_class_by_field_types = [ = ] ( il2cpp_class_t* klass ) {
			if ( klass == ignore )
				return false;

			void* iter = nullptr;
			int matches = 0;

			while ( field_info_t* field = klass->fields( &iter ) ) {
				int fl = field->flags();
				int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
				if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( wanted_flags && !( fl & wanted_flags ) ) )
					continue;

				if ( strcmp( field->type()->name(), field_type->name() ) )
					continue;

				matches++;
			}

			if ( field_type_ct ) {
				return matches == field_type_ct;
			} else {
				return matches >= 1;
			}
		};

		return search_for_class( search_for_class_by_field_types );
	}

	inline std::vector<il2cpp::il2cpp_class_t*> search_for_classes_by_field_types( il2cpp_type_t* field_type, int field_type_ct, int wanted_vis, int wanted_flags ) {
		const auto search_for_classes_by_field_types = [=]( il2cpp_class_t* klass ) {
			void* iter = nullptr;
			int matches = 0;

			while ( field_info_t* field = klass->fields( &iter ) ) {
				int fl = field->flags();
				int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
				if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( wanted_flags && !( fl & wanted_flags ) ) )
					continue;

				if ( strcmp( field->type()->name(), field_type->name() ) )
					continue;

				matches++;
			}

			if ( field_type_ct ) {
				return matches == field_type_ct;
			}
			else {
				return matches >= 1;
			}
		};

		return search_for_classes( search_for_classes_by_field_types );
	}

	inline il2cpp_class_t* search_for_static_class_with_method_with_rettype_param_types( int method_ct, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct ) {
		const auto search_for_static_class_with_method_with_rettype_param_types = [ = ] ( il2cpp_class_t* klass ) {
			il2cpp::il2cpp_type_t* type = klass->type( );

			if ( !type )
				return false;

			uint32_t attrs = type->attributes( );

			if ( attrs != 0 )
				return false;

			bool matched = false;
			void* iter = nullptr;
			uint32_t method_count = 0;

			while ( method_info_t* method = klass->methods( &iter ) ) {
				method_count++;

				uint32_t param_count = method->param_count( );
				if ( param_count != param_ct )
					continue;

				il2cpp::il2cpp_type_t* ret = method->return_type( );
				if ( !ret || strcmp( ret->name( ), ret_type->name( ) ) != 0 )
					continue;

				int vis = method->flags( ) & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
				if ( wanted_vis && vis != wanted_vis )
					continue;

				if ( wanted_flags && !( method->flags( ) & wanted_flags ) )
					continue;

				int matchedTypes = 0;
				for ( uint32_t i = 0; i < param_count; i++ ) {
					il2cpp_type_t* param = method->get_param( i );
					if ( !param )
						continue;

					if ( strcmp( param->name( ), param_types[ i ]->name( ) ) == 0 )
						matchedTypes++;
				}

				if ( matchedTypes == param_ct )
					matched = true;
			}

			if ( method_count != method_ct )
				return false;
			if ( !matched )
				return false;

			return true;
		};

		return search_for_class( search_for_static_class_with_method_with_rettype_param_types );
	}

	inline method_info_t* get_method_by_return_type_and_param_types_str( method_filter_t filter, il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, const char** param_strs, int param_ct );
	inline il2cpp_class_t* search_for_class_containing_method_prototypes( const std::vector<method_search_flags_t>& search_params ) {
		const auto search_for_static_class_with_method_with_rettype_param_types = [&]( il2cpp_class_t* klass ) {
			int matching_methods = 0;
			void* iter = nullptr;

			for ( const method_search_flags_t& param : search_params ) {
				method_info_t* method = get_method_by_return_type_and_param_types_str(
				    NO_FILT,
					klass,
					param.m_ret_type,
					param.m_wanted_vis,
					param.m_wanted_attrs,
					( const char** )&param.m_params_strs[ 0 ],
					param.m_param_ct );

				if ( method ) {
					matching_methods++;
				}
			}

			return matching_methods == search_params.size();
		};

		return search_for_class( search_for_static_class_with_method_with_rettype_param_types );
	}

#define MAX_TYPES 16

	inline il2cpp_class_t* search_for_class_containing_field_types_str( const char** field_types, int field_types_ct ) {
		const auto search_for_class_containing_field_types = [&]( il2cpp_class_t* klass ) {
			bool matched[ MAX_TYPES ]{};

			void* iter = nullptr;
			while ( field_info_t* field = klass->fields( &iter ) ) {
				il2cpp_type_t* type = field->type();
				if ( !type )
					continue;

				for ( int i = 0; i < field_types_ct; i++ ) {
					if ( !matched[ i ] && !strcmp( type->name(), field_types[ i ] ) ) {
						matched[ i ] = true;
						break;
					}
				}
			}

			int matches = 0;
			for ( int i = 0; i < MAX_TYPES; i++ ) {
				if ( matched[ i ] ) {
					matches++;
				}
			}

			return matches == field_types_ct;
		};

		return search_for_class( search_for_class_containing_field_types );
	}

	inline il2cpp_class_t* search_for_class_containing_field_names( const char** field_names, int field_names_ct ) {
		const auto search_for_class_containing_field_types = [&]( il2cpp_class_t* klass ) {
			bool matched[ MAX_TYPES ]{};

			void* iter = nullptr;
			while ( field_info_t* field = klass->fields( &iter ) ) {
				for ( int i = 0; i < field_names_ct; i++ ) {
					if ( !matched[ i ] && !strcmp( field->name(), field_names[ i ] ) ) {
						matched[ i ] = true;
						break;
					}
				}
			}

			int matches = 0;
			for ( int i = 0; i < MAX_TYPES; i++ ) {
				if ( matched[ i ] ) {
					matches++;
				}
			}

			return matches == field_names_ct;
		};

		return search_for_class( search_for_class_containing_field_types );
	}

	template<typename Comparator>
	inline method_info_t* get_method_from_class( method_filter_t filter, il2cpp_class_t* klass, Comparator comparator ) {
		void* iter = nullptr;
		while ( method_info_t* method = klass->methods( &iter ) ) {
			if ( method->should_filter( filter ) )
				continue;

			if ( comparator( method ) )
				return method;
		}

		return nullptr;
	}

	inline method_info_t* get_method_by_name( il2cpp_class_t* klass, const char* method_name, int param_ct = 1337 )
	{
		const auto get_method_by_name = [ = ] ( method_info_t* method ) -> bool {
			if ( param_ct != 1337 && method->param_count() != param_ct )
				return false;

			const char* name = method->name( );
			return name && strcmp( name, method_name ) == 0;
		};

		return get_method_from_class( NO_FILT, klass, get_method_by_name );
	}

	inline method_info_t* get_method_by_return_type_str( method_filter_t filter, il2cpp_class_t* klass, const char* ret_type_name, int param_ct )
	{
		const auto get_method_by_return_type_name = [ = ] ( method_info_t* method ) -> bool {
			const char* name = method->return_type( )->name( );
			if ( param_ct != -1 && ( param_ct != method->param_count( ) ) )
				return false;

			return name && strcmp( name, ret_type_name ) == 0;
		};

		return get_method_from_class( filter, klass, get_method_by_return_type_name );
	}

	inline method_info_t* get_method_by_return_type_and_param_types( method_filter_t filter, il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct ) {
		void* iter = nullptr;

		const auto get_method_by_return_type_and_param_types = [ = ] ( method_info_t* method ) -> bool {
			uint32_t count = method->param_count( );
			if ( count != param_ct )
				return false;

			il2cpp::il2cpp_type_t* ret = method->return_type( );
			if ( !ret || strcmp( ret->name( ), ret_type->name( ) ) != 0 )
				return false;

			int vis = method->flags( ) & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
			if ( wanted_vis && vis != wanted_vis )
				return false;

			if ( wanted_flags && !( method->flags( ) & wanted_flags ) )
				return false;

			int matchedTypes = 0;
			for ( uint32_t i = 0; i < count; i++ ) {
				il2cpp_type_t* param = method->get_param( i );
				if ( !param )
					continue;

				if ( strcmp( param->name( ), param_types[ i ]->name( ) ) == 0 )
					matchedTypes++;
			}

			return matchedTypes == param_ct;
		};

		return get_method_from_class( filter, klass, get_method_by_return_type_and_param_types );
	}

	inline method_info_t* get_method_by_return_type_and_param_types_str( method_filter_t filter, il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, const char** param_strs, int param_ct ) {
		void* iter = nullptr;

		const auto get_method_by_return_type_and_param_types = [=]( method_info_t* method ) -> bool {
			uint32_t count = method->param_count();
			if ( count != param_ct )
				return false;

			il2cpp::il2cpp_type_t* ret = method->return_type();
			if ( !ret || strcmp( ret->name(), ret_type->name() ) != 0 )
				return false;

			int vis = method->flags() & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
			if ( wanted_vis && vis != wanted_vis )
				return false;

			if ( wanted_flags && !( method->flags() & wanted_flags ) )
				return false;

			int matchedTypes = 0;
			for ( uint32_t i = 0; i < count; i++ ) {
				il2cpp_type_t* param = method->get_param( i );
				if ( !param )
					continue;

				if ( strcmp( param->name(), param_strs[ i ] ) == 0 )
					matchedTypes++;
			}

			return matchedTypes == param_ct;
		};

		return get_method_from_class( filter, klass, get_method_by_return_type_and_param_types );
	}

	inline method_info_t* get_method_by_return_type_and_param_types_size( method_filter_t filter, int idx, il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct, il2cpp_class_t* method_attr_klass, bool want_or_ignore ) {
		struct method_info_match_t {
			method_info_t* method;
			size_t length;
		};

		std::vector<method_info_match_t> matches;

		void* iter = nullptr;
		while ( method_info_t* method = klass->methods( &iter ) ) {
			if ( method->should_filter( filter ) )
				continue;

			uint32_t count = method->param_count();
			if ( count != param_ct )
				continue;

			il2cpp::il2cpp_type_t* ret = method->return_type();
			if ( !ret || strcmp( ret->name(), ret_type->name() ) != 0 )
				continue;

			if ( method_attr_klass ) {
				bool has_attr = method_has_attribute( method, method_attr_klass );
				if ( has_attr && want_or_ignore == e_attr_search::attr_search_ignore )
					continue;
				if ( !has_attr && want_or_ignore == e_attr_search::attr_search_want )
					continue;
			}

			int vis = method->flags() & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
			if ( wanted_vis && vis != wanted_vis )
				continue;
			if ( wanted_flags && !( method->flags() & wanted_flags ) )
				continue;

			int matchedTypes = 0;
			for ( uint32_t i = 0; i < count; i++ ) {
				il2cpp_type_t* param = method->get_param( i );
				if ( !param )
					continue;

				if ( strcmp( param->name(), param_types[ i ]->name() ) == 0 )
					matchedTypes++;
			}

			if ( matchedTypes == param_ct ) {
				method_info_match_t buffer;
				buffer.method = method;
				buffer.length = util::get_function_attributes( method->get_fn_ptr<void*>(), 16384 ).length;
				matches.push_back( std::move( buffer ) );
			}
		}

		std::sort( matches.begin(), matches.end(), 
			[]( const method_info_match_t& r1, const method_info_match_t& r2 ) -> bool { return r1.length > r2.length; } );

		if ( idx > matches.size() - 1 )
			return nullptr;

		return matches.at( idx ).method;
	}

	inline method_info_t* get_method_by_param_type( method_filter_t filter, il2cpp_class_t* klass, const char* method_name, il2cpp_class_t* param_type, int param_idx ) {
		const auto get_method_by_param_type_idx = [=]( method_info_t* method ) -> bool {
			if ( method->param_count() < param_idx + 1 )
				return false;

			if ( strcmp( method->name(), method_name ) )
				return false;

			return method->get_param( param_idx )->klass() == param_type;
		};

		return get_method_from_class( filter, klass, get_method_by_param_type_idx );
	}

	inline method_info_t* get_method_by_param_name( method_filter_t filter, il2cpp_class_t* klass, const char* method_name, const char* param_name, int param_idx ) {
		const auto get_method_by_param_name_idx = [=]( method_info_t* method ) -> bool {
			if ( method->param_count() < param_idx + 1 )
				return false;

			if ( strcmp( method->name(), method_name ) )
				return false;

			return strcmp( method->get_param_name( param_idx ), param_name ) == 0;
		};

		return get_method_from_class( filter, klass, get_method_by_param_name_idx );
	}

	inline method_info_t* get_method_by_return_type_attrs( method_filter_t filter, il2cpp_class_t* klass, il2cpp_class_t* ret_type_klass, int wanted_flags = 0, int wanted_vis = 0, int param_ct = -1 ) {
		void* iter = nullptr;

		const auto get_method_by_return_type_attrs = [ = ] ( method_info_t* method ) -> bool {
			uint32_t count = method->param_count( );
			if ( param_ct != -1 && count != param_ct )
				return false;

			il2cpp_type_t* returnType = method->return_type( );

			if ( strcmp( returnType->klass( )->name( ), ret_type_klass->name( ) ) == 0 )
			{
				uint32_t vis = method->flags( ) & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
				if ( wanted_vis && ( vis != wanted_vis ) )
					return false;

				if ( wanted_flags && !( method->flags( ) & wanted_flags ) )
					return false;

				return true;
			}

			return false;
		};

		return get_method_from_class( filter, klass, get_method_by_return_type_attrs );
	}

	inline std::vector<method_info_t*> get_methods_by_return_type_attrs( method_filter_t filter, il2cpp_class_t* klass, il2cpp_class_t* ret_type_klass, int wanted_flags = 0, int wanted_vis = 0, int param_ct = -1 ) {
		std::vector<method_info_t*> matches;

		void* iter = nullptr;
		while ( method_info_t* method = klass->methods( &iter ) ) {
			if ( method->should_filter( filter ) )
				continue;

			uint32_t count = method->param_count();
			if ( param_ct != -1 && count != param_ct )
				continue;

			il2cpp_type_t* returnType = method->return_type();

			if ( strcmp( returnType->klass()->name(), ret_type_klass->name() ) == 0 )
			{
				uint32_t vis = method->flags() & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
				if ( wanted_vis && ( vis != wanted_vis ) )
					continue;

				if ( wanted_flags && !( method->flags() & wanted_flags ) )
					continue;

				matches.push_back( method );
			}
		}

		return matches;
	}

	inline method_info_t* get_method_by_param_class( method_filter_t filter, il2cpp_class_t* klass, il2cpp_class_t* param_klass, int param_ct, int wanted_vis, int wanted_flags )
	{
		const auto get_method_by_param_class = [ = ] ( method_info_t* method ) -> bool {
			uint32_t count = method->param_count( );
			if ( count != param_ct )
				return false;

			for ( uint32_t i = 0; i < count; i++ )
			{
				il2cpp_type_t* param = method->get_param( i );
				if ( !param )
					continue;

				if ( param->klass( ) == param_klass )
				{
					int vis = method->flags( ) & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
					if ( wanted_vis && wanted_vis != vis )
						return false;

					if ( wanted_flags && !( method->flags( ) & wanted_flags ) )
						return false;

					return true;
				}
			}

			return false;
		};

		return get_method_from_class( filter, klass, get_method_by_param_class );
	}

	inline method_info_t* get_method_by_return_type_attrs_method_attr( method_filter_t filter, il2cpp_class_t* klass, il2cpp_class_t* ret_type_klass, il2cpp_class_t* method_attr_klass, int wanted_flags = 0, int wanted_vis = 0, int param_ct = -1, bool want_or_ignore = false ) {
		void* iter = nullptr;

		const auto get_method_by_return_type_attrs = [ = ] ( method_info_t* method ) -> bool {
			uint32_t count = method->param_count( );
			if ( param_ct != -1 && count != param_ct )
				return false;

			il2cpp_type_t* return_type = method->return_type( );
			if ( strcmp( return_type->klass( )->name( ), ret_type_klass->name( ) ) == 0 )
			{
				bool has_attr = method_has_attribute( method, method_attr_klass );
				if ( has_attr && want_or_ignore == e_attr_search::attr_search_ignore )
					return false;
				if ( !has_attr && want_or_ignore == e_attr_search::attr_search_want )
					return false;

				uint32_t vis = method->flags( ) & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
				if ( wanted_vis && ( vis != wanted_vis ) )
					return false;

				if ( wanted_flags && !( method->flags( ) & wanted_flags ) )
					return false;

				return true;
			}

			return false;
		};

		return get_method_from_class( filter, klass, get_method_by_return_type_attrs );
	}

	inline void get_methods_in_method_impl( std::vector<method_info_t*>* methods, uint64_t method_addr, uint32_t limit ) {
		if ( limit == 0 )
			return;

		il2cpp::call_set_t* calls = il2cpp::get_inverse_calls( method_addr );
		if ( !calls )
			return;

		for ( auto call : *calls ) {
			if ( il2cpp::method_info_t* method = il2cpp::method_info_t::from_addr( call ) )
				methods->push_back( method );

			get_methods_in_method_impl( methods, call, limit - 1 );
		}
	}

	inline std::vector<method_info_t*> get_methods_in_method( const method_filter_t& filter, bool include_self = false ) {
		std::vector<method_info_t*> methods;
		if ( !filter.target )
			return methods;

		if ( include_self )
			if ( method_info_t* method = il2cpp::method_info_t::from_addr( filter.target ) )
				methods.push_back( method );

		get_methods_in_method_impl( &methods, filter.target, filter.max_depth );
		return methods;
	}

	inline il2cpp::method_info_t* get_method_in_method_by_return_type( il2cpp_class_t* klass, method_filter_t filter, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, int param_ct ) {
		std::vector<method_info_t*> methods = get_methods_in_method( filter );

		for ( uint32_t i = 0; i < methods.size(); i++ ) {
			method_info_t* method = methods.at( i );
			if ( !method )
				continue;

			if ( klass != WILDCARD_VALUE( il2cpp_class_t* ) )
				if ( method->klass() != klass )
					continue;

			uint32_t count = method->param_count();
			if ( count != param_ct )
				continue;

			il2cpp::il2cpp_type_t* ret = method->return_type();
			if ( strcmp( ret->name(), ret_type->name() ) )
				continue;

			int vis = method->flags() & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
			if ( wanted_vis && vis != wanted_vis )
				continue;

			if ( wanted_flags && !( method->flags() & wanted_flags ) )
				continue;

			return method;
		}

		return nullptr;
	}

	inline il2cpp::method_info_t* get_method_in_method_by_return_type_and_param_types( il2cpp_class_t* klass, method_filter_t filter, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct ) {
		std::vector<method_info_t*> methods = get_methods_in_method( filter );

		for ( uint32_t i = 0; i < methods.size(); i++ ) {
			method_info_t* method = methods.at( i );
			if ( !method )
				continue;

			if ( klass != WILDCARD_VALUE( il2cpp_class_t* ) ) 
				if ( method->klass() != klass )
					continue;

			uint32_t count = method->param_count();
			if ( count != param_ct )
				continue;

			if ( ret_type != WILDCARD_VALUE( il2cpp_type_t* ) ) {
				il2cpp::il2cpp_type_t* ret = method->return_type();
				if ( strcmp( ret->name(), ret_type->name() ) )
					continue;
			}

			int vis = method->flags() & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
			if ( wanted_vis && vis != wanted_vis )
				continue;

			if ( wanted_flags && !( method->flags() & wanted_flags ) )
				continue;

			int match_requirement = 0;
			for ( uint32_t j = 0; j < param_ct; j++ )
				if ( param_types[ j ] != WILDCARD_VALUE( il2cpp_type_t* ) )
					match_requirement++;

			int matched_types = 0;
			for ( uint32_t k = 0; k < count; k++ ) {
				if ( param_types[ k ] == WILDCARD_VALUE( il2cpp_type_t* ) )
					continue;

				il2cpp_type_t* param = method->get_param( k );
				if ( !param )
					continue;

				if ( strcmp( param->name(), param_types[ k ]->name() ) == 0 )
					matched_types++;
			}

			if ( matched_types == match_requirement )
				return method;
		}

		return nullptr; 
	}

	struct functions_in_function_t {
		functions_in_function_t( uint64_t _parent, std::vector<uint64_t> _children ) :
			parent( _parent ), children( _children ) {};

		functions_in_function_t( uint64_t _parent ) : parent( _parent ) {};

		uint64_t parent;
		std::vector<uint64_t> children;
	};

	inline void get_functions_in_function_tree_impl( std::vector<functions_in_function_t>* functions_in_functions, uint64_t function, uint32_t limit ) {
		if ( limit == 0 )
			return;

		functions_in_function_t functions_in_function( function );

		il2cpp::call_set_t* calls = il2cpp::get_inverse_calls( function );
		if ( !calls )
			return;

		for ( auto call : *calls ) {
			functions_in_function.children.push_back( call );
			get_functions_in_function_tree_impl( functions_in_functions, call, limit - 1 );
		}

		functions_in_functions->push_back( functions_in_function );
	}

	inline std::vector<functions_in_function_t> get_functions_in_function_tree( const method_filter_t& filter ) {
		std::vector<functions_in_function_t> functions_in_function;
		if ( !filter.target )
			return functions_in_function;

		get_functions_in_function_tree_impl( &functions_in_function, filter.target, filter.max_depth );
		return functions_in_function;
	}

	inline il2cpp::method_info_t* get_method_containing_function( method_filter_t filter, uint64_t function ) {
		std::vector<functions_in_function_t> function_tree = get_functions_in_function_tree( filter );

		for ( uint32_t i = 0; i < function_tree.size(); i++ ) {
			const functions_in_function_t& functions_in_function = function_tree.at( i );

			for ( uint32_t j = 0; j < functions_in_function.children.size(); j++ ) {
				uint64_t child_function = functions_in_function.children.at( j );
				if ( !child_function )
					continue;

				if ( child_function == function ) {
					return il2cpp::method_info_t::from_addr( functions_in_function.parent );
				}
			}
		}

		return nullptr;
	}

	struct virtual_method_t {
		virtual_method_t( method_info_t* _method, uint32_t _offset ) :
			method( _method ), offset( _offset ) {};

		method_info_t* method;
		uint32_t offset;
	};

	inline std::vector<uint32_t> get_virtual_method_offsets( const method_filter_t& filter ) {
		std::vector<uint32_t> method_offsets;
		std::vector<method_info_t*> methods = get_methods_in_method( filter, true );

		for ( uint32_t i = 0; i < methods.size(); i++ ) {
			method_info_t* method = methods.at( i );
			if ( !method )
				continue;

			uint64_t address = method->get_fn_ptr<uint64_t>();
			if ( !address )
				continue;

			uint64_t limit = filter.ignore ? filter.ignore : get_function_size( address );
			if ( !limit )
				continue;

			std::vector<hde64s> instructions;

			uint64_t len = 0;
			while ( len < limit ) {
				uint8_t* inst = ( uint8_t* )address + len;

				hde64s hs;
				hde64_disasm( inst, &hs );

				if ( hs.flags & F_ERROR ) {
					break;
				}

				if ( hs.opcode == 0xFF ) {
					if ( hs.modrm_mod == 0x2 ) {
						method_offsets.push_back( hs.disp.disp32 );
					}

					else if ( hs.modrm_mod == 0x3 ) {
						uint32_t disp32 = 0;

						size_t instruction_ct = instructions.size();
						size_t backtrack_ct = min( instruction_ct, 4 );

						for ( uint32_t i = 0; i < backtrack_ct; i++ ) {
							const hde64s& instr = instructions.at( instruction_ct - i - 1 );

							if ( instr.opcode == 0x8B &&
								instr.modrm_mod == 0x2 ) {

								if ( hs.rex_b == instr.rex_r &&
									hs.modrm_rm == instr.modrm_reg ) {
									method_offsets.push_back( instr.disp.disp32 );
								}
							}
						}
					}
				}

				len += hs.len;
				instructions.push_back( hs );
			}
		}

		return method_offsets;
	}

	inline virtual_method_t get_virtual_method_by_return_type_and_param_types( method_filter_t filter, il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct ) {
		std::vector<uint32_t> method_offsets = get_virtual_method_offsets( filter );

		for ( uint32_t i = 0; i < method_offsets.size(); i++ ) {
			uint64_t deref_addr = ( uint64_t )klass + method_offsets.at( i );
			if ( !is_valid_ptr( deref_addr ) )
				continue;

			il2cpp::method_info_t* method = il2cpp::method_info_t::from_addr( *( uint64_t* )( deref_addr ) );
			if ( !method )
				continue;

			if ( method->klass() != klass )
				continue;

			uint32_t count = method->param_count();
			if ( count != param_ct )
				continue;

			il2cpp::il2cpp_type_t* ret = method->return_type();
			if ( !ret || strcmp( ret->name(), ret_type->name() ) != 0 )
				continue;		

			int vis = method->flags() & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
			if ( wanted_vis && vis != wanted_vis ) 
				continue;

			if ( wanted_flags && !( method->flags() & wanted_flags ) )
				continue;

			int match_requirement = 0;
			for ( uint32_t j = 0; j < param_ct; j++ )
				if ( param_types[ j ] != WILDCARD_VALUE( il2cpp_type_t* ) )
					match_requirement++;

			int matched_types = 0;
			for ( uint32_t k = 0; k < count; k++ ) {
				if ( param_types[ k ] == WILDCARD_VALUE( il2cpp_type_t* ) )
					continue;

				il2cpp_type_t* param = method->get_param( k );
				if ( !param )
					continue;

				if ( strcmp( param->name(), param_types[ k ]->name() ) == 0 )
					matched_types++;
			}

			if ( matched_types != match_requirement )
				continue;

			return virtual_method_t( method, method_offsets.at( i ) );
		}	

		return virtual_method_t( nullptr, 0 );
	}

	inline virtual_method_t get_virtual_method_by_return_type( method_filter_t filter, il2cpp_class_t* klass, il2cpp_type_t* ret_type, int param_ct, int wanted_vis, int wanted_flags ) {
		std::vector<uint32_t> method_offsets = get_virtual_method_offsets( filter );

		for ( uint32_t i = 0; i < method_offsets.size(); i++ ) {
			uint64_t deref_addr = ( uint64_t )klass + method_offsets.at( i );
			if ( !is_valid_ptr( deref_addr ) )
				continue;

			il2cpp::method_info_t* method = il2cpp::method_info_t::from_addr( *( uint64_t* )( deref_addr ) );
			if ( !method )
				continue;

			if ( method->klass() != klass )
				continue;

			uint32_t count = method->param_count();
			if ( count != param_ct )
				continue;

			il2cpp::il2cpp_type_t* ret = method->return_type();
			if ( !ret || strcmp( ret->name(), ret_type->name() ) != 0 )
				continue;

			int vis = method->flags() & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
			if ( wanted_vis && vis != wanted_vis )
				continue;

			if ( wanted_flags && !( method->flags() & wanted_flags ) )
				continue;

			return virtual_method_t( method, method_offsets.at( i ) );
		}

		return virtual_method_t( nullptr, 0 );
	}

	inline virtual_method_t get_virtual_method_by_name( il2cpp_class_t* klass, const char* method_name, int param_ct = 1337 ) {
		const auto get_virtual_method_by_name = [ = ]( method_info_t* method ) -> bool {
			if ( param_ct != 1337 && method->param_count() != param_ct )
				return false;

			if ( !( method->flags() & METHOD_ATTRIBUTE_VIRTUAL ) )
				return false;

			const char* name = method->name();
			if ( !name )
				return false;

			return strcmp( name, method_name ) == 0;
		};

		method_info_t* method_info = get_method_from_class( NO_FILT, klass, get_virtual_method_by_name );
		if ( !method_info )
			return virtual_method_t( nullptr, 0 );

		return virtual_method_t( method_info, method_info->get_vtable_offset() );
	}

	template<typename Comparator>
	inline field_info_t* get_field_from_class( il2cpp_class_t* klass, Comparator comparator ) {
		void* iter = nullptr;
		while ( field_info_t* field = klass->fields( &iter ) )
		{
			if ( comparator( field ) )
				return field;
		}

		return nullptr;
	}
	
	inline field_info_t* get_field_if_type_contains( il2cpp_class_t* klass, const char* search, int wanted_vis = 0, int flags = 0, int type_flags =0 )
	{
		const auto get_field_if_type_contains = [ = ] ( field_info_t* field ) -> bool {
			const char* name = field->type( )->name( );
			if ( !name )
				return false;

			int fl = field->flags( );
			int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( flags && !( fl & flags ) ) )
				return false;

			if ( type_flags && !( field->type()->klass()->flags() & type_flags ) )
				return false;

			return strstr( name, search ) != nullptr;
		};

		return get_field_from_class( klass, get_field_if_type_contains );
	}

	inline field_info_t* get_field_if_name_contains( il2cpp_class_t* klass, il2cpp_type_t* wanted_type, const char* search, int wanted_vis = 0, int flags = 0 )
	{
		const auto get_field_if_name_contains = [=]( field_info_t* field ) -> bool {
			const char* name = field->type()->name();
			if ( !name )
				return false;

			if ( !strstr( field->name(), search ) )
				return false;

			int fl = field->flags();
			int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( flags && !( fl & flags ) ) )
				return false;

			return strcmp( name, wanted_type->name() ) == 0;
		};

		return get_field_from_class( klass, get_field_if_name_contains );
	}

	inline field_info_t* get_field_if_type_contains_without_attrs( il2cpp_class_t* klass, const char* search, int wanted_vis = 0, int flags = 0 )
	{
		const auto get_field_if_type_contains = [ = ] ( field_info_t* field ) -> bool {
			const char* name = field->type( )->name( );
			if ( !name )
				return false;

			int fl = field->flags( );
			int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( flags && ( fl & flags ) ) )
				return false;

			return strstr( name, search ) != nullptr;
		};

		return get_field_from_class( klass, get_field_if_type_contains );
	}

	inline field_info_t* get_field_from_flags( il2cpp_class_t* klass, int flags ) {
		const auto get_field_from_flags = [=]( field_info_t* field ) -> bool {
			if ( field->flags() == flags )
				return true;

			return false;
		};

		return get_field_from_class( klass, get_field_from_flags );
	}

	inline field_info_t* get_field_from_field_type_class( il2cpp_class_t* klass, il2cpp_class_t* wanted_klass )
	{
		const auto get_field_from_field_type_class = [ = ] ( field_info_t* field ) -> bool {
			const char* name = field->type( )->name( );
			if ( !name )
				return false;

			return strcmp( name, wanted_klass->type()->name( ) ) == 0;
		};

		return get_field_from_class( klass, get_field_from_field_type_class );
	}

	inline field_info_t* get_field_by_offset( il2cpp_class_t* klass, uint64_t offset )
	{
		const auto get_field_by_offset = [ = ] ( field_info_t* field ) -> bool {
			return field->offset( ) == offset;
		};

		return get_field_from_class( klass, get_field_by_offset );
	}

	inline field_info_t* get_field_by_name( il2cpp_class_t* klass, const char* name )
	{
		const auto get_field_by_name = [ = ] ( field_info_t* field ) -> bool {
			return strcmp( field->name( ), name ) == 0;
		};

		return get_field_from_class( klass, get_field_by_name );
	}

	inline field_info_t* get_field_by_type_name_attrs( il2cpp_class_t* klass, const char* type_name, int wanted_vis = 0, int wanted_attrs = 0 )
	{
		const auto get_field_by_type_name_attrs = [ = ] ( field_info_t* field ) -> bool {
			const char* name = field->type( )->name( );
			if ( !name )
				return false;

			int attrs = field->type( )->attributes( );
			int vis = attrs & TYPE_ATTRIBUTE_VISIBILITY_MASK;
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( wanted_attrs && !( wanted_attrs & attrs ) ) )
				return false;

			return strcmp( name, type_name ) == 0;
		};

		return  get_field_from_class( klass, get_field_by_type_name_attrs );
	} 

	inline field_info_t* get_field_if_type_contains_terms( il2cpp_class_t* klass, const char** search_terms, int search_terms_ct ) {
		const auto get_field_if_type_contains_multiple = [ = ] ( field_info_t* field ) -> bool {			
			const char* name = field->type( )->name( );
			if ( !name )
				return false;

			for ( int i = 0; i < search_terms_ct; i++ ) {
				const char* term = search_terms[ i ];
				if ( !strstr( name, term ) )
					return false;
			}

			return true;
		};

		return get_field_from_class( klass, get_field_if_type_contains_multiple );
	}

	inline field_info_t* get_field_by_type_attrs_method_attr( il2cpp_class_t* klass, il2cpp_class_t* type_klass, il2cpp_class_t* method_attr_klass, int wanted_flags = 0, int wanted_vis = 0, bool want_or_ignore = false ) {
		void* iter = nullptr;

		const auto get_field_by_type_attrs_method_attrs = [ = ] ( field_info_t* field ) -> bool {
			if ( strcmp( field->type( )->name( ), type_klass->type( )->name( ) ) == 0 )
			{
				bool has_attr = field_has_attribute( field, method_attr_klass );
				if ( has_attr && want_or_ignore == e_attr_search::attr_search_ignore )
					return false;
				if ( !has_attr && want_or_ignore == e_attr_search::attr_search_want )
					return false;

				uint32_t vis = field->flags( ) & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
				if ( wanted_vis && ( vis != wanted_vis ) )
					return false;

				if ( wanted_flags && !( field->flags( ) & wanted_flags ) )
					return false;

				return true;
			}

			return false;
		};

		return get_field_from_class( klass, get_field_by_type_attrs_method_attrs );
	}

	inline std::vector<field_info_t*> get_fields_of_type( il2cpp_class_t* klass, il2cpp_type_t* wanted_type, int wanted_vis, int wanted_attrs ) {
		std::vector<field_info_t*> fields;

		void* iter = nullptr;
		while ( il2cpp::field_info_t* field = klass->fields( &iter ) ) {
			if ( strcmp( field->type()->name(), wanted_type->name() ) )
				continue;

			int attrs = field->type()->attributes();
			int vis = attrs & TYPE_ATTRIBUTE_VISIBILITY_MASK;

			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( wanted_attrs && !( wanted_attrs & attrs ) ) ) 
				continue;

			fields.push_back( field );
		}

		return fields;
	}

	template <typename T, typename C>
	inline field_info_t* get_static_field_if_value_is( il2cpp_class_t* klass, const char* search, int wanted_vis, int flags, C compare )
	{
		flags |= FIELD_ATTRIBUTE_STATIC;

		if ( !klass->static_field_data() )
			return nullptr;

		void* iter = nullptr;
		while ( field_info_t* field = klass->fields( &iter ) ) {
			const char* name = field->type()->name();
			if ( !name )
				continue;

			int fl = field->flags();
			int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( flags && !( fl & flags ) ) )
				continue;

			if ( !strstr( name, search ) )
				continue;

			T value = field->static_get_value<T>();

			if ( compare( value ) )
				return field;
		}

		return nullptr;
	}

	template <typename T, typename C>
	inline field_info_t* get_static_field_if_value_is( il2cpp_class_t* klass, il2cpp_class_t* type, int wanted_vis, int flags, C compare )
	{
		flags |= FIELD_ATTRIBUTE_STATIC;

		if ( !klass->static_field_data() )
			return nullptr;

		void* iter = nullptr;
		while ( field_info_t* field = klass->fields( &iter ) ) {
			if ( field->type()->klass() != type )
				continue;

			int fl = field->flags();
			int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( flags && !( fl & flags ) ) )
				continue;

			T value = field->static_get_value<T>();

			if ( compare( value ) )
				return field;
		}

		return nullptr;
	}

	inline void init() {
		ASSIGN_TYPE( domain_get );
		ASSIGN_TYPE( domain_get_assemblies );
		ASSIGN_TYPE( domain_assembly_open );

		ASSIGN_TYPE( assembly_get_image );

		ASSIGN_TYPE( class_from_name );
		ASSIGN_TYPE( image_get_class_count );
		ASSIGN_TYPE( image_get_class );

		ASSIGN_TYPE( type_get_object );
		ASSIGN_TYPE( type_get_class_or_element_class );
		ASSIGN_TYPE( type_get_name );
		ASSIGN_TYPE( type_get_attrs );

		ASSIGN_TYPE( class_get_methods );
		ASSIGN_TYPE( class_get_nested_types )
		ASSIGN_TYPE( class_get_fields);
		ASSIGN_TYPE( class_get_type );
		ASSIGN_TYPE( class_get_name );
		ASSIGN_TYPE( class_get_namespace );
		ASSIGN_TYPE( class_from_il2cpp_type );
		ASSIGN_TYPE( class_get_static_field_data );
		ASSIGN_TYPE( class_get_parent );
		ASSIGN_TYPE( class_get_interfaces );
		ASSIGN_TYPE( class_get_image );
		ASSIGN_TYPE( class_get_flags );

		ASSIGN_TYPE( method_get_param_count );
		ASSIGN_TYPE( method_get_name );
		ASSIGN_TYPE( method_get_param_name );
		ASSIGN_TYPE( method_get_param );
		ASSIGN_TYPE( method_get_return_type );
		ASSIGN_TYPE( method_get_class );
		ASSIGN_TYPE( method_get_flags );
		ASSIGN_TYPE( method_has_attribute );

		ASSIGN_TYPE( field_get_offset );
		ASSIGN_TYPE( field_get_type );
		ASSIGN_TYPE( field_get_parent );
		ASSIGN_TYPE( field_get_name );
		ASSIGN_TYPE( field_static_get_value );
		ASSIGN_TYPE( field_get_flags );
		ASSIGN_TYPE( field_has_attribute );

		ASSIGN_TYPE( object_get_class );
		ASSIGN_TYPE( object_new );

		ASSIGN_TYPE( resolve_icall );

		// Build the call cache
		build_call_cache();
	}
}