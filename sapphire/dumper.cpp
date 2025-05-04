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

#define SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE( klass, filter, ret_type, wanted_vis, wanted_flags, param_ct ) \
	il2cpp::get_method_in_method_by_return_type( klass, filter, ret_type, wanted_vis, wanted_flags, param_ct ); \

#define SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES( klass, filter, ret_type, wanted_vis, wanted_flags, ... ) \
[=]( ) -> il2cpp::method_info_t* \
{ \
	il2cpp::il2cpp_type_t* param_types[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_method_in_method_by_return_type_and_param_types( klass, filter, ret_type, wanted_vis, wanted_flags, param_types, _countof( param_types ) ); \
} ( )

#define SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE_PARAM_TYPES( filter, ret_type, wanted_vis, wanted_flags, ... ) \
[=]( ) -> il2cpp::virtual_method_t \
{ \
	il2cpp::il2cpp_type_t* param_types[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_virtual_method_by_return_type_and_param_types( filter, dumper_klass, ret_type, wanted_vis, wanted_flags, param_types, _countof( param_types ) ); \
} ( )

#define SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE( filter, ret_type, wanted_vis, wanted_flags ) \
	il2cpp::get_virtual_method_by_return_type_and_param_types( filter, dumper_klass, ret_type, wanted_vis, wanted_flags, nullptr, 0 ); 

#define DUMPER_VIS_DONT_CARE 0 
#define DUMPER_ATTR_DONT_CARE 0

#define DUMPER_RVA( X ) X == 0 ? 0 : X - dumper::game_base
#define DUMPER_RVA_UNITY( X ) X - dumper::unity_base
#define DUMPER_CLASS( name ) il2cpp::get_class_by_name( name )
#define DUMPER_CLASS_NAMESPACE( namespaze, name ) il2cpp::get_class_by_name( name, namespaze )
#define DUMPER_METHOD( klass, name ) il2cpp::get_method_by_name( klass, name )->get_fn_ptr< uint64_t >()

#define DUMPER_TYPE( name ) DUMPER_CLASS( name )->type()
#define DUMPER_TYPE_NAMESPACE( namespaze, name ) DUMPER_CLASS_NAMESPACE( namespaze, name )->type()

#define DUMPER_PTR_CLASS_NAME(dump_name, klass_ptr) \
    dumper::write_to_file( \
        "#define " dump_name "_ClassName \"%s\"\n", \
        clean_inner_klass_name(klass_ptr) \
    ); \
    dumper::write_to_file( \
        "#define " dump_name "_ClassNameShort \"%s\"\n\n", \
        klass_ptr->name() \
    );

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

#define DUMP_CLASS_NAME( dump_name, klass_ptr ) \
	dumper::write_to_file( "// obf name: %s::%s\n", klass_ptr->namespaze(), klass_ptr->name() ); \
	DUMPER_PTR_CLASS_NAME( dump_name, klass_ptr );	\
	
#define DUMP_MEMBER_BY_X( NAME, X ) uint64_t NAME##_Offset = X; dumper::write_to_file("\tconstexpr const static size_t " #NAME " = 0x%x;\n", static_cast<uint32_t>( NAME##_Offset ) )

#define DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( NAME, field_type, wanted_vis, wanted_attrs )			DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_type_name_attrs( dumper_klass, field_type, wanted_vis, wanted_attrs )->offset( dumper_klass ) ) 
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( NAME, search )									DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_if_type_contains( dumper_klass, search )->offset( dumper_klass ) )
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( NAME, search, wanted_vis, wanted_attrs )	DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_if_type_contains( dumper_klass, search, wanted_vis, wanted_attrs )->offset( dumper_klass ) )
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS( NAME, wanted_klass )										DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_from_field_type_class( dumper_klass, wanted_klass )->offset( dumper_klass ) )
#define DUMP_MEMBER_BY_NAME( NAME )																	DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_name( dumper_klass, #NAME )->offset( dumper_klass ) )
#define DUMP_MEMBER_BY_NEAR_OFFSET( NAME, off )														DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_offset( dumper_klass, off )->offset( dumper_klass ) )
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( NAME, ... )								DUMP_MEMBER_BY_X( NAME, SEARCH_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( __VA_ARGS__ )->offset( dumper_klass ) )
#define DUMP_MEMBER_BY_TYPE_METHOD_ATTRIBUTE(NAME, type_klass, method_attr, wanted_vis, wanted_attrs, want_or_ignore ) DUMP_MEMBER_BY_X( NAME, il2cpp::get_field_by_type_attrs_method_attr( dumper_klass, type_klass, method_attr, wanted_attrs, wanted_vis, want_or_ignore )->offset( dumper_klass ) )

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
			dumper::write_to_file( "\tconstexpr const static size_t %s_%d = 0x%x;\n", NAME, i, field->offset( dumper_klass ) ); i++; \
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
#define DUMP_METHOD_BY_NAME_STR( NAME, method_name ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_name( dumper_klass, method_name )->get_fn_ptr<uint64_t>()));
#define DUMP_METHOD_BY_NAME_STR_ARG_CT( NAME, method_name, arg_count ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_name( dumper_klass, method_name, arg_count )->get_fn_ptr<uint64_t>()));
#define DUMP_METHOD_BY_PARAM_NAME( NAME, method_name, param_name, param_idx ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_param_name( NO_FILT, dumper_klass, method_name, param_name, param_idx )->get_fn_ptr<uint64_t>() ) ) 
#define DUMP_METHOD_BY_ICALL( NAME, ICALL ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA_UNITY( il2cpp::resolve_icall( ICALL ) ) );

#define DUMP_VIRTUAL_METHOD( NAME, virtual_method ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( virtual_method.method->get_fn_ptr<uint64_t>( ) ) ); dumper::write_to_file( "\tconstexpr const static size_t %s_vtableoff = 0x%x;\n", #NAME, virtual_method.offset )

const char* format_string( const char* fmt, ... ) {
	static char buffer[ 256 ];
	va_list val;
	va_start( val, fmt );
	vsnprintf( buffer, sizeof( buffer ), fmt, val );
	va_end( val );
	return buffer;
}

il2cpp::il2cpp_class_t* get_outer_class( il2cpp::il2cpp_class_t * klass ) {
	if ( !klass )
		return nullptr;

	if ( !klass->type() )
		return nullptr;

	const char* begin = klass->type()->name();
	const char* end = strchr( begin, '.' );
	size_t length = end - begin;

	char buffer[ 128 ]{};
	memcpy( buffer, begin, length );
	buffer[ length ] = '\0';

	return il2cpp::get_class_by_name( buffer, klass->namespaze() );
}

il2cpp::il2cpp_class_t* get_inner_static_class( il2cpp::il2cpp_class_t* klass ) {
	if ( !klass )
		return nullptr;

	if ( !klass->type() )
		return nullptr;

	void* iter = nullptr;
	while ( il2cpp::il2cpp_class_t* _klass = klass->nested_types( &iter ) ) {
		if ( _klass->method_count() != 1 )
			continue;

		if ( !il2cpp::get_method_by_name( _klass, ".cctor" ) )
			continue;

		iter = nullptr;
		while ( il2cpp::field_info_t* field = _klass->fields( &iter ) )
			if ( !( field->flags() & FIELD_ATTRIBUTE_STATIC ) )
				continue;

		return _klass;
	}

	return nullptr;
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
FILE* dumper::outfile_log_handle = nullptr;
uint64_t dumper::game_base = 0;
uint64_t dumper::unity_base = 0;
il2cpp::il2cpp_class_t** dumper::type_info_definition_table = 0;

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

void dumper::write_to_log( const char* format, ... ) {
	char buffer[ 1024 ] = { 0 };
	memset( buffer, 0, sizeof( buffer ) );

	va_list args;
	va_start( args, format );
	vsprintf_s( buffer, format, args );
	va_end( args );

	fwrite( buffer, strlen( buffer ), 1, outfile_log_handle );
	fflush( outfile_log_handle );
}

const char* format_method( il2cpp::method_info_t* method ) {
	static char buffer[ 1024 ]{};
	memset( buffer, 0, sizeof( buffer ) );

	uint32_t param_ct = method->param_count();

	snprintf( buffer, sizeof( buffer ), "%s.%s(",
		method->klass()->type()->name(), method->name() );

	for ( uint32_t j = 0; j < param_ct; ++j ) {
		snprintf( buffer + strlen( buffer ), sizeof( buffer ) - strlen( buffer ),
			"%s %s%s",
			method->get_param( j )->klass()->type()->name(),
			method->get_param_name( j ),
			( j == param_ct - 1 ) ? "" : ", " );
	}

	snprintf( buffer + strlen( buffer ), sizeof( buffer ) - strlen( buffer ), ");\n" );
	return buffer;
}

#define VALUE_CLASS 0
#define VALUE_METHOD 1

#define CHECK_RESOLVED_VALUE( value_type, dump_name, value ) \
	if ( !value ) { \
        dumper::write_to_log("[ERROR] Failed to resolve %s\n", dump_name ); \
        fclose( outfile_handle ); \
        fclose( outfile_log_handle ); \
        return; \
    } else { \
		if ( value_type == VALUE_CLASS ) { \
			dumper::write_to_log( "[SUCCESS] %s: %s\n", dump_name, ( ( il2cpp::il2cpp_class_t* ) value)->type()->name() ); \
		} \
		else if ( value_type == VALUE_METHOD ) { \
			const char* method_fmt = format_method( ( il2cpp::method_info_t* )value ); \
			dumper::write_to_log( "[SUCCESS] %s: %s", dump_name, method_fmt ); \
		} \
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

char* dumper::clean_inner_klass_name( il2cpp::il2cpp_class_t* klass )
{
	static char buffer[ 1024 ] = { 0 };
	memset( buffer, 0, sizeof( buffer ) );

	il2cpp::il2cpp_type_t* type = klass->type();
	if ( !type )
		return nullptr;

	strcpy( buffer, type->name() );

	if ( !strchr( buffer, '.' ) )
		return buffer;

	*strchr( buffer, '.' ) = '/';

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

			if ( il2cpp::method_info_t* method = il2cpp::method_info_t::from_addr( call ) ) {
				const char* method_name_fmt = format_method( method );
				write_to_file( "%s", method_name_fmt );
			}

			else
				write_to_file( "%02i %p\n", idx++, call - game_base );

			dump_call( call, limit - 1, depth + 1 );
		}
	}
}

#define GA_EXPORT_RVA( x ) DUMPER_RVA( ( uint64_t )GetProcAddress( ( HMODULE )game_base, x )

void dumper::write_game_assembly() {
	PIMAGE_DOS_HEADER dos_header = ( PIMAGE_DOS_HEADER )( game_base );
	PIMAGE_NT_HEADERS nt_headers = ( PIMAGE_NT_HEADERS )( game_base + dos_header->e_lfanew );

	uint64_t gc_handles = 0;
	uint8_t* sig = FIND_PATTERN_IMAGE( game_base, "\x48\x8D\x05\xCC\xCC\xCC\xCC\x83\xE1\x07\xC1\xCC\x03" );

	if ( sig ) {
		gc_handles = DUMPER_RVA( ( uint64_t )dumper::relative_32( sig, 3 ) );
	}

	dumper::write_to_file( "namespace GameAssembly {\n" );
	dumper::write_to_file( "\tconstexpr const static size_t timestamp = 0x%x;\n", nt_headers->FileHeader.TimeDateStamp );
	dumper::write_to_file( "\tconstexpr const static size_t gc_handles = 0x%x;\n", gc_handles );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_resolve_icall = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_resolve_icall" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_array_new = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_array_new" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_assembly_get_image = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_assembly_get_image" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_class_from_name = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_class_from_name" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_class_get_method_from_name = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_class_get_method_from_name" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_class_get_type = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_class_get_type" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_domain_get = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_domain_get" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_domain_get_assemblies = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_domain_get_assemblies" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_gchandle_get_target = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_gchandle_get_target" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_gchandle_new = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_gchandle_new" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_gchandle_free = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_gchandle_free" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_method_get_name = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_method_get_name" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_object_new = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_object_new" ) ) );
	dumper::write_to_file( "\tconstexpr const static size_t il2cpp_type_get_object = 0x%x;\n", GA_EXPORT_RVA( "il2cpp_type_get_object" ) ) );
	dumper::write_to_file( "}\n\n" );
}

void( *o_base_entity_server_rpc_object )( rust::base_entity*, system_c::string_t*, il2cpp::il2cpp_object_t*, il2cpp::method_info_t* ) = nullptr;

il2cpp::il2cpp_class_t* projectile_attack_networkable_id = nullptr;
il2cpp::il2cpp_class_t* protobuf_player_projectile_update_class = nullptr;
il2cpp::il2cpp_class_t* protobuf_player_projectile_attack_class = nullptr;

bool resolved_projectile_shoot = false;
bool resolved_projectile_update = false;
bool resolved_projectile_attack = false;

#define SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( instance, il2cpp_type, type ) { \
	std::vector<il2cpp::field_info_t*> fields = il2cpp::get_fields_of_type( dumper_klass, il2cpp_type, DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE ); \
	for ( auto field : fields ) \
		*( type* )( instance + field->offset() ) = ( type )field->offset(); }

#define SET_ALL_FIELDS_OF_TYPE_TO_VALUE( instance, il2cpp_type, type, value ) { \
	std::vector<il2cpp::field_info_t*> fields = il2cpp::get_fields_of_type( dumper_klass, il2cpp_type, DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE ); \
	for ( auto field : fields ) \
		*( type* )( instance + field->offset() ) = ( type )value; }

void dumper::hk_base_entity_server_rpc_object( rust::base_entity* base_entity, system_c::string_t* func_name, il2cpp::il2cpp_object_t* arg, il2cpp::method_info_t* method ) {
	if ( !resolved_projectile_shoot && !wcscmp( func_name->str, L"CLProject" ) ) {
		il2cpp::il2cpp_class_t* proto_buf_projectile_shoot_class = arg->get_class();
		il2cpp::field_info_t* proto_buf_projectile_shoot_projectiles = nullptr;

		il2cpp::il2cpp_class_t* proto_buf_projectile_shoot_projectile_class = nullptr;

		if ( proto_buf_projectile_shoot_class ) {
			proto_buf_projectile_shoot_projectiles = il2cpp::get_field_if_type_contains( proto_buf_projectile_shoot_class, "List<%", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

			if ( proto_buf_projectile_shoot_projectiles ) {
				proto_buf_projectile_shoot_projectile_class = proto_buf_projectile_shoot_projectiles->type()->klass()->get_generic_argument_at( 0 );
			}
		}

		DUMPER_CLASS_BEGIN_FROM_PTR( "ProtoBuf_ProjectileShoot", proto_buf_projectile_shoot_class );
		DUMPER_SECTION( "Offsets" );
			DUMP_MEMBER_BY_X( projectiles, proto_buf_projectile_shoot_projectiles->offset() );
		DUMPER_CLASS_END;

		DUMPER_CLASS_BEGIN_FROM_PTR( "ProtoBuf_ProjectileShoot_Projectile", proto_buf_projectile_shoot_projectile_class );
		DUMPER_SECTION( "Offsets" );
			auto projectiles = *( system_c::list<uint64_t>** )( arg + proto_buf_projectile_shoot_projectiles->offset() );
			
			if ( projectiles ) {
				for ( int i = 0; i < projectiles->_size; i++ ) {
					uint64_t projectile = projectiles->at( i );
					if ( !projectile )
						continue;
				
					SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( projectile, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), int );
					SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( projectile, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), float );
				}
			}
		DUMPER_CLASS_END;

		resolved_projectile_shoot = true;
	}

	else if ( !resolved_projectile_update && !wcscmp( func_name->str, L"OnProjectileUpdate" ) ) {
		protobuf_player_projectile_update_class = arg->get_class();

		DUMPER_CLASS_BEGIN_FROM_PTR( "ProtoBuf_PlayerProjectileUpdate", protobuf_player_projectile_update_class );
		DUMPER_SECTION( "Offsets" );
			uint64_t player_projectile_update = ( uint64_t )arg;
			
			if ( player_projectile_update ) {
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( player_projectile_update, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), int );
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( player_projectile_update, DUMPER_TYPE_NAMESPACE( "System", "Single" ), float );
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( player_projectile_update, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), float );
			}

			DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( ShouldPool, "System.Boolean", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE ); 
		DUMPER_SECTION( "Functions" );
			DUMP_METHOD_BY_NAME( Dispose );
		DUMPER_CLASS_END;

		resolved_projectile_update = true;
	}

	else if ( !resolved_projectile_attack && !wcscmp( func_name->str, L"OnProjectileAttack" ) ) {
		protobuf_player_projectile_attack_class = arg->get_class(); // ProtoBuf.PlayerProjectileAttack
		il2cpp::field_info_t* protobuf_player_projectile_attack_player_attack = nullptr; // ProtoBuf.PlayerProjectileAttack.playerAttack

		il2cpp::il2cpp_class_t* protobuf_player_attack_class = nullptr; // ProtoBuf.PlayerAttack
		il2cpp::field_info_t* protobuf_player_attack_attack = nullptr; // ProtoBuf.PlayerAttack.attack

		il2cpp::il2cpp_class_t* protobuf_attack_class = nullptr; // ProtoBuf.Attack

		if ( protobuf_player_projectile_attack_class ) {
			protobuf_player_projectile_attack_player_attack = il2cpp::get_field_if_type_contains( protobuf_player_projectile_attack_class, "%", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

			if ( protobuf_player_projectile_attack_player_attack ) {
				protobuf_player_attack_class = protobuf_player_projectile_attack_player_attack->type()->klass();

				if ( protobuf_player_attack_class ) {
					protobuf_player_attack_attack = il2cpp::get_field_if_type_contains( protobuf_player_attack_class, "%", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

					if ( protobuf_player_attack_attack ) {
						protobuf_attack_class = protobuf_player_attack_attack->type()->klass();
					}
				}
			}
		}

		uint64_t protobuf_player_projectile_attack = ( uint64_t )arg;
		uint64_t protobuf_player_attack = 0;
		uint64_t protobuf_attack = 0;

		if ( is_valid_ptr( protobuf_player_projectile_attack ) ) {
			protobuf_player_attack = *( uint64_t* )( protobuf_player_projectile_attack + protobuf_player_projectile_attack_player_attack->offset() );

			if ( is_valid_ptr( protobuf_player_attack ) ) {
				protobuf_attack = *( uint64_t* )( protobuf_player_attack + protobuf_player_attack_attack->offset() );
			}
		}

		DUMPER_CLASS_BEGIN_FROM_PTR( "ProtoBuf_PlayerProjectileAttack", protobuf_player_projectile_attack_class );
		DUMPER_SECTION( "Offsets" );
			DUMP_MEMBER_BY_FIELD_TYPE_CLASS( playerAttack, protobuf_player_attack_class );

			if ( protobuf_player_projectile_attack ) {
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( protobuf_player_projectile_attack, DUMPER_TYPE_NAMESPACE( "System", "Single" ), float );
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( protobuf_player_projectile_attack, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), float );
			}
		DUMPER_CLASS_END;

		DUMPER_CLASS_BEGIN_FROM_PTR( "ProtoBuf_PlayerAttack", protobuf_player_attack_class );
		DUMPER_SECTION( "Offsets" );
			DUMP_MEMBER_BY_FIELD_TYPE_CLASS( attack, protobuf_attack_class );

			if ( protobuf_player_attack ) {
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( protobuf_player_attack, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), int );
			}
		DUMPER_CLASS_END;

		DUMPER_CLASS_BEGIN_FROM_PTR( "ProtoBuf_Attack", protobuf_attack_class );
		DUMPER_SECTION( "Offsets" );
			if ( protobuf_attack ) {
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( protobuf_attack, DUMPER_TYPE_NAMESPACE( "System", "UInt32" ), uint32_t );
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( protobuf_attack, projectile_attack_networkable_id->type(), uint64_t );
				SET_ALL_FIELDS_OF_TYPE_TO_OFFSET( protobuf_attack, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), float );
			}
		DUMPER_CLASS_END;

		resolved_projectile_attack = true;
	}

	return o_base_entity_server_rpc_object( base_entity, func_name, arg, method );
}

