#pragma once
#include <windows.h>
#include <cstdio>
#include <vector>
#include <algorithm>

#include "util.hpp"

#define STR( x ) #x
#define CREATE_TYPE( name, args ) using il2cpp_##name = args; inline il2cpp_##name name;
#define ASSIGN_TYPE( name ) name = (decltype(name)) GetProcAddress( GetModuleHandleA( "GameAssembly.dll" ), STR( il2cpp_##name ) );

namespace il2cpp
{
	// get_method_by_return_type_attrs_method_attr
	enum e_attr_search : uint8_t {
		attr_search_want = 0,
		attr_search_ignore
	};

#define METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK        0x0007
#define METHOD_ATTRIBUTE_COMPILER_CONTROLLED       0x0000
#define METHOD_ATTRIBUTE_PRIVATE                   0x0001
#define METHOD_ATTRIBUTE_FAM_AND_ASSEM             0x0002
#define METHOD_ATTRIBUTE_ASSEM                     0x0003
#define METHOD_ATTRIBUTE_FAMILY                    0x0004
#define METHOD_ATTRIBUTE_FAM_OR_ASSEM              0x0005
#define METHOD_ATTRIBUTE_PUBLIC                    0x0006

#define METHOD_ATTRIBUTE_STATIC                    0x0010
#define METHOD_ATTRIBUTE_FINAL                     0x0020
#define METHOD_ATTRIBUTE_VIRTUAL                   0x0040
#define METHOD_ATTRIBUTE_HIDE_BY_SIG               0x0080

#define FIELD_ATTRIBUTE_FIELD_ACCESS_MASK     0x0007
#define FIELD_ATTRIBUTE_COMPILER_CONTROLLED   0x0000
#define FIELD_ATTRIBUTE_PRIVATE               0x0001
#define FIELD_ATTRIBUTE_FAM_AND_ASSEM         0x0002
#define FIELD_ATTRIBUTE_ASSEMBLY              0x0003
#define FIELD_ATTRIBUTE_FAMILY                0x0004
#define FIELD_ATTRIBUTE_FAM_OR_ASSEM          0x0005
#define FIELD_ATTRIBUTE_PUBLIC                0x0006

#define FIELD_ATTRIBUTE_STATIC                0x0010
#define FIELD_ATTRIBUTE_INIT_ONLY             0x0020
#define FIELD_ATTRIBUTE_LITERAL               0x0040
#define FIELD_ATTRIBUTE_NOT_SERIALIZED        0x0080
#define FIELD_ATTRIBUTE_SPECIAL_NAME          0x0200
#define FIELD_ATTRIBUTE_PINVOKE_IMPL          0x2000

#define TYPE_ATTRIBUTE_VISIBILITY_MASK       0x00000007
#define TYPE_ATTRIBUTE_NOT_PUBLIC            0x00000000
#define TYPE_ATTRIBUTE_PUBLIC                0x00000001
#define TYPE_ATTRIBUTE_NESTED_PUBLIC         0x00000002
#define TYPE_ATTRIBUTE_NESTED_PRIVATE        0x00000003
#define TYPE_ATTRIBUTE_NESTED_FAMILY         0x00000004
#define TYPE_ATTRIBUTE_NESTED_ASSEMBLY       0x00000005
#define TYPE_ATTRIBUTE_NESTED_FAM_AND_ASSEM  0x00000006
#define TYPE_ATTRIBUTE_NESTED_FAM_OR_ASSEM   0x00000007
#define TYPE_ATTRIBUTE_INTERFACE			 0x00000020
#define TYPE_ATTRIBUTE_ABSTRACT 			 0x00000080
#define TYPE_ATTRIBUTE_SEALED 		         0x00000100
#define TYPE_ATTRIBUTE_SERIALIZABLE		     0x00002000

	struct property_info_t;
	struct method_info_t;
	struct field_info_t;
	struct il2cpp_type_t;
	struct il2cpp_class_t;
	struct il2cpp_image_t;
	struct il2cpp_assembly_t;
	struct il2cpp_domain_t;
	struct il2cpp_object_t;

