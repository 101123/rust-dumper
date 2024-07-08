#pragma once

class rust::classes::BasePlayer
{
	static il2cpp::Il2CppClass* StaticClass( )
	{
		return il2cpp::GetClassByName( "BasePlayer" );
	}
public:
	DECLARE_MEMBER_FROM_TYPE_AND_ATTRS( String*, "System.String", TYPE_ATTRIBUTE_NESTED_FAMILY, _displayName );
	DECLARE_MEMBER_FROM_TYPE( BaseMovement*, "BaseMovement", movement );
	DECLARE_MEMBER_FROM_TYPE( ModelState*, "ModelState", modelState );
	DECLARE_MEMBER_FROM_NAME( uint64_t, playerFlags );
	DECLARE_MEMBER_FROM_NAME( uint64_t, currentTeam );

	PlayerInventory* inventory( )
	{
		uint64_t inventoryOffset = il2cpp::GetFieldIfTypeContains( StaticClass( ), "PlayerInventory" )->Offset( );
		if ( !inventoryOffset )
			return nullptr;

		printf( "PlayerInventory: 0x%x\n", inventoryOffset );

		void* hiddenInventory = *( void** )( uint64_t( this ) + inventoryOffset );
		if ( !hiddenInventory )
			return nullptr;

		return GetHiddenValue<PlayerInventory*>( hiddenInventory );
	}

	uint64_t clActiveItem( )
	{
		uint64_t clActiveItemOffset = currentTeam_Offset( ) + 0x40;

		il2cpp::FieldInfo* fieldInfo = il2cpp::GetFieldByOffset( StaticClass( ), clActiveItemOffset );
		if ( !fieldInfo )
			return { };

		uint64_t clActiveItem = *( uint64_t* )( uint64_t( this ) + clActiveItemOffset );
		if ( !clActiveItem )
			return { };

		using DecryptFn_t = uint64_t( * )( uint64_t, uint64_t );

		uint64_t gameBase = ( uint64_t )( GetModuleHandleA( "GameAssembly.dll" ) );
		
		DecryptFn_t fn = ( DecryptFn_t )( gameBase + 0x143A880 );
		uint64_t methodTypeInfo = ( uint64_t )( gameBase + 0x3C39CC0 );

		return fn( clActiveItem, methodTypeInfo ); //GetEncryptedValue<uint64_t>( &clActiveItem, fieldInfo->Type( )->Class( ) );
	}

	uint32_t userId( )
	{
		uint64_t userIdOffset = playerFlags_Offset( ) + 0x48;

		il2cpp::FieldInfo* fieldInfo = il2cpp::GetFieldByOffset( StaticClass( ), userIdOffset );
		if ( !fieldInfo )
			return { };

		uint64_t* encUserId = ( uint64_t* )( uint64_t( this ) + userIdOffset );
		if ( !encUserId )
			return { };

		return GetEncryptedValue<uint32_t>( encUserId, fieldInfo->Type( )->Class( ) );
	}

	PlayerEyes* eyes( )
	{
		uint64_t eyesOffset = il2cpp::GetFieldIfTypeContains( StaticClass( ), "PlayerEyes" )->Offset( );
		if ( !eyesOffset )
			return nullptr;

		void* hiddenPlayerEyes = *( void** )( uint64_t( this ) + eyesOffset );
		if ( !hiddenPlayerEyes )
			return nullptr;

		return GetHiddenValue<PlayerEyes*>( hiddenPlayerEyes );
	}
};