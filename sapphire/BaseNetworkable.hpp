#pragma once
#include "rust.hpp"

class rust::classes::BaseNetworkable_EntityRealm
{
private:
public:
	ListDictionary<uint32_t, BaseNetworkable*>* entityList( )
	{
		if ( !this )
			return nullptr;

		HiddenValue* encEntityList = *( HiddenValue** )( ( uint64_t )this + 0x10 );
		if ( !encEntityList )
			return nullptr;

		return GetHiddenValue< ListDictionary<uint32_t, BaseNetworkable*>* >( encEntityList );
	}
};

class rust::classes::BaseNetworkable
{
private:
	static il2cpp::Il2CppClass* StaticClass( )
	{
		return il2cpp::GetClassByName( "BaseNetworkable" );
	}
public:
	static BaseNetworkable_EntityRealm* clientEntities( )
	{
		HiddenValue* clientEntitiesEnc = ( HiddenValue* )( StaticClass( )->StaticFieldData( )[ 0 ] );
		if ( !clientEntitiesEnc )
			return nullptr;

		return GetHiddenValue<BaseNetworkable_EntityRealm*>( clientEntitiesEnc );
	}

	unity::Internal::_GameObject* GetInternalObject( )
	{
		if ( !this )
			return nullptr;

		auto cachedPtr = *reinterpret_cast< std::uintptr_t* >( uint64_t( this ) + 0x10 );
		if ( !cachedPtr )
			return nullptr;

		return *reinterpret_cast< unity::Internal::_GameObject** >( cachedPtr + 0x30 );
	}
};