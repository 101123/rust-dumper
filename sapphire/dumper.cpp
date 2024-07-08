#include "dumper.hpp"

#define DUMPER_RVA( X ) X - dumper::game_base
#define DUMPER_CLASS( Name ) il2cpp::GetClassByName( Name )

#define SEARCH_FOR_CLASS_BY_FIELD_COUNT( FieldCount, Equality, ... ) \
	[]() -> il2cpp::Il2CppClass* \
	{ \
		il2cpp::Il2CppClass* matchClasses[ ] = { __VA_ARGS__ }; \
		return il2cpp::SearchForClassByFieldCount( matchClasses, _countof( matchClasses ), FieldCount, Equality ); \
	}() \

#define DUMPER_CLASS_PRINT_HEADER( KlassName ) \
	dumper::write_to_file( "namespace %s {\n", dumper::clean_klass_name( KlassName ) );
#define DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( KlassName, NameSpaze ) \
	{ \
		il2cpp::Il2CppClass* dumperClass = il2cpp::GetClassByName( KlassName, NameSpaze ); \
		DUMPER_CLASS_PRINT_HEADER( KlassName );

#define DUMPER_CLASS_BEGIN_FROM_NAME( KlassName ) DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( KlassName, "" )

#define DUMP_CLASS_NAME( DumpName, ClassPtr ) dumper::write_to_file( "#define " DumpName "_ClassName \"%s\"\n\n", ClassPtr->Name( ) );

#define DUMPER_CLASS_BEGIN_FROM_PTR( DumpName, ClassPtr ) \
	{ \
		il2cpp::Il2CppClass* dumperClass = ClassPtr; \
		dumper::write_to_file( "// obf name: %s::%s\n", dumperClass->Namespace(), dumperClass->Name() ); \
		DUMP_CLASS_NAME( DumpName, ClassPtr );	\
		DUMPER_CLASS_PRINT_HEADER( DumpName );

#define DUMPER_CLASS_END dumper::write_to_file( "}\n\n" ); } 

#define DUMP_MEMBER_BY_X( NAME, X ) uint64_t NAME##_Offset = X; dumper::write_to_file("\tconstexpr const static size_t " #NAME " = 0x%x;\n", static_cast<uint32_t>( NAME##_Offset ) )

#define DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( NAME, FieldType, Attrs ) DUMP_MEMBER_BY_X( NAME, il2cpp::GetFieldByTypeName( dumperClass, FieldType, Attrs )->Offset( ) ) 
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( NAME, Search )       DUMP_MEMBER_BY_X( NAME, il2cpp::GetFieldIfTypeContains( dumperClass, Search )->Offset( ) )
#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( NAME, Search, Visibility, Attrs ) DUMP_MEMBER_BY_X( NAME, il2cpp::GetFieldIfTypeContains( dumperClass, Search, Visibility, Attrs )->Offset( ) )

#define DUMP_MEMBER_BY_FIELD_TYPE_CLASS( NAME, WantedKlass )		   DUMP_MEMBER_BY_X( NAME, il2cpp::GetFieldFromTypeClass( dumperClass, WantedKlass )->Offset( ) )
#define DUMP_MEMBER_BY_NAME( NAME )									   DUMP_MEMBER_BY_X( NAME, il2cpp::GetFieldByName( dumperClass, #NAME )->Offset( ) )
#define DUMP_MEMBER_BY_NEAR_OFFSET( NAME, Off )						   DUMP_MEMBER_BY_X( NAME, il2cpp::GetFieldByOffset( dumperClass, Off )->Offset( ) )

char* GetInnerClassFromEncClass( const char* name )
{
	static char buf[ 128 ] = { 0 };
	const char* start = strchr( name, '<' );
	const char* end = strrchr( name, '>' );

	if ( !start || !end || start >= end )
		return nullptr;

	start++;
	strncpy( buf, start, end - start );

	return buf;
}

#define DUMP_ENCRYPTED_MEMBER_GETTER_AND_SETTER( NAME, Off )	\
	il2cpp::Il2CppType* encryptedType = il2cpp::GetFieldByOffset( dumperClass, Off )->Type(); \
	il2cpp::MethodInfo* encGetter = il2cpp::GetMethodByReturnTypeString( encryptedType->Class( ), GetInnerClassFromEncClass( encryptedType->Name( ) ) ); \
	il2cpp::MethodInfo* encSetter = il2cpp::GetMethodByReturnTypeString( encryptedType->Class( ), "System.Void" ); \
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_EncryptedValueGetter = 0x%x;\n",  DUMPER_RVA( encGetter->GetFnPointer<uint64_t>() ) );\
	dumper::write_to_file("\tconstexpr const static size_t " #NAME "_EncryptedValueSetter = 0x%x;\n",  DUMPER_RVA( encSetter->GetFnPointer<uint64_t>() ) );

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

void dumper::produce( )
{
	game_base = ( uint64_t )( GetModuleHandleA( "GameAssembly.dll" ) );
	outfile_handle = fopen( "C:\\dumps\\output.h", "w" );
	if ( !outfile_handle )
		return;

	dumper::write_to_file( "#include <cstdint>\n\n" );

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseNetworkable" );
		DUMP_MEMBER_BY_NAME( prefabID ); // Dump a member by it's name.
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( parentEntity, SEARCH_FOR_CLASS_BY_FIELD_COUNT( 2, 0, DUMPER_CLASS( "BaseEntity" ), DUMPER_CLASS( "NetworkableId" ) ) ); // Search for EntityRef class.
		DUMP_MEMBER_BY_NEAR_OFFSET( children, DUMPER_OFFSET( parentEntity ) + 0x10 ); // Dump a member by an offset from another member of the class.
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "Model" );
		DUMP_MEMBER_BY_NAME( boneTransforms );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseEntity" );
		DUMP_MEMBER_BY_NAME( flags );
		DUMP_MEMBER_BY_NAME( model );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseCombatEntity" );
		DUMP_MEMBER_BY_NAME( lifestate );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ModelState" );
		DUMP_MEMBER_BY_NAME( flags );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ItemDefinition" );
		DUMP_MEMBER_BY_NAME( itemid );
		DUMP_MEMBER_BY_NAME( category );
		DUMP_MEMBER_BY_NAME( shortname );
		DUMP_MEMBER_BY_NAME( displayName );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "RecoilProperties" );
		DUMP_MEMBER_BY_NAME( recoilYawMin );
		DUMP_MEMBER_BY_NAME( recoilYawMax );
		DUMP_MEMBER_BY_NAME( recoilPitchMin );
		DUMP_MEMBER_BY_NAME( recoilPitchMax );
		DUMP_MEMBER_BY_NAME( newRecoilOverride );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME_NAMESPACE( "Magazine", "ProtoBuf" );
		DUMP_MEMBER_BY_NAME( ammoType );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "AttackEntity" );
		DUMP_MEMBER_BY_NAME( repeatDelay );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BaseProjectile" );
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
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "FlintStrikeWeapon" );
		DUMP_MEMBER_BY_NAME( strikeRecoil );
		DUMP_MEMBER_BY_NEAR_OFFSET( sightAimConeScale, DUMPER_OFFSET( strikeRecoil ) + 0x8 );
	DUMPER_CLASS_END;

	auto player_loot_class = DUMPER_CLASS( "PlayerLoot" );
	auto entity_source_offset = il2cpp::GetFieldByName( player_loot_class, "entitySource" )->Offset( );
	auto item_class = il2cpp::GetFieldByOffset( player_loot_class, entity_source_offset + 0x8 )->Type( )->Class( );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Item", item_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( uid, DUMPER_CLASS( "ItemId" ) );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( info, DUMPER_CLASS( "ItemDefinition" ) );

		// position is the next int after Item.Flags 
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( flags, "Flag" );
		DUMP_MEMBER_BY_NEAR_OFFSET( position, DUMPER_OFFSET( flags ) + 0x4 );

		// This one is trickier I think this is fine for now.
		DUMP_MEMBER_BY_NEAR_OFFSET( amount, DUMPER_OFFSET( info ) + 0x20 );
		DUMP_MEMBER_BY_NEAR_OFFSET( heldEntity, DUMPER_OFFSET( amount ) + 0x8 );
	DUMPER_CLASS_END;

	il2cpp::Il2CppClass* base_raidable_animal_class = DUMPER_CLASS( "BaseRidableAnimal" );
	uint64_t is_lootable_offset = il2cpp::GetFieldByName( base_raidable_animal_class, "isLootable" )->Offset( );
	il2cpp::Il2CppClass* item_container_class = il2cpp::GetFieldByOffset( base_raidable_animal_class, is_lootable_offset + 0x7 )->Type( )->Class( );

	char searchBuf[ 128 ] = { 0 };
	sprintf_s( searchBuf, "System.Collections.Generic.List<%s>", item_class->Name( ) );

	DUMPER_CLASS_BEGIN_FROM_PTR( "ItemContainer", item_container_class );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( itemList, searchBuf );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerInventory" );
		DUMP_MEMBER_BY_NAME( crafting );
		DUMP_MEMBER_BY_NEAR_OFFSET( containerBelt, DUMPER_OFFSET( crafting ) - 0x10 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerInput" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS( state, SEARCH_FOR_CLASS_BY_FIELD_COUNT( 3, 0, DUMPER_CLASS( "InputMessage" ), DUMPER_CLASS( "InputMessage" ) ) ); // Search for InputState class.
		DUMP_MEMBER_BY_NEAR_OFFSET( bodyAngles, DUMPER_OFFSET( state ) + 0x1C );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerEyes" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( viewOffset, "PlayerEyes" ); // Contains PlayerEyes.EncryptedValue<Vector3>
		DUMP_MEMBER_BY_NEAR_OFFSET( bodyRotation, DUMPER_OFFSET( viewOffset ) + 0xC ); // <bodyRotation>k__BackingField
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BasePlayer" );
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
		DUMP_MEMBER_BY_FIELD_TYPE_NAME_ATTRS( _displayName, "System.String", TYPE_ATTRIBUTE_NESTED_FAMILY );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( _lookingAtCollider, "UnityEngine.Collider" );
		DUMP_MEMBER_BY_NEAR_OFFSET( _lookingAtEntity, DUMPER_OFFSET( _lookingAtCollider ) - 0x8 );

	/*
		parser.ParseTypeMethods(basePlayer, [](const char* className, const char* methodName, uint32_t rva) {
			uint64_t hash = CoreSDK_Hash(methodName);

			if (hash == H("OnViewModeChanged"))
				Offsets::BasePlayer__OnViewModeChanged = rva;
		});
	*/
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "BuildingPrivlidge" );
		DUMP_MEMBER_BY_NAME( allowedConstructionItems );
		DUMP_MEMBER_BY_NEAR_OFFSET( cachedProtectedMinutes, DUMPER_OFFSET( allowedConstructionItems ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "WorldItem" );
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
		DUMP_MEMBER_BY_NAME( timerText );
		DUMP_MEMBER_BY_NAME( hackProgressBeep );
		DUMP_MEMBER_BY_NEAR_OFFSET( hackSeconds, DUMPER_OFFSET( hackProgressBeep ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "ProjectileWeaponMod" );
		DUMP_MEMBER_BY_NAME( projectileVelocity );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "PlayerWalkMovement" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( capsule, "UnityEngine.CapsuleCollider" );
		DUMP_MEMBER_BY_NEAR_OFFSET( groundTime, DUMPER_OFFSET( capsule ) + 0x14 );
	DUMPER_CLASS_END;


	DUMPER_CLASS_BEGIN_FROM_NAME( "ConsoleSystem/Command" );
		DUMP_MEMBER_BY_NAME( AllowRunFromServer );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "LootableCorpse" );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS( playerSteamID, "System.UInt64" );
		DUMP_MEMBER_BY_NEAR_OFFSET( _playerName, DUMPER_OFFSET( playerSteamID ) + 0x8 );
	DUMPER_CLASS_END;

	DUMPER_CLASS_BEGIN_FROM_NAME( "DroppedItemContainer" )
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

	il2cpp::Il2CppClass* convarGraphicsClass = il2cpp::SearchForClassByMethodReturnTypeName( "UnityEngine.FullScreenMode", METHOD_ATTRIBUTE_PRIVATE, METHOD_ATTRIBUTE_STATIC );

	DUMPER_CLASS_BEGIN_FROM_PTR( "Convar_Graphics", convarGraphicsClass );
		DUMP_MEMBER_BY_FIELD_TYPE_CLASS_CONTAINS_ATTRS( _fov, convarGraphicsClass->Name( ), FIELD_ATTRIBUTE_PRIVATE, FIELD_ATTRIBUTE_STATIC );
		DUMP_ENCRYPTED_MEMBER_GETTER_AND_SETTER( _fov, DUMPER_OFFSET( _fov ) );
	DUMPER_CLASS_END;

	fclose( outfile_handle );
}

void dumper::components::dump_typeinfo( uint64_t base_addr, il2cpp::Il2CppClass* klass )
{
	// uint64_t base_addr = ( uint64_t )( GetModuleHandleA( "GameAssembly.dll" ) );

	
// printf( "convarGraphicsClass: 0x%p\n", convarGraphicsClass );
// 
// if ( convarGraphicsClass )
// {
// 	components::dump_typeinfo( assemblyBase, convarGraphicsClass );
// }

}