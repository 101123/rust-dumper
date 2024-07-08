#pragma once
#include "il2cpp_lib.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "color.hpp"
#include "system.hpp"

#include "unity_sdk.hpp"

// #define DECLARE_MEMBER( type, klass, name ) type & name( ) { SAPPHIRE_FIELD( klass, ##name ) return *reinterpret_cast< type* >( this + name ); }

#define DECLARE_MEMBER_FROM_NAME( T, Name ) \
static size_t Name##_Offset( ) { \
	return il2cpp::GetFieldByName( StaticClass( ), #Name )->Offset( ); \
} \
T& Name( ) { \
	static size_t Offset = il2cpp::GetFieldByName( StaticClass( ), #Name )->Offset( ); \
		return *( T* )( uint64_t( this ) + Offset );\
}

#define DECLARE_MEMBER_FROM_TYPE_AND_ATTRS( T, FieldType, Attrs, Name )  \
static size_t Name##_Offset( ) { \
	return il2cpp::GetFieldByTypeName( StaticClass( ), FieldType, Attrs )->Offset( ); \
} \
\
T& Name( ) { \
	static size_t Offset = il2cpp::GetFieldByTypeName( StaticClass( ), FieldType, Attrs )->Offset( ); \
	return *( T* )( uint64_t( this ) + Offset );\
}

#define DECLARE_MEMBER_FROM_TYPE( T, FieldType, Name ) DECLARE_MEMBER_FROM_TYPE_AND_ATTRS( T, FieldType, 0, Name )

// Gets the decryption function for classes inside of the templated encryption rust is now using like so SomeGarbage<BaseNetworkable.EntityRealm> -> BaseNetworkable.EntityRealm

__forceinline il2cpp::MethodInfo* GetHiddenValueGetter( il2cpp::Il2CppClass* encryptedKlass )
{
	const char* encName = encryptedKlass->Type( )->Name( );
	const char* start = strchr( encName, '<' );
	const char* end = strrchr( encName, '>' );

	if ( !start || !end || start >= end )
		return nullptr;

	start++;

	char buf[ 128 ] = { 0 };
	strncpy( buf, start, end - start );

	il2cpp::MethodInfo* method = il2cpp::GetMethodByReturnTypeString( encryptedKlass, buf );
	if ( !method )
		return nullptr;

	return method;
}

__forceinline il2cpp::MethodInfo* GetHiddenValueSetter( il2cpp::Il2CppClass* encryptedKlass )
{
	const char* encName = encryptedKlass->Type( )->Name( );
	const char* start = strchr( encName, '<' );
	const char* end = strrchr( encName, '>' );

	if ( !start || !end || start >= end )
		return nullptr;

	start++;

	char buf[ 128 ] = { 0 };
	strncpy( buf, start, end - start );

	il2cpp::MethodInfo* method = il2cpp::GetMethodByReturnTypeString( encryptedKlass, "System.Void" );
	if ( !method )
		return nullptr;

	return method;
}

template<typename T>
__forceinline T GetHiddenValue( void* instance )
{
	if ( !instance )
		return nullptr;

	using GetFn_t = T( * )( void*, void* );

	il2cpp::Il2CppClass* encInstanceClass = il2cpp::object_get_class( instance );
	if ( !encInstanceClass )
		return nullptr;

	il2cpp::MethodInfo* getMethod = GetHiddenValueGetter( encInstanceClass );
	il2cpp::MethodInfo* disposeMethod = il2cpp::GetMethodByName( encInstanceClass, "Dispose" );
	if ( !getMethod || !disposeMethod )
		return nullptr;

	return getMethod->GetFnPointer<GetFn_t>( )( instance, disposeMethod );
}

template<typename T>
__forceinline T GetEncryptedValue( void* instance, il2cpp::Il2CppClass* encValueClass )
{
	if ( !instance || !encValueClass )
		return T( );

	using GetFn_t = T( * )( void*, void* );

	il2cpp::MethodInfo* getMethod = GetHiddenValueGetter( encValueClass );
	if ( !getMethod )
		return T( );

	printf( "encGetter: 0x%p\n", getMethod->GetFnPointer<void*>( ) );

	return getMethod->GetFnPointer<GetFn_t>( )( instance, getMethod );
}

template<typename T>
__forceinline void SetEncryptedValue( void* instance, il2cpp::Il2CppClass* encValueClass, T* value )
{
	if ( !instance || !encValueClass )
		return;

	using SetFn_t = void( * )( void*, T*, void* );

	static auto gameBase = ( uint64_t )( GetModuleHandleA( "GameAssembly.dll" ) );

	il2cpp::MethodInfo* setMethod = GetHiddenValueSetter( encValueClass );
	if ( !setMethod )
		return;

	SetFn_t setter = ( SetFn_t )( gameBase + 0x1438710 );

	// uint64_t typeInfo = *(uint64_t* )( gameBase +  )

	// printf( "encSetter: 0x%p\n", setMethod->GetFnPointer<void*>( ) );

	return setter( instance, value, setMethod );
}

#include "rust.hpp"

// #include "c_base_projectile.hpp"
// #include "c_item_mod_projectile.hpp"
// #include "c_projectile.hpp"
// #include "c_base_weapon.hpp"
#include "PlayerInventory.hpp"
#include "HiddenValue.hpp"
#include "BaseNetworkable.hpp"

#include "BasePlayer.hpp"
// #include "c_hit_info.hpp"
// #include "c_item_definition.hpp"
#include "ModelState.hpp"
// #include "c_networkable.hpp"
#include "PlayerEyes.hpp"
// #include "c_transform.hpp"
#include "BaseMovement.hpp"

#include "silent_aim.hpp"
#include "visuals.hpp"
#include "notifications.hpp"
#include "hitmarkers.hpp"