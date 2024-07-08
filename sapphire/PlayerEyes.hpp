#pragma once

class rust::classes::PlayerEyes
{
	static il2cpp::Il2CppClass* StaticClass( )
	{
		return il2cpp::GetClassByName( "PlayerEyes" );
	}
public:
	DECLARE_MEMBER_FROM_NAME( math::vector_t, thirdPersonSleepingOffset );

	// 0x18
	math::vector_t HeadAngles( )
	{
		// 	private Vector3 %341fe923bff7202f096300f61463ad8f5499a1fe; // 
		return *( math::vector_t* )( uint64_t( this ) + 0x5C );
	}

	math::vector_t ViewOffset( )
	{
		// cancer name.
		uint64_t viewOffOffset = thirdPersonSleepingOffset_Offset( ) + 0x18;
		if ( !viewOffOffset )
			return { };

		il2cpp::FieldInfo* fieldInfo = il2cpp::GetFieldByOffset( StaticClass( ), viewOffOffset );
		if ( !fieldInfo )
			return { };

		uint64_t encViewOffset = *( uint64_t* )( uint64_t( this ) + viewOffOffset );
		if ( !encViewOffset )
			return { };

		return GetEncryptedValue<math::vector_t>( &encViewOffset, fieldInfo->Type( )->Class( ) );
	}
};