std::vector<il2cpp::il2cpp_class_t*> get_field_classes_in_class( il2cpp::il2cpp_class_t* klass ) {
	std::vector<il2cpp::il2cpp_class_t*> klasses;
	if ( !klass )
		return klasses;

	if ( klass->field_count() == 0 )
		return klasses;

	void* iter = nullptr;
	while ( il2cpp::field_info_t* field = klass->fields( &iter ) ) {
		if ( ( field->flags() & FIELD_ATTRIBUTE_STATIC ) )
			continue;

		klasses.push_back( field->type()->klass() );
	}

	return klasses;
}

void get_class_by_field_in_class_impl( il2cpp::il2cpp_class_t* klass, const char* target, il2cpp::il2cpp_class_t** result, uint32_t limit ) {
	if ( limit == 0 )
		return;

	if ( *result )
		return;

	std::vector<il2cpp::il2cpp_class_t*> klasses = get_field_classes_in_class( klass );
	if ( klasses.size() == 0 )
		return;

	for ( il2cpp::il2cpp_class_t* _klass : klasses ) {
		if ( !_klass )
			continue;

		if ( strstr( _klass->type()->name(), target ) ) {
			*result = klass;
			return;
		}

		get_class_by_field_in_class_impl( _klass, target, result, limit - 1 );
	}
}

il2cpp::il2cpp_class_t* get_class_by_field_in_class( il2cpp::il2cpp_class_t* klass, const char* target, uint32_t limit = 1 ) {
	il2cpp::il2cpp_class_t* result = nullptr;
	get_class_by_field_in_class_impl( klass, target, &result, limit );
	return result;
}

int get_button_offset( const wchar_t* button_command ) {
	rust::console_system::command* command = rust::console_system::client::find( system_c::string_t::create_string( button_command ) );
	if ( !command )
		return 0;

	uint64_t setter = command->set();
	if ( !setter )
		return 0;

	if ( setter ) {
		uint64_t address = setter;
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
				if ( hs.disp.disp32 != 0xB8 ) {
					if ( hs.disp.disp32 > 0 && hs.disp.disp32 < 0x1000 ) {
						return hs.disp.disp32;
					}
				}
			}

			len += instr_len;
		}
	}

	return 0;
}

#define WAIT( key ) while ( !( GetAsyncKeyState( key ) & 0x1 ) ) Sleep( 100 );

#define FLOAT_IS_EQUAL( a, b, c ) abs( a - b ) < c
#define VECTOR_IS_EQUAL( a, b, c ) abs( a.distance( b ) ) < c 

#define SERVER_IP L"103.214.71.31" 
#define SERVER_PORT 28015
#define WORLD_SIZE 4000
#define CAMSPEED 1.0525f
#define CAMLERP 1.0252f