	// Domain.
	CREATE_TYPE( domain_get, il2cpp_domain_t* ( * )( ) );
	CREATE_TYPE( domain_get_assemblies, il2cpp_assembly_t** ( * )( void*, size_t* ) );
	CREATE_TYPE( domain_assembly_open, il2cpp_assembly_t* ( * )( void*, const char* ) );

	// Assembly.
	CREATE_TYPE( assembly_get_image, il2cpp_image_t* ( * )( void* ) );

	// Image.
	CREATE_TYPE( class_from_name, il2cpp_class_t* ( * )( void*, const char*, const char* ) );
	CREATE_TYPE( image_get_class_count, size_t( * )( il2cpp_image_t* ) );
	CREATE_TYPE( image_get_class, il2cpp_class_t* ( * )( il2cpp_image_t*, size_t ) );
	// Types.
	CREATE_TYPE( type_get_object, il2cpp_object_t* ( * )( void* ) );
	CREATE_TYPE( type_get_class_or_element_class, il2cpp_class_t* ( * )( void* ) );
	CREATE_TYPE( type_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( type_get_attrs, uint32_t( * )( il2cpp_type_t* ) );
	// Class.
	CREATE_TYPE( class_get_methods, method_info_t* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_fields, field_info_t* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_type, il2cpp_type_t* ( * )( void* ) );
	CREATE_TYPE( class_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( class_get_namespace, const char* ( * )( void* ) );
	CREATE_TYPE( class_from_il2cpp_type, il2cpp_class_t* ( * )( il2cpp_type_t* ) );
	CREATE_TYPE( class_get_static_field_data, uint64_t*( * )( void* ) );
	CREATE_TYPE( class_get_parent, il2cpp_class_t* ( * )( il2cpp_class_t* klass ) );
	CREATE_TYPE( class_get_interfaces, il2cpp_class_t* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_image, il2cpp_image_t* ( * )( void* ) );

	// Methods.
	CREATE_TYPE( method_get_param_count, uint32_t( * )( void* ) );
	CREATE_TYPE( method_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( method_get_param_name, const char* ( * )( method_info_t*, uint32_t ) );
	CREATE_TYPE( method_get_param, il2cpp_type_t* ( * )( method_info_t*, uint32_t ) );
	CREATE_TYPE( method_get_return_type, il2cpp_type_t* ( * )( void* ) );
	CREATE_TYPE( method_get_class, il2cpp_class_t* ( * )( method_info_t* ) );
	CREATE_TYPE( method_get_flags, uint32_t ( * )( method_info_t*, uint32_t* ) );
	CREATE_TYPE( method_has_attribute, bool( * )( method_info_t*, il2cpp_class_t* ) );
	// Fields.
	CREATE_TYPE( field_get_offset, size_t( * )( void* ) );
	CREATE_TYPE( field_get_type, il2cpp_type_t* ( * )( void* ) );
	CREATE_TYPE( field_get_parent, il2cpp_class_t* ( * )( void* ) );
	CREATE_TYPE( field_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( field_static_get_value, void( * )( void*, uint64_t* ) );
	CREATE_TYPE( field_get_flags, int( * )( void* ) );
	CREATE_TYPE( field_has_attribute, bool( * )( void*, il2cpp_class_t* ) );

	// Object.
	CREATE_TYPE( object_get_class, il2cpp_class_t* ( * )( void* ) );

	CREATE_TYPE( resolve_icall, std::uintptr_t( * )( const char* ) );

	struct il2cpp_type_t
	{
		il2cpp_class_t* klass( )
		{
			if ( !this )
				return nullptr;
			return type_get_class_or_element_class( this );
		}

		const char* name( )
		{
			if ( !this )
				return nullptr;
			return type_get_name( this );
		}

		uint32_t attributes( )
		{
			if ( !this )
				return 0;
			return type_get_attrs( this );
		}
	};

	struct property_info_t { };

	struct method_info_t
	{
		const char* name( )
		{
			if ( !this )
				return nullptr;

			return method_get_name( this );
		}

		il2cpp_type_t* get_param( uint32_t index )
		{
			if ( !this )
				return nullptr;

			return method_get_param( this, index );
		}

		const char* get_param_name( uint32_t index )
		{
			if ( !this )
				return nullptr;

			return method_get_param_name( this, index );
		}

		uint32_t param_count( )
		{
			if ( !this )
				return 0;

			return method_get_param_count( this );
		}

		il2cpp_type_t* return_type( )
		{
			if ( !this )
				return nullptr;

			return method_get_return_type( this );
		}

		il2cpp_class_t* klass( )
		{
			if ( !this )
				return nullptr;

			return method_get_class( this );
		}

		uint32_t flags( )
		{
			if ( !this )
				return { };

			return method_get_flags( this, nullptr );
		}

		template<typename T>
		T get_fn_ptr( )
		{
			if ( !this )
				return T();

			return *( T* )( this );
		}
	};

	struct field_info_t
	{
		il2cpp_type_t* type( )
		{
			if ( !this )
				return nullptr;

			return field_get_type( this );
		}

		const char* name( )
		{
			if ( !this )
				return nullptr;

			return field_get_name( this );
		}

		size_t offset( )
		{
			if ( !this )
				return 0;

			return field_get_offset( this );
		}

		uint64_t static_get_value( )
		{
			if ( !this )
				return 0;

			uint64_t value{ };
			field_static_get_value( this, &value );
			return value;
		}

		int flags( )
		{
			if ( !this )
				return 0;

			return field_get_flags( this );
		}
	};

	struct il2cpp_class_t
	{ 
		const char* name( )
		{
			if ( !this )
				return nullptr;

			return class_get_name( this );
		}

		const char* namespaze( )
		{
			if ( !this )
				return nullptr;

			return class_get_namespace( this );
		}

		il2cpp_type_t* type( )
		{
			if ( !this )
				return nullptr;
			return class_get_type( this );
		}

		field_info_t* fields( void** iter )
		{
			if ( !this )
				return nullptr;
			return class_get_fields( this, iter );
		}

		uint32_t field_count( ) {
			if ( !this )
				return 0;

			uint32_t count{};
			void* iter = nullptr;
			while ( field_info_t* field = fields( &iter ) )
				count++;

			return count;
		}

		method_info_t* methods( void** iter )
		{
			if ( !this )
				return nullptr;

			return class_get_methods( this, iter );
		}

		uint64_t* static_field_data( )
		{
			if ( !this )
				return 0;

			return class_get_static_field_data( this );
		}

		il2cpp_class_t* parent( )
		{
			if ( !this )
				return nullptr;

			return class_get_parent( this );
		}

		il2cpp_class_t* interfaces( void** iter ) {
			if ( !this )
				return nullptr;

			return class_get_interfaces( this, iter );
		}

		il2cpp_image_t* image( ) {
			if ( !this )
				return nullptr;

			return class_get_image( this );
		}
	};

	struct image_global_metadata_t {
		int32_t m_type_start;
		int32_t m_exported_type_start;
		int32_t m_custom_attribute_start;
		int32_t m_entry_point_index;
		il2cpp_image_t* image;
	};

	struct il2cpp_image_t
	{
		image_global_metadata_t* metadata( ) {
			return *( image_global_metadata_t** ) ( uint64_t( this ) + 0x28 );
		}

		uint32_t type_count( ) {
			return *( uint32_t* ) ( uint64_t( this ) + 0x18 );
		}

		size_t class_count( )
		{
			if ( !this )
				return {};

			return image_get_class_count( this );
		}

		il2cpp_class_t* get_class( size_t index )
		{
			if ( !this )
				return nullptr;

			return image_get_class( this, index );
		}

		il2cpp_class_t* get_class_from_name( const char* namespaze, const char* klass )
		{
			if ( !this )
				return nullptr;

			return class_from_name( this, namespaze, klass );
		}
	};

	struct il2cpp_object_t
	{

	};

	struct il2cpp_assembly_t
	{
		il2cpp_image_t* image( )
		{
			if ( !this )
				return nullptr;

			return assembly_get_image( this );
		}
	};

	struct il2cpp_domain_t
	{
		static il2cpp_domain_t* get( )
		{
			return domain_get( );
		}

		il2cpp_assembly_t** get_assemblies( size_t* size )
		{
			if ( !this )
				return nullptr;

			return domain_get_assemblies( this, size );
		}

		il2cpp_assembly_t* get_assembly_from_name( const char* name )
		{
			if ( !this )
				return nullptr;

			return domain_assembly_open( this, name );
		}
	};

	extern il2cpp_class_t** s_TypeInfoDefinitionTable;

	inline int32_t get_typedef_idx_for_class( il2cpp_class_t* klass ) {
		il2cpp_image_t* image = klass->image( );
		if ( !image )
			return 0;

		uint32_t type_count = image->type_count( );
		for ( int i = 0; i < type_count; i++ ) {
			il2cpp_class_t* type = s_TypeInfoDefinitionTable[ image->metadata( )->m_type_start + i ];
			if ( !type )
				continue;

			if ( strcmp( type->namespaze( ), klass->namespaze( ) ) == 0 && strcmp( type->name( ), klass->name( ) ) == 0 )
				return i;
		}

		return 0;

	}
	inline il2cpp_assembly_t* get_assembly_by_name( const char* assemblyName )
	{
		il2cpp_domain_t* domain = il2cpp_domain_t::get( );
		if ( !domain )
			return nullptr;

		return domain->get_assembly_from_name( assemblyName );
	}

	template<typename T>
	inline il2cpp_class_t* get_class_from_class_field_type( il2cpp_class_t* klass, T comparator ) {
		void* iter = nullptr;
		while ( field_info_t* field = klass->fields( &iter ) )
		{
			if ( comparator( field ) )
				return class_from_il2cpp_type( field->type( ) );
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
	inline il2cpp_class_t* search_for_class_by_parent_class_type( il2cpp_type_t* parent_klass_type ) {
		const auto search_for_class_by_parent_class_type = [ = ] ( il2cpp_class_t* klass ) -> bool {
			il2cpp_class_t* parent = klass->parent( );
			if ( !parent )
				return false;

			return strcmp( parent->type( )->name( ), parent_klass_type->name( ) ) == 0;
		};

		return search_for_class( search_for_class_by_parent_class_type );
	}
	inline il2cpp_class_t* search_for_class_by_interfaces_contain( const char* search ) {
		const auto search_for_class_by_interfaces_contain = [ = ] ( il2cpp_class_t* klass ) {
			void* iter = nullptr;
			while ( il2cpp_class_t* iface = klass->interfaces( &iter ) ) {
				if ( strstr( iface->name( ), search ) )
					return true;
			}

			return false;
		};

		return search_for_class( search_for_class_by_interfaces_contain );
	}
	inline il2cpp_class_t* search_for_class_by_field_types( il2cpp_type_t* field_type, int field_type_ct, uint32_t wanted_flags ) {
		const auto search_for_class_by_field_types = [ = ] ( il2cpp_class_t* klass ) {
			void* iter = nullptr;

			int matchingFields = 0;

			while ( field_info_t* field = klass->fields( &iter ) ) {
				if ( ( wanted_flags && ( field->flags( ) & wanted_flags ) ) && strcmp( field->type( )->name( ), field_type->name( ) ) == 0 )
					matchingFields++;
			}

			return matchingFields == field_type_ct;
		};

		return search_for_class( search_for_class_by_field_types );
	}

	inline il2cpp_class_t* search_for_static_class_with_method_with_rettype_param_types( int method_ct, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct ) {
		const auto search_for_static_class_with_method_with_rettype_param_types = [ = ] ( il2cpp_class_t* klass ) {
			il2cpp::il2cpp_type_t* type = klass->type( );

			if ( !type )
				return false;

			uint32_t attrs = type->attributes( );

			if ( attrs != 0 )
				return false;

			void* iter = nullptr;
			uint32_t count = 0;

			while ( method_info_t* method = klass->methods( &iter ) ) {
				uint32_t count = method->param_count( );
				if ( count != param_ct )
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
				for ( uint32_t i = 0; i < count; i++ ) {
					il2cpp_type_t* param = method->get_param( i );
					if ( !param )
						continue;

					if ( strcmp( param->name( ), param_types[ i ]->name( ) ) == 0 )
						matchedTypes++;
				}

				if ( matchedTypes == param_ct )
					return true;
			}

			if ( count != method_ct )
				return false;

			return false;
		};

		return search_for_class( search_for_static_class_with_method_with_rettype_param_types );
	}
	template<typename Comparator>
	inline method_info_t* get_method_from_class( il2cpp_class_t* klass, Comparator comparator ) {
		void* iter = nullptr;
		while ( method_info_t* method = klass->methods( &iter ) ) {
			if ( comparator( method ) )
				return method;
		}

		return nullptr;
	}
	inline method_info_t* get_method_by_name( il2cpp_class_t* klass, const char* method_name )
	{
		const auto get_method_by_name = [ = ] ( method_info_t* method ) -> bool {
			const char* name = method->name( );
			return name && strcmp( name, method_name ) == 0;
		};

		return get_method_from_class( klass, get_method_by_name );
	}
	inline method_info_t* get_method_by_return_type_str( il2cpp_class_t* klass, const char* ret_type_name, int param_ct = -1 )
	{
		const auto get_method_by_return_type_name = [ = ] ( method_info_t* method ) -> bool {
			const char* name = method->return_type( )->name( );
			if ( param_ct != -1 && ( param_ct != method->param_count( ) ) )
				return false;

			return name && strcmp( name, ret_type_name ) == 0;
		};

		return get_method_from_class( klass, get_method_by_return_type_name );
	}
	inline method_info_t* get_method_by_return_type_and_param_types( il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct ) {
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

		return get_method_from_class( klass, get_method_by_return_type_and_param_types );
	}
	inline method_info_t* get_method_by_return_type_and_param_types_str( il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, const char** param_strs, int param_ct ) {
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

		return get_method_from_class( klass, get_method_by_return_type_and_param_types );
	}

	inline method_info_t* get_method_by_return_type_and_param_types_size( int idx, il2cpp_class_t* klass, il2cpp_type_t* ret_type, int wanted_vis, int wanted_flags, il2cpp_type_t** param_types, int param_ct, il2cpp_class_t* method_attr_klass, bool want_or_ignore ) {
		struct method_info_match_t {
			method_info_t* method;
			size_t length;
		};

		std::vector<method_info_match_t> matches;

		void* iter = nullptr;
		while ( method_info_t* method = klass->methods( &iter ) ) {
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
				buffer.length = get_fn_length( method->get_fn_ptr<void*>(), 16384 );
				matches.push_back( std::move( buffer ) );
			}
		}

		std::sort( matches.begin(), matches.end(), 
			[]( const method_info_match_t& r1, const method_info_match_t& r2 ) -> bool { return r1.length > r2.length; } );

		if ( idx > matches.size() - 1 )
			return nullptr;

		return matches.at( idx ).method;
	}

	inline method_info_t* get_method_by_param_type_name( il2cpp_class_t* klass, const char* name, int param_ct, uint32_t flags )
	{
		void* iter = nullptr;

		const auto get_method_by_param_type_name = [ = ] ( method_info_t* method ) -> bool {
			uint32_t count = method->param_count( );
			if ( count != param_ct )
				return false;

			for ( uint32_t i = 0; i < count; i++ )
			{
				il2cpp_type_t* param = method->get_param( i );
				if ( !param )
					continue;

				if ( strcmp( param->name( ), name ) == 0 )
				{
					if ( flags && !( method->flags( ) & flags ) )
						continue;

					return true;
				}
			}

			return false;
		};

		return get_method_from_class( klass, get_method_by_param_type_name );
	}
	inline method_info_t* get_method_by_return_type_attrs( il2cpp_class_t* klass, il2cpp_class_t* ret_type_klass, int wanted_flags = 0, int wanted_vis = 0, int param_ct = -1 ) {
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

		return get_method_from_class( klass, get_method_by_return_type_attrs );
	}
	inline method_info_t* get_method_by_param_class( il2cpp_class_t* klass, il2cpp_class_t* param_klass, int param_ct, int wanted_vis, int wanted_flags )
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

		return get_method_from_class( klass, get_method_by_param_class );
	}

	inline method_info_t* get_method_by_return_type_attrs_method_attr( il2cpp_class_t* klass, il2cpp_class_t* ret_type_klass, il2cpp_class_t* method_attr_klass, int wanted_flags = 0, int wanted_vis = 0, int param_ct = -1, bool want_or_ignore = false ) {
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

		return get_method_from_class( klass, get_method_by_return_type_attrs );
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
	
	inline field_info_t* get_field_if_type_contains( il2cpp_class_t* klass, const char* search, int wanted_vis = 0, int flags = 0 )
	{
		const auto get_field_if_type_contains = [ = ] ( field_info_t* field ) -> bool {
			const char* name = field->type( )->name( );
			if ( !name )
				return false;

			int fl = field->flags( );
			int vis = fl & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( flags && !( fl & flags ) ) )
				return false;

			return strstr( name, search ) != nullptr;
		};

		return get_field_from_class( klass, get_field_if_type_contains );
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

			return strcmp( name, wanted_klass->name( ) ) == 0;
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

	inline void init( )
	{
		ASSIGN_TYPE( domain_get );
		ASSIGN_TYPE( domain_get_assemblies );
		ASSIGN_TYPE( domain_assembly_open );

		// Assembly.
		ASSIGN_TYPE( assembly_get_image );

		// Image.
		ASSIGN_TYPE( class_from_name );
		ASSIGN_TYPE( image_get_class_count );
		ASSIGN_TYPE( image_get_class );
		// Types.
		ASSIGN_TYPE( type_get_object );
		ASSIGN_TYPE( type_get_class_or_element_class );
		ASSIGN_TYPE( type_get_name );
		ASSIGN_TYPE( type_get_attrs );

		// Class.
		ASSIGN_TYPE( class_get_methods );
		ASSIGN_TYPE( class_get_fields);
		ASSIGN_TYPE( class_get_type );
		ASSIGN_TYPE( class_get_name );
		ASSIGN_TYPE( class_get_namespace );
		ASSIGN_TYPE( class_from_il2cpp_type );
		ASSIGN_TYPE( class_get_static_field_data );
		ASSIGN_TYPE( class_get_parent );
		ASSIGN_TYPE( class_get_interfaces );
		ASSIGN_TYPE( class_get_image );

		// Methods.
		ASSIGN_TYPE( method_get_param_count );
		ASSIGN_TYPE( method_get_name );
		ASSIGN_TYPE( method_get_param_name );
		ASSIGN_TYPE( method_get_param );
		ASSIGN_TYPE( method_get_return_type );
		ASSIGN_TYPE( method_get_class );
		ASSIGN_TYPE( method_get_flags );
		ASSIGN_TYPE( method_has_attribute );

		// Fields.
		ASSIGN_TYPE( field_get_offset );
		ASSIGN_TYPE( field_get_type );
		ASSIGN_TYPE( field_get_parent );
		ASSIGN_TYPE( field_get_name );
		ASSIGN_TYPE( field_static_get_value );
		ASSIGN_TYPE( field_get_flags );
		ASSIGN_TYPE( field_has_attribute );

		// Object.
		ASSIGN_TYPE( object_get_class );

		ASSIGN_TYPE( resolve_icall );
	}
}