#include "dumper.hpp"
#include "hde/hde64.h"

// il2cpp::global_metadata_header_t* il2cpp::s_GlobalMetadata = nullptr;

il2cpp::il2cpp_class_t** il2cpp::s_TypeInfoDefinitionTable = nullptr;

#define SEARCH_FOR_CLASS_BY_FIELD_COUNT( field_ct, equality, ... ) \
[=]( ) -> il2cpp::il2cpp_class_t* \
{ \
	il2cpp::il2cpp_class_t* match_klasses[ ] = { __VA_ARGS__ }; \
	return il2cpp::search_for_class_by_field_count( match_klasses, _countof( match_klasses ), field_ct, equality ); \
} ( ) \

#define SEARCH_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( ... ) \
[=]( ) -> il2cpp::field_info_t* \
{ \
	const char* search_terms[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_field_if_type_contains_terms( dumper_klass, search_terms, _countof( search_terms ) ); \
} ( )

#define SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES( ret_type, wanted_vis, wanted_flags, ... ) \
[=]( ) -> il2cpp::method_info_t* \
{ \
	il2cpp::il2cpp_type_t* param_types[ ] = { __VA_ARGS__ }; \
	return il2cpp::get_method_by_return_type_and_param_types( dumper_klass, ret_type, wanted_vis, wanted_flags, param_types, _countof( param_types ) ); \
} ( )

#define DUMPER_VIS_DONT_CARE 0 
#define DUMPER_ATTR_DONT_CARE 0

#define DUMPER_RVA( X ) X - dumper::game_base
#define DUMPER_CLASS( name ) il2cpp::get_class_by_name( name )
#define DUMPER_CLASS_NAMESPACE( namespaze, name ) il2cpp::get_class_by_name( name, namespaze )

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

#define DUMPER_CLASS_END dumper::write_to_file( "}\n\n" ); } 

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

#define DUMP_METHOD_BY_RETURN_TYPE_ATTRS( NAME, ret_type, param_ct, wanted_vis, wanted_attrs ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_return_type_attrs( dumper_klass, ret_type, wanted_attrs, wanted_vis, param_ct )->get_fn_ptr<uint64_t>() ) )
#define DUMP_METHOD_BY_SIG_REL( NAME, base, sig, off ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( uint64_t( dumper::relative_32( FIND_PATTERN( base, 0x1000, sig ), off ) ) ) )
#define DUMP_METHOD_BY_INFO_PTR( NAME, ptr ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( ptr->get_fn_ptr<uint64_t>( ) ) )
#define DUMP_METHOD_BY_PARAM_CLASS( NAME, param_class, param_ct, wanted_vis, wanted_flags ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_param_class( dumper_klass, param_class, param_ct, wanted_vis, wanted_flags )->get_fn_ptr<uint64_t>( ) ) )

#define DUMP_METHOD_BY_RETURN_TYPE_METHOD_ATTRIBUTE(NAME, ret_type, method_attr, param_ct, wanted_vis, wanted_attrs, want_or_ignore ) DUMP_MEMBER_BY_X( NAME, DUMPER_RVA( il2cpp::get_method_by_return_type_attrs_method_attr( dumper_klass, ret_type, method_attr, wanted_attrs, wanted_vis, param_ct, want_or_ignore )->get_fn_ptr<uint64_t>() ) )


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

#define DUMP_ENCRYPTED_MEMBER_GETTER_AND_SETTER( NAME, off ) \
	{ il2cpp::il2cpp_type_t* enc_type = il2cpp::get_field_by_offset( dumper_klass, off )->type(); \
	il2cpp::method_info_t* enc_getter = il2cpp::get_method_by_return_type_str( enc_type->klass( ), GetInnerClassFromEncClass( enc_type->name( ) ), 0 ); \
	il2cpp::method_info_t* enc_setter = il2cpp::get_method_by_return_type_str( enc_type->klass( ), "System.Void", 1 ); \
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_EncryptedValueGetter = 0x%x;\n",  DUMPER_RVA( enc_getter->get_fn_ptr<uint64_t>() ) );\
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_EncryptedValueSetter = 0x%x;\n",  DUMPER_RVA( enc_setter->get_fn_ptr<uint64_t>() ) ); }