void dumper::produce_unity() {
	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Object", "UnityEngine" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( m_CachedPtr );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( Destroy, "UnityEngine.Object::Destroy(UnityEngine.Object,System.Single)" );
		DUMP_METHOD_BY_ICALL( DestroyImmediate, "UnityEngine.Object::DestroyImmediate(UnityEngine.Object,System.Boolean)" );
		DUMP_METHOD_BY_ICALL( DontDestroyOnLoad, "UnityEngine.Object::DontDestroyOnLoad(UnityEngine.Object)" );
		DUMP_METHOD_BY_ICALL( FindObjectFromInstanceID, "UnityEngine.Object::FindObjectFromInstanceID(System.Int32)" );
		DUMP_METHOD_BY_ICALL( GetName, "UnityEngine.Object::GetName(UnityEngine.Object)" );
		DUMP_METHOD_BY_ICALL( get_hideFlags, "UnityEngine.Object::get_hideFlags()" );
		DUMP_METHOD_BY_ICALL( set_hideFlags, "UnityEngine.Object::set_hideFlags(UnityEngine.HideFlags)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "GameObject", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( SetActive, "UnityEngine.GameObject::SetActive(System.Boolean)" );
		DUMP_METHOD_BY_ICALL( Internal_AddComponentWithType, "UnityEngine.GameObject::Internal_AddComponentWithType(System.Type)" );
		DUMP_METHOD_BY_ICALL( GetComponent, "UnityEngine.GameObject::GetComponent(System.Type)" );
		DUMP_METHOD_BY_ICALL( GetComponentCount, "UnityEngine.GameObject::GetComponentCount()" );
		DUMP_METHOD_BY_ICALL( GetComponentInChildren, "UnityEngine.GameObject::GetComponentInChildren(System.Type,System.Boolean)" );
		DUMP_METHOD_BY_ICALL( GetComponentInParent, "UnityEngine.GameObject::GetComponentInParent(System.Type,System.Boolean)" );
		DUMP_METHOD_BY_ICALL( GetComponentsInternal, "UnityEngine.GameObject::GetComponentsInternal(System.Type,System.Boolean,System.Boolean,System.Boolean,System.Boolean,System.Object)" );
		DUMP_METHOD_BY_ICALL( Internal_CreateGameObject, "UnityEngine.GameObject::Internal_CreateGameObject(UnityEngine.GameObject,System.String)" );
		DUMP_METHOD_BY_ICALL( get_layer, "UnityEngine.GameObject::get_layer()" );
		DUMP_METHOD_BY_ICALL( get_tag, "UnityEngine.GameObject::get_tag()" );
		DUMP_METHOD_BY_ICALL( get_transform, "UnityEngine.GameObject::get_transform()" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Component", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( get_gameObject, "UnityEngine.Component::get_gameObject()" );
		DUMP_METHOD_BY_ICALL( get_transform, "UnityEngine.Component::get_transform()" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Behaviour", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( set_enabled, "UnityEngine.Behaviour::set_enabled(System.Boolean)" );
	DUMPER_CLASS_END;
	
	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Transform", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( GetChild, "UnityEngine.Transform::GetChild(System.Int32)" );
		DUMP_METHOD_BY_ICALL( GetParent, "UnityEngine.Transform::GetParent()" );
		DUMP_METHOD_BY_ICALL( GetRoot, "UnityEngine.Transform::GetRoot()" );
		DUMP_METHOD_BY_ICALL( InverseTransformDirection_Injected, "UnityEngine.Transform::InverseTransformDirection_Injected(UnityEngine.Vector3&,UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( InverseTransformPoint_Injected, "UnityEngine.Transform::InverseTransformPoint_Injected(UnityEngine.Vector3&,UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( InverseTransformVector_Injected, "UnityEngine.Transform::InverseTransformVector_Injected(UnityEngine.Vector3&,UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( SetLocalPositionAndRotation_Injected, "UnityEngine.Transform::SetLocalPositionAndRotation_Injected(UnityEngine.Vector3&,UnityEngine.Quaternion&)" );
		DUMP_METHOD_BY_ICALL( SetPositionAndRotation_Injected, "UnityEngine.Transform::SetPositionAndRotation_Injected(UnityEngine.Vector3&,UnityEngine.Quaternion&)" );
		DUMP_METHOD_BY_ICALL( TransformDirection_Injected, "UnityEngine.Transform::TransformDirection_Injected(UnityEngine.Vector3&,UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( TransformPoint_Injected, "UnityEngine.Transform::TransformPoint_Injected(UnityEngine.Vector3&,UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( TransformVector_Injected, "UnityEngine.Transform::TransformVector_Injected(UnityEngine.Vector3&,UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( get_childCount, "UnityEngine.Transform::get_childCount()" );
		DUMP_METHOD_BY_ICALL( get_eulerAngles, "UnityEngine.Transform::get_childCount()" );
		DUMP_METHOD_BY_ICALL( get_localPosition_Injected, "UnityEngine.Transform::get_localPosition_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( get_localRotation_Injected, "UnityEngine.Transform::get_localRotation_Injected(UnityEngine.Quaternion&)" );
		DUMP_METHOD_BY_ICALL( get_localScale_Injected, "UnityEngine.Transform::get_localScale_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( get_lossyScale_Injected, "UnityEngine.Transform::get_lossyScale_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( get_position_Injected, "UnityEngine.Transform::get_position_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( get_rotation_Injected, "UnityEngine.Transform::get_rotation_Injected(UnityEngine.Quaternion&)" );
		DUMP_METHOD_BY_ICALL( set_localPosition_Injected, "UnityEngine.Transform::set_localPosition_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( set_localRotation_Injected, "UnityEngine.Transform::set_localRotation_Injected(UnityEngine.Quaternion&)" );
		DUMP_METHOD_BY_ICALL( set_localScale_Injected, "UnityEngine.Transform::set_localScale_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( set_position_Injected, "UnityEngine.Transform::set_position_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( set_rotation_Injected, "UnityEngine.Transform::set_rotation_Injected(UnityEngine.Quaternion&)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Camera", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( WorldToScreenPoint_Injected, "UnityEngine.Camera::WorldToScreenPoint_Injected(UnityEngine.Vector3&,UnityEngine.Camera/MonoOrStereoscopicEye,UnityEngine.Vector3&)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Time", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( get_deltaTime, "UnityEngine.Time::get_deltaTime()" );
		DUMP_METHOD_BY_ICALL( get_fixedDeltaTime, "UnityEngine.Time::get_fixedDeltaTime()" );
		DUMP_METHOD_BY_ICALL( get_fixedTime, "UnityEngine.Time::get_fixedTime()" );
		DUMP_METHOD_BY_ICALL( get_frameCount, "UnityEngine.Time::get_frameCount()" );
		DUMP_METHOD_BY_ICALL( get_realtimeSinceStartup, "UnityEngine.Time::get_realtimeSinceStartup()" );
		DUMP_METHOD_BY_ICALL( get_smoothDeltaTime, "UnityEngine.Time::get_smoothDeltaTime()" );
		DUMP_METHOD_BY_ICALL( get_time, "UnityEngine.Time::get_time()" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Material", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( CreateWithMaterial, "UnityEngine.Material::CreateWithMaterial(UnityEngine.Material,UnityEngine.Material)" );
		DUMP_METHOD_BY_ICALL( CreateWithShader, "UnityEngine.Material::CreateWithShader(UnityEngine.Material,UnityEngine.Shader)" );	
		DUMP_METHOD_BY_ICALL( SetBufferImpl, "UnityEngine.Material::SetBufferImpl(System.Int32,UnityEngine.ComputeBuffer)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "MaterialPropertyBlock", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME_STR( ctor, ".ctor" );
		DUMP_METHOD_BY_ICALL( SetFloatImpl, "UnityEngine.MaterialPropertyBlock::SetFloatImpl(System.Int32,System.Single)" );
		DUMP_METHOD_BY_ICALL( SetTextureImpl, "UnityEngine.MaterialPropertyBlock::SetTextureImpl(System.Int32,UnityEngine.Texture)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Shader", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( PropertyToID, "UnityEngine.Shader::PropertyToID(System.String)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Mesh", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( Internal_Create, "UnityEngine.Mesh::Internal_Create(UnityEngine.Mesh)" );
		DUMP_METHOD_BY_ICALL( MarkDynamicImpl, "UnityEngine.Mesh::MarkDynamicImpl()" );
		DUMP_METHOD_BY_ICALL( ClearImpl, "UnityEngine.Mesh::ClearImpl(System.Boolean)" );
		DUMP_METHOD_BY_ICALL( set_subMeshCount, "UnityEngine.Mesh::set_subMeshCount(System.Int32)" );
		DUMP_METHOD_BY_ICALL( SetVertexBufferParamsFromPtr, "UnityEngine.Mesh::SetVertexBufferParamsFromPtr" );
		DUMP_METHOD_BY_ICALL( SetIndexBufferParams, "UnityEngine.Mesh::SetIndexBufferParams" );
		DUMP_METHOD_BY_ICALL( InternalSetVertexBufferData, "UnityEngine.Mesh::InternalSetVertexBufferData" );
		DUMP_METHOD_BY_ICALL( InternalSetIndexBufferData, "UnityEngine.Mesh::InternalSetIndexBufferData" );
		DUMP_METHOD_BY_ICALL( SetAllSubMeshesAtOnceFromNativeArray, "UnityEngine.Mesh::SetAllSubMeshesAtOnceFromNativeArray" );
		DUMP_METHOD_BY_ICALL( UploadMeshDataImpl, "UnityEngine.Mesh::UploadMeshDataImpl(System.Boolean)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Texture", "UnityEngine" );
		DUMP_METHOD_BY_ICALL( set_filterMode, "UnityEngine.Texture::set_filterMode(UnityEngine.FilterMode)" );
		DUMP_METHOD_BY_ICALL( GetNativeTexturePtr, "UnityEngine.Texture::GetNativeTexturePtr()" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Texture2D", "UnityEngine" );
		DUMP_METHOD_BY_NAME_STR_ARG_CT( ctor, ".ctor", 9 );
		DUMP_METHOD_BY_ICALL( Internal_CreateImpl, "UnityEngine.Texture2D::Internal_CreateImpl(UnityEngine.Texture2D,System.Int32,System.Int32,System.Int32,UnityEngine.Experimental.Rendering.GraphicsFormat,UnityEngine.TextureColorSpace,UnityEngine.Experimental.Rendering.TextureCreationFlags,System.IntPtr,System.String)" );
		DUMP_METHOD_BY_ICALL( GetRawImageDataSize, "UnityEngine.Texture2D::GetRawImageDataSize()" );
		DUMP_METHOD_BY_ICALL( GetWritableImageData, "UnityEngine.Texture2D::GetWritableImageData(System.Int32)" );
		DUMP_METHOD_BY_ICALL( ApplyImpl, "UnityEngine.Texture2D::ApplyImpl(System.Boolean,System.Boolean)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "CommandBuffer", "UnityEngine.Rendering" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME_STR( ctor, ".ctor" );
		DUMP_METHOD_BY_ICALL( Clear, "UnityEngine.Rendering.CommandBuffer::Clear()" );
		DUMP_METHOD_BY_ICALL( ClearRenderTarget_Injected, "UnityEngine.Rendering.CommandBuffer::ClearRenderTarget_Injected(UnityEngine.Rendering.RTClearFlags,UnityEngine.Color&,System.Single,System.UInt32)" );
		DUMP_METHOD_BY_ICALL( SetViewport_Injected, "UnityEngine.Rendering.CommandBuffer::SetViewport_Injected(UnityEngine.Rect&)" );
		DUMP_METHOD_BY_ICALL( SetViewProjectionMatrices_Injected, "UnityEngine.Rendering.CommandBuffer::SetViewProjectionMatrices_Injected(UnityEngine.Matrix4x4&,UnityEngine.Matrix4x4&)" );
		DUMP_METHOD_BY_ICALL( EnableScissorRect_Injected, "UnityEngine.Rendering.CommandBuffer::EnableScissorRect_Injected(UnityEngine.Rect&)" );
		DUMP_METHOD_BY_ICALL( DisableScissorRect, "UnityEngine.Rendering.CommandBuffer::DisableScissorRect()" );
		DUMP_METHOD_BY_ICALL( Internal_DrawProceduralIndexedIndirect_Injected, "UnityEngine.Rendering.CommandBuffer::Internal_DrawProceduralIndexedIndirect_Injected()" );
		DUMP_METHOD_BY_ICALL( Internal_DrawMesh_Injected, "UnityEngine.Rendering.CommandBuffer::Internal_DrawMesh_Injected(UnityEngine.Mesh,UnityEngine.Matrix4x4&,UnityEngine.Material,System.Int32,System.Int32,UnityEngine.MaterialPropertyBlock)" )
	DUMPER_CLASS_END; 

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "ComputeBuffer", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME_STR_ARG_CT( ctor, ".ctor", 5 );
		DUMP_METHOD_BY_ICALL( get_count, "UnityEngine.ComputeBuffer::get_count()" );
		DUMP_METHOD_BY_NAME( Release );
		DUMP_METHOD_BY_ICALL( InternalSetNativeData, "UnityEngine.ComputeBuffer::InternalSetNativeData(System.IntPtr,System.Int32,System.Int32,System.Int32,System.Int32)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "GraphicsBuffer", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME_STR_ARG_CT( ctor, ".ctor", 3 );
		DUMP_METHOD_BY_ICALL( get_count, "UnityEngine.GraphicsBuffer::get_count()" );
		DUMP_METHOD_BY_NAME_STR_ARG_CT( Dispose, "Dispose", 0 );
		DUMP_METHOD_BY_ICALL( InternalSetNativeData, "UnityEngine.GraphicsBuffer::InternalSetNativeData(System.IntPtr,System.Int32,System.Int32,System.Int32,System.Int32)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Event", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( get_current );
		DUMP_METHOD_BY_ICALL( get_type, "UnityEngine.Event::get_type()" );
		DUMP_METHOD_BY_ICALL( PopEvent, "UnityEngine.Event::PopEvent(UnityEngine.Event)" );
		DUMP_METHOD_BY_ICALL( Internal_Use, "UnityEngine.Event::Internal_Use()" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Graphics", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( ExecuteCommandBuffer, "UnityEngine.Graphics::ExecuteCommandBuffer(UnityEngine.Rendering.CommandBuffer)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Matrix4x4", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( Ortho_Injected, "UnityEngine.Matrix4x4::Ortho_Injected(System.Single,System.Single,System.Single,System.Single,System.Single,System.Single,UnityEngine.Matrix4x4&)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "AssetBundle", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( LoadFromMemory_Internal, "UnityEngine.AssetBundle::LoadFromMemory_Internal" );
		DUMP_METHOD_BY_ICALL( LoadFromFile_Internal, "UnityEngine.AssetBundle::LoadFromFile_Internal(System.String,System.UInt32,System.UInt64)" );
		DUMP_METHOD_BY_ICALL( LoadAsset_Internal, "UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)" );
		DUMP_METHOD_BY_ICALL( Unload, "UnityEngine.AssetBundle::Unload(System.Boolean)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Screen", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( get_width, "UnityEngine.Screen::get_width()" );
		DUMP_METHOD_BY_ICALL( get_height, "UnityEngine.Screen::get_height()" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Input", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( get_mousePosition_Injected, "UnityEngine.Input::get_mousePosition_Injected(UnityEngine.Vector3&)" );
		DUMP_METHOD_BY_ICALL( get_mouseScrollDelta_Injected, "UnityEngine.Input::get_mouseScrollDelta_Injected(UnityEngine.Vector2&)" );
		DUMP_METHOD_BY_ICALL( GetMouseButtonDown, "UnityEngine.Input::GetMouseButtonDown(System.Int32)" );
		DUMP_METHOD_BY_ICALL( GetMouseButtonUp, "UnityEngine.Input::GetMouseButtonUp(System.Int32)" );
		DUMP_METHOD_BY_ICALL( GetMouseButton, "UnityEngine.Input::GetMouseButton(System.Int32)" );
		DUMP_METHOD_BY_ICALL( GetKeyDownInt, "UnityEngine.Input::GetKeyDownInt(UnityEngine.KeyCode)" );
		DUMP_METHOD_BY_ICALL( GetKeyUpInt, "UnityEngine.Input::GetKeyUpInt(UnityEngine.KeyCode)" );
		DUMP_METHOD_BY_ICALL( GetKeyInt, "UnityEngine.Input::GetKeyInt(UnityEngine.KeyCode)" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Application", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_ICALL( Quit, "UnityEngine.Application::Quit(System.Int32)" );
	DUMPER_CLASS_END;
}

bool resolve_type_info_definition_table() {
	uint8_t* type_info_definition_table_address = FIND_PATTERN_IMAGE( dumper::game_base, "\x48\xF7\xE1\x48\x8B\xCA\x48\xC1\xE9\x04\xBA\x08\x00\x00\x00" );
	if ( !is_valid_ptr( type_info_definition_table_address ) )
		return false;

	type_info_definition_table_address = dumper::relative_32( type_info_definition_table_address + 21, 3 );
	if ( !is_valid_ptr( type_info_definition_table_address ) )
		return false;

	dumper::type_info_definition_table = *( il2cpp::il2cpp_class_t*** )( type_info_definition_table_address );
	return true;
}

int get_class_type_definition_index( il2cpp::il2cpp_class_t* klass ) {
	for ( int i = 0; i < 21070; i++ ) {
		if ( dumper::type_info_definition_table[ i ] == klass ) {
			return i;
		}
	}

	return -1;
}

void dumper::produce() {
	game_base = ( uint64_t )GetModuleHandleA( "GameAssembly.dll" );
	if ( !game_base )
		return;

	unity_base = ( uint64_t )GetModuleHandleA( "UnityPlayer.dll" );
	if ( !unity_base )
		return;

	if ( !resolve_type_info_definition_table() )
		return;

	outfile_handle = fopen( "C:\\dumps\\dumper_output.h", "w" );
	if ( !outfile_handle )
		return;

	outfile_log_handle = fopen( "C:\\dumps\\dumper_output.log", "w" );
	if ( !outfile_log_handle )
		return;

	dumper::write_to_file( "#include <cstdint>\n\n" );
	dumper::write_game_assembly();
	dumper::produce_unity();

	il2cpp::il2cpp_class_t* xmas_refill_class = DUMPER_CLASS( "XMasRefill" );
	il2cpp::il2cpp_class_t* network_message_class = nullptr;
	il2cpp::il2cpp_class_t* network_netread_class = nullptr;

	if ( xmas_refill_class ) {
		il2cpp::method_info_t* xmas_refill_on_rpc_message = il2cpp::get_method_by_return_type_attrs( 
			NO_FILT, xmas_refill_class, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ), METHOD_ATTRIBUTE_VIRTUAL, METHOD_ATTRIBUTE_PUBLIC, 3 );

		if ( xmas_refill_on_rpc_message ) {
			network_message_class = xmas_refill_on_rpc_message->get_param( 2 )->klass();

			if ( network_message_class ) {
				network_netread_class = get_class_by_field_in_class( network_message_class, "System.String", 2 );
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "XMasRefill", xmas_refill_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.Message", network_message_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.NetRead", network_netread_class );

	const char* network_netwrite_field_types[] = {
		"System.Int32",
		"Network.Priority",
		"Network.SendMethod",
		"System.SByte"
	};

	il2cpp::il2cpp_class_t* network_netwrite_class = il2cpp::search_for_class_containing_field_types_str( network_netwrite_field_types, _countof( network_netwrite_field_types ) );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.NetWrite", network_netwrite_class );

	il2cpp::method_info_t* network_base_network_start_write = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BaseEntity" ), "ServerRPC" ), 2 ),
		network_netwrite_class->type(),
		METHOD_ATTRIBUTE_PUBLIC,
		DUMPER_ATTR_DONT_CARE,
		0,
	);

	CHECK_RESOLVED_VALUE( VALUE_METHOD, "Network.BaseNetwork::StartWrite", network_base_network_start_write );

	il2cpp::il2cpp_class_t* network_base_network_class = network_base_network_start_write->klass();

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.BaseNetwork", network_base_network_class );

	const char* send_methods[] = { "Reliable", "ReliableUnordered", "Unreliable" };
	il2cpp::il2cpp_class_t* network_send_method_class = il2cpp::search_for_class_containing_field_names( send_methods, _countof( send_methods ) );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.SendMethod", network_send_method_class );

	const char* priorities[] = { "Immediate", "Normal" };
	il2cpp::il2cpp_class_t* network_priority_class = il2cpp::search_for_class_containing_field_names( priorities, _countof( priorities ) );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.Priority", network_priority_class );

	il2cpp::field_info_t* network_netwrite_connections_field = il2cpp::get_field_if_type_contains( network_netwrite_class, "System.Collections.Generic.List<", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
	il2cpp::il2cpp_class_t* network_connections_list_class = network_netwrite_connections_field->type()->klass();
	il2cpp::il2cpp_class_t* network_connection_class = nullptr;

	if ( network_connections_list_class ) {
		network_connection_class = network_connections_list_class->get_generic_argument_at( 0 );
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "System.Collections.Generic.List<Network.Connection>", network_connections_list_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.Connection", network_connection_class );

	const char* network_send_info_field_types[] = {
		network_send_method_class->type()->name(),
		"System.SByte",
		network_priority_class->type()->name(),
		network_connection_class->type()->name(),
	};

	il2cpp::il2cpp_class_t* network_send_info_class = il2cpp::search_for_class_containing_field_types_str( network_send_info_field_types, 4 );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.SendInfo", network_send_info_class );

	auto network_base_network_inheritors = il2cpp::get_inheriting_classes( network_base_network_class );
	auto it0 = std::find_if( network_base_network_inheritors.begin(), network_base_network_inheritors.end(), [ network_connection_class ]( il2cpp::il2cpp_class_t* klass ) {
		return il2cpp::get_field_from_field_type_class( klass, network_connection_class ) != nullptr;
	} );

	il2cpp::il2cpp_class_t* network_client_class = it0 != network_base_network_inheritors.end() ? *it0 : nullptr;

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.Client", network_client_class );

	il2cpp::il2cpp_class_t* network_net_class = il2cpp::search_for_class_by_field_types( network_client_class->type(), 0, DUMPER_ATTR_DONT_CARE, FIELD_ATTRIBUTE_STATIC );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.Net", network_net_class );

	auto network_client_class_inheritors = il2cpp::get_inheriting_classes( network_client_class );
	auto it2 = std::find_if( network_client_class_inheritors.begin(), network_client_class_inheritors.end(), []( il2cpp::il2cpp_class_t* klass ) {
		return il2cpp::get_field_if_type_contains( klass, "%", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
	} );

	il2cpp::il2cpp_class_t* facepunch_network_raknet_client_class = it2 != network_client_class_inheritors.end() ? *it2 : nullptr;

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Facepunch.Network.Raknet.Client", facepunch_network_raknet_client_class );

	il2cpp::virtual_method_t facepunch_network_raknet_client_is_connected = il2cpp::get_virtual_method_by_return_type_and_param_types(
		FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseEntity" ), "ServerRPC" ) ),
		facepunch_network_raknet_client_class,
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
		DUMPER_ATTR_DONT_CARE,
		METHOD_ATTRIBUTE_VIRTUAL,
		nullptr,
		0
	);

	CHECK_RESOLVED_VALUE( VALUE_METHOD, "Facepunch.Network.Raknet.Client::IsConnected", facepunch_network_raknet_client_is_connected.method );

	il2cpp::il2cpp_class_t* commands_display_class = il2cpp::search_for_class_by_method_in_assembly( "Facepunch.Console", "<Find>b__0", nullptr, -1 );
	il2cpp::il2cpp_class_t* console_system_command_class = nullptr;
	il2cpp::il2cpp_class_t* console_system_class = nullptr;

	if ( commands_display_class ) {
		il2cpp::method_info_t* commands_find = il2cpp::get_method_by_name( commands_display_class, "<Find>b__0" );

		if ( commands_find ) {
			il2cpp::il2cpp_type_t* param_type = commands_find->get_param( 0 );

			if ( param_type ) {
				console_system_command_class = param_type->klass();

				if ( console_system_command_class ) {
					console_system_class = get_outer_class( console_system_command_class );
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Commands.<>c__DisplayClass0_0", commands_display_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConsoleSystem.Command", console_system_command_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConsoleSystem", console_system_class );

	il2cpp::method_info_t* console_system_run = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		console_system_class,
		FILT( DUMPER_METHOD( DUMPER_CLASS( "QuickCraftButton" ), "OnPointerClick" ) ),
		DUMPER_TYPE_NAMESPACE( "System", "String" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		WILDCARD_VALUE( il2cpp::il2cpp_type_t* ),
		DUMPER_TYPE_NAMESPACE( "System", "String" ),
		WILDCARD_VALUE( il2cpp::il2cpp_type_t* )
	);

	CHECK_RESOLVED_VALUE( VALUE_METHOD, "ConsoleSystem::Run", console_system_run );

	il2cpp::il2cpp_class_t* console_system_option_class = console_system_run->get_param( 0 )->klass();

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConsoleSystem.Option", console_system_option_class );

	il2cpp::method_info_t* console_system_index_client_find = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS( "ShowIfConvarEnabled" ), "OnEnable" ) ),
		console_system_command_class->type(),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "System", "String" ),
	);

	il2cpp::il2cpp_class_t* console_system_index_client_class = console_system_index_client_find->klass();

	CHECK_RESOLVED_VALUE( VALUE_METHOD, "ConsoleSystem.Index.Client::Find", console_system_index_client_find );

	std::string console_system_index_class_name = std::string( console_system_index_client_class->type()->name() );
	console_system_index_class_name = console_system_index_class_name.substr( 0, console_system_index_class_name.find_last_of( '.' ) );
	std::replace( console_system_index_class_name.begin(), console_system_index_class_name.end(), '.', '/' );

	il2cpp::il2cpp_class_t* console_system_index_class = DUMPER_CLASS( console_system_index_class_name.c_str() );
	il2cpp::il2cpp_class_t* console_system_index_static_class = get_inner_static_class( console_system_index_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConsoleSystem.Index", console_system_index_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConsoleSystem.Index (static)", console_system_index_static_class );

	rust::console_system::get_override_offset = 
		il2cpp::get_field_if_type_contains( console_system_command_class, "System.Func<System.String>" )->offset();

	rust::console_system::set_override_offset = 
		il2cpp::get_field_if_type_contains( console_system_command_class, "System.Action<System.String>" )->offset();

	rust::console_system::call_offset =
		il2cpp::get_field_if_type_contains( console_system_command_class, "System.Action<%", FIELD_ATTRIBUTE_PUBLIC )->offset();

	rust::console_system::console_system_index_client_find = 
		( decltype( rust::console_system::console_system_index_client_find ) )console_system_index_client_find->get_fn_ptr<uint64_t>();

	rust::console_system::command* steamstatus_command = rust::console_system::client::find( system_c::string_t::create_string( L"global.steamstatus" ) );
	il2cpp::il2cpp_class_t* console_system_arg_class = nullptr;
	il2cpp::il2cpp_class_t* facepunch_network_steam_networking_class = nullptr;

	if ( steamstatus_command ) {
		il2cpp::method_info_t* steamstatus_wrapper = il2cpp::method_info_t::from_addr( steamstatus_command->call() );

		if ( steamstatus_wrapper ) {
			if ( steamstatus_wrapper->param_count() == 1 ) {
				console_system_arg_class = steamstatus_wrapper->get_param( 0 )->klass();
			}

			il2cpp::method_info_t* steamstatus = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE(
				WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
				FILT( steamstatus_wrapper->get_fn_ptr<uint64_t>() ),
				DUMPER_TYPE_NAMESPACE( "System", "String" ),
				METHOD_ATTRIBUTE_PUBLIC,
				METHOD_ATTRIBUTE_STATIC,
				0,
			);

			if ( steamstatus ) {
				facepunch_network_steam_networking_class = steamstatus->klass();
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConsoleSystem.Arg", console_system_arg_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Facepunch.Network.SteamNetworking", facepunch_network_steam_networking_class );

	il2cpp::method_info_t* game_physics_handle_ignore_collision = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS( "PoweredLightsDeployer" ), "OnInput" ) ),
		DUMPER_TYPE_NAMESPACE( "System", "Int32" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
		DUMPER_TYPE_NAMESPACE( "System", "Int32" )
	);

	il2cpp::il2cpp_class_t* game_physics_class = game_physics_handle_ignore_collision->klass();

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "GamePhysics", game_physics_class );

	il2cpp::il2cpp_class_t* player_loot_class = DUMPER_CLASS( "PlayerLoot" );
	il2cpp::il2cpp_class_t* item_class = nullptr;
	il2cpp::il2cpp_class_t* item_id_class = nullptr;

	if ( player_loot_class ) {
		il2cpp::field_info_t* entity_source = il2cpp::get_field_by_name( player_loot_class, "entitySource" );

		if ( entity_source ) {
			item_class = il2cpp::get_field_by_offset( player_loot_class, entity_source->offset() + 0x8 )->type()->klass();

			if ( item_class ) {
				il2cpp::method_info_t* base_entity_get_item = il2cpp::get_method_by_return_type_attrs( NO_FILT, DUMPER_CLASS( "BaseEntity" ), item_class, 0, 0, 1 );

				if ( base_entity_get_item ) {
					il2cpp::il2cpp_type_t* param_type = base_entity_get_item->get_param( 0 );

					if ( param_type ) {
						item_id_class = param_type->klass();
					}
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "PlayerLoot", player_loot_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Item", item_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ItemId", item_id_class );

	il2cpp::il2cpp_class_t* item_definition_class = DUMPER_CLASS( "ItemDefinition" );
	il2cpp::il2cpp_class_t* translate_phrase_class = nullptr;

	if ( item_definition_class ) {
		translate_phrase_class = il2cpp::get_field_by_name( item_definition_class, "displayName" )->type()->klass();
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ItemDefinition", item_definition_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Translate.Phrase", translate_phrase_class );

	il2cpp::il2cpp_class_t* item_container_class = nullptr;
	il2cpp::il2cpp_class_t* item_container_id_class = nullptr;

	if ( player_loot_class ) {
		il2cpp::il2cpp_class_t* templated_item_container_class = il2cpp::get_field_by_name( player_loot_class, "containers" )->type()->klass();

		if ( templated_item_container_class ) {
			item_container_class = templated_item_container_class->get_generic_argument_at( 0 );

			if ( item_container_class ) {
				il2cpp::method_info_t* item_container_find_container = il2cpp::get_method_by_return_type_attrs( NO_FILT, item_container_class, item_container_class, 0, 0, 1 );

				if ( item_container_find_container ) {
					il2cpp::il2cpp_type_t* param_type = item_container_find_container->get_param( 0 );

					if ( param_type ) {
						item_container_id_class = param_type->klass();
					}
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ItemContainer", item_container_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ItemContainerId", item_container_id_class );

	il2cpp::il2cpp_class_t* wallpaper_planner_class = DUMPER_CLASS( "WallpaperPlanner" );
	il2cpp::il2cpp_class_t* networkable_id_class = nullptr;

	if ( wallpaper_planner_class ) {
		il2cpp::method_info_t* wallpaper_planner_get_deployable = il2cpp::get_method_by_return_type_attrs( NO_FILT, wallpaper_planner_class, DUMPER_CLASS( "Deployable" ), 0, 0, 1 );

		if ( wallpaper_planner_get_deployable ) {
			il2cpp::il2cpp_type_t* param_type = wallpaper_planner_get_deployable->get_param( 0 );

			if ( param_type ) {
				networkable_id_class = param_type->klass();
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "WallpaperPlanner", wallpaper_planner_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "NetworkableId", networkable_id_class );

	projectile_attack_networkable_id = networkable_id_class;

	il2cpp::il2cpp_class_t* network_networkable_class = get_class_by_field_in_class( DUMPER_CLASS( "BaseNetworkable" ), network_connection_class->type()->name(), 2 );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Network.Networkable", network_networkable_class );

#ifdef HOOK
	uint64_t sleeping_bag_client_rename = DUMPER_METHOD( DUMPER_CLASS( "SleepingBag" ), "ClientRename" );

	if ( sleeping_bag_client_rename ) {
		uint8_t* jmp = FIND_PATTERN( sleeping_bag_client_rename, 128, "\x5F\xE9\xCC\xCC\xCC\xCC" );

		if ( is_valid_ptr( jmp ) ) {
			void* hook_addr = relative_32( jmp + 1, 1 );

			if ( is_valid_ptr( hook_addr ) ) {
				hook_t base_entity_server_rpc_object_hook( hook_addr, hk_base_entity_server_rpc_object, ( void** )&o_base_entity_server_rpc_object );
				base_entity_server_rpc_object_hook.create();
				base_entity_server_rpc_object_hook.enable();

				std::cout << "Hooked!\n";

				while ( true ) {
					Sleep( 1 );

					if ( GetAsyncKeyState( 'V' ) || ( resolved_projectile_shoot && resolved_projectile_update && resolved_projectile_attack ) ) {
						std::cout << "Removing Hook!\n";

						base_entity_server_rpc_object_hook.remove();
						break;
					}
				}
			}
		}
	}
#endif

	il2cpp::il2cpp_class_t* player_input_class = DUMPER_CLASS( "PlayerInput" );
	il2cpp::il2cpp_class_t* input_state_class = nullptr;
	il2cpp::il2cpp_class_t* input_message_class = nullptr;

	if ( player_input_class ) {
		void* iter = nullptr;
		
		// Find InputState by finding a field with an obfuscated type, that isn't NetworkableId, which we resolved previously
		while ( il2cpp::field_info_t* field = player_input_class->fields( &iter ) ) {
			const char* type_name = field->type()->name();
			if ( !type_name )
				continue;

			// We're only looking for obfuscated types
			if ( type_name[ 0 ] != '%' )
				continue;

			// We don't want NetworkableId
			if ( !strcmp( type_name, networkable_id_class->name() ) )
				continue;

			input_state_class = field->type()->klass();

			// Find InputMessage by finding a field that isn't static, and is of an obfuscated type
			if ( input_state_class ) {
				iter = nullptr;

				while ( il2cpp::field_info_t* field = input_state_class->fields( &iter ) ) {
					// We don't want static fields
					if ( ( field->flags() & FIELD_ATTRIBUTE_STATIC ) )
						continue;

					const char* type_name = field->type()->name();
					if ( !type_name )
						continue;

					// We're only looking for obfuscated types
					if ( type_name[ 0 ] != '%' )
						continue;

					input_message_class = field->type()->klass();
					break;
				}
			}

			break;
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "PlayerInput", player_input_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "InputState", input_state_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "InputMessage", input_message_class );

	const char* player_tick_field_types[] = {
		input_message_class->name(),
		"UnityEngine.Vector3",
		item_id_class->name(),
		"UnityEngine.Vector3",
		networkable_id_class->name(),
		"System.UInt32",
		"System.Boolean"
	};

	il2cpp::il2cpp_class_t* player_tick_class = il2cpp::search_for_class_containing_field_types_str( player_tick_field_types, _countof( player_tick_field_types ) );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "PlayerTick", player_tick_class );

	const char* model_state_flags[] = { "OnPhone", "HeadLook", "HasParachute" };
	il2cpp::il2cpp_class_t* model_state_flag_class = il2cpp::search_for_class_containing_field_names( model_state_flags, _countof( model_state_flags ) );
	il2cpp::il2cpp_class_t* model_state_class = get_outer_class( model_state_flag_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ModelState.Flag", model_state_flag_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ModelState", model_state_class );

	il2cpp::il2cpp_class_t* world_serialization_display_class = il2cpp::search_for_class_by_method_in_assembly( "Rust.World", "<GetPaths>b__0", nullptr, -1 );
	il2cpp::il2cpp_class_t* world_serialization_class = get_outer_class( world_serialization_display_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "WorldSerialization.<>c__DisplayClass15_0", world_serialization_display_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "WorldSerialization", world_serialization_class );

	il2cpp::il2cpp_class_t* entity_ref_class = SEARCH_FOR_CLASS_BY_FIELD_COUNT( 2, 0, DUMPER_CLASS( "BaseEntity" ), networkable_id_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "EntityRef", entity_ref_class );

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

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "HitboxCollision", hitbox_collision_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "HitTest", hit_test_class );

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

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "WeaponRack", weapon_rack_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "WeaponRackSlot", weapon_rack_slot_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "GameManager", game_manager_class );

	il2cpp::il2cpp_class_t* prefab_pool_collection_class = get_class_by_field_in_class( game_manager_class, "<System.UInt32,%", 2 );
	il2cpp::il2cpp_class_t* prefab_pool_class = nullptr;

	if ( prefab_pool_collection_class ) {
		il2cpp::field_info_t* storage = il2cpp::get_field_if_type_contains( prefab_pool_collection_class, "Dictionary" );

		if ( storage ) {
			prefab_pool_class = storage->type()->klass()->get_generic_argument_at( 1 );
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "PrefabPoolCollection", prefab_pool_collection_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "PrefabPool", prefab_pool_class );

	il2cpp::il2cpp_class_t* invisible_vending_machine_class = DUMPER_CLASS( "InvisibleVendingMachine" );
	il2cpp::il2cpp_class_t* base_networkable_load_info_class = nullptr;
	il2cpp::il2cpp_class_t* protobuf_entity_class = nullptr;

	if ( invisible_vending_machine_class ) {
		il2cpp::method_info_t* invisible_vending_machine_load = il2cpp::get_method_by_return_type_attrs( NO_FILT, invisible_vending_machine_class, DUMPER_CLASS_NAMESPACE( "System", "Void" ), METHOD_ATTRIBUTE_VIRTUAL, METHOD_ATTRIBUTE_PUBLIC, 1 );

		if ( invisible_vending_machine_load ) {
			il2cpp::il2cpp_type_t* param_type = invisible_vending_machine_load->get_param( 0 );

			if ( param_type ) {
				base_networkable_load_info_class = param_type->klass();

				if ( base_networkable_load_info_class ) {
					protobuf_entity_class = il2cpp::get_field_if_type_contains( base_networkable_load_info_class, "%" )->type()->klass();
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "InvisibleVendingMachine", invisible_vending_machine_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "BaseNetworkable.LoadInfo", base_networkable_load_info_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ProtoBuf.Entity", protobuf_entity_class );

	il2cpp::il2cpp_class_t* hide_if_aiming_class = DUMPER_CLASS( "HideIfAiming" );
	il2cpp::il2cpp_class_t* effect_class = nullptr;

	if ( hide_if_aiming_class ) {
		il2cpp::method_info_t* hide_if_aiming_setup_effect = il2cpp::get_method_by_return_type_attrs( NO_FILT, hide_if_aiming_class, DUMPER_CLASS_NAMESPACE( "System", "Void" ), METHOD_ATTRIBUTE_VIRTUAL, METHOD_ATTRIBUTE_PUBLIC, 1 );

		if ( hide_if_aiming_setup_effect ) {
			il2cpp::il2cpp_type_t* param_type = hide_if_aiming_setup_effect->get_param( 0 );

			if ( param_type ) {
				effect_class = param_type->klass();
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "HideIfAiming", hide_if_aiming_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Effect", effect_class );

	auto effect_network_candidates = il2cpp::search_for_classes_by_field_types( effect_class->type(), 0, DUMPER_ATTR_DONT_CARE, FIELD_ATTRIBUTE_STATIC );

	auto it1 = std::find_if( effect_network_candidates.begin(), effect_network_candidates.end(), []( il2cpp::il2cpp_class_t* klass ) {
		il2cpp::il2cpp_class_t* outer_class = get_outer_class( klass );
		return il2cpp::get_field_from_field_type_class( outer_class, DUMPER_CLASS_NAMESPACE( "UnityEngine", "Vector3" ) ) == nullptr;
	} );

	il2cpp::il2cpp_class_t* effect_network_static_class = it1 != effect_network_candidates.end() ? *it1 : nullptr;
	il2cpp::il2cpp_class_t* effect_network_class = get_outer_class( effect_network_static_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "EffectNetwork (static)", effect_network_static_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "EffectNetwork", effect_network_class );

	il2cpp::il2cpp_class_t* main_camera_class = DUMPER_CLASS( "MainCamera" );  // MainCamera
	il2cpp::il2cpp_class_t* rust_camera_main_camera_class = nullptr; // RustCamera<MainCamera>
	il2cpp::il2cpp_class_t* singleton_component_rust_camera_main_camera_class = nullptr; // SingletonComponent<RustCamera<MainCamera>>

	if ( main_camera_class ) {
		rust_camera_main_camera_class = main_camera_class->parent();

		if ( rust_camera_main_camera_class ) {
			singleton_component_rust_camera_main_camera_class = rust_camera_main_camera_class->parent();
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "MainCamera", main_camera_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "RustCamera<MainCamera>", rust_camera_main_camera_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "SingletonComponent<RustCamera<MainCamera>>", singleton_component_rust_camera_main_camera_class );
	
	il2cpp::il2cpp_class_t* ui_chat_class = DUMPER_CLASS( "UIChat" );
	il2cpp::il2cpp_class_t* priority_list_component_ui_chat_class = nullptr;
	il2cpp::il2cpp_class_t* list_component_ui_chat_class = nullptr;
	il2cpp::il2cpp_class_t* list_hash_set_ui_chat_class = nullptr;

	if ( ui_chat_class ) {
		priority_list_component_ui_chat_class = ui_chat_class->parent();

		if ( priority_list_component_ui_chat_class ) {
			list_component_ui_chat_class = priority_list_component_ui_chat_class->parent();

			if ( list_component_ui_chat_class ) {
				il2cpp::field_info_t* list_hash_set = il2cpp::get_field_if_type_contains( list_component_ui_chat_class, ui_chat_class->name() );

				if ( list_hash_set ) {
					list_hash_set_ui_chat_class = list_hash_set->type()->klass();
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "UIChat", ui_chat_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "PriorityListComponent<UIChat>", priority_list_component_ui_chat_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ListComponent<UIChat>", list_component_ui_chat_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ListHashSet<UIChat>", list_hash_set_ui_chat_class );

	il2cpp::method_info_t* aimcone_util_get_modified_aimcone_direction = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS( "SpinUpWeapon" ), "FireFakeBulletClient" ) ),
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "System", "Single" ),
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
	);

	CHECK_RESOLVED_VALUE( VALUE_METHOD, "AimConeUtil::GetModifiedAimConeDirection", aimcone_util_get_modified_aimcone_direction );

	il2cpp::il2cpp_class_t* convar_graphics_class = il2cpp::search_for_class_by_method_return_type_name( "UnityEngine.FullScreenMode", METHOD_ATTRIBUTE_PRIVATE, METHOD_ATTRIBUTE_STATIC );
	il2cpp::il2cpp_class_t* convar_graphics_static_class = get_inner_static_class( convar_graphics_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Graphics", convar_graphics_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Graphics (static)", convar_graphics_static_class );

	il2cpp::il2cpp_class_t* convar_server_class = il2cpp::search_for_class_by_method_return_type_name( "Rust.Era", METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC );
	il2cpp::il2cpp_class_t* convar_server_static_class = get_inner_static_class( convar_server_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Server", convar_server_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Server (static)", convar_server_static_class );

	il2cpp::il2cpp_class_t* convar_admin_class = nullptr;
	il2cpp::il2cpp_class_t* convar_admin_static_class = nullptr;

	rust::console_system::command* ent_command = rust::console_system::client::find( system_c::string_t::create_string( L"global.ent" ) );

	if ( ent_command ) {
		uint64_t call = ent_command->call();

		if ( call ) {
			il2cpp::method_info_t* convar_admin_ent = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
				WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
				FILT( call ),
				DUMPER_TYPE_NAMESPACE( "System", "String" ),
				METHOD_ATTRIBUTE_PUBLIC,
				METHOD_ATTRIBUTE_STATIC,
				console_system_arg_class->type(),
			);

			if ( convar_admin_ent ) {
				convar_admin_class = convar_admin_ent->klass();

				if ( convar_admin_class ) {
					convar_admin_static_class = get_inner_static_class( convar_admin_class );
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Admin", convar_admin_class );

	il2cpp::il2cpp_class_t* convar_player_class = nullptr;
	il2cpp::il2cpp_class_t* convar_player_static_class = nullptr;

	rust::console_system::command* cinematic_list_command = rust::console_system::client::find( system_c::string_t::create_string( L"player.cinematic_list" ) );

	if ( cinematic_list_command ) {
		uint64_t call = cinematic_list_command->call();

		if ( call ) {
			uint8_t* jmp = FIND_PATTERN( call, 16, "\xE9\xCC\xCC\xCC\xCC" );

			if ( jmp ) {
				il2cpp::method_info_t* convar_player_cinematic_list = il2cpp::method_info_t::from_addr( ( uint64_t )relative_32( jmp, 1 ) );

				if ( convar_player_cinematic_list ) {
					convar_player_class = convar_player_cinematic_list->klass();

					if ( convar_player_class ) {
						convar_player_static_class = get_inner_static_class( convar_player_class );
					}
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Player", convar_player_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Player (static)", convar_player_static_class );

	il2cpp::field_info_t* client_tickrate_field = il2cpp::get_field_if_type_contains( convar_player_static_class, "<System.Int32>", FIELD_ATTRIBUTE_PUBLIC, FIELD_ATTRIBUTE_STATIC );
	il2cpp::il2cpp_class_t* encrypted_value_class = nullptr;

	if ( client_tickrate_field ) {
		encrypted_value_class = client_tickrate_field->type()->klass();
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "EncryptedValue<float>", encrypted_value_class );

	il2cpp::method_info_t* convar_client_connect = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS( "UIFriends" ), "OnInviteAccepted" ) ),
		DUMPER_TYPE_NAMESPACE( "System", "String" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "System", "String" ),
		DUMPER_TYPE_NAMESPACE( "System", "String" ),
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
	);
	
	il2cpp::il2cpp_class_t* convar_client_class = convar_client_connect->klass();
	il2cpp::il2cpp_class_t* convar_client_static_class = get_inner_static_class( convar_client_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Client", convar_client_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ConVar.Client (static)", convar_client_static_class );

	il2cpp::il2cpp_class_t* buttons_conbutton_class = nullptr;
	il2cpp::il2cpp_class_t* buttons_class = nullptr;
	il2cpp::il2cpp_class_t* buttons_static_class = nullptr;

	rust::console_system::command* buttons_pets_command = rust::console_system::client::find( system_c::string_t::create_string( L"buttons.pets" ) );

	if ( buttons_pets_command ) {
		il2cpp::method_info_t* buttons_conbutton_set_is_pressed = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
			WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
			FILT( buttons_pets_command->set() ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);

		if ( buttons_conbutton_set_is_pressed ) {
			buttons_conbutton_class = buttons_conbutton_set_is_pressed->klass();

			if ( buttons_conbutton_class ) {
				buttons_class = get_outer_class( buttons_conbutton_class );

				if ( buttons_class ) {
					buttons_static_class = get_inner_static_class( buttons_class );
				}
			}
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Buttons.ConButton", buttons_conbutton_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Buttons", buttons_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "Buttons (static)", buttons_static_class );

	il2cpp::il2cpp_class_t* item_manager_class = nullptr;
	il2cpp::il2cpp_class_t* item_manager_static_class = nullptr;

	il2cpp::method_info_t* item_manager_find_item_definition = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS( "IndustrialConveyor/ItemFilter" ), "set_TargetItemName" ) ),
		DUMPER_TYPE( "ItemDefinition" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "System", "String" ),
	);

	if ( item_manager_find_item_definition ) {
		item_manager_class = item_manager_find_item_definition->klass();

		if ( item_manager_class ) {
			item_manager_static_class = get_inner_static_class( item_manager_class );
		}
	}

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ItemManager", item_manager_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "ItemManager (static)", item_manager_static_class );

	il2cpp::il2cpp_class_t* base_player_class = DUMPER_CLASS( "BasePlayer" );
	il2cpp::il2cpp_class_t* base_player_static_class = get_inner_static_class( base_player_class );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "BasePlayer", base_player_class );
	CHECK_RESOLVED_VALUE( VALUE_CLASS, "BasePlayer (static)", base_player_static_class );

	il2cpp::il2cpp_class_t* buffer_stream_class = get_class_by_field_in_class( network_netwrite_class, "System.Byte", 2 );

	CHECK_RESOLVED_VALUE( VALUE_CLASS, "BufferStream", buffer_stream_class );

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

	il2cpp::il2cpp_class_t* vehicle_module_storage_class = DUMPER_CLASS( "VehicleModuleStorage" );
	il2cpp::il2cpp_type_t* list_game_options_type = nullptr;

	if ( vehicle_module_storage_class ) {
		il2cpp::method_info_t* get_menu_options = il2cpp::get_method_by_name( vehicle_module_storage_class, "GetMenuOptions" );

		if ( get_menu_options ) {
			list_game_options_type = get_menu_options->get_param( 0 );
		}
	}

	il2cpp::method_info_t* open_context_menu = il2cpp::get_method_by_return_type_attrs(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "Hammer" ), "OnInput" ) ),
			DUMPER_CLASS( "Hammer" ),
			DUMPER_CLASS_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE
	);

	il2cpp::il2cpp_type_t* building_block_get_build_menu_params[] = { DUMPER_TYPE( "BasePlayer" ), DUMPER_TYPE_NAMESPACE( "System", "Action" ) };

	il2cpp::virtual_method_t building_block_get_build_menu = il2cpp::get_virtual_method_by_return_type_and_param_types(
		FILT_I( open_context_menu->get_fn_ptr<uint64_t>(), 1000, 0 ),
		DUMPER_CLASS( "BuildingBlock" ),
		list_game_options_type,
		METHOD_ATTRIBUTE_PUBLIC,
		DUMPER_ATTR_DONT_CARE,
		building_block_get_build_menu_params,
		_countof( building_block_get_build_menu_params )
	);

	il2cpp::virtual_method_t base_melee_do_attack = il2cpp::get_virtual_method_by_return_type_and_param_types(
		FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseMelee" ), "OnViewmodelEvent" ) ),
		DUMPER_CLASS( "BaseMelee" ),
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		DUMPER_ATTR_DONT_CARE,
		DUMPER_ATTR_DONT_CARE,
		nullptr,
		0
	);

	il2cpp::il2cpp_type_t* base_melee_process_attack_params[] = { hit_test_class->type() }; 

	il2cpp::virtual_method_t base_melee_process_attack = il2cpp::get_virtual_method_by_return_type_and_param_types(
		FILT_I( base_melee_do_attack.method->get_fn_ptr<uint64_t>(), 2000, 0  ),
		DUMPER_CLASS( "BaseMelee" ),
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		METHOD_ATTRIBUTE_FAMILY,
		METHOD_ATTRIBUTE_VIRTUAL,
		base_melee_process_attack_params,
		_countof( base_melee_process_attack_params )
	);

	il2cpp::il2cpp_class_t* base_networkable_static_class = get_inner_static_class( DUMPER_CLASS( "BaseNetworkable" ) );
	il2cpp::il2cpp_class_t* base_networkable_entity_realm_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseNetworkable" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( prefabID );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( net, network_networkable_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( parentEntity, entity_ref_class ); 
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( children, "System.Collections.Generic.List<BaseEntity>", FIELD_ATTRIBUTE_PUBLIC, FIELD_ATTRIBUTE_INIT_ONLY );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* hidden_value_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "BaseNetworkable_Static", base_networkable_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* client_entities = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "BaseNetworkable", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* client_entities ) { return client_entities != nullptr; } );
		DUMP_MEMBER_BY_X( clientEntities, client_entities->offset() );

		base_networkable_entity_realm_class = client_entities->type()->klass()->get_generic_argument_at( 0 );
		hidden_value_class = client_entities->type()->klass();
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* system_list_dictionary_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "BaseNetworkable_EntityRealm", base_networkable_entity_realm_class )
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* entity_list = il2cpp::get_field_if_type_contains( dumper_klass, "BaseNetworkable" );
		DUMP_MEMBER_BY_X( entityList, entity_list->offset() );

		system_list_dictionary_class = entity_list->type()->klass()->get_generic_argument_at( 0 );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* base_networkable_entity_realm_find = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "DemoShotPlayback" ), "Update" ) ),
			DUMPER_TYPE( "BaseNetworkable" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			networkable_id_class->type()
		);

		DUMP_METHOD_BY_INFO_PTR( Find, base_networkable_entity_realm_find );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* system_buffer_list_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "System_ListDictionary", system_list_dictionary_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* _vals = il2cpp::get_field_if_type_contains( dumper_klass, "<BaseNetworkable>" );
		DUMP_MEMBER_BY_X( vals, _vals->offset() );

		system_buffer_list_class = _vals->type()->klass();
	DUMPER_SECTION( "Functions" );
		const char* try_get_value_params[] = {
			networkable_id_class->name(),
			"BaseNetworkable&"
		};

		il2cpp::method_info_t* try_get_value = il2cpp::get_method_by_return_type_and_param_types_str(
			NO_FILT,
			dumper_klass,
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			try_get_value_params,
			_countof( try_get_value_params )
		);

		DUMP_METHOD_BY_INFO_PTR( TryGetValue, try_get_value );
		DUMP_MEMBER_BY_X( TryGetValue_methodinfo, DUMPER_RVA( find_value_in_data_section( try_get_value ) ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "System_BufferList", system_buffer_list_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( count, DUMPER_CLASS_NAMESPACE( "System", "Int32" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( buffer, "BaseNetworkable" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "SingletonComponent", singleton_component_rust_camera_main_camera_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* instance = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, main_camera_class->name(), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instance ) { return instance != nullptr; } );
		DUMP_MEMBER_BY_X( Instance, instance->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Model" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( rootBone );
		DUMP_MEMBER_BY_NAME( headBone );
		DUMP_MEMBER_BY_NAME( boneTransforms );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* position_lerp_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseEntity" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( bounds );
		DUMP_MEMBER_BY_NAME( model );
		DUMP_MEMBER_BY_NAME( flags );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( triggers, "List<TriggerBase>" );

		auto position_lerp = il2cpp::get_field_if_type_contains( dumper_klass, "%", FIELD_ATTRIBUTE_ASSEMBLY, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( positionLerp, position_lerp->offset( ) );
		position_lerp_class = position_lerp->type( )->klass( );

	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME_STR_ARG_CT( ServerRPC, "ServerRPC", 1 );

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
		il2cpp::field_info_t* _list = il2cpp::get_field_if_type_contains( interpolator_class, "System.Collections.Generic.List" );
		il2cpp::il2cpp_class_t* transform_snapshot_class = _list->type()->klass()->get_generic_argument_at( 0 );

		DUMPER_CLASS_BEGIN_FROM_PTR( "Interpolator", interpolator_class );
		DUMP_MEMBER_BY_X( list, _list->offset() );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( last, transform_snapshot_class );
		DUMPER_CLASS_END;
	}

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseCombatEntity" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( skeletonProperties );
		DUMP_MEMBER_BY_NAME( lifestate );
		DUMP_MEMBER_BY_NAME( markAttackerHostile );
		DUMP_MEMBER_BY_NEAR_OFFSET( _health, DUMPER_OFFSET( markAttackerHostile ) + 0x2 );
		DUMP_MEMBER_BY_NEAR_OFFSET( _maxHealth, DUMPER_OFFSET( markAttackerHostile ) + 0x6 );

		il2cpp::field_info_t* last_notify_frame = il2cpp::get_field_if_name_contains( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), "%", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( lastNotifyFrame, last_notify_frame->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SkeletonProperties" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( bones );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( quickLookup, "Dictionary" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SkeletonProperties/BoneProperty" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( name );
		DUMP_MEMBER_BY_NAME( area );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemDefinition" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( itemid );
		DUMP_MEMBER_BY_NAME( shortname );
		DUMP_MEMBER_BY_NAME( displayName );
		DUMP_MEMBER_BY_NAME( iconSprite );
		DUMP_MEMBER_BY_NAME( category );
		DUMP_MEMBER_BY_NAME( stackable );
		DUMP_MEMBER_BY_NAME( rarity );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( ItemModWearable, DUMPER_CLASS( "ItemModWearable" ) );
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
		DUMP_MEMBER_BY_NAME( capacity );
		DUMP_MEMBER_BY_NAME( contents );
		DUMP_MEMBER_BY_NAME( ammoType );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "AttackEntity" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( deployDelay );
		DUMP_MEMBER_BY_NAME( repeatDelay );
		DUMP_MEMBER_BY_NAME( animationDelay );
		DUMP_MEMBER_BY_NAME( noHeadshots );
		DUMP_MEMBER_BY_NEAR_OFFSET( nextAttackTime, DUMPER_OFFSET( noHeadshots ) + 0x2 );
		DUMP_MEMBER_BY_NEAR_OFFSET( timeSinceDeploy, DUMPER_OFFSET( noHeadshots ) + 0x1A );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( SpectatorNotifyTick );

		il2cpp::method_info_t* attack_entity_start_attack_cooldown = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "BeginCycle" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_FAMILY,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Single" )
		);

		DUMP_METHOD_BY_INFO_PTR( StartAttackCooldown, attack_entity_start_attack_cooldown );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseProjectile" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( projectileVelocityScale );
		DUMP_MEMBER_BY_NAME( automatic );
		DUMP_MEMBER_BY_NAME( reloadTime );
		DUMP_MEMBER_BY_NAME( primaryMagazine );
		DUMP_MEMBER_BY_NAME( fractionalReload );
		DUMP_MEMBER_BY_NAME( aimSway );
		DUMP_MEMBER_BY_NAME( aimSwaySpeed );
		DUMP_MEMBER_BY_NAME( recoil );
		DUMP_MEMBER_BY_NAME( aimconeCurve );
		DUMP_MEMBER_BY_NAME( aimCone );		
		DUMP_MEMBER_BY_NAME( hipAimCone );
		DUMP_MEMBER_BY_NAME( noAimingWhileCycling );
		DUMP_MEMBER_BY_NAME( isBurstWeapon );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( cachedModHash, "System.UInt32" );
		DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeScale, DUMPER_OFFSET( cachedModHash ) + 0x4 ); 
		DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeOffset, DUMPER_OFFSET( sightAimConeScale ) + 0x4 );
		DUMP_MEMBER_BY_NEAR_OFFSET( hipAimConeScale, DUMPER_OFFSET( sightAimConeOffset ) + 0x4 );
		DUMP_MEMBER_BY_NEAR_OFFSET( hipAimConeOffset, DUMPER_OFFSET( hipAimConeScale ) + 0x4 );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( LaunchProjectile );

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

		il2cpp::virtual_method_t base_projectile_get_aimcone = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE(
			FILT_I( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "LaunchProjectile" ), 500, 0 ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_VIRTUAL
		);

		DUMP_VIRTUAL_METHOD( GetAimCone, base_projectile_get_aimcone );

		il2cpp::virtual_method_t base_projectile_update_ammo_display = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "ProcessSpectatorViewmodelEvent" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			0,
			METHOD_ATTRIBUTE_FAMILY,
			METHOD_ATTRIBUTE_VIRTUAL
		);

		DUMP_VIRTUAL_METHOD( UpdateAmmoDisplay, base_projectile_update_ammo_display );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseLauncher" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SpinUpWeapon" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	uint64_t( *main_camera_trace )( float, uint64_t, float ) = nullptr;

	char searchBuf[ 128 ] = { 0 };
	sprintf_s( searchBuf, "%s.Type", hit_test_class->name() );

	il2cpp::method_info_t* gametrace_trace = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "HitTest", hit_test_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( type, searchBuf );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( AttackRay, DUMPER_CLASS_NAMESPACE( "UnityEngine", "Ray" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( RayHit, DUMPER_CLASS_NAMESPACE( "UnityEngine", "RaycastHit" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( damageProperties, DUMPER_CLASS( "DamageProperties" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( gameObject, DUMPER_CLASS_NAMESPACE( "UnityEngine", "GameObject" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( collider, DUMPER_CLASS_NAMESPACE( "UnityEngine", "Collider" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( ignoredTypes, "System.Collections.Generic.List<System.Type>" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( HitTransform, DUMPER_CLASS_NAMESPACE( "UnityEngine", "Transform" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( HitPart, DUMPER_CLASS_NAMESPACE( "System", "UInt32" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( HitMaterial, DUMPER_CLASS_NAMESPACE( "System", "String" ) );

		rust::console_system::command* battery_command = rust::console_system::client::find( system_c::string_t::create_string( L"electricbattery.battery" ) );

		if ( battery_command ) {
			uint64_t call = battery_command->call();

			if ( call ) {
				il2cpp::il2cpp_type_t* param_types[] = {
					DUMPER_TYPE_NAMESPACE( "System", "Single" ),
					DUMPER_TYPE( "BaseEntity" ),
					DUMPER_TYPE_NAMESPACE( "System", "Single" )
				};

				il2cpp::method_info_t* main_camera_trace_method = il2cpp::get_method_by_return_type_and_param_types(
					FILT_N( call, 2 ),
					DUMPER_CLASS( "MainCamera" ),
					hit_test_class->type(),
					METHOD_ATTRIBUTE_PUBLIC,
					METHOD_ATTRIBUTE_STATIC,
					param_types,
					_countof( param_types )
				);

				if ( main_camera_trace_method ) {
					main_camera_trace = ( decltype( main_camera_trace ) )main_camera_trace_method->get_fn_ptr<void*>();

					if ( main_camera_trace ) {
						uint64_t hit_test = main_camera_trace( 69420.f, 0xDEADBEEFCAFEBEEF, 0.f );

						if ( hit_test ) {
							std::vector<il2cpp::field_info_t*> bools = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Boolean" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
							std::vector<il2cpp::field_info_t*> floats = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
							std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
							std::vector<il2cpp::field_info_t*> base_entities = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE( "BaseEntity" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

							for ( il2cpp::field_info_t* bl : bools ) {
								bool value = *( bool* )( hit_test + bl->offset() );
								
								if ( value == true ) {
									DUMP_MEMBER_BY_X( DidHit, bl->offset() );
								}
							}

							for ( il2cpp::field_info_t* flt : floats ) {
								float value = *( float* )( hit_test + flt->offset() );

								if ( value == 69420.f ) {
									DUMP_MEMBER_BY_X( MaxDistance, flt->offset() );
								}
							}

							for ( il2cpp::field_info_t* vector : vectors ) {
								unity::vector3_t value = *( unity::vector3_t* )( hit_test + vector->offset() );

								if ( value.magnitude() > 1.01f ) {
									DUMP_MEMBER_BY_X( HitPoint, vector->offset() );
								}

								else {
									DUMP_MEMBER_BY_X( HitNormal, vector->offset() );
								}
							}

							for ( il2cpp::field_info_t* base_entity : base_entities ) {
								uint64_t value = *( uint64_t* )( hit_test + base_entity->offset() );

								if ( value != 0xDEADBEEFCAFEBEEF ) {
									DUMP_MEMBER_BY_X( HitEntity, base_entity->offset() );
								}

								else {
									DUMP_MEMBER_BY_X( ignoreEntity, base_entity->offset() );
								}
							}
						}

						gametrace_trace = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
							WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
							FILT( main_camera_trace_method->get_fn_ptr<uint64_t>() ),
							DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
							METHOD_ATTRIBUTE_PUBLIC,
							METHOD_ATTRIBUTE_STATIC,
							hit_test_class->type(),
							DUMPER_TYPE_NAMESPACE( "System", "Int32" )
						);
					}
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "GameTrace", gametrace_trace->klass() );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( Trace, gametrace_trace );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseMelee" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( damageProperties );
		DUMP_MEMBER_BY_NAME( maxDistance );
		DUMP_MEMBER_BY_NAME( attackRadius );
		DUMP_MEMBER_BY_NAME( blockSprintOnAttack );
		DUMP_MEMBER_BY_NAME( gathering );
		DUMP_MEMBER_BY_NAME( canThrowAsProjectile );
	DUMPER_SECTION( "Functions" );
		DUMP_MEMBER_BY_X( ProcessAttack, DUMPER_RVA( base_melee_process_attack.method->get_fn_ptr<uint64_t>() ) );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( DoThrow,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseMelee" ), "OnViewmodelEvent" ) ),
		    DUMPER_CLASS_NAMESPACE( "System", "Void" ), 0, METHOD_ATTRIBUTE_ASSEM, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "FlintStrikeWeapon" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( successFraction );
		DUMP_MEMBER_BY_NAME( strikeRecoil );
		DUMP_MEMBER_BY_NEAR_OFFSET( _didSparkThisFrame, DUMPER_OFFSET( strikeRecoil ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "CompoundBowWeapon" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( stringHoldDurationMax );
		DUMP_MEMBER_BY_NAME( stringBonusVelocity );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GetStringBonusScale,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "CompoundBowWeapon" ), "OnInput" ) ),
			DUMPER_CLASS_NAMESPACE( "System", "Single" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE 
		);
	DUMPER_CLASS_END;

	sprintf_s( searchBuf, "System.Collections.Generic.List<%s>", item_class->name( ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "ItemContainer", item_container_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( uid, item_container_id_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( itemList, searchBuf );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_STR( FindItemsByItemID, FILT_N( DUMPER_METHOD( DUMPER_CLASS( "SellOrderEntry" ), "UpdateNotifications" ), 3 ), searchBuf, 1 );
			
		il2cpp::method_info_t* item_container_get_slot = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "RepairBenchPanel" ), "Update" ) ),
			item_class->type(),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Int32" )
		);

		DUMP_METHOD_BY_INFO_PTR( GetSlot, item_container_get_slot );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerLoot" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( containers, item_container_class->name() );
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
				game_object->add_component( DUMPER_TYPE( "PlayerLoot" ) );

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

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( loot, DUMPER_CLASS( "PlayerLoot" ) );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( Initialize, player_inventory_initialize_method );

		sprintf_s( searchBuf, "System.Collections.Generic.List<%s>", item_class->name() );
		DUMP_METHOD_BY_RETURN_TYPE_STR( FindItemsByItemID, FILT_N( DUMPER_METHOD( DUMPER_CLASS( "SellOrderEntry" ), "UpdateNotifications" ), 2 ), searchBuf, 1 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerEyes" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( viewOffset, format_string( "%s<UnityEngine.Vector3>", encrypted_value_class->name() ) );
		DUMP_MEMBER_BY_NEAR_OFFSET( bodyRotation, DUMPER_OFFSET( viewOffset ) + 0x10 ); // <bodyRotation>k__BackingField
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

		il2cpp::method_info_t* player_eyes_set_rotation = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseMountable" ), "PlayerMounted" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Quaternion" )
		);

		DUMP_METHOD_BY_INFO_PTR( set_rotation, player_eyes_set_rotation );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( HeadForward,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "AttackHeliUIDialog" ), "Update" ) ),
			DUMPER_CLASS_NAMESPACE( "UnityEngine", "Vector3" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* player_eyes_static_class = get_inner_static_class( DUMPER_CLASS( "PlayerEyes" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "PlayerEyes_Static", player_eyes_static_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* eye_offset = il2cpp::get_static_field_if_value_is<unity::vector3_t>( dumper_klass, "UnityEngine.Vector3", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( unity::vector3_t eye_offset ) { return eye_offset == unity::vector3_t( 0.f, 1.5f, 0.f ); } );
		DUMP_MEMBER_BY_X( EyeOffset, eye_offset->offset() );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* player_belt_class = il2cpp::search_for_class_by_field_types( DUMPER_TYPE( "BasePlayer" ), 1, FIELD_ATTRIBUTE_FAMILY, DUMPER_ATTR_DONT_CARE );

	uint64_t( *player_belt_get_active_item )( uint64_t ) = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "PlayerBelt", player_belt_class );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* player_belt_change_select = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BasePlayer" ), "ClientUpdateLocalPlayer" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Int32" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);

		DUMP_METHOD_BY_INFO_PTR( ChangeSelect, player_belt_change_select );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GetActiveItem,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BasePlayer" ), "ModifyCamera" ) ),
			item_class,
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);

		player_belt_get_active_item = ( decltype( player_belt_get_active_item ) )( game_base + GetActiveItem_Offset );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* pet_command_desc_class = DUMPER_CLASS( "PetCommandList/PetCommandDesc" );
	il2cpp::il2cpp_class_t* local_player_static_class = il2cpp::search_for_class_by_field_types( pet_command_desc_class->type(), 0, FIELD_ATTRIBUTE_PUBLIC, FIELD_ATTRIBUTE_STATIC );
	il2cpp::il2cpp_class_t* local_player_class = get_outer_class( local_player_static_class );

	uint64_t( *local_player_get_entity )( ) = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "LocalPlayer", local_player_class );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* local_player_item_command = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "FrequencyConfig" ), "Confirm" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			item_id_class->type(),
			DUMPER_TYPE_NAMESPACE( "System", "String" )
		);

		DUMP_METHOD_BY_INFO_PTR( ItemCommand, local_player_item_command );

		il2cpp::method_info_t* local_player_move_item = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "ItemIcon" ), "OnDroppedValue" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			item_id_class->type(),
			item_container_id_class->type(),
			DUMPER_TYPE_NAMESPACE( "System", "Int32" ),
			DUMPER_TYPE_NAMESPACE( "System", "Int32" )
		);

		DUMP_METHOD_BY_INFO_PTR( MoveItem, local_player_move_item );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_Entity,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "EggHuntNote" ), "Update" ) ),
			DUMPER_CLASS( "BasePlayer" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC
		);

		local_player_get_entity = ( decltype( local_player_get_entity ) )( game_base + get_Entity_Offset );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "LocalPlayer_Static", local_player_static_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* entity = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "BasePlayer", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* entity ) { return entity != nullptr; } );
		DUMP_MEMBER_BY_X( Entity, entity->offset() );
	DUMPER_CLASS_END;

	size_t player_model_offset = -1;
	size_t last_sent_tick_offset = -1;
	size_t belt_offset = -1;

	uint64_t base_player_send_client_tick = 0;
	uint64_t base_player_client_input = 0;

	DUMPER_CLASS_BEGIN_FROM_PTR( "BasePlayer_Static", base_player_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* visible_player_list = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "<System.UInt64,BasePlayer>", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* visible_player_list ) { return visible_player_list != nullptr; } );
		DUMP_MEMBER_BY_X( visiblePlayerList, visible_player_list->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BasePlayer" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( playerModel, DUMPER_CLASS( "PlayerModel" ) );
		player_model_offset = playerModel_Offset;

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( input, DUMPER_CLASS( "PlayerInput" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( movement, DUMPER_CLASS( "BaseMovement" ) );
		DUMP_MEMBER_BY_NAME( currentTeam );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( clActiveItem, item_id_class->name() );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( modelState, model_state_class );
		DUMP_MEMBER_BY_NAME( playerFlags );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( eyes, "PlayerEyes" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( playerRigidbody, "UnityEngine.Rigidbody" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( userID, format_string( "%s<System.UInt64>", encrypted_value_class->name() ) );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( UserIDString, "System.String", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( inventory, "PlayerInventory" );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( _displayName, "System.String", FIELD_ATTRIBUTE_FAMILY, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( _lookingAt, "UnityEngine.GameObject", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( lastSentTickTime, format_string( "%s<System.Single>", encrypted_value_class->name() ) );

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( lastSentTick, player_tick_class );
		last_sent_tick_offset = lastSentTick_Offset;

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( mounted, entity_ref_class ); 

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( Belt, player_belt_class );
		belt_offset = Belt_Offset;

		if ( local_player_get_entity && last_sent_tick_offset != -1 ) {
			uint64_t local_player = local_player_get_entity();

			if ( local_player ) {			
				std::vector<il2cpp::field_info_t*> base_entities = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE( "BaseEntity" ), FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );

				for ( il2cpp::field_info_t* base_entity : base_entities ) {
					rust::base_entity* value = *( rust::base_entity** )( local_player + base_entity->offset() );

					if ( value == ( rust::base_entity* )local_player ) {
						DUMP_MEMBER_BY_X( _lookingAtEntity, base_entity->offset() );
					}
				}	
			}
		}

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( currentGesture, DUMPER_CLASS( "GestureConfig" ) );
		DUMP_MEMBER_BY_NAME( weaponMoveSpeedScale );
		DUMP_MEMBER_BY_NAME( clothingBlocksAiming );
		DUMP_MEMBER_BY_NAME( clothingMoveSpeedReduction );
		DUMP_MEMBER_BY_NAME( clothingWaterSpeedBonus );
		DUMP_MEMBER_BY_NAME( equippingBlocked );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( ClientUpdateLocalPlayer )
		DUMP_METHOD_BY_NAME( Menu_AssistPlayer );
		DUMP_METHOD_BY_NAME( OnViewModeChanged );
		DUMP_METHOD_BY_NAME( ChatMessage );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( IsOnGround, NO_FILT, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ), 0, METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_VIRTUAL );

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

#ifdef HOOK
		il2cpp::method_info_t* base_player_send_projectile_update = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "AdjustVelocity" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			protobuf_player_projectile_update_class->type()
		);

		DUMP_METHOD_BY_INFO_PTR( SendProjectileUpdate, base_player_send_projectile_update );

		il2cpp::method_info_t* base_player_send_projectile_attack = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		    FILT_N( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "Update" ), 6 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			protobuf_player_projectile_attack_class->type()
		);

		DUMP_METHOD_BY_INFO_PTR( SendProjectileAttack, base_player_send_projectile_attack );
#endif
		il2cpp::method_info_t* base_player_can_build = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "Hammer" ), "OnInput" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" )
		);

		DUMP_METHOD_BY_INFO_PTR( CanBuild, base_player_can_build );

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GetMounted,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "WaypointRace" ), "Update" ) ),
			DUMPER_CLASS( "BaseMountable" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GetHeldEntity,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "Crosshair" ), "Update" ) ),
			DUMPER_CLASS( "HeldEntity" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_inventory,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "SelectedItem" ), "Update" ) ),
			DUMPER_CLASS( "PlayerInventory" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);

		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_eyes,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "LiquidWeaponEffects" ), "FixedUpdate" ) ),
			DUMPER_CLASS( "PlayerEyes" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);

		il2cpp::method_info_t* base_player_send_client_tick_ = il2cpp::get_method_containing_function(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BasePlayer" ), "ClientUpdateLocalPlayer" ), 3 ),
			DUMPER_METHOD( DUMPER_CLASS_NAMESPACE( "System.Diagnostics", "Stopwatch" ), "get_ElapsedMilliseconds" )
		); base_player_send_client_tick = base_player_send_client_tick_->get_fn_ptr<uint64_t>();

		DUMP_METHOD_BY_INFO_PTR( SendClientTick, base_player_send_client_tick_ );

		il2cpp::virtual_method_t base_player_client_input_ = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BasePlayer" ), "ClientUpdateLocalPlayer" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_ASSEM,
			METHOD_ATTRIBUTE_VIRTUAL,
			input_state_class->type(),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
		); base_player_client_input = base_player_client_input_.method->get_fn_ptr<uint64_t>();

		DUMP_VIRTUAL_METHOD( ClientInput, base_player_client_input_ );

		il2cpp::virtual_method_t base_player_max_health = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseCombatEntity" ), "ResetState" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_ATTR_DONT_CARE,
			METHOD_ATTRIBUTE_VIRTUAL
		);

		DUMP_VIRTUAL_METHOD( MaxHealth, base_player_max_health );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ScientistNPC" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TunnelDweller" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "UnderwaterDweller" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ScarecrowNPC" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "GingerbreadNPC" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseMovement" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( adminCheat, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( Owner, DUMPER_CLASS( "BasePlayer" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerWalkMovement" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( capsule, "UnityEngine.CapsuleCollider" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( ladder, DUMPER_CLASS( "TriggerLadder" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( modify, "BaseEntity.MovementModify" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( BlockJump );
		DUMP_METHOD_BY_NAME( BlockSprint );

		il2cpp::method_info_t* player_walk_movement_ground_check = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "PlayerWalkMovement" ), "OnCollisionEnter" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Collision" ),
		);

		DUMP_METHOD_BY_INFO_PTR( GroundCheck, player_walk_movement_ground_check );

		il2cpp::virtual_method_t player_walk_movement_client_input = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( base_player_client_input ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			input_state_class->type(),
			model_state_class->type()
		);

		DUMP_VIRTUAL_METHOD( ClientInput, player_walk_movement_client_input );

		il2cpp::virtual_method_t player_walk_movement_do_fixed_update = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_I( DUMPER_METHOD( DUMPER_CLASS( "BasePlayer" ), "DoMovement" ), 200, 1 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			DUMPER_ATTR_DONT_CARE,
			DUMPER_ATTR_DONT_CARE,
			model_state_class->type()
		);

		DUMP_VIRTUAL_METHOD( DoFixedUpdate, player_walk_movement_do_fixed_update );

		il2cpp::virtual_method_t player_walk_movement_frame_update = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BasePlayer" ), "ClientUpdateLocalPlayer" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE( "BasePlayer" ),
			model_state_class->type()
		);

		DUMP_VIRTUAL_METHOD( FrameUpdate, player_walk_movement_frame_update );
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
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( hackSeconds, DUMPER_CLASS_NAMESPACE( "System", "Single" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ProjectileWeaponMod" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( repeatDelay );
		DUMP_MEMBER_BY_NAME( projectileVelocity );
		DUMP_MEMBER_BY_NAME( projectileDamage );
		DUMP_MEMBER_BY_NAME( projectileDistance );
		DUMP_MEMBER_BY_NAME( aimsway );
		DUMP_MEMBER_BY_NAME( aimswaySpeed );
		DUMP_MEMBER_BY_NAME( recoil );
		DUMP_MEMBER_BY_NAME( sightAimCone );
		DUMP_MEMBER_BY_NAME( hipAimCone );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ProjectileWeaponMod/Modifier" );
		DUMP_MEMBER_BY_NAME( enabled );
		DUMP_MEMBER_BY_NAME( scalar );
		DUMP_MEMBER_BY_NAME( offset );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConsoleSystem", console_system_class );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( Run, console_system_run );
	DUMPER_CLASS_END;

	DUMPER_PTR_CLASS_NAME( "ConsoleSystem_Option", console_system_option_class );
	DUMPER_PTR_CLASS_NAME( "ConsoleSystem_Command", console_system_command_class );

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConsoleSystem_Index_Static", console_system_index_static_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* all = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, console_system_command_class->type()->name(), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* all ) { return all != nullptr; } );
		DUMP_MEMBER_BY_X( All, all->offset() );
	DUMPER_CLASS_END;

	DUMP_CLASS_NAME( "ConsoleSystem_Arg", console_system_arg_class );
	DUMP_CLASS_NAME( "Facepunch_Network_SteamNetworking", facepunch_network_steam_networking_class );

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
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets " );
		il2cpp::field_info_t* main_camera = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "UnityEngine.Camera", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( unity::component_t* camera ) { return camera != nullptr; } );
		DUMP_MEMBER_BY_X( mainCamera, main_camera->offset() );

		il2cpp::field_info_t* main_camera_transform = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "UnityEngine.Transform", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( unity::component_t* transform ) { return transform != nullptr; } );
		DUMP_MEMBER_BY_X( mainCameraTransform, main_camera_transform->offset() );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* main_camera_update = il2cpp::get_method_by_name( dumper_klass, "Update" );
		DUMP_METHOD_BY_INFO_PTR( Update, main_camera_update );

		DUMP_MEMBER_BY_X( Trace, DUMPER_RVA( ( uint64_t )main_camera_trace ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "CameraMan" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	size_t input_state_offset = -1;
	size_t model_state_offset = -1;

	DUMPER_CLASS_BEGIN_FROM_PTR( "PlayerTick", player_tick_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( inputState, input_message_class );
		input_state_offset = inputState_Offset;

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( modelState, model_state_class );
		model_state_offset = modelState_Offset;

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( activeItem, item_id_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( parentID, networkable_id_class );

		if ( local_player_get_entity && last_sent_tick_offset != -1 ) {
			uint64_t local_player = local_player_get_entity();

			if ( local_player ) {
				uint64_t last_sent_tick = *( uint64_t* )( local_player + last_sent_tick_offset );
				
				if ( last_sent_tick ) {
					std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

					if ( vectors.size() == 2 ) {
						unity::vector3_t vector1 = *( unity::vector3_t* )( last_sent_tick + vectors.at( 0 )->offset() );
						unity::vector3_t vector2 = *( unity::vector3_t* )( last_sent_tick + vectors.at( 1 )->offset() );

						il2cpp::field_info_t* position_field = ( vector1.y < vector2.y ) ? vectors.at( 0 ) : vectors.at( 1 );
						il2cpp::field_info_t* eye_pos_field = ( vector1.y > vector2.y ) ? vectors.at( 0 ) : vectors.at( 1 );

						DUMP_MEMBER_BY_X( position, position_field->offset() );
						DUMP_MEMBER_BY_X( eyePos, eye_pos_field->offset() );
					}
				}
			}
		}
	DUMPER_SECTION( "Functions" );
		il2cpp::virtual_method_t player_tick_write_to_stream_delta = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( base_player_send_client_tick ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			DUMPER_ATTR_DONT_CARE,
			DUMPER_ATTR_DONT_CARE,
			WILDCARD_VALUE( il2cpp::il2cpp_type_t* ),
			player_tick_class->type()
		);

		DUMP_VIRTUAL_METHOD( WriteToStreamDelta, player_tick_write_to_stream_delta );
	DUMPER_CLASS_END;

	il2cpp::field_info_t* _buttons = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "InputMessage", input_message_class );
	DUMPER_SECTION( "Offsets" );
		_buttons = il2cpp::get_field_from_field_type_class( input_message_class, DUMPER_CLASS_NAMESPACE( "System", "Int32" ) );
		DUMP_MEMBER_BY_X( buttons, _buttons->offset() );

		if ( local_player_get_entity && last_sent_tick_offset != -1 && input_state_offset != -1 ) {
			uint64_t local_player = local_player_get_entity();

			if ( local_player ) {
				uint64_t last_sent_tick = *( uint64_t* )( local_player + last_sent_tick_offset );

				if ( last_sent_tick ) {
					uint64_t input_state = *( uint64_t* )( last_sent_tick + input_state_offset );

					if ( input_state ) {
						std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

						for ( il2cpp::field_info_t* vector : vectors ) {
							unity::vector3_t value = *( unity::vector3_t* )( input_state + vector->offset() );

							if ( value.magnitude() > 0.1f ) {
								DUMP_MEMBER_BY_X( aimAngles, vector->offset() );
							}

							else {
								DUMP_MEMBER_BY_X( mouseDelta, vector->offset() );
							}
						}
					}
				}
			}
		}
	DUMPER_CLASS_END;

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

					*( uint32_t* )( input_message_c + _buttons->offset() ) = 1337;

					input_state_flip( input_state, input_message_c );

					for ( uint32_t i = 0; i < 2; i++ ) {
						uint64_t input_message = *( uint64_t* )( input_state + fields.at( i )->offset() );

						if ( input_message ) {
							uint32_t buttons = *( uint32_t* )( input_message + _buttons->offset() );

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

	DUMPER_CLASS_BEGIN_FROM_PTR( "ModelState", model_state_class );
	DUMPER_SECTION( "Offsets" );
		if ( local_player_get_entity && last_sent_tick_offset != -1 ) {
			uint64_t local_player = local_player_get_entity();

			if ( local_player ) {
				uint64_t last_sent_tick = *( uint64_t* )( local_player + last_sent_tick_offset );

				if ( last_sent_tick ) {
					uint64_t model_state = *( uint64_t* )( last_sent_tick + model_state_offset );

					if ( model_state ) {
						std::vector<il2cpp::field_info_t*> ints = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
						std::vector<il2cpp::field_info_t*> floats = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
						std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

						for ( il2cpp::field_info_t* _int : ints ) {
							int value = *( int* )( model_state + _int->offset() );

							if ( value & rust::model_state::e_flag::flying ) {
								DUMP_MEMBER_BY_X( flags, _int->offset() );
								break;
							}
						}

						for ( il2cpp::field_info_t* _float : floats ) {
							float value = *( int* )( model_state + _float->offset() );

							if ( value > 0.f ) {
								DUMP_MEMBER_BY_X( waterLevel, _float->offset() );
								break;
							}
						}

						for ( il2cpp::field_info_t* vector : vectors ) {
							unity::vector3_t value = *( unity::vector3_t* )( model_state + vector->offset() );

							if ( value.magnitude() > 0.1f ) {
								DUMP_MEMBER_BY_X( lookDir, vector->offset() );
								break;
							}
						}
					}
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Item", item_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( info, DUMPER_CLASS( "ItemDefinition" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( uid, item_id_class );

		if ( local_player_get_entity && belt_offset != -1 ) {
			uint64_t local_player = local_player_get_entity();

			if ( local_player ) {
				uint64_t belt = *( uint64_t* )( local_player + belt_offset );

				if ( belt ) {
					uint64_t active_item = player_belt_get_active_item( belt );

					if ( active_item ) {
						std::vector<il2cpp::field_info_t*> floats = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );
						std::vector<il2cpp::field_info_t*> ints = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );
						std::vector<il2cpp::field_info_t*> item_containers = il2cpp::get_fields_of_type( dumper_klass, item_container_class->type(), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );
						std::vector<il2cpp::field_info_t*> entity_refs = il2cpp::get_fields_of_type( dumper_klass, entity_ref_class->type(), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );

						for ( il2cpp::field_info_t* flt : floats ) {
							float value = *( float* )( active_item + flt->offset() );

							if ( value == 52.5125f ) {
								DUMP_MEMBER_BY_X( _condition, flt->offset() );
							}

							else if ( value == 97.5125f ) {
								DUMP_MEMBER_BY_X( _maxCondition, flt->offset() );
							}
						}

						for ( il2cpp::field_info_t* _int : ints ) {
							int value = *( int* )( active_item + _int->offset() );

							if ( value == 69 ) {
								DUMP_MEMBER_BY_X( amount, _int->offset() );
							}

							else if ( value == 4 ) {
								DUMP_MEMBER_BY_X( position, _int->offset() );
							}
						}

						for ( il2cpp::field_info_t* item_container : item_containers ) {
							void* value = *( void** )( active_item + item_container->offset() );

							if ( !value ) {
								DUMP_MEMBER_BY_X( contents, item_container->offset() );
							}

							else {
								DUMP_MEMBER_BY_X( parent, item_container->offset() );
							}
						}

						for ( il2cpp::field_info_t* entity_ref : entity_refs ) {
							void* value = *( void** )( active_item + entity_ref->offset() );

							if ( value ) {
								DUMP_MEMBER_BY_X( heldEntity, entity_ref->offset() );
							}

							else {
								DUMP_MEMBER_BY_X( worldEnt, entity_ref->offset() );
							}
						}
					}
				}
			}
		}

	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_iconSprite,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "ItemPreviewIcon" ), "OnItemIconChanged" ) ),
			DUMPER_CLASS_NAMESPACE( "UnityEngine", "Sprite" ),
			0,
			METHOD_ATTRIBUTE_ASSEM,
			DUMPER_ATTR_DONT_CARE
		);

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
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);

		DUMP_METHOD_BY_INFO_PTR( GetWaterLevel, water_level_get_water_level );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConVar_Graphics_Static", convar_graphics_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* fov = il2cpp::get_static_field_if_value_is<uint32_t>( dumper_klass, format_string( "%s<System.Single>", encrypted_value_class->name() ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( uint32_t value ) { return value != 0; } );
		DUMP_MEMBER_BY_X( _fov, fov->offset() );
	DUMPER_SECTION( "Functions" );
		rust::console_system::command* fov_command = rust::console_system::client::find( system_c::string_t::create_string( L"graphics.fov" ) );

		if ( fov_command ) {
			DUMP_MEMBER_BY_X( _fov_getter, DUMPER_RVA( fov_command->get() ) );
			DUMP_MEMBER_BY_X( _fov_setter, DUMPER_RVA( fov_command->set() ) );
		}

		rust::console_system::command* viewmodelcamera_command = rust::console_system::client::find( system_c::string_t::create_string( L"graphics.viewmodelcamera" ) );

		if ( viewmodelcamera_command ) {
			DUMP_MEMBER_BY_X( viewmodelcamera_getter, DUMPER_RVA( viewmodelcamera_command->get() ) );
			DUMP_MEMBER_BY_X( viewmodelcamera_setter, DUMPER_RVA( viewmodelcamera_command->set() ) );
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseFishingRod" );
	DUMPER_SECTION( "Offsets" )
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( CurrentState, "CatchState" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( currentBobber, "FishingBobber" );
		DUMP_MEMBER_BY_NAME( MaxCastDistance );
		DUMP_MEMBER_BY_NAME( BobberPreview );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( clientStrainAmountNormalised, "System.Single", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( strainGainMod, DUMPER_CLASS( "SoundModulation/Modulator" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( aimAnimationReady, "System.Boolean", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );

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

	DUMPER_CLASS_BEGIN_FROM_NAME( "FishingBobber" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( bobberRoot );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "GameManifest" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GUIDToObject, FILT( resource_ref_get ), DUMPER_CLASS_NAMESPACE( "UnityEngine", "Object" ), 1, METHOD_ATTRIBUTE_ASSEM, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "GameManager", game_manager_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( pool, prefab_pool_collection_class );
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

	il2cpp::il2cpp_class_t* game_manager_static_class = get_inner_static_class( game_manager_class );

	DUMPER_CLASS_BEGIN_FROM_PTR( "GameManager_Static", game_manager_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* client_ = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, game_manager_class->name(), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* client ) { return client != nullptr; } );
		DUMP_MEMBER_BY_X( client, client_->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "PrefabPoolCollection", prefab_pool_collection_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( storage, "Dictionary" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "PrefabPool", prefab_pool_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( stack, "Poolable" );
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
		DUMP_MEMBER_BY_NAME( category );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Projectile" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( initialVelocity );
		DUMP_MEMBER_BY_NAME( drag );
		DUMP_MEMBER_BY_NAME( gravityModifier );
		DUMP_MEMBER_BY_NAME( thickness );
		DUMP_MEMBER_BY_NAME( initialDistance );
		DUMP_MEMBER_BY_NAME( swimScale );
		DUMP_MEMBER_BY_NAME( swimSpeed );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( owner, DUMPER_CLASS( "BasePlayer" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( sourceProjectilePrefab, DUMPER_CLASS( "Projectile" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( mod, DUMPER_CLASS( "ItemModProjectile" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( hitTest, hit_test_class );

		void( *projectile_initialize_velocity )( uint64_t, unity::vector3_t ) = ( decltype( projectile_initialize_velocity ) )DUMPER_METHOD( dumper_klass, "InitializeVelocity" );

		il2cpp::method_info_t* projectile_launch_method = il2cpp::get_method_by_return_type_attrs(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BaseProjectile" ), "LaunchProjectile" ), 2),
			DUMPER_CLASS( "Projectile" ),
			DUMPER_CLASS_NAMESPACE( "System", "Void" ),
			DUMPER_ATTR_DONT_CARE,
			METHOD_ATTRIBUTE_ASSEM,
			0
		);

		void( *projectile_launch )( uint64_t ) = ( decltype( projectile_launch ) )projectile_launch_method->get_fn_ptr<uint64_t>();
		void( *projectile_on_disable )( uint64_t ) = ( decltype( projectile_on_disable ) )DUMPER_METHOD( dumper_klass, "OnDisable" );

		if ( projectile_initialize_velocity && is_valid_ptr( projectile_launch ) && projectile_on_disable ) {
			unity::game_object_t* game_object = unity::game_object_t::create( L"" );
			game_object->add_component( dumper_klass->type() );

			// Spawn the projectile super high up, so the results are always the same regardless of map
			if ( unity::transform_t* transform = game_object->get_transform() ) {
				transform->set_position( unity::vector3_t( 420.f, 4200.f, 420.f ) );
			}

			if ( uint64_t projectile = game_object->get_component( dumper_klass->type() ) ) {
				projectile_initialize_velocity( projectile, unity::vector3_t( 1337.f, 1337.f, 1337.f ) );

				std::vector<il2cpp::field_info_t*> floats = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );
				std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );
				std::vector<il2cpp::field_info_t*> internal_vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), FIELD_ATTRIBUTE_ASSEMBLY, DUMPER_ATTR_DONT_CARE );

				for ( il2cpp::field_info_t* flt : floats ) {
					float value = *( float* )( projectile + flt->offset() );

					if ( value == unity::time::get_fixed_time() ) {
						DUMP_MEMBER_BY_X( launchTime, flt->offset() );
					}
				}

				for ( il2cpp::field_info_t* vector : internal_vectors ) {
					unity::vector3_t value = *( unity::vector3_t* )( projectile + vector->offset() );

					if ( value == unity::vector3_t( 1337.f, 1337.f, 1337.f ) ) {
						DUMP_MEMBER_BY_X( currentVelocity, vector->offset() );
					}

					else if ( value == unity::vector3_t( 420.f, 4200.f, 420.f ) ) {
						DUMP_MEMBER_BY_X( currentPosition, vector->offset() );
					}
				}

				projectile_launch( projectile );

				for ( il2cpp::field_info_t* flt : floats ) {
					float value = *( float* )( projectile + flt->offset() );

					if ( value == INFINITY ) {
						DUMP_MEMBER_BY_X( maxDistance, flt->offset() );
					}

					else if ( FLOAT_IS_EQUAL( value, 289.436f, 0.01f ) ) {
						DUMP_MEMBER_BY_X( traveledDistance, flt->offset() );
					}

					else if ( FLOAT_IS_EQUAL( value, 0.125f, 0.001f ) ) {
						DUMP_MEMBER_BY_X( traveledTime, flt->offset() );
					}

					else if ( FLOAT_IS_EQUAL( value, 0.09375f, 0.001f ) ) {
						DUMP_MEMBER_BY_X( previousTraveledTime, flt->offset() );
					}
				}

				for ( il2cpp::field_info_t* vector : vectors ) {
					unity::vector3_t value = *( unity::vector3_t* )( projectile + vector->offset() );

					if ( VECTOR_IS_EQUAL( value, unity::vector3_t( 420.f, 4200.f, 420.f ), 0.01 ) ) {
						DUMP_MEMBER_BY_X( sentPosition, vector->offset() );
					}

					else if ( VECTOR_IS_EQUAL( value, unity::vector3_t( 545.344f, 4325.31f, 545.344f ), 0.01f ) ) {
						DUMP_MEMBER_BY_X( previousPosition, vector->offset() );
					}

					else if ( VECTOR_IS_EQUAL( value, unity::vector3_t( 1337.f, 1336.08f, 1337.f ), 0.01f ) ) {
						DUMP_MEMBER_BY_X( previousVelocity, vector->offset() );
					}
				}

				SET_ALL_FIELDS_OF_TYPE_TO_VALUE( projectile, DUMPER_TYPE_NAMESPACE( "System", "Single" ), float, 0.f );

				projectile_on_disable( projectile );

				for ( il2cpp::field_info_t* flt : floats ) {
					float value = *( float* )( projectile + flt->offset() );

					if ( FLOAT_IS_EQUAL( value, 1.f, 0.01f ) ) {
						DUMP_MEMBER_BY_X( integrity, flt->offset() );
					}
				}
			}
		}
	DUMPER_SECTION( "Functions" );
		il2cpp::virtual_method_t projectile_calculate_effect_scale = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "Update" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			0,
			METHOD_ATTRIBUTE_FAMILY,
			METHOD_ATTRIBUTE_VIRTUAL
		); 

		DUMP_VIRTUAL_METHOD( CalculateEffectScale, projectile_calculate_effect_scale );

		il2cpp::method_info_t* projectile_set_effect_scale = il2cpp::get_method_containing_function(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "Update" ), 2 ),
			DUMPER_METHOD( DUMPER_CLASS( "ScaleRenderer" ), "SetScale" )
		);

		DUMP_METHOD_BY_INFO_PTR( SetEffectScale, projectile_set_effect_scale );

		il2cpp::method_info_t* projectile_update_velocity = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_I( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "Update" ), projectile_set_effect_scale->get_fn_ptr<uint64_t>(), 1 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Single" )
		);

		DUMP_METHOD_BY_INFO_PTR( UpdateVelocity, projectile_update_velocity );

		il2cpp::il2cpp_type_t* param_types[] = {
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Behaviour" ),
			DUMPER_TYPE_NAMESPACE( "System", "Action" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
		};

		il2cpp::method_info_t* invoke_handler_invoke = il2cpp::get_method_by_return_type_and_param_types(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "GlassPane" ), "OnDisable" ) ),
			DUMPER_CLASS( "InvokeHandler" ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			param_types, 
			_countof( param_types )
		);

		il2cpp::method_info_t* projectile_retire = il2cpp::get_method_containing_function(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "Update" ), 2 ),
			invoke_handler_invoke->get_fn_ptr<uint64_t>()
		);

		DUMP_METHOD_BY_INFO_PTR( Retire, projectile_retire );

		il2cpp::method_info_t* projectile_do_hit = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES_SIZE(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "Projectile" ), "Update" ), 5 ),
			0,
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			hit_test_class->type(),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
		);

		DUMP_METHOD_BY_INFO_PTR( DoHit, projectile_do_hit );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "CraftingQueue" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( icons, "List" );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* crafting_queue_static_class = get_inner_static_class( DUMPER_CLASS( "CraftingQueue" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "CraftingQueue_Static", crafting_queue_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* is_crafting = il2cpp::get_static_field_if_value_is<bool>( dumper_klass, "System.Boolean", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( bool is_crafting ) { return is_crafting == true; } );
		DUMP_MEMBER_BY_X( isCrafting, is_crafting->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "CraftingQueueIcon" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( endTime, "System.Single" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( item, DUMPER_CLASS( "ItemDefinition" ) );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* planner_guide_class = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Planner_Static", get_inner_static_class( DUMPER_CLASS( "Planner" ) ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* _guide = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "Planner.%", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* guide ) { return guide != nullptr; } );
		DUMP_MEMBER_BY_X( guide, _guide->offset() );

		planner_guide_class = _guide->type()->klass();
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Planner_Guide", planner_guide_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( lastPlacement, "Construction.%" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Planner" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( _currentConstruction, DUMPER_CLASS( "Construction" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Construction" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( holdToPlaceDuration );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( grades, "ConstructionGrade[]" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BuildingBlock" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( blockDefinition, DUMPER_CLASS( "Construction" ) );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* held_entity_class = DUMPER_CLASS( "HeldEntity" );
	il2cpp::field_info_t* punches_field = nullptr;

	DUMPER_CLASS_BEGIN_FROM_PTR( "HeldEntity", held_entity_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( ownerItemUID, item_id_class );

		punches_field = il2cpp::get_field_if_type_contains( dumper_klass, "List<HeldEntity", FIELD_ATTRIBUTE_FAMILY, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( _punches, punches_field->offset() );

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( viewModel, DUMPER_CLASS( "ViewModel" ) ); // <viewModel>k__BackingField
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( OnDeploy );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* punch_entry_class = punches_field->type()->klass()->get_generic_argument_at( 0 );

	DUMPER_CLASS_BEGIN_FROM_PTR( "PunchEntry", punch_entry_class );
	DUMPER_SECTION( "Offsets" );
		void( *held_entity_add_punch )( uint64_t, unity::vector3_t, float ) = ( decltype( held_entity_add_punch ) )DUMPER_METHOD( held_entity_class, "AddPunch" );

		if ( held_entity_add_punch ) {
			unity::game_object_t* game_object = unity::game_object_t::create( L"" );
			game_object->add_component( held_entity_class->type() );

			if ( uint64_t held_entity = game_object->get_component( held_entity_class->type() ) ) {
				held_entity_add_punch( held_entity, unity::vector3_t( 1337.f, 1337.f, 1337.f ), 420.f );

				std::vector<il2cpp::field_info_t*> floats = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
				std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

				system_c::list<uint64_t>* punches = *( system_c::list<uint64_t>** )( held_entity + punches_field->offset() );

				for ( int i = 0; i < punches->_size; i++ ) {
					uint64_t punch = punches->at( i );
					if ( !punch )
						continue;

					for ( il2cpp::field_info_t* flt : floats ) {
						float value = *( float* )( punch + flt->offset() );

						if ( value == 420.f ) {
							DUMP_MEMBER_BY_X( duration, flt->offset() );
						}

						else {
							DUMP_MEMBER_BY_X( startTime, flt->offset() );
						}
					}

					for ( il2cpp::field_info_t* vector : vectors ) {
						unity::vector3_t value = *( unity::vector3_t* )( punch + vector->offset() );

						if ( value == unity::vector3_t( 1337.f, 1337.f, 1337.f ) ) {
							DUMP_MEMBER_BY_X( amount, vector->offset() );
						}

						else {
							DUMP_MEMBER_BY_X( amountAdded, vector->offset() );
						}
					}
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "IronSights" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( zoomFactor );
		DUMP_MEMBER_BY_NAME( ironsightsOverride );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "IronSightOverride" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( zoomFactor );
		DUMP_MEMBER_BY_NAME( fovBias );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseViewModel" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( useViewModelCamera );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( ironSights, DUMPER_CLASS( "IronSights" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( model, DUMPER_CLASS( "Model" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( lower, DUMPER_CLASS( "ViewmodelLower" ) );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_ActiveModel,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "ViewmodelEditor" ), "Update" ) ),
			DUMPER_CLASS( "BaseViewModel" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC
		);
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ViewModel" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( instance, DUMPER_CLASS( "BaseViewModel" ) ); 
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( Play );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "MedicalTool" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( resetTime, "System.Single", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "WaterBody" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( meshFilter, "UnityEngine.MeshFilter" ); // <MeshFilter>k__BackingField
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "WaterSystem_Static", get_inner_static_class( DUMPER_CLASS( "WaterSystem" ) ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* ocean = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "WaterBody", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* ocean ) { return ocean != nullptr; } );
		DUMP_MEMBER_BY_X( Ocean, ocean->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "WaterSystem" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_Ocean, NO_FILT, DUMPER_CLASS( "WaterBody" ), 0, METHOD_ATTRIBUTE_PUBLIC, METHOD_ATTRIBUTE_STATIC );
	DUMPER_CLASS_END;

	unity::component_t* _terrain_texturing = nullptr;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TerrainMeta" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* position = il2cpp::get_static_field_if_value_is<unity::vector3_t>( dumper_klass, "UnityEngine.Vector3", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::vector3_t position ) { return position == unity::vector3_t( -2000.f, -500.f, -2000.f ); } );
		DUMP_MEMBER_BY_X( Position, position->offset() );

		il2cpp::field_info_t* size = il2cpp::get_static_field_if_value_is<unity::vector3_t>( dumper_klass, "UnityEngine.Vector3", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::vector3_t size ) { return size == unity::vector3_t( 4000.f, 1000.f, 4000.f ); } );
		DUMP_MEMBER_BY_X( Size, size->offset() );

		il2cpp::field_info_t* one_over_size = il2cpp::get_static_field_if_value_is<unity::vector3_t>( dumper_klass, "UnityEngine.Vector3", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::vector3_t one_over_size ) { return one_over_size.x == ( 1.f / ( float )WORLD_SIZE ); } );
		DUMP_MEMBER_BY_X( OneOverSize, one_over_size->offset() );

		il2cpp::field_info_t* terrain_collision = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "TerrainCollision", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::component_t* terrain_collision ) { return terrain_collision != nullptr; } );
		DUMP_MEMBER_BY_X( Collision, terrain_collision->offset() );

		il2cpp::field_info_t* terrain_height_map = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "TerrainHeightMap", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::component_t* terrain_height_map ) { return terrain_height_map != nullptr; } );
		DUMP_MEMBER_BY_X( HeightMap, terrain_height_map->offset() );

		il2cpp::field_info_t* splat_map = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "TerrainSplatMap", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::component_t* terrain_splat_map ) { return terrain_splat_map != nullptr; } );
		DUMP_MEMBER_BY_X( SplatMap, splat_map->offset() );

		il2cpp::field_info_t* terrain_topology_map = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "TerrainTopologyMap", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, []( unity::component_t* terrain_topology_map ) { return terrain_topology_map != nullptr; } );
		DUMP_MEMBER_BY_X( TopologyMap, terrain_topology_map->offset() );

		il2cpp::field_info_t* terrain_texturing = il2cpp::get_static_field_if_value_is<unity::component_t*>( dumper_klass, "TerrainTexturing", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE, [&_terrain_texturing]( unity::component_t* terrain_texturing ) { if ( terrain_texturing != nullptr ) { _terrain_texturing = terrain_texturing;  return terrain_texturing != nullptr; } } );
		DUMP_MEMBER_BY_X( Texturing, terrain_texturing->offset() );
	DUMPER_CLASS_END;

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
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TerrainHeightMap" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( normY, "System.Single" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TerrainSplatMap" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( num, "System.Int32" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TerrainTexturing" );
	DUMPER_SECTION( "Offsets" );
		if ( _terrain_texturing != nullptr ) {
			std::vector<il2cpp::field_info_t*> ints = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
			std::vector<il2cpp::field_info_t*> floats = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );

			for ( il2cpp::field_info_t* _int : ints ) {
				int value = *( int* )( _terrain_texturing + _int->offset() );

				if ( value > 100 ) {
					DUMP_MEMBER_BY_X( shoreMapSize, _int->offset() );
				}
			}

			for ( il2cpp::field_info_t* flt : floats ) {
				float value = *( float* )( _terrain_texturing + flt->offset() );

				if ( value == WORLD_SIZE ) {
					DUMP_MEMBER_BY_X( terrainSize, flt->offset() );
				}

				else {
					DUMP_MEMBER_BY_X( shoreDistanceScale, flt->offset() );
				}
			}
		}

		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( shoreVectors, "float4" );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* world_class_static_class = il2cpp::search_for_class_by_field_types( world_serialization_class->type(), 0, FIELD_ATTRIBUTE_PUBLIC, FIELD_ATTRIBUTE_STATIC );

	DUMPER_CLASS_BEGIN_FROM_PTR( "World_Static", world_class_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* size = il2cpp::get_static_field_if_value_is<uint32_t>( dumper_klass, "System.UInt32", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( uint32_t size ) { return size == WORLD_SIZE; } );
		DUMP_MEMBER_BY_X( _size, size->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemIcon" );
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
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* item_icon_static_class = get_inner_static_class( DUMPER_CLASS( "ItemIcon" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "ItemIcon_Static", item_icon_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" )
		il2cpp::field_info_t* container_loot_start_times = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "Dictionary", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* dictionary ) { return dictionary != nullptr; } );
		DUMP_MEMBER_BY_X( containerLootStartTimes, container_loot_start_times->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Effect", effect_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( pooledString, DUMPER_CLASS_NAMESPACE( "System", "String" ) );

		il2cpp::method_info_t* effect_ctor_6451102976 = il2cpp::get_method_by_name( effect_class, ".ctor", 4 );

		if ( effect_ctor_6451102976 ) {
			void( *effect_ctor )( uint8_t*, system_c::string_t, unity::vector3_t, unity::vector3_t, void* ) = ( decltype( effect_ctor ) )effect_ctor_6451102976->get_fn_ptr<void*>();

			if ( effect_ctor ) {
				uint8_t effect[ 512 ]{};

				effect_ctor( effect, L"", unity::vector3_t( 69.f, 69.f, 69.f ), unity::vector3_t(), nullptr );

				std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );

				for ( il2cpp::field_info_t* vector : vectors ) {
					unity::vector3_t value = *( unity::vector3_t* )( effect + vector->offset() );

					if ( value == unity::vector3_t( 69.f, 69.f, 69.f ) ) {
						DUMP_MEMBER_BY_X( worldPos, vector->offset() );
					}
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "EffectNetwork", effect_network_class );
	DUMPER_SECTION( "Functions" );

	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "EffectNetwork_Static", effect_network_static_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* _effect = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, effect_class->name(), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE, []( void* effect ) { return effect != nullptr; } );
		DUMP_MEMBER_BY_X( effect, _effect->offset() );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* _cctor = il2cpp::get_method_by_name( dumper_klass, ".cctor" );
		DUMP_METHOD_BY_INFO_PTR( cctor, _cctor );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BuildingBlock" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( grade );
	DUMPER_SECTION( "Functions" );

	if ( uint64_t get_build_menu = building_block_get_build_menu.method->get_fn_ptr<uint64_t>() ) {
		DUMP_MEMBER_BY_X( GetBuildMenu, DUMPER_RVA( get_build_menu ) );

		il2cpp::method_info_t* building_block_has_upgrade_privilege = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( get_build_menu, 3 ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE( "BuildingGrade/Enum" ),
			DUMPER_TYPE_NAMESPACE( "System", "UInt64" ),
			DUMPER_TYPE( "BasePlayer" )
		);

		DUMP_METHOD_BY_INFO_PTR( HasUpgradePrivilege, building_block_has_upgrade_privilege );

		il2cpp::method_info_t* building_block_can_afford_upgrade = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_I( get_build_menu, building_block_has_upgrade_privilege->get_fn_ptr<uint64_t>(), 3 ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE( "BuildingGrade/Enum" ),
			DUMPER_TYPE_NAMESPACE( "System", "UInt64" ),
			DUMPER_TYPE( "BasePlayer" )
		);

		DUMP_METHOD_BY_INFO_PTR( CanAffordUpgrade, building_block_can_afford_upgrade );
	}

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
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* base_screen_shake_static_class = get_inner_static_class( DUMPER_CLASS( "BaseScreenShake" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "BaseScreenShake_Static", base_screen_shake_static_class );
	DUMPER_SECTION( "Offsets" )
		il2cpp::field_info_t* instances_list = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "BaseScreenShake", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instances_list ) { return instances_list != nullptr; } );
		DUMP_MEMBER_BY_X( list, instances_list->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "FlashbangOverlay" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* instance = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "FlashbangOverlay", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instance ) { return instance != nullptr; } );
		DUMP_MEMBER_BY_X( Instance, instance->offset() );

		il2cpp::field_info_t* flash_length = il2cpp::get_field_if_name_contains( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), "%", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( flashLength, flash_length->offset() );
	DUMPER_CLASS_END;

	const char* string_pool_field_types[] = {
		"System.Collections.Generic.Dictionary<System.UInt32,System.String>",
		"System.Collections.Generic.Dictionary<System.String,System.UInt32>",
	};

	il2cpp::il2cpp_class_t* string_pool_class = il2cpp::search_for_class_containing_field_types_str( string_pool_field_types, _countof( string_pool_field_types ) );
	
	if ( string_pool_class ) {
		DUMPER_CLASS_BEGIN_FROM_PTR( "StringPool", string_pool_class );
		DUMPER_SECTION( "Offsets" );
			il2cpp::field_info_t* to_number = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, string_pool_field_types[ 1 ], DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE, []( void* to_number ) { return to_number != nullptr; } );
			DUMP_MEMBER_BY_X( toNumber, to_number->offset() );
		DUMPER_SECTION( "Functions" );
			il2cpp::method_info_t* string_pool_get = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
				FILT( DUMPER_METHOD( DUMPER_CLASS( "BaseEntity" ), "RequestFileFromServer" ) ),
				DUMPER_TYPE_NAMESPACE( "System", "UInt32" ),
				METHOD_ATTRIBUTE_PUBLIC,
				METHOD_ATTRIBUTE_STATIC,
				DUMPER_TYPE_NAMESPACE( "System", "String" )
			);

			DUMP_METHOD_BY_INFO_PTR( Get, string_pool_get );
		DUMPER_CLASS_END;
	}

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_Networkable", network_networkable_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( ID, networkable_id_class );
	DUMPER_CLASS_END;

	il2cpp::field_info_t* _cl = nullptr;
	uint64_t on_network_message = 0;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_Net", network_net_class );
	DUMPER_SECTION( "Offsets" );
		 _cl = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, network_client_class->name(), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE, []( void* cl ) { return cl != nullptr; } );
		DUMP_MEMBER_BY_X( cl, _cl->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_Client", network_client_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( Connection, network_connection_class );

		uint64_t client = _cl->static_get_value<uint64_t>();

		if ( client ) {
			il2cpp::field_info_t* callback_handler_field = il2cpp::get_field_if_type_contains( dumper_klass, "%", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, TYPE_ATTRIBUTE_INTERFACE );

			if ( callback_handler_field ) {
				uint64_t callback_handler = *( uint64_t* )( client + callback_handler_field->offset() );

				if ( is_valid_ptr( callback_handler ) ) {
					uint64_t klass = *( uint64_t* )( callback_handler );

					if ( is_valid_ptr( klass ) ) {
						on_network_message = *( uint64_t* )( klass + 0x1C8 );
					}
				}
			}

			std::vector<il2cpp::field_info_t*> ints = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Int32" ), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );
			std::vector<il2cpp::field_info_t*> strings = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "String" ), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );

			for ( il2cpp::field_info_t* _int : ints ) {
				int value = *( int* )( client + _int->offset() );

				if ( value == SERVER_PORT ) {
					DUMP_MEMBER_BY_X( ConnectedPort, _int->offset() );
				}
			}

			for ( il2cpp::field_info_t* string : strings ) {
				system_c::string_t* value = *( system_c::string_t** )( client + string->offset() );

				if ( value ) {
					if ( !wcscmp( value->str, SERVER_IP ) ) {
						DUMP_MEMBER_BY_X( ConnectedAddress, string->offset() );
					}

					else {
						DUMP_MEMBER_BY_X( ServerName, string->offset() );
					}
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_BaseNetwork", network_base_network_class );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( StartWrite, network_base_network_start_write );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_SendInfo", network_send_info_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( method, network_send_method_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( channel, DUMPER_CLASS_NAMESPACE( "System", "SByte" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( priority, network_priority_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( connections, network_connections_list_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( connection, network_connection_class );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_Message", network_message_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( type, ".Type" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( read, network_netread_class );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_NetRead", network_netread_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( stream, buffer_stream_class );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Network_NetWrite", network_netwrite_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( stream, buffer_stream_class );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* network_netwrite_uint32 = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BaseEntity" ), "ServerRPC" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "UInt32" )
		);

		il2cpp::method_info_t* network_netwrite_string = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS_NAMESPACE( "Rust.UI.ServerAdmin", "ServerAdminUI" ), "RefreshUGC" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "String" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);

		DUMP_METHOD_BY_INFO_PTR( String, network_netwrite_string );

		il2cpp::method_info_t* network_netwrite_send = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BaseEntity" ), "ServerRPC" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			network_send_info_class->type()
		);

		DUMP_METHOD_BY_INFO_PTR( Send, network_netwrite_send );
	DUMPER_CLASS_END;

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
		DUMPER_CLASS_END;
	}

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIInventory" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( Close,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "UIInventory" ), "Update" ) ),
			DUMPER_CLASS_NAMESPACE( "System", "Void" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC
		);
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "GrowableEntity" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( Properties );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( State, "PlantProperties.State" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlantProperties" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( stages );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlantProperties/Stage" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( resources );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Text", "UnityEngine.UI" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( m_Text );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_Sky" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( Cycle );
		DUMP_MEMBER_BY_NAME( Atmosphere );
		DUMP_MEMBER_BY_NAME( Day );
		DUMP_MEMBER_BY_NAME( Night );
		DUMP_MEMBER_BY_NAME( Stars );
		DUMP_MEMBER_BY_NAME( Clouds );
		DUMP_MEMBER_BY_NAME( Ambient );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_Instance,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "TOD_LightAtNight" ), "Update" ) ),
			dumper_klass,
			0,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_ATTR_DONT_CARE
		);
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* tod_sky_static_class = get_inner_static_class( DUMPER_CLASS( "TOD_Sky" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "TOD_Sky_Static", tod_sky_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* _instances = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "List<TOD_Sky>", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* list ) { return list != nullptr; } );
		DUMP_MEMBER_BY_X( instances, _instances->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_CycleParameters" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( get_DateTime,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "EnvSync" ), "Update" ) ),
			DUMPER_CLASS_NAMESPACE( "System", "DateTime" ),
			0,
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE
		);
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_AtmosphereParameters" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( RayleighMultiplier );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_DayParameters" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( SkyColor );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_NightParameters" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( MoonColor );
		DUMP_MEMBER_BY_NAME( CloudColor );
		DUMP_MEMBER_BY_NAME( AmbientColor );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_StarParameters" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( Size );
		DUMP_MEMBER_BY_NAME( Brightness );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_CloudParameters" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( Brightness );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TOD_AmbientParameters" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( Saturation );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIHUD" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( Hunger );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "HudElement" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( lastValue );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIBelt" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( ItemIcons );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemModCompostable" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( MaxBaitStack );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* resource_ref = DUMPER_CLASS( "GameObjectRef" )->parent();

	DUMPER_CLASS_BEGIN_FROM_PTR( "GameObjectRef", resource_ref );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( guid );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "EnvironmentManager" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_STR( Get,
			FILT( DUMPER_METHOD( DUMPER_CLASS( "BiomeVisuals" ), "Start" ) ),
			"EnvironmentType",
			2
		);
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Translate_Phrase", translate_phrase_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( english );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ResourceDispenser/GatherPropertyEntry" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( gatherDamage );
		DUMP_MEMBER_BY_NAME( destroyFraction );
		DUMP_MEMBER_BY_NAME( conditionLost );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ResourceDispenser/GatherProperties" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( Tree );
		DUMP_MEMBER_BY_NAME( Ore );
		DUMP_MEMBER_BY_NAME( Flesh );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIChat" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( chatArea );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ListComponent", list_component_ui_chat_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* instance_list = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "UIChat", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instance_list ) { return instance_list != nullptr; } );
		DUMP_MEMBER_BY_X( InstanceList, instance_list->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ListHashSet", list_hash_set_ui_chat_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( vals, "<UIChat>" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PatrolHelicopter" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( mainRotor );
		DUMP_MEMBER_BY_NAME( weakspots );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Chainsaw" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( ammo );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* camera_update_hook_static_class = get_inner_static_class( DUMPER_CLASS( "CameraUpdateHook" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "CameraUpdateHook_Static", camera_update_hook_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* _action = il2cpp::get_static_field_if_value_is<uint64_t>( dumper_klass, "System.Action", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( uint64_t action ) {
			if ( !action )
				return false;

			il2cpp::method_info_t* method = *( il2cpp::method_info_t** )( action + 0x28 );
			if ( !method )
				return false;

			if ( strcmp( method->name(), "<AddStopwatch>b__1" ) )
				return false;

			return true;
		} );

		DUMP_MEMBER_BY_X( action, _action->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SteamClientWrapper" );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* steam_client_wrapper_get_avatar_texture = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "UIVoiceIcon" ), "Setup" ) ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Texture" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "UInt64" ),
		);

		DUMP_METHOD_BY_INFO_PTR( GetAvatarTexture, steam_client_wrapper_get_avatar_texture );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "AimConeUtil", aimcone_util_get_modified_aimcone_direction->klass() );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( GetModifiedAimConeDirection, aimcone_util_get_modified_aimcone_direction );
	DUMPER_CLASS_END;
	
	DUMPER_CLASS_BEGIN_FROM_PTR( "Buttons_ConButton", buttons_conbutton_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( IsDown, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Buttons_Static", buttons_static_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_X( Pets, get_button_offset( L"buttons.pets" ) );
		DUMP_MEMBER_BY_X( Attack, get_button_offset( L"buttons.attack" ) );
		DUMP_MEMBER_BY_X( Attack2, get_button_offset( L"buttons.attack2" ) );
		DUMP_MEMBER_BY_X( Forward, get_button_offset( L"buttons.forward" ) );
		DUMP_MEMBER_BY_X( Backward, get_button_offset( L"buttons.backward" ) );
		DUMP_MEMBER_BY_X( Right, get_button_offset( L"buttons.right" ) );
		DUMP_MEMBER_BY_X( Left, get_button_offset( L"buttons.left" ) );
		DUMP_MEMBER_BY_X( Sprint, get_button_offset( L"buttons.sprint" ) );
	DUMPER_SECTION( "Functions" );
		DUMP_MEMBER_BY_X( Pets_setter, DUMPER_RVA( buttons_pets_command->set() ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerModel" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( _multiMesh, DUMPER_CLASS( "SkinnedMultiMesh" ) );

		if ( local_player_get_entity && player_model_offset != -1 ) {
			unity::component_t* local_player = ( unity::component_t* )local_player_get_entity();

			if ( local_player ) {
				uint64_t player_model = *( uint64_t* )( local_player + player_model_offset );

				if ( player_model ) {
					void( *player_model_update_local_velocity )( uint64_t, unity::vector3_t, unity::transform_t* ) =
						( decltype( player_model_update_local_velocity ) )DUMPER_METHOD( dumper_klass, "UpdateLocalVelocity" );

					if ( player_model_update_local_velocity ) {
						player_model_update_local_velocity( player_model, unity::vector3_t( 69.f, 69.f, 69.f ), nullptr );
					}

					std::vector<il2cpp::field_info_t*> vectors = il2cpp::get_fields_of_type( dumper_klass, DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), DUMPER_ATTR_DONT_CARE, DUMPER_ATTR_DONT_CARE );

					for ( il2cpp::field_info_t* vector : vectors ) {
						unity::vector3_t value = *( unity::vector3_t* )( player_model + vector->offset() );

						if ( abs( value.distance( local_player->get_transform()->get_position() ) < 1.f ) ) {
							DUMP_MEMBER_BY_X( position, vector->offset() );
						}

						else if ( value == unity::vector3_t( 69.f, 69.f, 69.f ) ) {
							DUMP_MEMBER_BY_X( newVelocity, vector->offset() );
						}
					}
				}
			}
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SkinnedMultiMesh" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( Renderers, "List<UnityEngine.Renderer>" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseMountable" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( pitchClamp );
		DUMP_MEMBER_BY_NAME( yawClamp );
		DUMP_MEMBER_BY_NAME( canWieldItems );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ProgressBar" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* instance = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "ProgressBar", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instance ) { return instance != nullptr; } );
		DUMP_MEMBER_BY_X( Instance, instance->offset() );

		il2cpp::field_info_t* time_counter = il2cpp::get_field_if_name_contains( dumper_klass, DUMPER_TYPE_NAMESPACE( "System", "Single" ), "%", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_X( timeCounter, time_counter->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BowWeapon" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( attackReady, "System.Boolean", FIELD_ATTRIBUTE_FAMILY, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( wasAiming, "System.Boolean", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "CrossbowWeapon" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "MiniCrossbow" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConVar_Admin_Static", convar_admin_static_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* admin_time = il2cpp::get_static_field_if_value_is<uint32_t>( dumper_klass, "<System.Single>", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( uint32_t value ) { return value != 0; } );
		DUMP_MEMBER_BY_X( admintime, admin_time->offset() );
	DUMPER_SECTION( "Functions" );
		rust::console_system::command* admintime_command = rust::console_system::client::find( system_c::string_t::create_string( L"global.admintime" ) );

		if ( admintime_command ) {
			DUMP_MEMBER_BY_X( admintime_getter, DUMPER_RVA( admintime_command->get() ) );
			DUMP_MEMBER_BY_X( admintime_setter, DUMPER_RVA( admintime_command->set() ) );
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConVar_Player_Static", convar_player_static_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* client_tick_interval = il2cpp::get_static_field_if_value_is<uint32_t>( dumper_klass, "<System.Single>", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( uint32_t value ) { return value != 0; } );
		DUMP_MEMBER_BY_X( clientTickInterval, client_tick_interval->offset() );

	DUMPER_SECTION( "Functions" );
		rust::console_system::command* tickrate_cl_command = rust::console_system::client::find( system_c::string_t::create_string( L"player.tickrate_cl" ) );

		if ( tickrate_cl_command ) {
			DUMP_MEMBER_BY_X( clientTickRate_getter, DUMPER_RVA( tickrate_cl_command->get() ) );
			DUMP_MEMBER_BY_X( clientTickRate_setter, DUMPER_RVA( tickrate_cl_command->set() ) );
		}
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ColliderInfo" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( flags );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "CodeLock" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( hasCode, "System.Boolean", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_NEAR_OFFSET( hasAuth, DUMPER_OFFSET( hasCode ) + 0x1 );
		DUMP_MEMBER_BY_NEAR_OFFSET( hasGuestAuth, DUMPER_OFFSET( hasCode ) + 0x2 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "AutoTurret" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( authorizedPlayers, "HashSet<%" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( lastYaw, DUMPER_CLASS_NAMESPACE( "UnityEngine", "Quaternion" ) );
		DUMP_MEMBER_BY_NAME( muzzlePos );
		DUMP_MEMBER_BY_NAME( gun_yaw );
		DUMP_MEMBER_BY_NAME( gun_pitch );
		DUMP_MEMBER_BY_NAME( sightRange );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* auto_turret_is_authed = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "AutoTurret" ), "GetMenuOptions" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "UInt64" )
		);

		DUMP_METHOD_BY_INFO_PTR( IsAuthed, auto_turret_is_authed );
	DUMPER_CLASS_END;

	if ( on_network_message ) {
		uint64_t create_or_update_entity = 0;

		DUMPER_CLASS_BEGIN_FROM_NAME( "Client" );
		DUMPER_SECTION( "Functions" );
			il2cpp::method_info_t* method = il2cpp::method_info_t::from_addr( on_network_message );
			il2cpp::virtual_method_t virtual_method( method, method->get_vtable_offset() );
			DUMP_VIRTUAL_METHOD( OnNetworkMessage, virtual_method );

			il2cpp::method_info_t* client_create_or_update_entity = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
				FILT_N( on_network_message, 3 ),
				DUMPER_TYPE( "BaseEntity" ),
				METHOD_ATTRIBUTE_PRIVATE,
				DUMPER_ATTR_DONT_CARE,
				protobuf_entity_class->type(),
				DUMPER_TYPE_NAMESPACE( "System", "Int64" )
			); create_or_update_entity = client_create_or_update_entity->get_fn_ptr<uint64_t>();

			DUMP_METHOD_BY_INFO_PTR( CreateOrUpdateEntity, client_create_or_update_entity );
		DUMPER_CLASS_END;

		if ( create_or_update_entity ) {
			DUMPER_CLASS_BEGIN_FROM_NAME( "BaseNetworkable" );
			DUMPER_SECTION( "Functions" );
				il2cpp::virtual_method_t base_networkable_load = SEARCH_FOR_VIRTUAL_METHOD_WITH_RETTYPE_PARAM_TYPES(
					FILT_I( create_or_update_entity, 1000, 1 ),
					DUMPER_TYPE_NAMESPACE( "System", "Void" ),
					DUMPER_ATTR_DONT_CARE,
					DUMPER_ATTR_DONT_CARE,
					base_networkable_load_info_class->type()
				); 

				DUMP_VIRTUAL_METHOD( Load, base_networkable_load );
			DUMPER_CLASS_END;
		}
	}

	DUMPER_CLASS_BEGIN_FROM_PTR( "ItemManager_Static", item_manager_static_class );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* item_list = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "List<ItemDefinition>", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* item_list ) { return item_list != nullptr; } );
		DUMP_MEMBER_BY_X( itemList, item_list->offset() );

		il2cpp::field_info_t* item_dictionary = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "Dictionary<System.Int32,ItemDefinition>", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* item_dictionary ) { return item_dictionary != nullptr; } );
		DUMP_MEMBER_BY_X( itemDictionary, item_dictionary->offset() );

		il2cpp::field_info_t* item_dictionary_by_name = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "Dictionary<System.String,ItemDefinition>", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* item_dictionary_by_name ) { return item_dictionary_by_name != nullptr; } );
		DUMP_MEMBER_BY_X( itemDictionaryByName, item_dictionary_by_name->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConVar_Server_Static", convar_server_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "ServerAdminUGCEntry", "Rust.UI.ServerAdmin" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( ReceivedDataFromServer );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "LoadingScreen" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( panel );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "MixerSnapshotManager" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( defaultSnapshot );
		DUMP_MEMBER_BY_NAME( loadingSnapshot );
	DUMPER_CLASS_END;

	DUMPER_PTR_CLASS_NAME( "MapView_Static", get_inner_static_class( DUMPER_CLASS( "MapView" ) ) );

	DUMPER_CLASS_BEGIN_FROM_NAME( "MapView" );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* map_view_world_pos_to_image_pos = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "MLRSMainUI" ), "CentreMap" ) ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector2" ),
			METHOD_ATTRIBUTE_PUBLIC,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
		);

		DUMP_METHOD_BY_INFO_PTR( WorldPosToImagePos, map_view_world_pos_to_image_pos );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "GamePhysics", game_physics_class )
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* game_physics_trace = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES_STR(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "CameraMan" ), "FocusOnTarget" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			"UnityEngine.Ray",
			"System.Single",
			"UnityEngine.RaycastHit&",
			"System.Single",
			"System.Int32",
			"UnityEngine.QueryTriggerInteraction",
			"BaseEntity"
		);
		DUMP_METHOD_BY_INFO_PTR( Trace, game_physics_trace );

		il2cpp::method_info_t* game_physics_line_of_sight_internal = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BasePlayer" ), "GetMenuOptions" ), 3 ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PRIVATE,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "System", "Int32" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_TYPE( "BaseEntity" )
		);

		DUMP_METHOD_BY_INFO_PTR( LineOfSightInternal, game_physics_line_of_sight_internal );

		il2cpp::method_info_t* game_physics_verify = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "SoundSource" ), "DoOcclusionCheck" ), 2 ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "RaycastHit" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE( "BaseEntity" )
		);

		DUMP_METHOD_BY_INFO_PTR( Verify, game_physics_verify );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "DDraw", "UnityEngine" );
	DUMPER_SECTION( "Functions" );
		il2cpp::method_info_t* unity_engine_ddraw_sphere = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "PoweredLightsDeployer" ), "OnInput" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Color" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);

		DUMP_METHOD_BY_INFO_PTR( Sphere, unity_engine_ddraw_sphere );

		il2cpp::method_info_t* unity_engine_ddraw_line = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT( DUMPER_METHOD( DUMPER_CLASS( "PoweredLightsDeployer" ), "OnInput" ) ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PUBLIC,
			METHOD_ATTRIBUTE_STATIC,
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
			DUMPER_TYPE_NAMESPACE( "UnityEngine", "Color" ),
			DUMPER_TYPE_NAMESPACE( "System", "Single" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
			DUMPER_TYPE_NAMESPACE( "System", "Boolean" )
		);

		DUMP_METHOD_BY_INFO_PTR( Line, unity_engine_ddraw_line );
	DUMPER_CLASS_END;

	il2cpp::method_info_t* raycast_hit_ex_get_entity = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS( "CameraMan" ), "FocusOnTarget" ) ),
		DUMPER_TYPE( "BaseEntity" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "RaycastHit" )
	);

	DUMPER_CLASS_BEGIN_FROM_PTR( "RaycastHitEx", raycast_hit_ex_get_entity->klass() );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( GetEntity, raycast_hit_ex_get_entity );
	DUMPER_CLASS_END;

	il2cpp::method_info_t* on_parent_destroying_ex_broadcast_on_parent_destroying = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS( "EffectRecycle" ), "Recycle" ) ),
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "GameObject" )
	);

	DUMPER_CLASS_BEGIN_FROM_PTR( "OnParentDestroyingEx", on_parent_destroying_ex_broadcast_on_parent_destroying->klass() );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( BroadcastOnParentDestroying, on_parent_destroying_ex_broadcast_on_parent_destroying );
	DUMPER_CLASS_END;

	il2cpp::method_info_t* console_network_client_run_on_server = SEARCH_FOR_METHOD_IN_METHOD_WITH_RETTYPE_PARAM_TYPES(
		WILDCARD_VALUE( il2cpp::il2cpp_class_t* ),
		FILT( DUMPER_METHOD( DUMPER_CLASS_NAMESPACE( "Rust.UI.ServerAdmin", "ServerAdminUI" ), "OnEnable" ) ),
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
		METHOD_ATTRIBUTE_PUBLIC,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "System", "String" )
	);

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConsoleNetwork", console_network_client_run_on_server->klass() );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_INFO_PTR( ClientRunOnServer, console_network_client_run_on_server );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ThrownWeapon" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( maxThrowVelocity );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "MapInterface" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( scrollRectZoom );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ScrollRectZoom" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( zoom );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "MapView" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( scrollRect );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "StorageContainer" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( inventorySlots );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerCorpse" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( clientClothing, item_container_class );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "TimedExplosive" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( explosionRadius );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "SmokeGrenade_Static", get_inner_static_class( DUMPER_CLASS( "SmokeGrenade" ) ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* active_grenades = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "SmokeGrenade", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* active_grenades ) { return active_grenades != nullptr; } );
		DUMP_MEMBER_BY_X( activeGrenades, active_grenades->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SmokeGrenade" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( smokeEffectInstance, DUMPER_CLASS_NAMESPACE( "UnityEngine", "GameObject" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "GrenadeWeapon" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( drop, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ViewmodelLower" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( lowerOnSprint );
		DUMP_MEMBER_BY_NAME( lowerWhenCantAttack );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( shouldLower, "System.Boolean", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( rotateAngle, "System.Single", FIELD_ATTRIBUTE_ASSEMBLY, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "ConVar_Client_Static", convar_client_static_class );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* _camlerp = il2cpp::get_static_field_if_value_is<float>( dumper_klass, "System.Single", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( float camlerp ) { return FLOAT_IS_EQUAL( camlerp, CAMLERP, 0.001f ); } );
		DUMP_MEMBER_BY_X( camlerp, _camlerp->offset() );

		il2cpp::field_info_t* _camspeed = il2cpp::get_static_field_if_value_is<float>( dumper_klass, "System.Single", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( float camspeed ) { return FLOAT_IS_EQUAL( camspeed, CAMSPEED, 0.001f ); } );
		DUMP_MEMBER_BY_X( camspeed, _camspeed->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SamSite" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( staticRespawn );
		DUMP_MEMBER_BY_NAME( Flag_TargetMode );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ServerProjectile" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( drag );
		DUMP_MEMBER_BY_NAME( gravityModifier );
		DUMP_MEMBER_BY_NAME( speed );
		DUMP_MEMBER_BY_NAME( radius );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIFogOverlay" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( group );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "FoliageGrid" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( CellSize );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemModWearable" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( movementProperties );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ClothingMovementProperties" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( speedReduction );
		DUMP_MEMBER_BY_NAME( minSpeedReduction );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "GestureConfig" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( actionType );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "RCMenu" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( autoTurretFogDistance );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "Facepunch_Network_Raknet_Client", facepunch_network_raknet_client_class );
	DUMPER_SECTION( "Functions" );
		DUMP_VIRTUAL_METHOD( IsConnected, facepunch_network_raknet_client_is_connected );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "EncryptedValue", encrypted_value_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( _value, DUMPER_CLASS_NAMESPACE( "System", "Single" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( _padding, DUMPER_CLASS_NAMESPACE( "System", "Int32" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "HiddenValue", hidden_value_class );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( _handle, "GCHandle" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( _accessCount, DUMPER_CLASS_NAMESPACE( "System", "Int32" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( _hasValue, DUMPER_CLASS_NAMESPACE( "System", "Boolean" ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "SimplePrivilege/<>c__DisplayClass3_0" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME_STR( IsAuthed, "<IsAuthed>b__0" );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemModRFListener" );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_NAME( ConfigureClicked );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "UIFogOverlay" );
	DUMPER_SECTION( "Offsets" );
		il2cpp::field_info_t* instance = il2cpp::get_static_field_if_value_is<void*>( dumper_klass, "UIFogOverlay", FIELD_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE, []( void* instance ) { return instance != nullptr; } );
		DUMP_MEMBER_BY_X( Instance, instance->offset() );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_PTR( "BufferStream", buffer_stream_class );
		il2cpp::method_info_t* buffer_stream_ensure_capacity = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
			FILT_N( DUMPER_METHOD( DUMPER_CLASS( "BaseEntity" ), "ServerRPC" ), 3 ),
			DUMPER_TYPE_NAMESPACE( "System", "Void" ),
			METHOD_ATTRIBUTE_PRIVATE,
			DUMPER_ATTR_DONT_CARE,
			DUMPER_TYPE_NAMESPACE( "System", "Int32" )
		);

		DUMP_METHOD_BY_INFO_PTR( EnsureCapacity, buffer_stream_ensure_capacity );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "FreeableLootContainer" );
	DUMPER_SECTION( "Il2Cpp" );
		DUMP_MEMBER_BY_X( TypeDefinitionIndex, get_class_type_definition_index( dumper_klass ) );
	DUMPER_CLASS_END;

	fclose( outfile_handle );
	fclose( outfile_log_handle );	
}