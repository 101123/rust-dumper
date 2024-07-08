#pragma once
#include "sapphire_importer.hpp"

#include <windows.h>
#include <cstdio>

#define STR( x ) #x
#define CREATE_TYPE( name, args ) using il2cpp_##name = args; inline il2cpp_##name name;
#define ASSIGN_TYPE( name ) name = (il2cpp_##name)GetProcAddress( GetModuleHandleA( "GameAssembly.dll" ), STR( il2cpp_##name ) ); printf(#name ": 0x%p\n", name);
namespace il2cpp
{
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

	struct PropertyInfo;
	struct MethodInfo;
	struct FieldInfo;
	struct Il2CppType;
	struct Il2CppClass;
	struct Il2CppImage;
	struct Il2CppAssembly;
	struct Il2CppDomain;
	struct Il2CppObject;

	// Domain.
	CREATE_TYPE( domain_get, Il2CppDomain* ( * )( ) );
	CREATE_TYPE( domain_get_assemblies, Il2CppAssembly** ( * )( void*, size_t* ) );
	CREATE_TYPE( domain_assembly_open, Il2CppAssembly* ( * )( void*, const char* ) );

	// Assembly.
	CREATE_TYPE( assembly_get_image, Il2CppImage* ( * )( void* ) );

	// Image.
	CREATE_TYPE( class_from_name, Il2CppClass* ( * )( void*, const char*, const char* ) );
	CREATE_TYPE( image_get_class_count, size_t( * )( Il2CppImage* ) );
	CREATE_TYPE( image_get_class, Il2CppClass* ( * )( Il2CppImage*, size_t ) );
	// Types.
	CREATE_TYPE( type_get_object, Il2CppObject* ( * )( void* ) );
	CREATE_TYPE( type_get_class_or_element_class, Il2CppClass* ( * )( void* ) );
	CREATE_TYPE( type_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( type_get_attrs, uint32_t( * )( Il2CppType* ) );
	// Class.
	CREATE_TYPE( class_get_methods, MethodInfo* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_fields, FieldInfo* ( * )( void*, void** ) );
	CREATE_TYPE( class_get_type, Il2CppType* ( * )( void* ) );
	CREATE_TYPE( class_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( class_get_namespace, const char* ( * )( void* ) );
	CREATE_TYPE( class_from_il2cpp_type, Il2CppClass* ( * )( Il2CppType* ) );
	CREATE_TYPE( class_get_static_field_data, uint64_t*( * )( void* ) );
	CREATE_TYPE( class_get_parent, Il2CppClass* ( * )( Il2CppClass* klass ) );

	// Methods.
	CREATE_TYPE( method_get_param_count, uint32_t( * )( void* ) );
	CREATE_TYPE( method_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( method_get_param_name, const char* ( * )( MethodInfo*, uint32_t ) );
	CREATE_TYPE( method_get_param, Il2CppType* ( * )( MethodInfo*, uint32_t ) );
	CREATE_TYPE( method_get_return_type, Il2CppType* ( * )( void* ) );
	CREATE_TYPE( method_get_class, Il2CppClass* ( * )( MethodInfo* ) );
	CREATE_TYPE( method_get_flags, uint32_t ( * )( MethodInfo*, uint32_t* ) );

	// Fields.
	CREATE_TYPE( field_get_offset, size_t( * )( void* ) );
	CREATE_TYPE( field_get_type, Il2CppType* ( * )( void* ) );
	CREATE_TYPE( field_get_parent, Il2CppClass* ( * )( void* ) );
	CREATE_TYPE( field_get_name, const char* ( * )( void* ) );
	CREATE_TYPE( field_static_get_value, void( * )( void*, uint64_t* ) );
	CREATE_TYPE( field_get_flags, int( * )( void* ) );
	// Object.
	CREATE_TYPE( object_get_class, Il2CppClass* ( * )( void* ) );

	CREATE_TYPE( resolve_icall, std::uintptr_t( * )( const char* ) );

	struct Il2CppType
	{
		Il2CppClass* Class( )
		{
			if ( !this )
				return nullptr;

			return type_get_class_or_element_class( this );
		}

		const char* Name( )
		{
			if ( !this )
				return nullptr;

			return type_get_name( this );
		}

		uint32_t Attributes( )
		{
			if ( !this )
				return 0;

			return type_get_attrs( this );
		}
	};


	struct PropertyInfo
	{

	};

	struct MethodInfo
	{
		const char* Name( )
		{
			if ( !this )
				return nullptr;

			return method_get_name( this );
		}

		Il2CppType* GetParam( uint32_t index )
		{
			if ( !this )
				return nullptr;

			return method_get_param( this, index );
		}

		const char* GetParamName( uint32_t index )
		{
			if ( !this )
				return nullptr;

			return method_get_param_name( this, index );
		}

		uint32_t ParamCount( )
		{
			if ( !this )
				return 0;

			return method_get_param_count( this );
		}

		Il2CppType* ReturnType( )
		{
			if ( !this )
				return nullptr;

			return method_get_return_type( this );
		}

		Il2CppClass* Class( )
		{
			if ( !this )
				return nullptr;

			return method_get_class( this );
		}

		uint32_t Flags( )
		{
			if ( !this )
				return { };

			return method_get_flags( this, nullptr );
		}

		template<typename T>
		T GetFnPointer( )
		{
			if ( !this )
				return T();

			return *( T* )( this );
		}
	};

	struct FieldInfo
	{
		Il2CppType* Type( )
		{
			if ( !this )
				return nullptr;

			return field_get_type( this );
		}

		const char* Name( )
		{
			if ( !this )
				return nullptr;

			return field_get_name( this );
		}

		size_t Offset( )
		{
			if ( !this )
				return 0;

			return field_get_offset( this );
		}

		uint64_t StaticGetValue( )
		{
			if ( !this )
				return 0;

			uint64_t value{ };
			field_static_get_value( this, &value );
			return value;
		}

		int Flags( )
		{
			if ( !this )
				return 0;

			return field_get_flags( this );
		}
	};

	struct Il2CppClass
	{
		const char* Name( )
		{
			if ( !this )
				return nullptr;

			return class_get_name( this );
		}
		const char* Namespace( )
		{
			if ( !this )
				return nullptr;

			return class_get_namespace( this );
		}

		Il2CppType* Type( )
		{
			if ( !this )
				return nullptr;
			return class_get_type( this );
		}

		FieldInfo* Fields( void** iter )
		{
			if ( !this )
				return nullptr;
			return class_get_fields( this, iter );
		}

		MethodInfo* Methods( void** iter )
		{
			if ( !this )
				return nullptr;

			return class_get_methods( this, iter );
		}

		uint64_t* StaticFieldData( )
		{
			if ( !this )
				return 0;

			return class_get_static_field_data( this );
		}

		Il2CppClass* Parent( )
		{
			if ( !this )
				return nullptr;

			return class_get_parent( this );
		}
	};

	struct Il2CppImage
	{
		size_t ClassCount( )
		{
			if ( !this )
				return {};

			return image_get_class_count( this );
		}

		Il2CppClass* GetClass( size_t index )
		{
			if ( !this )
				return nullptr;

			return image_get_class( this, index );
		}

		Il2CppClass* GetClassFromName( const char* namespaze, const char* klass )
		{
			if ( !this )
				return nullptr;

			return class_from_name( this, namespaze, klass );
		}
	};

	struct Il2CppObject
	{

	};

	struct Il2CppAssembly
	{
		Il2CppImage* Image( )
		{
			if ( !this )
				return nullptr;

			return assembly_get_image( this );
		}
	};

	struct Il2CppDomain
	{
		static Il2CppDomain* Get( )
		{
			return domain_get( );
		}

		Il2CppAssembly** GetAssemblies( size_t* size )
		{
			if ( !this )
				return nullptr;

			return domain_get_assemblies( this, size );
		}

		Il2CppAssembly* GetAssemblyFromName( const char* name )
		{
			if ( !this )
				return nullptr;

			return domain_assembly_open( this, name );
		}
	};

	inline Il2CppClass* GetClassFromFieldType( Il2CppClass* klass, const char* fieldName )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( strcmp( field->Name( ), fieldName ) == 0 )
				return class_from_il2cpp_type( field->Type( ) );
		}

		return nullptr;
	}

	inline FieldInfo* GetFieldIfTypeContains( Il2CppClass* klass, const char* search, int visibility = 0, int flags = 0 )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( const char* name = field->Type( )->Name( ) )
			{
				int vis = field->Flags( ) & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
				if ( visibility && ( vis != visibility ) )
					continue;

				if ( flags && !( field->Flags( ) & flags ) )
					continue;

				if ( strstr( name, search ) != nullptr )
					return field;
			}
		}