#define DUMP_HIDDEN_MEMBER_KEY_GETTER_AND_SETTER( NAME, off ) \
	{ il2cpp::il2cpp_type_t* enc_type = il2cpp::get_field_by_offset( dumper_klass, off )->type(); \
	il2cpp::method_info_t* enc_getter = il2cpp::get_method_by_return_type_str( enc_type->klass( ), GetInnerClassFromEncClass( enc_type->name( ) ), 0 ); \
	il2cpp::method_info_t* enc_setter = il2cpp::get_method_by_return_type_str( enc_type->klass( ), "System.Void", 1 ); \
	il2cpp::field_info_t* key = il2cpp::get_field_if_type_contains(enc_type->klass( ), "System.UInt64", DUMPER_VIS_DONT_CARE, DUMPER_ATTR_DONT_CARE );\
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_HiddenValueKeyPos = 0x%x;\n", key->offset() );\
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_HiddenValueGetter = 0x%x;\n",  DUMPER_RVA( enc_getter->get_fn_ptr<uint64_t>() ) );\
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_HiddenValueSetter = 0x%x;\n",  DUMPER_RVA( enc_setter->get_fn_ptr<uint64_t>() ) ); }

#define DUMPER_OFFSET( NAME ) NAME##_Offset


FILE* dumper::outfile_handle = nullptr;
uint64_t dumper::game_base = 0;

