#include "dumper.hpp"
#include "util.hpp"

#define SEARCH_FOR_CLASS_BY_FIELD_COUNT( field_ct, equality, ... ) \
[=]( ) -> il2cpp::il2cpp_class_t* \
{ \
	il2cpp::il2cpp_class_t* match_klasses[ ] = { __VA_ARGS__ }; \
	return il2cpp::search_for_class_by_field_count( match_klasses, _countof( match_klasses ), field_ct, equality ); \
} ( ) \

#define SEARCH_FOR_STATIC_CLASS_BY_METHOD_COUNT( method_ct, ret_type, wanted_vis, wanted_flags, ... ) \
[=]( ) -> il2cpp::il2cpp_class_t* \
{ \
	il2cpp::il2cpp_type_t* param_types[ ] = { __VA_ARGS__ }; \
	return il2cpp::search_for_static_class_with_method_with_rettype_param_types( method_ct, ret_type, wanted_vis, wanted_flags, param_types, _countof( param_types ) ); \
} ( )

#define SEARCH_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( ... ) \
[=]( ) -> il2cpp::field_info_t* \
{ \
	const char* search_terms[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_field_if_type_contains_terms( dumper_klass, search_terms, _countof( search_terms ) ); \
} ( )

#define SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES( filter, ret_type, wanted_vis, wanted_flags, ... ) \
[=]( ) -> il2cpp::method_info_t* \
{ \
	il2cpp::il2cpp_type_t* param_types[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_method_by_return_type_and_param_types( filter, dumper_klass, ret_type, wanted_vis, wanted_flags, param_types, _countof( param_types ) ); \
} ( )

#define SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES_STR( filter, ret_type, wanted_vis, wanted_flags, ... ) \
[=]( ) -> il2cpp::method_info_t* \
{ \
	const char* params[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_method_by_return_type_and_param_types_str(                                          \
		    filter, dumper_klass, ret_type, wanted_vis, wanted_flags, params, _countof( params ) );            \
	} ( )

#define SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES_SIZE( filter, idx, ret_type, wanted_vis, wanted_flags, ... )        \
	[=]( ) -> il2cpp::method_info_t* \
{ \
	il2cpp::il2cpp_type_t* param_types[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_method_by_return_type_and_param_types_size( filter, idx, dumper_klass, ret_type, wanted_vis, wanted_flags, param_types, _countof( param_types ), nullptr, false ); \
} ( )

#define DUMPER_VIS_DONT_CARE 0 
#define DUMPER_ATTR_DONT_CARE 0

#define DUMPER_RVA( X ) X - dumper::game_base
#define DUMPER_CLASS( name ) il2cpp::get_class_by_name( name )
#define DUMPER_CLASS_NAMESPACE( namespaze, name ) il2cpp::get_class_by_name( name, namespaze )
#define DUMPER_METHOD( klass, name ) il2cpp::get_method_by_name( klass, name )->get_fn_ptr< uint64_t >()

#define DUMPER_TYPE( name ) DUMPER_CLASS( name )->type()
#define DUMPER_TYPE_NAMESPACE( namespaze, name ) DUMPER_CLASS_NAMESPACE( namespaze, name )->type()

#define DUMPER_PTR_CLASS_NAME( dump_name, klass_ptr ) dumper::write_to_file( "#define " dump_name "_ClassName \"%s\"\n\n", klass_ptr->name( ) );
#define DUMPER_SECTION( dump_name ) dumper::write_to_file( "\t\n// " dump_name "\n" );
#define DUMPER_CLASS_HEADER( klass_name ) dumper::write_to_file( "namespace %s {\n", dumper::clean_klass_name( klass_name ) );

#define DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( klass_name, namespaze ) \
	{ \
		il2cpp::il2cpp_class_t* dumper_klass = il2cpp::get_class_by_name( klass_name, namespaze ); \
		DUMPER_CLASS_HEADER( klass_name );

#define DUMPER_CLASS_BEGIN_FROM_NAME( klass_name ) DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( klass_name, "" )

#define DUMPER_CLASS_BEGIN_FROM_PTR( dump_name, klass_ptr ) \
	{ \
		il2cpp::il2cpp_class_t* dumper_klass = klass_ptr; \
		dumper::write_to_file( "// obf name: %s::%s\n", dumper_klass->namespaze(), dumper_klass->name() ); \
		DUMPER_PTR_CLASS_NAME( dump_name, klass_ptr );	\
		DUMPER_CLASS_HEADER( dump_name );

#define DUMPER_CLASS_END dumper::write_to_file( "}\n\n" ); dumper::flush(); } 

#define DUMP_MEMBER_BY_X( NAME, X ) uint64_t NAME##_Offset = X; dumper::write_to_file("\tconstexpr const static size_t " #NAME " = 0x%x;\n", static_cast<uint32_t>( NAME##_Offset ) )

#define DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( NAME, field_type, wanted_vis, wanted_attrs )			DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_type_name_attrs( dumper_klass, field_type, wanted_vis, wanted_attrs )->offset( ) ) 
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( NAME, search )									DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_if_type_contains( dumper_klass, search )->offset( ) )
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( NAME, search, wanted_vis, wanted_attrs )	DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_if_type_contains( dumper_klass, search, wanted_vis, wanted_attrs )->offset( ) )
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS( NAME, wanted_klass )										DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_from_field_type_class( dumper_klass, wanted_klass )->offset( ) )
#define DUMP_MEMBER_BY_NAME( NAME )																	DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_name( dumper_klass, #NAME )->offset( ) )
#define DUMP_MEMBER_BY_NEAR_OFFSET( NAME, off )														DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_offset( dumper_klass, off )->offset( ) )
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( NAME, ... )								DUMP_MEMBER_BY_X( NAME, SEARCH_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( __VA_ARGS__ )->offset( ) )
#define DUMP_MEMBER_BY_TYPE_METHOD_ATTRIBUTE(NAME, type_klass, method_attr, wanted_vis, wanted_attrs, want_or_ignore ) DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_type_attrs_method_attr( dumper_klass, type_klass, method_attr, wanted_attrs, wanted_vis, want_or_ignore )->offset() )

#define DUMP_ALL_MEMBERS_OF_TYPE( NAME, wanted_type, wanted_vis, wanted_attrs ) \
[=] ( ) {	\
	void* iter = nullptr; \
	int i = 0; \
	while ( il2cpp::field_info_t* field = dumper_klass->fields( &iter ) ) { \
		if ( strcmp( field->type( )->name( ), wanted_type->name( ) ) == 0 ) { \
			int attrs = field->type( )->attributes( ); \
			int vis = attrs & TYPE_ATTRIBUTE_VISIBILITY_MASK; \
			if ( ( wanted_vis && ( vis != wanted_vis ) ) || ( wanted_attrs && !( wanted_attrs & attrs ) ) ) \
				continue; \
\
			dumper::write_to_file( "\tconstexpr const static size_t %s_%d = 0x%x;\n", NAME, i, field->offset( ) ); i++; \
		} \
	} \
} ( )

#define DUMP_METHOD_BY_RETURN_TYPE_STR( NAME, filter, ret_type, param_ct ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_return_type_str( filter, dumper_klass, ret_type, param_ct )->get_fn_ptr<uint64_t>() ) )
#define DUMP_METHOD_BY_RETURN_TYPE_ATTRS( NAME, filter, ret_type, param_ct, wanted_vis, wanted_attrs ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_return_type_attrs( filter, dumper_klass, ret_type, wanted_attrs, wanted_vis, param_ct )->get_fn_ptr<uint64_t>() ) )
#define DUMP_METHOD_BY_RETURN_TYPE_SIZE( NAME, filter, ret_type, wanted_vis, wanted_attrs, idx ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_return_type_and_param_types_size( filter, idx, dumper_klass, ret_type, wanted_attrs, wanted_vis, nullptr, 0, nullptr, false )->get_fn_ptr<uint64_t>() ) )
#define DUMP_METHOD_BY_SIG_REL( NAME, base, sig, off ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( uint64_t( dumper::relative_32( FIND_PATTERN( base, 0x1000, sig ), off ) ) ) )
#define DUMP_METHOD_BY_INFO_PTR( NAME, ptr ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( ptr->get_fn_ptr<uint64_t>( ) ) )
#define DUMP_METHOD_BY_PARAM_CLASS( NAME, filter, param_class, param_ct, wanted_vis, wanted_flags ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_param_class( filter, dumper_klass, param_class, param_ct, wanted_vis, wanted_flags )->get_fn_ptr<uint64_t>( ) ) )

#define DUMP_METHOD_BY_RETURN_TYPE_METHOD_ATTRIBUTE(NAME, filter, ret_type, method_attr, param_ct, wanted_vis, wanted_attrs, want_or_ignore ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_return_type_attrs_method_attr( filter, dumper_klass, ret_type, method_attr, wanted_attrs, wanted_vis, param_ct, want_or_ignore )->get_fn_ptr<uint64_t>() ) )
#define DUMP_METHOD_BY_RETURN_TYPE_METHOD_ATTRIBUTE_SIZE(NAME, filter, ret_type, method_attr, wanted_vis, wanted_attrs, want_or_ignore, idx ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_return_type_and_param_types_size( filter, idx, dumper_klass, ret_type, wanted_vis, wanted_attrs, nullptr, 0, method_attr, want_or_ignore )->get_fn_ptr<uint64_t>() ) )

#define DUMP_METHOD_BY_NAME( NAME ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_name(dumper_klass, #NAME)->get_fn_ptr<uint64_t>()));

char* GetInnerClassFromEncClass( const char* name )
{
	static char buf[ 128 ] = { 0 };
	memset( buf, 0, sizeof( buf ) );

	const char* start = strchr( name, '<' );
	const char* end = strrchr( name, '>' );

	if ( !start || !end || start >= end )
		return nullptr;

	start++;
	strncpy( buf, start, end - start );

	return buf;
}

void dump_fn_to_file( const char* label, uint8_t* address ) {
	dumper::write_to_file( "\tconst static uint8_t %s[] = { ", label );

	size_t len = util::get_function_attributes( address, 16384 ).length;

	for ( uint32_t i = 0; i < len - 1; i++ ) {
		dumper::write_to_file( "0x%02X, ", address[ i ] );
	}

	dumper::write_to_file( "0x%02X };\n", address[ len - 1 ] );
}

#define DUMP_ENCRYPTED_MEMBER( NAME, FIELD ) \
	{ il2cpp::il2cpp_type_t* type = FIELD->type(); \
	il2cpp::il2cpp_class_t* klass = type->klass(); \
	il2cpp::method_info_t* to_string = il2cpp::get_method_by_name(klass, "ToString"); \
	dumper::write_to_file( "// type name: %s\n", type->name() ); \
	dumper::write_to_file("\tconstexpr const static size_t " #NAME " = 0x%x;\n", FIELD->offset() ); \
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_ToString = 0x%x;\n", DUMPER_RVA( to_string->get_fn_ptr<uint64_t>() ) ); }

#define DUMPER_OFFSET( NAME ) NAME##_Offset


FILE* dumper::outfile_handle = nullptr;
uint64_t dumper::game_base = 0;

void dumper::write_to_file( const char* format, ... )
{
	char buffer[ 1024 ] = { 0 };
	memset( buffer, 0, sizeof( buffer ) );

	va_list args;
	va_start( args, format );
	vsprintf_s( buffer, format, args );
	va_end( args );

	fwrite( buffer, strlen( buffer ), 1, outfile_handle );
}

void dumper::flush() {
	fflush( outfile_handle );
}

char* dumper::clean_klass_name( const char* klass_name )
{
	static char buffer[ 1024 ] = { 0 };
	memset( buffer, 0, sizeof( buffer ) );

	strcpy( buffer, klass_name );

	// Remove any possible junk.
	char junk_chars[ ] = { '/', '-', '.', '<', '>', '%' };
	for ( int i = 0; i < _countof( junk_chars ); i++ )
		while ( char* found = strchr( buffer, junk_chars[ i ] ) )
			*found = '_';

	return buffer;
}

void dumper::dump_call( uint64_t function, uint32_t limit, uint32_t depth ) {
	if ( limit == 0 )
		return;

	il2cpp::call_set_t* calls = il2cpp::get_inverse_calls( function );

	if ( calls ) {
		uint32_t idx = 0;

		for ( auto call : *calls ) {
			for ( uint32_t i = 0; i < depth; i++ )
				write_to_file( "\t" );

			write_to_file( "%02i %p\n", idx++, call - game_base );

			dump_call( call, limit - 1, depth + 1 );
		}
	}
}

void dumper::write_game_assembly() {
	PIMAGE_DOS_HEADER dos_header = ( PIMAGE_DOS_HEADER ) ( game_base );
	PIMAGE_NT_HEADERS nt_headers = ( PIMAGE_NT_HEADERS ) ( game_base + dos_header->e_lfanew );

	uint64_t gc_handles = 0;
	uint8_t* sig = FIND_PATTERN_IMAGE( game_base, "\x48\x8D\x05\xCC\xCC\xCC\xCC\x83\xE1\x07\xC1\xCC\x03" );

	if ( sig ) {
		gc_handles = DUMPER_RVA( ( uint64_t )dumper::relative_32( sig, 3 ) );
	}

	uint64_t convar_server_typeinfo = 0;
	sig = FIND_PATTERN_IMAGE( game_base, "\x0F\x2F\xC8\x0F\x86\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x44" );

	if ( sig ) {
		convar_server_typeinfo = DUMPER_RVA( ( uint64_t )dumper::relative_32( sig + 0x9, 3 ) );
	}

	dumper::write_to_file( "namespace GameAssembly {\n" );
	dumper::write_to_file( "\tconstexpr const static size_t timestamp = 0x%x;\n", nt_headers->FileHeader.TimeDateStamp );
	dumper::write_to_file( "\tconstexpr const static size_t gc_handles = 0x%x;\n", gc_handles );
	dumper::write_to_file( "\tconstexpr const static size_t ConVar_Server_TypeInfo = 0x%x;\n", convar_server_typeinfo );
	dumper::write_to_file( "}\n\n" );
}

void dumper::produce() {
	game_base = ( uint64_t ) ( GetModuleHandleA( "GameAssembly.dll" ) );
	outfile_handle = fopen( "C:\\dumps\\output.h", "w" );
	if ( !outfile_handle )
		return;

	dumper::write_to_file( "#include <cstdint>\n\n" );
	dumper::write_game_assembly();

	il2cpp::il2cpp_class_t* entity_ref_class = SEARCH_FOR_CLASS_BY_FIELD_COUNT( 2, 0, DUMPER_CLASS( "BaseEntity" ), DUMPER_CLASS( "NetworkableId" ) );
	il2cpp::il2cpp_class_t* hidden_value_base_class = nullptr;

	il2cpp::il2cpp_class_t* hitbox_collision_class = DUMPER_CLASS( "HitboxCollision" );
	il2cpp::il2cpp_class_t* hit_test_class = nullptr;

	if ( hitbox_collision_class ) {
		il2cpp::method_info_t* hitbox_collision_trace_test = il2cpp::get_method_by_return_type_attrs( NO_FILT, hitbox_collision_class, DUMPER_CLASS_NAMESPACE( "System", "Void" ), 0, 0, 2 );

		if ( hitbox_collision_trace_test ) {
			il2cpp::il2cpp_type_t* param_type = hitbox_collision_trace_test->get_param( 0 );

			if ( param_type ) {
				hit_test_class = param_type->klass();
			}
		}
	}

	il2cpp::il2cpp_class_t* weapon_rack_class = DUMPER_CLASS( "WeaponRack" );
	il2cpp::il2cpp_class_t* weapon_rack_slot_class = nullptr;
	il2cpp::il2cpp_class_t* game_manager_class = nullptr;

	if ( weapon_rack_class ) {
		il2cpp::method_info_t* weapon_rack_position_and_display_item = il2cpp::get_method_by_return_type_attrs( NO_FILT, weapon_rack_class, DUMPER_CLASS_NAMESPACE( "UnityEngine", "Collider" ), 0, 0, 7 );

		if ( weapon_rack_position_and_display_item ) {
			il2cpp::il2cpp_type_t* param_type = weapon_rack_position_and_display_item->get_param( 0 );

			if ( param_type ) {
				weapon_rack_slot_class = param_type->klass();

				if ( weapon_rack_slot_class ) {
					il2cpp::method_info_t* weapon_rack_slot_create_pegs = il2cpp::get_method_by_return_type_attrs( NO_FILT, weapon_rack_slot_class, DUMPER_CLASS_NAMESPACE( "System", "Void" ), 0, 0, 3 );

					if ( weapon_rack_slot_create_pegs ) {
						param_type = weapon_rack_slot_create_pegs->get_param( 2 );
						if ( param_type ) {
							game_manager_class = param_type->klass();
						}
					}
				}
			}
		}
	}	

	il2cpp::il2cpp_class_t* invisible_vending_machine_class = DUMPER_CLASS( "InvisibleVendingMachine" );
	il2cpp::il2cpp_class_t* base_networkable_load_info_class = nullptr;

	if ( invisible_vending_machine_class ) {
		il2cpp::method_info_t* invisible_vending_machine_load = il2cpp::get_method_by_return_type_attrs( NO_FILT, invisible_vending_machine_class, DUMPER_CLASS_NAMESPACE( "System", "Void" ), METHOD_ATTRIBUTE_VIRTUAL, METHOD_ATTRIBUTE_PUBLIC, 1 );

		if ( invisible_vending_machine_load ) {
			il2cpp::il2cpp_type_t* param_type = invisible_vending_machine_load->get_param( 0 );

			if ( param_type ) {
				base_networkable_load_info_class = param_type->klass();
			}
		}
	}

	int64_t on_rpc_message_method_offset = -1;

	il2cpp::il2cpp_type_t* param_types[] = {
		DUMPER_TYPE_NAMESPACE( "System", "UInt32" ),
		DUMPER_TYPE_NAMESPACE( "Network", "Message" )
	};

	il2cpp::method_info_t* base_entity_cl_rpc_message = il2cpp::get_method_by_return_type_and_param_types(
		FILT_N( DUMPER_METHOD( DUMPER_CLASS( "Client" ), "OnNetworkMessage" ), 2 ),
		DUMPER_CLASS( "BaseEntity" ),
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		METHOD_ATTRIBUTE_PUBLIC,
		DUMPER_ATTR_DONT_CARE,
		param_types,
		_countof( param_types )
	);

	if ( base_entity_cl_rpc_message ) {
		uint64_t address = base_entity_cl_rpc_message->get_fn_ptr<uint64_t>();
		uint64_t limit = 0x1000;
		uint64_t len = 0;

		while ( len < limit ) {
			uint8_t* inst = ( uint8_t* )address + len;

			hde64s hs;
			uint64_t instr_len = hde64_disasm( inst, &hs );

			if ( hs.flags & F_ERROR ) {
				break;
			}

			if ( hs.opcode == 0x8B ) {
				if ( hs.disp.disp32 > 0x138 ) {
					on_rpc_message_method_offset = hs.disp.disp32;
					break;
				}
			}

			len += instr_len;
		}
	}

	il2cpp::il2cpp_class_t* world_item_class = DUMPER_CLASS( "WorldItem" );
	uint64_t world_item_on_rpc_message = 0;

	if ( world_item_class && on_rpc_message_method_offset != -1 ) {
		world_item_on_rpc_message = *( uint64_t* )( ( uint64_t )world_item_class + on_rpc_message_method_offset );
	}

	il2cpp::il2cpp_class_t* scriptable_object_ref_class = DUMPER_CLASS( "ScriptableObjectRef" );
	uint64_t resource_ref_get = 0;

	if ( scriptable_object_ref_class ) {
		il2cpp::il2cpp_class_t* resource_ref_scriptable_object_class = scriptable_object_ref_class->parent();

		if ( resource_ref_scriptable_object_class ) {
			il2cpp::method_info_t* get_method = il2cpp::get_method_by_return_type_attrs( NO_FILT, resource_ref_scriptable_object_class, DUMPER_CLASS_NAMESPACE( "UnityEngine", "ScriptableObject" ), METHOD_ATTRIBUTE_VIRTUAL, METHOD_ATTRIBUTE_PUBLIC );

			if ( get_method ) {
				resource_ref_get = get_method->get_fn_ptr<uint64_t>();
			}
		}
	}

	uint64_t base_projectile_launch_projectile = DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "LaunchProjectile" );
	int64_t get_aimcone_method_offset = -1;

	if ( base_projectile_launch_projectile ) {
		uint64_t address = base_projectile_launch_projectile;
		uint64_t limit = 0x1000;
		uint64_t len = 0;

		uint32_t last_disps[ 2 ]{};
		uint32_t last_disps_ct = 0;

		while ( len < limit ) {
			uint8_t* inst = ( uint8_t* )address + len;

			hde64s hs;
			uint64_t instr_len = hde64_disasm( inst, &hs );

			if ( hs.flags & F_ERROR ) {
				break;
			}

			if ( hs.opcode == 0x8B ) {
				if ( last_disps_ct == 2 ) {
					last_disps[ 0 ] = last_disps[ 1 ];  
					last_disps[ 1 ] = hs.disp.disp32; 
				}

				else {
					last_disps[ last_disps_ct ] = hs.disp.disp32;
					last_disps_ct++;
				}
			}

			if ( hs.opcode == 0xFF && hs.modrm_reg == 0x2 ) {
				get_aimcone_method_offset = min( last_disps[ 0 ], last_disps[ 1 ] );
				break;
			}

			len += instr_len;
		}
	}

	il2cpp::il2cpp_class_t* base_networkable_entity_realm_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseNetworkable" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( prefabID ); // Dump a member by it's name.
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( net, "Network.Networkable", DUMPER_VIS_DONT_CARE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( parentEntity, entity_ref_class ); // Search for EntityRef class.
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( children, "System.Collections.Generic.List<BaseEntity>", FIELD_ATTRIBUTE_PUBLIC, FIELD_ATTRIBUTE_INIT_ONLY );

		il2cpp::field_info_t* client_entities = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "BaseNetworkable", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* client_entities ) { return client_entities != nullptr; } );
		DUMP_MEMBER_BY_X( clientEntities, client_entities->offset() );

		base_networkable_entity_realm_class = client_entities->type()->klass()->get_generic_argument_at( 0 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "BaseNetworkable_EntityRealm", base_networkable_entity_realm_class )
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( entityList, "ListDictionary" );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* base_networkable_entity_realm_find = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "DemoShotPlayback" ), "Update" ) ),
			DUMPER_TYPE( "BaseNetworkable" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE( "NetworkableId" )
		);
		DUMP_METHOD_BY_INFO_PTR( Find, base_networkable_entity_realm_find );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "Model" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( boneTransforms );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* position_lerp_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseEntity" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( flags );
		DUMP_MEMBER_BY_NAME( model );

		auto position_lerp = il2cpp::get_field_if_type_contains( dumper_klass, "%", FIELD_ATTRIBUTE_ASSEMBLY, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( positionLerp, position_lerp->offset( ) );
		position_lerp_class = position_lerp->type( )->klass( );

	DUMPER_SECTION( "Functions" );

		DUMP_METHOD_BY_INFO_PTR( FindBone, SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			NO_FILT, 
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Transform" ),
			DUMPER_VIS_DONT_CARE,
			METHOD_ATTRIBUTE_VIRTUAL,
			DUMPER_TYPE_NAMESPACE( "System", "String" ) ) );

		DUMP_METHOD_BY_RETURN_TYPE_SIZE( GetWorldVelocity, 
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "ZiplineAudio" ), "Update" ) ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE, 
			0 );
		DUMP_METHOD_BY_RETURN_TYPE_SIZE( GetParentVelocity, NO_FILT, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), METHOD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, 1 );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* interpolator_class = nullptr;
	if ( position_lerp_class ) {
		DUMPER_CLASS_BEGIN_FROM_PTR( "PositionLerp", position_lerp_class );

		DUMPER_SECTION( "Offsets" );

		auto interpolator = il2cpp::get_field_from_flags( position_lerp_class, FIELD_ATTRIBUTE_PRIVATE | FIELD_ATTRIBUTE_INIT_ONLY );
		DUMP_MEMBER_BY_X( interpolator, interpolator->offset( ) );

		interpolator_class = interpolator->type( )->klass( );

		DUMPER_CLASS_END;
	}

	if ( interpolator_class ) {
		DUMPER_CLASS_BEGIN_FROM_PTR( "Interpolator", interpolator_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( List, "System.Collections.Generic.List" );
		DUMPER_CLASS_END;
	}

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseCombatEntity" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( lifestate );
		DUMP_MEMBER_BY_NAME( markAttackerHostile );
		DUMP_MEMBER_BY_NEAR_OFFSET( _health, DUMPER_OFFSET( markAttackerHostile ) + 0x2 );
		DUMP_MEMBER_BY_NEAR_OFFSET( _maxHealth, DUMPER_OFFSET( markAttackerHostile ) + 0x6 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ModelState" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( flags );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemDefinition" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( itemid );
	DUMP_MEMBER_BY_NAME( category );
	DUMP_MEMBER_BY_NAME( shortname );
	DUMP_MEMBER_BY_NAME( displayName );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "RecoilProperties" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( recoilYawMin );
		DUMP_MEMBER_BY_NAME( recoilYawMax );
		DUMP_MEMBER_BY_NAME( recoilPitchMin );
		DUMP_MEMBER_BY_NAME( recoilPitchMax );
		DUMP_MEMBER_BY_NAME( overrideAimconeWithCurve );
		DUMP_MEMBER_BY_NAME( aimconeProbabilityCurve );
		DUMP_MEMBER_BY_NAME( newRecoilOverride );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseProjectile/Magazine" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( ammoType );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "AttackEntity" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( repeatDelay );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseProjectile" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( projectileVelocityScale );
		DUMP_MEMBER_BY_NAME( automatic );
		DUMP_MEMBER_BY_NAME( primaryMagazine );
		DUMP_MEMBER_BY_NAME( aimSway );
		DUMP_MEMBER_BY_NAME( aimSwaySpeed );
		DUMP_MEMBER_BY_NAME( recoil );
		DUMP_MEMBER_BY_NAME( aimconeCurve );
		DUMP_MEMBER_BY_NAME( aimCone );		
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( cachedModHash, "System.UInt32" );
		// timeSinceReloadFinished is used to get the 4 floats near it.
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( timeSinceReloadFinished, DUMPER_CLASS( "TimeSince" ) );
		DUMP_MEMBER_BY_NEAR_OFFSET( hipAimConeOffset, DUMPER_OFFSET( timeSinceReloadFinished ) - 0x8 ); // Ideally if they stay together you would just update this one and the rest should be fine.
		DUMP_MEMBER_BY_NEAR_OFFSET( hipAimConeScale, DUMPER_OFFSET( hipAimConeOffset ) - 0x4 );
		DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeOffset, DUMPER_OFFSET( hipAimConeScale ) - 0x4 );
		DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeScale, DUMPER_OFFSET( sightAimConeOffset ) - 0x4 );

		il2cpp::field_info_t* created_projectiles = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "Projectile", FIELD_ATTRIBUTE_PRIVATE, FIELD_ATTRIBUTE_INIT_ONLY, []( void* value ) { return value != nullptr; } );
		DUMP_MEMBER_BY_X( createdProjectiles, created_projectiles->offset() );
	DUMPER_SECTION( "Functions" );
	
	il2cpp::method_info_t* base_projectile_launch_projectile_clientside = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "LaunchProjectile" ) ),
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		DUMPER_VIS_DONT_CARE,
		DUMPER_ATTR_DONT_CARE,
		DUMPER_TYPE( "ItemDefinition" ),			
		DUMPER_TYPE_NAMESPACE( "System", "Int32" ), 
		DUMPER_TYPE_NAMESPACE( "System", "Single" ) 
	);

	DUMP_METHOD_BY_INFO_PTR( LaunchProjectileClientSide, base_projectile_launch_projectile_clientside );

	il2cpp::method_info_t* base_projectile_scale_repeat_delay = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
	    FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "BeginCycle" ) ),
		DUMPER_TYPE_NAMESPACE( "System", "Single" ),
		METHOD_ATTRIBUTE_PUBLIC,
		DUMPER_ATTR_DONT_CARE,
		DUMPER_TYPE_NAMESPACE( "System", "Single" )
	);

	DUMP_METHOD_BY_INFO_PTR( ScaleRepeatDelay, base_projectile_scale_repeat_delay );

	if ( get_aimcone_method_offset != -1 ) {
		uint64_t base_projectile_get_aimcone = *( uint64_t* )( ( uint64_t )dumper_klass + get_aimcone_method_offset );

		if ( base_projectile_get_aimcone ) {
			DUMP_MEMBER_BY_X( GetAimCone, DUMPER_RVA( base_projectile_get_aimcone ) );
		}
	}
	
	DUMP_METHOD_BY_RETURN_TYPE_ATTRS( UpdateAmmoDisplay, NO_FILT, DUMPER_CLASS_NAMESPACE( "System", "Void" ), 0, METHOD_ATTRIBUTE_FAMILY, METHOD_ATTRIBUTE_VIRTUAL );

	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseMelee" );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* base_melee_process_attack = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			NO_FILT, 
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_FAMILY,
			METHOD_ATTRIBUTE_VIRTUAL,
			hit_test_class->type()
		);
		DUMP_METHOD_BY_INFO_PTR( ProcessAttack, base_melee_process_attack );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( DoThrow,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseMelee" ), "OnViewmodelEvent" ) ),
		    DUMPER_CLASS_NAMESPACE( "System", "Void" ), 0, METHOD_ATTRIBUTE_ASSEM, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "FlintStrikeWeapon" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( strikeRecoil );
		DUMP_MEMBER_BY_NEAR_OFFSET( _didSparkThisFrame, DUMPER_OFFSET( strikeRecoil ) + 0x8 );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* player_loot_class = DUMPER_CLASS( "PlayerLoot" );
	size_t entity_source_offset = il2cpp::get_field_by_name( player_loot_class, "entitySource" )->offset( );
	il2cpp::il2cpp_class_t* item_class = il2cpp::get_field_by_offset( player_loot_class, entity_source_offset + 0x8 )->type( )->klass( );

	il2cpp::il2cpp_class_t* templated_item_container_class = il2cpp::get_field_by_name( player_loot_class, "containers" )->type()->klass();
	il2cpp::il2cpp_class_t* item_container_class = templated_item_container_class->get_generic_argument_at( 0 );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Item", item_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( uid, DUMPER_CLASS( "ItemId" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( info, DUMPER_CLASS( "ItemDefinition" ) );

		il2cpp::il2cpp_class_t* item_manager_class = il2cpp::search_for_class_by_method_return_type_name( "System.Collections.Generic.List<ItemBlueprint>", METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC );

		if ( item_manager_class && world_item_on_rpc_message ) {
			il2cpp::il2cpp_type_t* param_types[] = { 
				DUMPER_TYPE_NAMESPACE( "ProtoBuf", "Item" ), 
				item_class->type(), 
				DUMPER_TYPE_NAMESPACE( "System", "Boolean" ) 
			}; 

			il2cpp::method_info_t* item_manager_load_method = il2cpp::get_method_by_return_type_and_param_types(
				FILT( world_item_on_rpc_message ),
				item_manager_class,
				item_class->type(),
				METHOD_ATTRIBUTE_PUBLIC,
				METHOD_ATTRIBUTE_STATIC,
				param_types,
				_countof( param_types )
			);

			if ( !item_manager_load_method ) {
				item_manager_load_method = il2cpp::get_method_by_return_type_and_param_types(
					FILT_N( world_item_on_rpc_message, 2 ),
					item_manager_class,
					item_class->type(),
					METHOD_ATTRIBUTE_PUBLIC,
					METHOD_ATTRIBUTE_STATIC,
					param_types,
					_countof( param_types )
				);
			}
			
			if ( item_manager_load_method ) {
				uint32_t* ( *item_manager_load )( uint8_t*, void*, bool ) = ( decltype( item_manager_load ) )item_manager_load_method->get_fn_ptr<void*>();

				if ( item_manager_load ) {
					il2cpp::il2cpp_class_t* protobuf_item_condition_data_class = DUMPER_CLASS_NAMESPACE( "ProtoBuf", "Item/ConditionData" );
					il2cpp::il2cpp_class_t* protobuf_item_class = DUMPER_CLASS_NAMESPACE( "ProtoBuf", "Item" );

					if ( protobuf_item_condition_data_class && protobuf_item_class ) {
						uint8_t protobuf_item_condition_data[ 64 ]{};

						*( int* )( protobuf_item_condition_data + il2cpp::get_field_by_name( protobuf_item_condition_data_class, "condition" )->offset() ) = 1335;
						*( int* )( protobuf_item_condition_data + il2cpp::get_field_by_name( protobuf_item_condition_data_class, "maxCondition" )->offset() ) = 1336;

						uint8_t protobuf_item[ 256 ]{};

						*( int* )( protobuf_item + il2cpp::get_field_by_name( protobuf_item_class, "itemid" )->offset() ) = 1588298435;
						*( int* )( protobuf_item + il2cpp::get_field_by_name( protobuf_item_class, "slot" )->offset() ) = 1337;
						*( int* )( protobuf_item + il2cpp::get_field_by_name( protobuf_item_class, "amount" )->offset() ) = 1338;
						*( uint64_t* )( protobuf_item + il2cpp::get_field_by_name( protobuf_item_class, "worldEntity" )->offset() ) = 1339;
						*( uint64_t* )( protobuf_item + il2cpp::get_field_by_name( protobuf_item_class, "heldEntity" )->offset() ) = 1340;
						*( uint64_t* )( protobuf_item + il2cpp::get_field_by_name( protobuf_item_class, "conditionData" )->offset() ) = ( uint64_t )&protobuf_item_condition_data;

						uint32_t* created_item = item_manager_load( protobuf_item, nullptr, true );

						if ( created_item ) {
							for ( int i = 0; i < ( sizeof( protobuf_item ) / sizeof( uint32_t ) ); i++ ) {
								switch ( created_item[ i ] ) {
									case 1335: {
										DUMP_MEMBER_BY_X( condition, i * sizeof( uint32_t ) );
										break;
									}
									case 1336: {
										DUMP_MEMBER_BY_X( maxCondition, i * sizeof( uint32_t ) );
										break;
									}
									case 1337: {
										DUMP_MEMBER_BY_X( position, i * sizeof( uint32_t ) );
										break;
									}
									case 1338: {
										DUMP_MEMBER_BY_X( amount, i * sizeof( uint32_t ) );
										break;
									}
									case 1339: {
										DUMP_MEMBER_BY_X( worldEntity, i * sizeof( uint32_t ) - 8 );
										break;
									}
									case 1340: {
										DUMP_MEMBER_BY_X( heldEntity, i * sizeof( uint32_t ) - 8 );
										break;
									}
								}
							}
						}

						std::vector<il2cpp::field_info_t*> fields = il2cpp::get_fields_of_type( dumper_klass, item_container_class->type(), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

						for ( il2cpp::field_info_t* field : fields ) {
							uint64_t item_container = *( uint64_t* )( created_item + field->offset() );
							
							if ( !IsBadReadPtr( ( void* )item_container, 0x100 ) ) {
								DUMP_MEMBER_BY_X( contents, field->offset() );
								break;
							}
						}
					}
				}
			}
		}
	DUMPER_CLASS_END;

	char searchBuf[ 128 ] = { 0 };
	sprintf_s( searchBuf, "System.Collections.Generic.List<%s>", item_class->name( ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "ItemContainer", item_container_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( uid, "ItemContainerId" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( itemList, searchBuf );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_STR( FindItemsByItemID, FILT_N( DUMPER_METHOD( DUMPER_CLASS( "SellOrderEntry" ), "UpdateNotifications" ), 3 ), searchBuf, 1 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerInventory" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::method_info_t* player_inventory_initialize_method = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "PlayerInventory" ), "ClientInit" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_FAMILY,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE( "BasePlayer" )
		);

		sprintf_s( searchBuf, "%s.Flag", item_container_class->name() );
		il2cpp::field_info_t* flag = il2cpp::get_field_if_type_contains( item_container_class, searchBuf, FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

		if ( player_inventory_initialize_method && flag ) {
			void( *player_inventory_initialize )( uint64_t, void* ) = ( decltype( player_inventory_initialize ) )player_inventory_initialize_method->get_fn_ptr<void*>();

			if ( player_inventory_initialize ) {
				unity::game_object_t* game_object = unity::game_object_t::create( L"" );
				game_object->add_component( dumper_klass->type() );

				if ( uint64_t player_inventory = game_object->get_component( dumper_klass->type() ) ) {
					player_inventory_initialize( player_inventory, nullptr );

					std::vector<il2cpp::field_info_t*> fields = il2cpp::get_fields_of_type( dumper_klass, item_container_class->type(), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

					for ( il2cpp::field_info_t* field : fields ) {
						uint64_t item_container = *( uint64_t* )( player_inventory + field->offset() );

						if ( item_container ) {
							int item_container_flags = *( int* )( item_container + flag->offset() );

							if ( item_container_flags & rust::item_container::e_item_container_flag::belt ) {
								DUMP_MEMBER_BY_X( containerBelt, field->offset() );
							}

							else if ( item_container_flags & rust::item_container::e_item_container_flag::clothing ) {
								DUMP_MEMBER_BY_X( containerWear, field->offset() );
							}

							else {
								DUMP_MEMBER_BY_X( containerMain, field->offset() );
							}
						}
					}
				}
			}
		}		
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( Initialize, player_inventory_initialize_method );

		sprintf_s( searchBuf, "System.Collections.Generic.List<%s>", item_class->name() );
		DUMP_METHOD_BY_RETURN_TYPE_STR( FindItemsByItemID, FILT_N( DUMPER_METHOD( DUMPER_CLASS( "SellOrderEntry" ), "UpdateNotifications" ), 2 ), searchBuf, 1 );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* input_message_class = DUMPER_CLASS( "InputMessage" );
	il2cpp::il2cpp_class_t* input_state_class = SEARCH_FOR_CLASS_BY_FIELD_COUNT( 3, 0, input_message_class, input_message_class );

	DUMPER_CLASS_BEGIN_FROM_PTR( "InputState", input_state_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::method_info_t* input_state_flip_method = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			NO_FILT,
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			input_message_class->type()
		);

		if ( input_state_flip_method ) {
			void( *input_state_flip )( uint8_t*, uint8_t* ) = ( decltype( input_state_flip ) )input_state_flip_method->get_fn_ptr<void*>();

			if ( input_state_flip ) {
				std::vector<il2cpp::field_info_t*> fields = il2cpp::get_fields_of_type( dumper_klass, input_message_class->type(), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

				if ( fields.size() == 2 ) {
					uint8_t input_message_a[ 128 ]{};
					uint8_t input_message_b[ 128 ]{};

					uint8_t input_state[ 256 ]{};

					*( uint64_t* )( input_state + fields.at( 0 )->offset() ) = ( uint64_t )&input_message_a;
					*( uint64_t* )( input_state + fields.at( 1 )->offset() ) = ( uint64_t )&input_message_b;

					uint8_t input_message_c[ 128 ]{};

					*( uint32_t* )( input_message_c + il2cpp::get_field_by_name( input_message_class, "buttons" )->offset() ) = 1337;

					input_state_flip( input_state, input_message_c );

					for ( uint32_t i = 0; i < 2; i++ ) {
						uint64_t input_message = *( uint64_t* )( input_state + fields.at( i )->offset() );

						if ( input_message ) {
							uint32_t buttons = *( uint32_t* )( input_message + il2cpp::get_field_by_name( input_message_class, "buttons" )->offset() );

							if ( buttons == 1337 ) {
								DUMP_MEMBER_BY_X( current, fields.at( i )->offset() );

								uint32_t previous_index = i == 0 ? 1 : 0;
								DUMP_MEMBER_BY_X( previous, fields.at( previous_index )->offset() );
								break;
							}
						}
					}
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerInput" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( state, input_state_class );
		DUMP_MEMBER_BY_NEAR_OFFSET( bodyAngles, DUMPER_OFFSET( state ) + 0x1C );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerEyes" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( viewOffset, "PlayerEyes" ); // Contains PlayerEyes.EncryptedValue<Vector3>
		DUMP_MEMBER_BY_NEAR_OFFSET( bodyRotation, DUMPER_OFFSET( viewOffset ) + 0xC ); // <bodyRotation>k__BackingField
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_position,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "UIFogOverlay" ), "Update" ) ),
			DUMPER_CLASS_NAMESPACE( "UnityEngine", "Vector3" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_rotation, 
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "OnDrawGizmos" ) ), 
			DUMPER_CLASS_NAMESPACE( "UnityEngine", "Quaternion" ),
			0, 
			METHOD_ATTRIBUTE_PUBLIC, 
			DUMPER_ATTR_DONT_CARE 
		);
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerTick" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( inputState );
		DUMP_MEMBER_BY_NAME( position );
		DUMP_MEMBER_BY_NAME( modelState );
		DUMP_MEMBER_BY_NAME( activeItem );
		DUMP_MEMBER_BY_NAME( eyePos );
		DUMP_MEMBER_BY_NAME( parentID );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BasePlayer" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( playerModel, DUMPER_CLASS( "PlayerModel" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( input, DUMPER_CLASS( "PlayerInput" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( movement, DUMPER_CLASS( "BaseMovement" ) );
		DUMP_MEMBER_BY_NAME( currentTeam );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( clActiveItem, "ItemId" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( modelState, DUMPER_CLASS( "ModelState" ) );
		DUMP_MEMBER_BY_NAME( playerFlags );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( eyes, "PlayerEyes" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( playerRigidbody, "UnityEngine.Rigidbody" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( userID, "BasePlayer", "<System.UInt64>" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( inventory, "PlayerInventory" );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( _displayName, "System.String", TYPE_ATTRIBUTE_NESTED_FAMILY, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( _lookingAt, "UnityEngine.GameObject", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( lastSentTickTime, "BasePlayer", "System.Single" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( lastSentTick, DUMPER_CLASS( "PlayerTick" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( mounted, entity_ref_class ); 
		DUMP_MEMBER_BY_X( Belt, 0 );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_PARAM_CLASS( ClientInput, NO_FILT, input_state_class, 1, DUMPER_VIS_DONT_CARE, METHOD_ATTRIBUTE_VIRTUAL );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( IsOnGround, NO_FILT, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ), 0, METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_VIRTUAL );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GetHeldItemID, NO_FILT, DUMPER_CLASS( "ItemId" ), 0, METHOD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
		DUMP_METHOD_BY_RETURN_TYPE_STR( get_VisiblePlayerList, NO_FILT, "BufferList<BasePlayer>", 0 );

		il2cpp::method_info_t* base_player_get_speed = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "DoAttack" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),	
			DUMPER_TYPE_NAMESPACE( "System", "Single" ), 
			DUMPER_TYPE_NAMESPACE( "System", "Single" ) 
		);

		DUMP_METHOD_BY_INFO_PTR( GetSpeed, base_player_get_speed );

		il2cpp::method_info_t* base_player_send_projectile_update = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "AdjustVelocity" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "ProtoBuf", "PlayerProjectileUpdate" )
		);

		DUMP_METHOD_BY_INFO_PTR( SendProjectileUpdate, base_player_send_projectile_update );

		il2cpp::method_info_t* base_player_send_projectile_attack = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT_N( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "Update" ), 4 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "ProtoBuf", "PlayerProjectileAttack" )
		);

		DUMP_METHOD_BY_INFO_PTR( SendProjectileAttack, base_player_send_projectile_attack );

		il2cpp::il2cpp_class_t* model_state_class = DUMPER_CLASS( "ModelState" );
		il2cpp::method_info_t* model_state_get_on_ladder = il2cpp::get_method_by_name( model_state_class, "get_onLadder" );
		il2cpp::method_info_t* model_state_get_on_ground = il2cpp::get_method_by_name( model_state_class, "get_onground" );

		std::vector<il2cpp::method_info_t*> methods = il2cpp::get_methods_by_return_type_attrs( NO_FILT, dumper_klass, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ), METHOD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, 0 );

		for ( il2cpp::method_info_t* method : methods ) {
			util::function_attributes_t function_attributes = util::get_function_attributes( method->get_fn_ptr<void*>(), 0x1000 );

			if ( function_attributes.transfers_control_to( model_state_get_on_ladder->get_fn_ptr<void*>() ) ) {
				if ( !function_attributes.transfers_control_to( model_state_get_on_ground->get_fn_ptr<void*>() ) ) {
					DUMP_METHOD_BY_INFO_PTR( OnLadder, method );
					break;
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseMovement" );
	DUMPER_SECTION( "Functions" );	
		il2cpp::method_info_t* base_movement_set_target_movement = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			NO_FILT,
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_FAMILY,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
		);

		DUMP_METHOD_BY_INFO_PTR( set_TargetMovement, base_movement_set_target_movement );
	DUMPER_CLASS_END;


	DUMPER_CLASS_BEGIN_FROM_NAME( "BuildingPrivlidge" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( allowedConstructionItems );
	DUMP_MEMBER_BY_NEAR_OFFSET( cachedProtectedMinutes, DUMPER_OFFSET( allowedConstructionItems ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "WorldItem" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( allowPickup );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( item, item_class );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "HackableLockedCrate" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( timerText );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ProjectileWeaponMod" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( projectileVelocity );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerWalkMovement" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( capsule, "UnityEngine.CapsuleCollider" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS( ladder, DUMPER_CLASS( "TriggerLadder" ) );
	DUMP_MEMBER_BY_NEAR_OFFSET( groundTime, DUMPER_OFFSET( capsule ) + 0x20 );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( modify, "BaseEntity.MovementModify" );
	DUMPER_CLASS_END;


	DUMPER_CLASS_BEGIN_FROM_NAME( "ConsoleSystem/Command" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( AllowRunFromServer );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "LootableCorpse" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( playerSteamID, "System.UInt64" );

		il2cpp::field_info_t* player_name = il2cpp::get_field_if_name_contains( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "String" ), "%", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( _playerName, player_name->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "DroppedItemContainer" )
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( playerSteamID, "System.UInt64" );

		il2cpp::field_info_t* player_name = il2cpp::get_field_if_name_contains( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "String" ), "%", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( _playerName, player_name->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "MainCamera" )
	DUMPER_SECTION( "Offsets " );
		il2cpp::field_info_t* main_camera = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "UnityEngine.Camera", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( unity::component_t* camera ) { return camera != nullptr; } );
		DUMP_MEMBER_BY_X( mainCamera, main_camera->offset() );

		il2cpp::field_info_t* main_camera_transform = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "UnityEngine.Transform", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( unity::component_t* transform ) { return transform != nullptr; } );
		DUMP_MEMBER_BY_X( mainCameraTransform, main_camera_transform->offset() );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* main_camera_update = il2cpp::get_method_by_name( dumper_klass, "Update" );
		DUMP_METHOD_BY_INFO_PTR( Update, main_camera_update );
	DUMPER_CLASS_END

	/*
	uint32_t gameManager = parser.FindType(S("GameManager"));
	if (gameManager != -1) {
		parser.ParseTypeMethods(gameManager, [](const char* className, const char* methodName, uint32_t rva) {
			static int index;

			uint64_t hash = CoreSDK_Hash(methodName);

			if (hash == H("CreatePrefab")) {
				if (index == 2)
					Offsets::GameManager__CreatePrefab = rva;

				index++;
			}
		});
	}

	uint32_t behaviour = parser.FindType(S("Behaviour"));
	if (behaviour != -1) {
		parser.ParseTypeMethods(behaviour, [](const char* className, const char* methodName, uint32_t rva) {
			uint64_t hash = CoreSDK_Hash(methodName);

			if (hash == H("set_enabled"))
				Offsets::UnityEngine_Behaviour__set_enabled = rva;
		});
	}

	uint32_t serverAdminUGCEntry = parser.FindType(S("ServerAdminUGCEntry"));
	if (serverAdminUGCEntry != -1) {
		parser.ParseTypeMethods(serverAdminUGCEntry, [](const char* className, const char* methodName, uint32_t rva) {
			uint64_t hash = CoreSDK_Hash(methodName);

			if (hash == H("ReceivedDataFromServer"))
				Offsets::ServerAdminUGCEntry__ReceivedDataFromServer = rva;
		});
	}
	*/

	il2cpp::il2cpp_class_t* pet_command_desc_class = DUMPER_CLASS( "PetCommandList/PetCommandDesc" );
	il2cpp::il2cpp_class_t* local_player_class = il2cpp::search_for_class_by_field_types( pet_command_desc_class->type(), 0, FIELD_ATTRIBUTE_STATIC );

	DUMPER_CLASS_BEGIN_FROM_PTR( "LocalPlayer", local_player_class );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* local_player_item_command = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "FrequencyConfig" ), "Confirm" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE( "ItemId" ),
			DUMPER_TYPE_NAMESPACE( "System", "String" )
		);

		DUMP_METHOD_BY_INFO_PTR( ItemCommand, local_player_item_command );

		il2cpp::method_info_t* local_player_move_item = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "ItemIcon" ), "OnDroppedValue" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE( "ItemId" ),
			DUMPER_TYPE( "ItemContainerId" ),
			DUMPER_TYPE_NAMESPACE( "System", "Int32" ),
			DUMPER_TYPE_NAMESPACE( "System", "Int32" )
		);

		DUMP_METHOD_BY_INFO_PTR( MoveItem, local_player_move_item );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_Entity, 
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "LookAtWeaponRack" ), "Update" ) ),
			DUMPER_CLASS( "BasePlayer" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GetInventory,
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "WearableFeedbackItemIcon" ), "Update" ), 2 ),
		    DUMPER_CLASS( "PlayerInventory" ),
			0,
			METHOD_ATTRIBUTE_ASSEM,
			METHOD_ATTRIBUTE_STATIC );
	DUMPER_CLASS_END;

	auto water_level_search_types = std::vector<il2cpp::method_search_flags_t>{
		il2cpp::method_search_flags_t {  DUMPER_TYPE_NAMESPACE( "System", "Single" ), METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC, 6, { "UnityEngine.Vector3", "UnityEngine.Vector3", "System.Single", "System.Boolean", "System.Boolean", "BaseEntity" }},
		il2cpp::method_search_flags_t {  DUMPER_TYPE_NAMESPACE( "System", "Single" ), METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC, 4, { "UnityEngine.Bounds", "System.Boolean", "System.Boolean", "BaseEntity" } },
		il2cpp::method_search_flags_t {  DUMPER_TYPE_NAMESPACE( "System", "Boolean" ), METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC, 4, { "UnityEngine.Vector3", "System.Boolean", "System.Boolean", "BaseEntity" } },
	};

	auto water_level_class = il2cpp::search_for_class_containing_method_prototypes( water_level_search_types );

	DUMPER_CLASS_BEGIN_FROM_PTR( "WaterLevel", water_level_class );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* water_level_test = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "GroundVehicleAudio" ), "ClientTick" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE( "BaseEntity" )
		);

		DUMP_METHOD_BY_INFO_PTR( Test, water_level_test );

		il2cpp::method_info_t* water_level_get_water_level = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT_N( DUMPER_METHOD( DUMPER_CLASS( "AmbienceWaveSounds" ), "Update" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			METHOD_ATTRIBUTE_PRIVATE,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
		);

		DUMP_METHOD_BY_INFO_PTR( GetWaterLevel, water_level_get_water_level );

		il2cpp::method_info_t* water_level_factor = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			NO_FILT,
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE( "BaseEntity" )
		);

		DUMP_METHOD_BY_INFO_PTR( Factor, water_level_factor );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* convar_graphics_klass = il2cpp::search_for_class_by_method_return_type_name( "UnityEngine.FullScreenMode", METHOD_ATTRIBUTE_PRIVATE, METHOD_ATTRIBUTE_STATIC );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Convar_Graphics", convar_graphics_klass );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* fov = il2cpp::get_static_field_if_value_is<uint32_t>( dumper_klass, convar_graphics_klass->name(), FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( uint32_t value ) { return value != 0; } );
		DUMP_MEMBER_BY_X( _fov, fov->offset() );
	DUMPER_SECTION( "Functions" );
		il2cpp::il2cpp_class_t* console_system_index_client = DUMPER_CLASS( "ConsoleSystem/Index/Client" );
		il2cpp::il2cpp_class_t* console_system_command = DUMPER_CLASS( "ConsoleSystem/Command" );

		uint64_t( *console_system_index_client_find )( system_c::string_t* ) = ( decltype( console_system_index_client_find ) )DUMPER_METHOD( console_system_index_client, "Find" );

		if ( console_system_index_client_find ) {
			uint64_t fov_command = console_system_index_client_find( system_c::string_t::create_string( L"graphics.fov" ) );

			if ( fov_command ) {
				uint64_t getter_function = *( uint64_t* )( fov_command + il2cpp::get_field_by_name( console_system_command, "GetOveride" )->offset() );
				uint64_t setter_action = *( uint64_t* )( fov_command + il2cpp::get_field_by_name( console_system_command, "SetOveride" )->offset() );

				if ( getter_function ) {
					uint64_t getter = *( uint64_t* )( getter_function + 0x10 );
					DUMP_MEMBER_BY_X( _fov_getter, DUMPER_RVA( getter ) );
				}

				if ( setter_action ) {
					uint64_t setter = *( uint64_t* )( setter_action + 0x18 );
					DUMP_MEMBER_BY_X( _fov_setter, DUMPER_RVA( setter ) );
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseFishingRod" );
	DUMPER_SECTION( "Offsets" )
		DUMP_MEMBER_BY_TYPE_METHOD_ATTRIBUTE( CurrentState,
			DUMPER_CLASS( "BaseFishingRod/CatchState" ),
			DUMPER_CLASS_NAMESPACE( "System.Runtime.CompilerServices", "CompilerGeneratedAttribute" ),
			DUMPER_VIS_DONT_CARE,
			DUMPER_ATTR_DONT_CARE,
			il2cpp::attr_search_want );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( currentBobber, "FishingBobber" );
	DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( clientStrainAmountNormalised, "System.Single", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( UpdateLineRenderer, NO_FILT, DUMPER_CLASS_NAMESPACE( "System", "Void" ), 0, METHOD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );

		il2cpp::method_info_t* base_fishing_rod_evaluate_fishing_position = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES_STR(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseFishingRod" ), "OnInput" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			"UnityEngine.Vector3&",
			"BasePlayer",
			"BaseFishingRod.FailReason&",
			"WaterBody&"
		);

		DUMP_METHOD_BY_INFO_PTR( EvaluateFishingPosition, base_fishing_rod_evaluate_fishing_position );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "GameManifest" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GUIDToObject, FILT( resource_ref_get ), DUMPER_CLASS_NAMESPACE( "UnityEngine", "Object" ), 1, METHOD_ATTRIBUTE_ASSEM, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "GameManager", game_manager_class );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* game_manager_create_prefab = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "HBHFSensor" ), "Menu_Configure" ) ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "GameObject" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "String" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);
		DUMP_METHOD_BY_INFO_PTR( CreatePrefab, game_manager_create_prefab ); 
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemModProjectile" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( projectileObject );
		DUMP_MEMBER_BY_NAME( ammoType );
		DUMP_MEMBER_BY_NAME( projectileSpread );
		DUMP_MEMBER_BY_NAME( projectileVelocity );
		DUMP_MEMBER_BY_NAME( projectileVelocitySpread );
		DUMP_MEMBER_BY_NAME( useCurve );
		DUMP_MEMBER_BY_NAME( spreadScalar );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Projectile" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( initialVelocity );
		DUMP_MEMBER_BY_NAME( drag );
		DUMP_MEMBER_BY_NAME( gravityModifier );
		DUMP_MEMBER_BY_NAME( thickness );
		DUMP_MEMBER_BY_NAME( initialDistance );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( owner, DUMPER_CLASS( "BasePlayer" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( sourceProjectilePrefab, DUMPER_CLASS( "Projectile" ) );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* projectile_do_hit = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES_SIZE(
		    NO_FILT,
			0,
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			hit_test_class->type(),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
		);

		DUMP_METHOD_BY_INFO_PTR( DoHit, projectile_do_hit );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "ProjectileShoot", "ProtoBuf" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( projectiles );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "ProjectileShoot/Projectile", "ProtoBuf" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( projectileID );
		DUMP_MEMBER_BY_NAME( startPos );
		DUMP_MEMBER_BY_NAME( startVel );
		DUMP_MEMBER_BY_NAME( seed );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "GrowableEntity" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_METHOD_ATTRIBUTE_SIZE( CanPick, 
			NO_FILT,
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ), 
			DUMPER_CLASS_NAMESPACE( "System.Runtime.CompilerServices", "CompilerGeneratedAttribute" ), 
			METHOD_ATTRIBUTE_PUBLIC, 
			DUMPER_ATTR_DONT_CARE, 
			il2cpp::attr_search_ignore, 
			4 
		);
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "CraftingQueue" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( icons, "List" );

		il2cpp::field_info_t* is_crafting = il2cpp::get_static_field_if_value_is<bool>( dumper_klass, "System.Boolean", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( bool is_crafting ) { return is_crafting == true; } );
		DUMP_MEMBER_BY_X( isCrafting, is_crafting->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "CraftingQueueIcon" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( endTime, "System.Single" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( item, DUMPER_CLASS( "ItemDefinition" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Planner" );
	DUMPER_SECTION( "Offsets" );

	DUMP_MEMBER_BY_FIELD_TYPE_CLASS( currentConstruction, DUMPER_CLASS( "Construction" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "HeldEntity" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( viewModel, DUMPER_CLASS( "ViewModel" ) ); // <viewModel>k__BackingField
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ViewModel" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( instance, DUMPER_CLASS( "BaseViewModel" ) ); 
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "MedicalTool" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( resetTime, "System.Single", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "WaterSystem" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_Ocean, NO_FILT, DUMPER_CLASS( "WaterBody" ), 0, METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "WaterBody" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( meshFilter, "UnityEngine.MeshFilter" ); // <MeshFilter>k__BackingField
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "TerrainMeta" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* terrain_collision = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "TerrainCollision", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::component_t* terrain_collision ) { return terrain_collision != nullptr; } );
		DUMP_MEMBER_BY_X( collision, terrain_collision->offset() );

		il2cpp::field_info_t* terrain_height_map = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "TerrainHeightMap", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::component_t* terrain_collision ) { return terrain_collision != nullptr; } );
		DUMP_MEMBER_BY_X( heightMap, terrain_height_map->offset() );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "TerrainHeightMap" );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* terrain_height_map_get_height = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "TerrainAnchor" ), "Apply" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
		);

		DUMP_METHOD_BY_INFO_PTR( GetHeight, terrain_height_map_get_height );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "TerrainCollision" );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* terrain_collision_get_ignore = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT_N( DUMPER_METHOD( DUMPER_CLASS( "SocketMod_TerrainCheck" ), "OnDrawGizmos" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" )
		);
		DUMP_METHOD_BY_INFO_PTR( GetIgnore, terrain_collision_get_ignore );
	DUMPER_CLASS_END

	il2cpp::il2cpp_class_t* world_class = il2cpp::search_for_class_by_field_types( DUMPER_TYPE( "WorldSerialization" ), 0, FIELD_ATTRIBUTE_STATIC );

	DUMPER_CLASS_BEGIN_FROM_PTR( "World", world_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* size = il2cpp::get_static_field_if_value_is<uint32_t>( dumper_klass, "System.UInt32", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( uint32_t size ) { return size == 4000; } );
		DUMP_MEMBER_BY_X( _size, size->offset() );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemIcon" );
	DUMPER_SECTION("Offsets")
		il2cpp::field_info_t* container_loot_start_times = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "Dictionary", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( void* dictionary ) { return dictionary != nullptr; } );
		DUMP_MEMBER_BY_X( containerLootStartTimes, container_loot_start_times->offset() );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* item_icon_set_timed_loot_action = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "ItemIcon" ), "TryToMove" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			item_container_class->type(),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE_NAMESPACE( "System", "Action" )
		);
		DUMP_METHOD_BY_INFO_PTR( SetTimedLootAction, item_icon_set_timed_loot_action );
	DUMPER_CLASS_END

	/*
	// TODO: Fix this

	il2cpp::il2cpp_class_t* con_button_class = il2cpp::search_for_class_by_interfaces_contain( "IConsoleButton" );
	il2cpp::il2cpp_class_t* buttons_class = il2cpp::search_for_class_by_field_types( con_button_class->type( ), 56, FIELD_ATTRIBUTE_STATIC );
	DUMPER_CLASS_BEGIN_FROM_PTR( "Buttons", buttons_class );
	DUMPER_CLASS_END;
	*/

	DUMPER_CLASS_BEGIN_FROM_NAME("EffectData")
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( pooledstringid );
	DUMPER_CLASS_END

	il2cpp::il2cpp_class_t* effect_class = il2cpp::search_for_class_by_parent_class_type( DUMPER_TYPE( "EffectData" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Effect", effect_class );
	DUMPER_SECTION( "Offsets" );
	DUMP_ALL_MEMBERS_OF_TYPE( "UnkVector", DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), DUMPER_VIS_DONT_CARE, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BuildingBlock" );
	DUMPER_SECTION( "Functions" );

	il2cpp::method_info_t* building_block_upgrade_to_grade = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		NO_FILT,
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		METHOD_ATTRIBUTE_PRIVATE,
		DUMPER_ATTR_DONT_CARE,
		DUMPER_TYPE( "BuildingGrade/Enum" ),
		DUMPER_TYPE_NAMESPACE( "System", "UInt64" ),
		DUMPER_TYPE( "BasePlayer" )
	);

	DUMP_METHOD_BY_INFO_PTR( UpgradeToGrade, building_block_upgrade_to_grade );

	il2cpp::method_info_t* building_block_has_upgrade_privilege = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES_SIZE(
		NO_FILT,
		1,
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
		METHOD_ATTRIBUTE_PRIVATE,
		DUMPER_ATTR_DONT_CARE,
		DUMPER_TYPE( "BuildingGrade/Enum" ),
		DUMPER_TYPE_NAMESPACE( "System", "UInt64" ),
		DUMPER_TYPE( "BasePlayer" )
	);

	DUMP_METHOD_BY_INFO_PTR( HasUpgradePrivilege, building_block_has_upgrade_privilege );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerWalkMovement" );
	il2cpp::method_info_t* client_input = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		NO_FILT,
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		METHOD_ATTRIBUTE_PUBLIC,
		DUMPER_ATTR_DONT_CARE,
		input_state_class->type( ),
		DUMPER_TYPE( "ModelState" )
	);

	DUMP_METHOD_BY_INFO_PTR( ClientInput, client_input );
	DUMPER_CLASS_END;

	auto game_object_ex_search_types = std::vector<il2cpp::method_search_flags_t> {
		il2cpp::method_search_flags_t {  DUMPER_TYPE( "BaseEntity" ), METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC, 2, { "UnityEngine.GameObject", "System.Boolean" } },
		il2cpp::method_search_flags_t {  DUMPER_TYPE( "BaseEntity" ), METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC, 2, { "UnityEngine.Collider", "System.Boolean" } },
		il2cpp::method_search_flags_t {  DUMPER_TYPE( "BaseEntity" ), METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC, 2, { "UnityEngine.Transform", "System.Boolean" } }
	};

	auto game_object_ex_class = il2cpp::search_for_class_containing_method_prototypes( game_object_ex_search_types );

	DUMPER_CLASS_BEGIN_FROM_PTR( "GameObjectEx", game_object_ex_class );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* game_object_ex_to_base_entity = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "EntityHUDRender" ), "OnWillRenderObject" ) ),
			DUMPER_TYPE( "BaseEntity" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Transform" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);
		DUMP_METHOD_BY_INFO_PTR( ToBaseEntity, game_object_ex_to_base_entity );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIDeathScreen" )
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* ui_death_screen_set_visible = il2cpp::get_method_by_name( dumper_klass, "SetVisible" );
		DUMP_METHOD_BY_INFO_PTR( SetVisible, ui_death_screen_set_visible );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseScreenShake" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* instances_list = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "BaseScreenShake", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instances_list ) { return instances_list != nullptr; } );
		DUMP_MEMBER_BY_X( list, instances_list->offset() );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "FlashbangOverlay" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* instance = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "FlashbangOverlay", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instance ) { return instance != nullptr; } );
		DUMP_MEMBER_BY_X( Instance, instance->offset() );

		il2cpp::field_info_t* flash_length = il2cpp::get_field_if_name_contains( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), "%", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( flashLength, flash_length->offset() );
	DUMPER_CLASS_END

	const char* field_types[] = {
		"System.Collections.Generic.Dictionary<System.UInt32,System.String>",
		"System.Collections.Generic.Dictionary<System.String,System.UInt32>",
	};

	il2cpp::il2cpp_class_t* string_pool = il2cpp::search_for_class_containing_field_types_str( field_types, 2 );
	
	if ( string_pool ) {
		DUMPER_CLASS_BEGIN_FROM_PTR( "StringPool", string_pool );
		DUMPER_SECTION( "Functions" );
			il2cpp::method_info_t* string_pool_get = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
				FILT( DUMPER_METHOD( DUMPER_CLASS( "Poolable" ), "PostProcess" ) ),
				DUMPER_TYPE_NAMESPACE( "System", "UInt32" ),
				METHOD_ATTRIBUTE_PUBLIC,
				METHOD_ATTRIBUTE_STATIC,
				DUMPER_TYPE_NAMESPACE( "System", "String" )
			);

			DUMP_METHOD_BY_INFO_PTR( Get, string_pool_get );
		DUMPER_CLASS_END
	}

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "BaseNetwork", "Network" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( StartWrite );
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "NetWrite", "Network" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( Send );
		DUMP_METHOD_BY_NAME( UInt8 );
		DUMP_METHOD_BY_NAME( UInt16 );
		DUMP_METHOD_BY_NAME( UInt32 );
		DUMP_METHOD_BY_NAME( UInt64 );
		DUMP_METHOD_BY_NAME( Int8 );
		DUMP_METHOD_BY_NAME( Int16 );
		DUMP_METHOD_BY_NAME( Int32 );
		DUMP_METHOD_BY_NAME( Int64 );
		DUMP_METHOD_BY_NAME( Bool );
		DUMP_METHOD_BY_NAME( Float );
		DUMP_METHOD_BY_NAME( Double );
		DUMP_METHOD_BY_NAME( Bytes );
		DUMP_METHOD_BY_NAME( String );
		DUMP_METHOD_BY_NAME( Vector3 );
		DUMP_METHOD_BY_NAME( PacketID );
	DUMPER_CLASS_END

	if ( item_container_class ) {
		DUMPER_CLASS_BEGIN_FROM_NAME( "LootPanel" );
		DUMPER_SECTION( "Functions" );
			DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_Container_00, 
				FILT( DUMPER_METHOD( DUMPER_CLASS( "ResearchTablePanel" ), "Update" ) ), 
				item_container_class,
				0, 
				METHOD_ATTRIBUTE_PUBLIC, 
				DUMPER_ATTR_DONT_CARE
			);
		DUMPER_CLASS_END
	}

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIInventory" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( Close,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "UIInventory" ), "Update" ) ),
			DUMPER_CLASS_NAMESPACE( "System", "Void" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC
		);
	DUMPER_CLASS_END

	fclose( outfile_handle );
}