		return nullptr;
	}

	inline FieldInfo* GetFieldFromTypeClass( Il2CppClass* klass, Il2CppClass* wantedKlass )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( const char* name = field->Type( )->Name( ) )
			{
				if ( strcmp( name, wantedKlass->Name( ) ) == 0 )
					return field;
			}
		}

		return nullptr;
	}

	inline Il2CppClass* FindClassFromFieldType( Il2CppClass* klass, Il2CppType* wantedType )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( field->Type( ) == wantedType )
				return class_from_il2cpp_type( field->Type( ) );
		}

		return nullptr;
	}

	inline Il2CppClass* FindClassFromFieldClass( Il2CppClass* klass, Il2CppClass* wantedKlass )
	{
		return FindClassFromFieldType( klass, wantedKlass->Type( ) );
	}

	inline Il2CppClass* ClassFromMethod( Il2CppClass* klass, const char* methodName, const char* paramName, int paramCount = -1 )
	{
		bool checkMethodName = methodName != nullptr;
		bool checkParamName = paramName != nullptr;
		bool checkParamCount = paramCount != -1;

		void* iter = nullptr;
		while ( MethodInfo* method = klass->Methods( &iter ) )
		{
			int found = 0;

			if ( checkMethodName )
			{
				if ( strcmp( method->Name( ), methodName ) == 0 )
					found++;
			}

			if ( checkParamName || checkParamCount )
			{
				uint32_t numParams = method->ParamCount( );

				if ( checkParamCount && numParams != paramCount )
					found--;

				if ( checkParamName )
				{
					for ( uint32_t i = 0; i < numParams; i++ )
					{
						const char* name = method->GetParamName( i );
						if ( !name )
							continue;

						if ( strcmp( name, paramName ) == 0 )
							found++;
					}
				}
			}

			if ( found )
				return klass;
		}

		return nullptr;
	}

	inline Il2CppClass* AssemblySearchForClassByMethod( Il2CppAssembly* assembly, const char* methodName, const char* paramName, int paramCount = -1 )
	{
		if ( !assembly )
			return nullptr;

		Il2CppImage* image = assembly->Image( );
		if ( !image )
			return nullptr;

		size_t classCount = image->ClassCount( );

		for ( size_t i = 0; i < classCount; i++ )
		{
			Il2CppClass* klass = image->GetClass( i );
			if ( !klass )
				continue;

			if ( Il2CppClass* foundKlass = ClassFromMethod( klass, methodName, paramName, paramCount ) )
				return foundKlass;
		}

		return nullptr;
	}

	inline Il2CppAssembly* GetAssemblyByName( const char* assemblyName )
	{
		Il2CppDomain* domain = Il2CppDomain::Get( );
		if ( !domain )
			return nullptr;

		return domain->GetAssemblyFromName( assemblyName );
	}

	inline Il2CppClass* SearchForClassByMethod( const char* assemblyName, const char* methodName, const char* paramName, int paramCount = -1 )
	{
		return AssemblySearchForClassByMethod( GetAssemblyByName( assemblyName ), methodName, paramName, paramCount );
	}

	inline Il2CppClass* GetClassByName( const char* klass, const char* namespaze = "" )
	{
		Il2CppDomain* domain = Il2CppDomain::Get( );
		if ( !domain )
			return nullptr;

		size_t assemblyCount{};
		Il2CppAssembly** assemblies = domain->GetAssemblies( &assemblyCount );
		if ( !assemblies )
			return nullptr;

		for ( size_t i = 0; i < assemblyCount; i++ )
		{
			Il2CppAssembly* assembly = assemblies[ i ];
			if ( !assembly )
				continue;

			Il2CppImage* image = assembly->Image( );
			if ( !image )
				continue;

			if ( Il2CppClass* kl = image->GetClassFromName( namespaze, klass ) )
			{
				return kl;
			}
		}

		return nullptr;
	}

	inline Il2CppClass* SearchForClassByFieldCount( Il2CppClass** matchClasses, int matchClassesSize, int fieldCount, uint8_t equality )
	{
		Il2CppDomain* domain = Il2CppDomain::Get( );
		if ( !domain )
			return nullptr;

		size_t assemblyCount{};
		Il2CppAssembly** assemblies = domain->GetAssemblies( &assemblyCount );
		if ( !assemblies )
			return nullptr;

		for ( size_t i = 0; i < assemblyCount; i++ )
		{
			Il2CppAssembly* assembly = assemblies[ i ];
			if ( !assembly )
				continue;

			Il2CppImage* image = assembly->Image( );
			if ( !image )
				continue;

			size_t classCount = image->ClassCount( );
			for ( size_t j = 0; j < classCount; j++ )
			{
				Il2CppClass* klass = image->GetClass( j );
				if ( !klass )
					continue;

				int count = 0;

				void* iter = nullptr;
				while ( FieldInfo* field = klass->Fields( &iter ) )
				{
					count++;
				}

				if ( equality == 0 && count != fieldCount )
					continue;
				else if ( equality == 1 && count >= fieldCount )
					continue;
				else if ( equality == 2 && count <= fieldCount )
					continue;

				int correctFields = 0;
				for ( size_t k = 0; k < matchClassesSize; k++ )
				{
					Il2CppClass* matchClass = matchClasses[ k ];
					if ( !matchClass )
						continue;

					void* iter2 = nullptr;
					while ( FieldInfo* field = klass->Fields( &iter2 ) )
					{
						if ( field->Type( )->Class( ) == matchClass )
						{
							correctFields++;
							break;
						}
					}
				}

				if ( correctFields != matchClassesSize )
					continue;

				return klass;
			}
		}

		return nullptr;
	}

	inline Il2CppClass* SearchForClassByMethodReturnTypeName( const char* returnType, uint32_t visibility, uint32_t flags )
	{
		Il2CppDomain* domain = Il2CppDomain::Get( );
		if ( !domain )
			return nullptr;

		size_t assemblyCount{};
		Il2CppAssembly** assemblies = domain->GetAssemblies( &assemblyCount );
		if ( !assemblies )
			return nullptr;

		for ( size_t i = 0; i < assemblyCount; i++ )
		{
			Il2CppAssembly* assembly = assemblies[ i ];
			if ( !assembly )
				continue;

			Il2CppImage* image = assembly->Image( );
			if ( !image )
				continue;

			size_t classCount = image->ClassCount( );
			for ( size_t j = 0; j < classCount; j++ )
			{
				Il2CppClass* klass = image->GetClass( j );
				if ( !klass )
					continue;

				void* iter = nullptr;
				while ( MethodInfo* method = klass->Methods( &iter ) )
				{
					if ( const char* name = method->ReturnType( )->Name( ) )
					{
						if ( strcmp( name, returnType ) == 0 )
						{
							uint32_t vis = method->Flags( ) & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
							if ( visibility && ( vis != visibility ) )
								continue;

							if ( flags && !( method->Flags( ) & flags ) )
								continue;

							return klass;
						}
					}
				}
			}
		}

		return nullptr;
	}

	inline MethodInfo* GetMethodByName( Il2CppClass* klass, const char* methodName )
	{
		void* iter = nullptr;

		while ( MethodInfo* method = klass->Methods( &iter ) )
		{
			if ( const char* name = method->Name( ) )
			{
				if ( strcmp( name, methodName ) == 0 )
					return method;
			}
		}

		return nullptr;
	}
	inline MethodInfo* GetMethodByReturnTypeString( Il2CppClass* klass, const char* returnType )
	{
		void* iter = nullptr;

		while ( MethodInfo* method = klass->Methods( &iter ) )
		{
			if ( const char* name = method->ReturnType( )->Name( ) )
			{
				if ( strcmp( name, returnType ) == 0 )
					return method;
			}
		}

		return nullptr;
	}
	inline MethodInfo* GetMethodByParamTypeName( Il2CppClass* klass, const char* name, int paramCount, uint32_t flags )
	{
		void* iter = nullptr;

		while ( MethodInfo* method = klass->Methods( &iter ) )
		{
			uint32_t count = method->ParamCount( );
			if ( count != paramCount )
				continue;

			for ( uint32_t i = 0; i < count; i++ )
			{
				Il2CppType* param = method->GetParam( i );
				if ( !param )
					continue;

				if ( strcmp( param->Name( ), name ) == 0 )
				{
					if ( flags && !( method->Flags( ) & flags ) )
						continue;

					return method;
				}
			}
		}

		return nullptr;
	}
	inline MethodInfo* GetMethodByParamClass( Il2CppClass* klass, Il2CppClass* paramClass, int paramCount, uint32_t flags )
	{
		void* iter = nullptr;

		while ( MethodInfo* method = klass->Methods( &iter ) )
		{
			uint32_t count = method->ParamCount( );
			if ( count != paramCount )
				continue;

			for ( uint32_t i = 0; i < count; i++ )
			{
				Il2CppType* param = method->GetParam( i );
				if ( !param )
					continue;

				if ( param->Class( ) == paramClass )
				{
					if ( flags && ( method->Flags( ) & flags ) )
						return method;
				}
			}
		}

		return nullptr;
	}
	inline FieldInfo* GetFieldByOffset( Il2CppClass* klass, uint64_t offset )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( field->Offset( ) == offset )
				return field;
		}

		return nullptr;
	}
	inline FieldInfo* GetFieldByName( Il2CppClass* klass, const char* name )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( strcmp( field->Name( ), name ) == 0 )
				return field;
		}

		return nullptr;
	}
	inline FieldInfo* GetFieldByClass( Il2CppClass* klass, Il2CppClass* wantedKlass )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( field->Type( )->Class( ) == wantedKlass )
				return field;
		}

		return nullptr;
	}
	inline FieldInfo* GetFieldByTypeName( Il2CppClass* klass, const char* fieldType, uint32_t wantedAttrs = 0 )
	{
		void* iter = nullptr;
		while ( FieldInfo* field = klass->Fields( &iter ) )
		{
			if ( const char* name = field->Type( )->Name( ) )
			{
				if ( strcmp( name, fieldType ) == 0 )
				{
					uint32_t visibility = field->Type( )->Attributes( ) & TYPE_ATTRIBUTE_VISIBILITY_MASK;
					if ( wantedAttrs && ( visibility != wantedAttrs ) )
						continue;

					return field;
				}
			}
		}

		return nullptr;
	}

	inline void Init( )
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

		// Methods.
		ASSIGN_TYPE( method_get_param_count );
		ASSIGN_TYPE( method_get_name );
		ASSIGN_TYPE( method_get_param_name );
		ASSIGN_TYPE( method_get_param );
		ASSIGN_TYPE( method_get_return_type );
		ASSIGN_TYPE( method_get_class );
		ASSIGN_TYPE( method_get_flags );

		// Fields.
		ASSIGN_TYPE( field_get_offset );
		ASSIGN_TYPE( field_get_type );
		ASSIGN_TYPE( field_get_parent );
		ASSIGN_TYPE( field_get_name );
		ASSIGN_TYPE( field_static_get_value );
		ASSIGN_TYPE( field_get_flags );
		// Object.
		ASSIGN_TYPE( object_get_class );

		ASSIGN_TYPE( resolve_icall );
	}
}