void dumper::write_to_file( const char* foramt, ... )
{
	char buffer[ 1024 ] = { 0 };
	memset( buffer, 0, sizeof( buffer ) );

	va_list args;
	va_start( args, foramt );
	vsprintf_s( buffer, foramt, args );
	va_end( args );

	fwrite( buffer, strlen( buffer ), 1, outfile_handle );
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

void write_pe_checksum( uint8_t* image ) {
	PIMAGE_DOS_HEADER dos_header = ( PIMAGE_DOS_HEADER ) ( image );
	PIMAGE_NT_HEADERS nt_headers = ( PIMAGE_NT_HEADERS ) ( image + dos_header->e_lfanew );

	dumper::write_to_file( "namespace GameAssembly {\n" );
	dumper::write_to_file( "\tconstexpr const static size_t Checksum = 0x%x;\n", dos_header->e_csum );
	dumper::write_to_file( "}\n\n" );
}

size_t get_fn_length( void* address ) {
	size_t len = 0;

	while ( true ) {
		uint8_t* inst = ( uint8_t* ) address + len;
		if ( *inst == 0 || *inst == 0xCC )
			break;

		hde64s hs;
		uint32_t inst_len = hde64_disasm( inst, &hs );
		if ( hs.flags & F_ERROR )
			break;

		if ( hs.opcode == 0xC3 )
			return len;

		len += inst_len;
	}

	return len;
}

void dumper::produce( )
{
	game_base = ( uint64_t ) ( GetModuleHandleA( "GameAssembly.dll" ) );
	outfile_handle = fopen( "C:\\dumps\\output.h", "w" );
	if ( !outfile_handle )
		return;

	dumper::write_to_file( "#include <cstdint>\n\n" );

	write_pe_checksum( ( uint8_t* ) game_base );

	il2cpp::s_TypeInfoDefinitionTable = *( il2cpp::il2cpp_class_t*** ) ( game_base + 0x3E1D058 );

	il2cpp::il2cpp_class_t* entity_ref_class = SEARCH_FOR_CLASS_BY_FIELD_COUNT( 2, 0, DUMPER_CLASS( "BaseEntity" ), DUMPER_CLASS( "NetworkableId" ) );
	il2cpp::il2cpp_class_t* hidden_value_base_class = nullptr;

	/*printf( "get world velocity: %d\n", get_fn_length( ( void* ) ( game_base + 0x62C380 ) ) );
	printf( "get local velocity: %d\n", get_fn_length( ( void* ) ( game_base + 0x627920 ) ) );
	printf( "other: %d\n", get_fn_length( ( void* ) ( game_base + 0x0649A20 ) ) );*/

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseNetworkable" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( prefabID ); // Dump a member by it's name.
	DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( net, "Network.Networkable", DUMPER_VIS_DONT_CARE, DUMPER_ATTR_DONT_CARE );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS( parentEntity, entity_ref_class ); // Search for EntityRef class.
	DUMP_MEMBER_BY_NEAR_OFFSET( children, DUMPER_OFFSET( parentEntity ) + 0x10 ); // Dump a member by an offset from another member of the class.

	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( ClientEntities, "BaseNetworkable" );
	DUMP_HIDDEN_MEMBER_KEY_GETTER_AND_SETTER( ClientEntities, DUMPER_OFFSET( ClientEntities ) );

	hidden_value_base_class = il2cpp::get_field_by_offset( dumper_klass, DUMPER_OFFSET( ClientEntities ) )->type( )->klass( )->parent( );
	DUMPER_CLASS_END;

	if ( hidden_value_base_class ) {
		DUMPER_CLASS_BEGIN_FROM_PTR( "HiddenValueBase", hidden_value_base_class );
		DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( _values, "Dictionary" );
		DUMPER_CLASS_END;
	}

	DUMPER_CLASS_BEGIN_FROM_NAME( "Model" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( boneTransforms );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseEntity" );
	printf( "BaseEntity typedef index: %d\n", il2cpp::get_typedef_idx_for_class( dumper_klass ) );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( flags );
	DUMP_MEMBER_BY_NAME( model );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseCombatEntity" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( lifestate );
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
	DUMP_MEMBER_BY_NAME( newRecoilOverride );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Magazine", "ProtoBuf" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( ammoType );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "AttackEntity" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( repeatDelay );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseProjectile" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( automatic );
	DUMP_MEMBER_BY_NAME( primaryMagazine );
	DUMP_MEMBER_BY_NAME( aimSway );
	DUMP_MEMBER_BY_NAME( recoil );
	DUMP_MEMBER_BY_NAME( aimCone );
	// timeSinceReloadFinished is used to get the 4 floats near it.
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS( timeSinceReloadFinished, DUMPER_CLASS( "TimeSince" ) );
	DUMP_MEMBER_BY_NEAR_OFFSET( hipAimConeOffset, DUMPER_OFFSET( timeSinceReloadFinished ) - 0x8 ); // Ideally if they stay together you would just update this one and the rest should be fine.
	DUMP_MEMBER_BY_NEAR_OFFSET( hipAimConeScale, DUMPER_OFFSET( hipAimConeOffset ) - 0x4 );
	DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeOffset, DUMPER_OFFSET( hipAimConeScale ) - 0x4 );
	DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeScale, DUMPER_OFFSET( sightAimConeOffset ) - 0x4 );

	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( createdProjectiles, "Projectile", FIELD_ATTRIBUTE_PRIVATE, FIELD_ATTRIBUTE_INIT_ONLY | FIELD_ATTRIBUTE_STATIC );

	//DUMPER_SECTION( "Functions" );
	//DUMP_METHOD_BY_RETURN_TYPE_METHOD_ATTRIBUTE( UpdateAmmoDisplay,  )
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "FlintStrikeWeapon" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( strikeRecoil );
	DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeScale, DUMPER_OFFSET( strikeRecoil ) + 0x8 );
	DUMPER_CLASS_END;

	auto player_loot_class = DUMPER_CLASS( "PlayerLoot" );
	auto entity_source_offset = il2cpp::get_field_by_name( player_loot_class, "entitySource" )->offset( );
	auto item_class = il2cpp::get_field_by_offset( player_loot_class, entity_source_offset + 0x8 )->type( )->klass( );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Item", item_class );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS( uid, DUMPER_CLASS( "ItemId" ) );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS( info, DUMPER_CLASS( "ItemDefinition" ) );

	DUMP_ALL_MEMBERS_OF_TYPE( "UnkInt", DUMPER_CLASS_NAMESPACE( "System", "Int32" ), TYPE_ATTRIBUTE_PUBLIC, DUMPER_ATTR_DONT_CARE );
	DUMP_ALL_MEMBERS_OF_TYPE( "UnkEntityRef", entity_ref_class->type( ), TYPE_ATTRIBUTE_NOT_PUBLIC, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* base_raidable_animal_class = DUMPER_CLASS( "BaseRidableAnimal" );
	uint64_t is_lootable_offset = il2cpp::get_field_by_name( base_raidable_animal_class, "isLootable" )->offset( );
	il2cpp::il2cpp_class_t* item_container_class = il2cpp::get_field_by_offset( base_raidable_animal_class, is_lootable_offset + 0x7 )->type( )->klass( );

	char searchBuf[ 128 ] = { 0 };
	sprintf_s( searchBuf, "System.Collections.Generic.List<%s>", item_class->name( ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "ItemContainer", item_container_class );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( itemList, searchBuf );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerInventory" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( crafting );
	DUMP_MEMBER_BY_NEAR_OFFSET( containerBelt, DUMPER_OFFSET( crafting ) - 0x10 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerInput" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS( state, SEARCH_FOR_CLASS_BY_FIELD_COUNT( 3, 0, DUMPER_CLASS( "InputMessage" ), DUMPER_CLASS( "InputMessage" ) ) ); // Search for InputState class.
	DUMP_MEMBER_BY_NEAR_OFFSET( bodyAngles, DUMPER_OFFSET( state ) + 0x1C );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerEyes" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( viewOffset, "PlayerEyes" ); // Contains PlayerEyes.EncryptedValue<Vector3>
	DUMP_MEMBER_BY_NEAR_OFFSET( bodyRotation, DUMPER_OFFSET( viewOffset ) + 0xC ); // <bodyRotation>k__BackingField

	DUMPER_SECTION( "Functions" );

	DUMP_METHOD_BY_RETURN_TYPE_METHOD_ATTRIBUTE( get_Rotation,
		DUMPER_CLASS_NAMESPACE( "UnityEngine", "Quaternion" ),
		DUMPER_CLASS_NAMESPACE( "System.Runtime.CompilerServices", "CompilerGeneratedAttribute" ),
		DUMPER_ATTR_DONT_CARE,
		METHOD_ATTRIBUTE_PUBLIC,
		0,
		il2cpp::attr_search_ignore
	);

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
	DUMP_MEMBER_BY_NEAR_OFFSET( userID, DUMPER_OFFSET( playerRigidbody ) + 0x8 );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( inventory, "PlayerInventory" );
	DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( _displayName, "System.String", TYPE_ATTRIBUTE_NESTED_FAMILY, 0 );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( _lookingAtCollider, "UnityEngine.Collider" );
	DUMP_MEMBER_BY_NEAR_OFFSET( _lookingAtEntity, DUMPER_OFFSET( _lookingAtCollider ) - 0x8 );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_MULTIPLE( lastSentTickTime, "BasePlayer", "System.Single" );

	DUMPER_SECTION( "EncryptedValue Functions" );
	DUMP_ENCRYPTED_MEMBER_GETTER_AND_SETTER( lastSentTickTime, DUMPER_OFFSET( lastSentTickTime ) );
	DUMP_ENCRYPTED_MEMBER_GETTER_AND_SETTER( clActiveItem, DUMPER_OFFSET( clActiveItem ) );

	DUMPER_SECTION( "Functions" );
	DUMP_METHOD_BY_PARAM_CLASS( ClientInput, SEARCH_FOR_CLASS_BY_FIELD_COUNT( 3, 0, DUMPER_CLASS( "InputMessage" ), DUMPER_CLASS( "InputMessage" ) ), 1, DUMPER_VIS_DONT_CARE, METHOD_ATTRIBUTE_VIRTUAL );

	/*
		parser.ParseTypeMethods(basePlayer, [](const char* className, const char* methodName, uint32_t rva) {
			uint64_t hash = CoreSDK_Hash(methodName);

			if (hash == H("OnViewModeChanged"))
				Offsets::BasePlayer__OnViewModeChanged = rva;
		});
	*/
	DUMPER_CLASS_END;


	DUMPER_CLASS_BEGIN_FROM_NAME( "BuildingPrivlidge" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( allowedConstructionItems );
	DUMP_MEMBER_BY_NEAR_OFFSET( cachedProtectedMinutes, DUMPER_OFFSET( allowedConstructionItems ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "WorldItem" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( allowPickup );
	DUMP_MEMBER_BY_NEAR_OFFSET( item, DUMPER_OFFSET( allowPickup ) + 0x8 );
	/*
		parser.ParseTypeMethods(worldItem, [](const char* className, const char* methodName, uint32_t rva) {
			uint64_t hash = CoreSDK_Hash(methodName);

			if (hash == H("<GetMenuItems>b__11_1"))
				Offsets::WorldItem__Pickup = rva;
		});
	*/
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "HackableLockedCrate" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( timerText );
	DUMP_MEMBER_BY_NAME( hackProgressBeep );
	DUMP_MEMBER_BY_NEAR_OFFSET( hackSeconds, DUMPER_OFFSET( hackProgressBeep ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ProjectileWeaponMod" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( projectileVelocity );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerWalkMovement" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( capsule, "UnityEngine.CapsuleCollider" );
	DUMP_MEMBER_BY_NEAR_OFFSET( groundTime, DUMPER_OFFSET( capsule ) + 0x14 );
	DUMPER_CLASS_END;


	DUMPER_CLASS_BEGIN_FROM_NAME( "ConsoleSystem/Command" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_NAME( AllowRunFromServer );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "LootableCorpse" );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( playerSteamID, "System.UInt64" );
	DUMP_MEMBER_BY_NEAR_OFFSET( _playerName, DUMPER_OFFSET( playerSteamID ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "DroppedItemContainer" )
		DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( playerSteamID, "System.UInt64" );
	DUMP_MEMBER_BY_NEAR_OFFSET( _playerName, DUMPER_OFFSET( playerSteamID ) + 0x8 );
	/*
	parser.ParseTypeMethods(droppedItemContainer, [](const char* className, const char* methodName, uint32_t rva) {
		uint64_t hash = CoreSDK_Hash(methodName);

		if (hash == H("Menu_Open"))
			Offsets::DroppedItemContainer__Menu_Open = rva;
	});
	*/
	DUMPER_CLASS_END;

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

	il2cpp::il2cpp_class_t* convar_graphics_klass = il2cpp::search_for_class_by_method_return_type_name( "UnityEngine.FullScreenMode", METHOD_ATTRIBUTE_PRIVATE, METHOD_ATTRIBUTE_STATIC );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Convar_Graphics", convar_graphics_klass );
	DUMPER_SECTION( "Offsets" );
	DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( _fov, convar_graphics_klass->name( ), FIELD_ATTRIBUTE_PRIVATE, FIELD_ATTRIBUTE_STATIC );
	DUMPER_SECTION( "EncryptedValue Functions" );
	DUMP_ENCRYPTED_MEMBER_GETTER_AND_SETTER( _fov, DUMPER_OFFSET( _fov ) );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseFishingRod" );
	DUMPER_SECTION("Offsets")
		DUMP_MEMBER_BY_TYPE_METHOD_ATTRIBUTE( CurrentState,
			DUMPER_CLASS( "BaseFishingRod/CatchState" ),
			DUMPER_CLASS_NAMESPACE( "System.Runtime.CompilerServices", "CompilerGeneratedAttribute" ),
			DUMPER_VIS_DONT_CARE,
			DUMPER_ATTR_DONT_CARE,
			il2cpp::attr_search_want );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( currentBobber, "FishingBobber" );
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( clientStrainAmountNormalised, "System.Single", FIELD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
	DUMPER_SECTION( "Functions" );
		DUMP_METHOD_BY_RETURN_TYPE_ATTRS( UpdateLineRenderer, DUMPER_CLASS_NAMESPACE( "System", "Void" ), 0, METHOD_ATTRIBUTE_PRIVATE, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "GameManifest" );
	DUMPER_SECTION( "Functions" );
	DUMP_METHOD_BY_RETURN_TYPE_ATTRS( GUIDToObject, DUMPER_CLASS_NAMESPACE( "UnityEngine", "Object" ), 1, METHOD_ATTRIBUTE_ASSEM, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemModProjectile" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_NAME( projectileObject );
		DUMP_MEMBER_BY_NAME( ammoType );
		DUMP_MEMBER_BY_NAME( projectileVelocity );
		DUMP_MEMBER_BY_NAME( projectileVelocitySpread );
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
	DUMPER_CLASS_END

	DUMPER_CLASS_BEGIN_FROM_NAME( "CraftingQueue" );
	DUMPER_SECTION( "Offsets" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( icons, "List" );
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

	il2cpp::method_info_t* zipline_audio_update = il2cpp::get_method_by_name( DUMPER_CLASS( "ZiplineAudio" ), "Update" );

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseEntity" );
	DUMPER_SECTION( "Functions" );

	DUMP_METHOD_BY_INFO_PTR( FindBone, SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Transform" ),
		DUMPER_VIS_DONT_CARE,
		METHOD_ATTRIBUTE_VIRTUAL,
		DUMPER_TYPE_NAMESPACE( "System", "String" ) ) );

	DUMP_METHOD_BY_SIG_REL( GetWorldVelocity, zipline_audio_update->get_fn_ptr<uint8_t*>( ), "\x45\x33\xC0\x48\x8D\x48\x88\xE8\xCC\xCC\xCC\xCC\x33\xD2", 8 );
	DUMPER_CLASS_END;

	// LaunchProjectileClientSide.
	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseProjectile" );
	il2cpp::method_info_t* launch_projectile_client_side_info = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		DUMPER_VIS_DONT_CARE,
		DUMPER_ATTR_DONT_CARE,
		DUMPER_TYPE( "ItemDefinition" ),			// Param 1
		DUMPER_TYPE_NAMESPACE( "System", "Int32" ), // Param 2 
		DUMPER_TYPE_NAMESPACE( "System", "Single" ) // Param 3
	);

	DUMPER_SECTION( "Functions" );
	DUMP_METHOD_BY_INFO_PTR( LaunchProjectileClientSide, launch_projectile_client_side_info );
	DUMPER_CLASS_END;

	/*
	DUMPER_CLASS_BEGIN_FROM_NAME( "Projecitle" );
	il2cpp::il2cpp_type_t* hit_test_class_type = il2cpp::get_class_from_field_type_by_type_contains( dumper_klass, "%" )->type( );
	il2cpp::method_info_t* do_hit_method = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
		METHOD_ATTRIBUTE_PRIVATE,
		DUMPER_ATTR_DONT_CARE,
		hit_test_class_type,
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
	);
	DUMP_METHOD_BY_INFO_PTR( DoHit, do_hit_method );
	DUMPER_CLASS_END;*/

	il2cpp::il2cpp_class_t* con_button_class = il2cpp::search_for_class_by_interfaces_contain( "IConsoleButton" );
	il2cpp::il2cpp_class_t* buttons_class = il2cpp::search_for_class_by_field_types( con_button_class->type( ), 56, FIELD_ATTRIBUTE_STATIC );
	DUMPER_CLASS_BEGIN_FROM_PTR( "Buttons", buttons_class );
	DUMPER_CLASS_END;

	il2cpp::il2cpp_class_t* effect_class = il2cpp::search_for_class_by_parent_class_type( DUMPER_TYPE( "EffectData" ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Effect", effect_class );
	DUMPER_SECTION( "Offsets" );
	DUMP_ALL_MEMBERS_OF_TYPE( "UnkVector", DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ), DUMPER_VIS_DONT_CARE, DUMPER_ATTR_DONT_CARE );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BuildingBlock" );
	DUMPER_SECTION( "Functions" );

	il2cpp::method_info_t* upgrade_to_grade = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		DUMPER_TYPE_NAMESPACE( "System", "Void" ),
		METHOD_ATTRIBUTE_PRIVATE,
		DUMPER_ATTR_DONT_CARE,
		DUMPER_TYPE( "BuildingGrade/Enum" ),
		DUMPER_TYPE_NAMESPACE( "System", "UInt64" ),
		DUMPER_TYPE( "BasePlayer" )
	);

	DUMP_METHOD_BY_INFO_PTR( UpgradeToGrade, upgrade_to_grade );
	DUMPER_CLASS_END;
	
	DUMPER_CLASS_BEGIN_FROM_NAME( "WaterLevel" );
	il2cpp::method_info_t* test = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
		METHOD_ATTRIBUTE_PRIVATE,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" ),
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
		DUMPER_TYPE_NAMESPACE( "System", "Boolean" ),
		DUMPER_TYPE( "BaseEntity" )
	);

	DUMP_METHOD_BY_INFO_PTR( Test, test );

	// WaterLevel.GetWaterLevel - private static float GetWaterLevel(Vector3 pos)
	il2cpp::method_info_t* get_water_level = SEARCH_FOR_METHOD_WITH_RETTYPE_PARAM_TYPES(
		DUMPER_TYPE_NAMESPACE( "System", "Single" ),
		METHOD_ATTRIBUTE_PRIVATE,
		METHOD_ATTRIBUTE_STATIC,
		DUMPER_TYPE_NAMESPACE( "UnityEngine", "Vector3" )
	);

	DUMP_METHOD_BY_INFO_PTR( GetWaterLevel, get_water_level );
	DUMPER_CLASS_END;

	/*
		DUMP_METHOD_BY_RETURN_TYPE_METHOD_ATTRIBUTE( get_Rotation,
		DUMPER_CLASS_NAMESPACE( "UnityEngine", "Quaternion" ),
		DUMPER_CLASS_NAMESPACE( "System.Runtime.CompilerServices", "CompilerGeneratedAttribute" ),
		DUMPER_ATTR_DONT_CARE,
		METHOD_ATTRIBUTE_PUBLIC,
		0,
		il2cpp::attr_search_ignore
	);
	*/

	fclose( outfile_handle );
}