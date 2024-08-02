#pragma once

#include <windows.h>
#include <stdint.h>
#include <psapi.h>

#ifndef IL2CPP_INLINE
#define IL2CPP_INLINE __forceinline
#endif

namespace Il2Cpp {
	enum MetadataUsage {
		kMetadataUsageInvalid,
		kMetadataUsageTypeInfo,
		kMetadataUsageIl2CppType,
		kMetadataUsageMethodDef,
		kMetadataUsageFieldInfo,
		kMetadataUsageStringLiteral,
		kMetadataUsageMethodRef,
	};

#pragma pack( push, 1 )
	struct GlobalMetadataHeader {
		uint32_t sanity;
		uint32_t version;
		uint32_t stringLiteralOffset;
		int32_t stringLiteralSize;
		uint32_t stringLiteralDataOffset;
		int32_t stringLiteralDataSize;
		uint32_t stringOffset;
		int32_t stringSize;
		uint32_t eventsOffset;
		int32_t eventsSize;
		uint32_t propertiesOffset;
		int32_t propertiesSize;
		uint32_t methodsOffset;
		int32_t methodsSize;
		uint32_t parameterDefaultValuesOffset;
		int32_t parameterDefaultValuesSize;
		uint32_t fieldDefaultValuesOffset;
		int32_t fieldDefaultValuesSize;
		uint32_t fieldAndParameterDefaultValueDataOffset;
		int32_t fieldAndParameterDefaultValueDataSize;
		int32_t fieldMarshaledSizesOffset;
		int32_t fieldMarshaledSizesSize;
		uint32_t parametersOffset;
		int32_t parametersSize;
		uint32_t fieldsOffset;
		int32_t fieldsSize;
		uint32_t genericParametersOffset;
		int32_t genericParametersSize;
		uint32_t genericParameterConstraintsOffset;
		int32_t genericParameterConstraintsSize;
		uint32_t genericContainersOffset;
		int32_t genericContainersSize;
		uint32_t nestedTypesOffset;
		int32_t nestedTypesSize;
		uint32_t interfacesOffset;
		int32_t interfacesSize;
		uint32_t vtableMethodsOffset;
		int32_t vtableMethodsSize;
		int32_t interfaceOffsetsOffset;
		int32_t interfaceOffsetsSize;
		uint32_t typeDefinitionsOffset;
		int32_t typeDefinitionsSize;
		uint32_t imagesOffset;
		int32_t imagesSize;
		uint32_t assembliesOffset;
		int32_t assembliesSize;
	};

	struct ImageDefinition {
		uint32_t nameIndex;
		int32_t assemblyIndex;
		int32_t typeStart;
		uint32_t typeCount;
		int32_t exportedTypeStart;
		uint32_t exportedTypeCount;
		int32_t entryPointIndex;
		uint32_t token;
		int32_t customAttributeStart;
		uint32_t customAttributeCount;
	};

	struct TypeDefinition {
		uint32_t nameIndex;
		uint32_t namespaceIndex;
		int32_t byvalTypeIndex;
		int32_t declaringTypeIndex;
		int32_t parentIndex;
		int32_t elementTypeIndex;
		int32_t genericContainerIndex;
		uint32_t flags;
		int32_t fieldStart;
		int32_t methodStart;
		int32_t eventStart;
		int32_t propertyStart;
		int32_t nestedTypesStart;
		int32_t interfacesStart;
		int32_t vtableStart;
		int32_t interfaceOffsetsStart;
		uint16_t method_count;
		uint16_t property_count;
		uint16_t field_count;
		uint16_t event_count;
		uint16_t nested_type_count;
		uint16_t vtable_count;
		uint16_t interfaces_count;
		uint16_t interface_offsets_count;
		uint32_t bitfield;
		uint32_t token;
	};

	struct FieldDefinition {
		uint32_t nameIndex;
		int32_t typeIndex;
		uint32_t token;
	};

	struct MethodDefinition {
		uint32_t nameIndex;
		int32_t declaringType;
		int32_t returnType;
		int32_t parameterStart;
		int32_t genericContainerIndex;
		uint32_t token;
		uint16_t flags;
		uint16_t iflags;
		uint16_t slot;
		uint16_t parameterCount;
	};

	struct StringLiteral {
		uint32_t length;
		int32_t dataIndex;
	};

	struct CodeGenModule {
		const char* moduleName;
		int64_t methodPointerCount;
		uint64_t* methodPointers;
		int64_t adjustorThunkCount;
		uint64_t adjustorThunks;
		uint64_t invokerIndices;
		uint64_t reversePInvokeWrapperCount;
		uint64_t reversePInvokeWrapperIndices;
		int64_t rgctxRangesCount;
		uint64_t rgctxRanges;
		int64_t rgctxsCount;
		uint64_t rgctxs;
		uint64_t debuggerMetadata;
		uint64_t moduleInitializer;
		uint64_t staticConstructorTypeIndices;
		uint64_t metadataRegistration;
		uint64_t codeRegistaration;
	};

	struct CodeRegistration {
		uint64_t reversePInvokeWrapperCount;
		uint64_t reversePInvokeWrappers;
		uint64_t genericMethodPointersCount;
		uint64_t genericMethodPointers;
		uint64_t genericAdjustorThunks;
		uint64_t invokerPointersCount;
		uint64_t invokerPointers;
		uint64_t unresolvedVirtualCallCount;
		uint64_t unresolvedVirtualCallPointers;
		uint64_t unresolvedInstanceCallPointers;
		uint64_t unresolvedStaticCallPointers;
		uint64_t interopDataCount;
		uint64_t interopData;
		uint64_t windowsRuntimeFactoryCount;
		uint64_t windowsRuntimeFactoryTable;
		uint64_t codeGenModulesCount;
		uint64_t codeGenModules;
	};

	struct MetadataRegistration {
		int64_t genericClassesCount;
		uint64_t genericClasses;
		int64_t genericInstsCount;
		uint64_t genericInsts;
		int64_t genericMethodTableCount;
		uint64_t genericMethodTable;
		int64_t typesCount;
		uint64_t types;
		int64_t methodSpecsCount;
		uint64_t methodSpecs;
		int64_t fieldOffsetsCount;
		uint64_t fieldOffsets;
		int64_t typeDefinitionsSizesCount;
		uint64_t typeDefinitionsSizes;
	};

	struct Type {
		uint64_t datapoint;
		uint32_t bits;
	};
#pragma pack( pop )

	class Reader {
	public:
		Reader(const char* fileName, uint32_t flags = 0);
		~Reader();

		uint8_t* Data() const {
			return m_data;
		}

		uint32_t Size() const {
			return m_size;
		}

	private:
		HANDLE m_file;
		HANDLE m_mapping;
		uint8_t* m_data;
		uint32_t m_size;
	};

	class Parser {
	public:
		Parser(const char* assembly, const char* metadata);

		const char* GetString(uint32_t index);
		Type* GetType(uint32_t typeIndex);
		TypeDefinition* GetTypeDef(uint32_t typeDefIndex);
		ImageDefinition* GetTypeDefImage(uint32_t typeDefIndex);
		StringLiteral* GetStringLit(uint32_t stringLitIndex);
		CodeGenModule* GetCodeGenModule(ImageDefinition* image);
		uint32_t GetMethodRva(CodeGenModule* codeGenMod, MethodDefinition* methodDef);
		bool GetTypeDefName(TypeDefinition* typeDef, char* typeName);
		bool GetStringLitValue(StringLiteral* stringLit, char* stringLitValue);
		uint32_t FindType(const char* matchName, const char* matchNs = nullptr, int index = 0);

		template < typename T >
		void ParseType(uint32_t typeDefIndex, T&& callback) {
			if (!m_valid)
				return;

			TypeDefinition* typeDef = GetTypeDef(typeDefIndex);
			if (!typeDef)
				return;

			char typeDefName[128]{};
			if (!GetTypeDefName(typeDef, typeDefName))
				return;

			if (typeDef->fieldStart == -1 || typeDef->field_count == -1)
				return;

			if (typeDefIndex >= m_fieldOffsetsCount)
				return;

			uint32_t* typeDefOffsets = (uint32_t*)m_fieldOffsets[typeDefIndex];
			if (!typeDefOffsets)
				return;

			for (uint32_t n = typeDef->fieldStart, i = n, j = (n + typeDef->field_count); i < j; i++) {
				FieldDefinition* fieldDef = &m_fieldDefs[i];
				const char* fieldName = GetString(fieldDef->nameIndex);
				uint32_t fieldOffset = typeDefOffsets[i - n];

				callback(typeDefName, fieldName, fieldOffset);
			}
		}

		template < typename T >
		void ParseTypeMethods(uint32_t typeDefIndex, T&& callback) {
			if (!m_valid)
				return;

			TypeDefinition* typeDef = GetTypeDef(typeDefIndex);
			if (!typeDef)
				return;

			ImageDefinition* image = GetTypeDefImage(typeDefIndex);
			if (!image)
				return;

			CodeGenModule* codeGenMod = GetCodeGenModule(image);
			if (!codeGenMod)
				return;

			char typeDefName[128]{};
			if (!GetTypeDefName(typeDef, typeDefName))
				return;

			if (typeDef->methodStart == -1 || typeDef->method_count == -1)
				return;

			for (uint32_t n = typeDef->methodStart, i = n, j = (n + typeDef->method_count); i < j;
				i++) {
				MethodDefinition* methodDef = &m_methodDefs[i];
				const char* methodName = GetString(methodDef->nameIndex);
				uint32_t methodRva = GetMethodRva(codeGenMod, methodDef);

				callback(typeDefName, methodName, methodRva);
			}
		}

		template < typename T >
		void ParseTypes(T&& callback) {
			if (!m_valid)
				return;

			for (uint32_t i = 0; i < m_typeDefCount; i++)
				ParseType(i, callback);
		}

		template < typename T >
		void ParseMetadataUsage(T&& callback) {
			for (uint32_t i = 0; i < m_dataSize; i += 8) {
				uint64_t encodedToken = *(uint64_t*)&m_data[i];
				uint64_t usage = (encodedToken & 0xE0000000) >> 29;

				if (usage > 0 && usage <= 6) {
					uint64_t decodedIndex = (encodedToken & 0x1FFFFFFEU) >> 1;

					if (encodedToken == ((usage << 29ull) | (decodedIndex << 1ull)) + 1) {
						uint32_t rva = m_dataOffset + i;

						callback((Il2Cpp::MetadataUsage)usage, rva, decodedIndex);
					}
				}
			}
		}

		bool Valid() const {
			return m_valid;
		}

	private:
		bool ParseAssembly();
		bool ParseMetadata();

		bool m_valid;
		Reader m_assembly;
		Reader m_metadata;
		uint64_t m_assemblyBase;
		uint32_t m_dataOffset;
		uint8_t* m_data;
		uint32_t m_dataSize;
		char* m_strings;
		ImageDefinition* m_images;
		TypeDefinition* m_typeDefs;
		FieldDefinition* m_fieldDefs;
		MethodDefinition* m_methodDefs;
		StringLiteral* m_stringLits;
		uint8_t* m_stringLitData;
		uint64_t* m_fieldOffsets;
		Type** m_types;
		CodeGenModule** m_codeGenModules;
		uint32_t m_stringsSize;
		uint32_t m_imagesCount;
		uint32_t m_typeDefCount;
		uint32_t m_fieldDefCount;
		uint32_t m_methodDefCount;
		uint32_t m_stringLitCount;
		uint32_t m_stringLitDataSize;
		uint32_t m_fieldOffsetsCount;
		uint32_t m_typesCount;
		uint32_t m_codeGenModulesCount;
	};

	IL2CPP_INLINE bool ComparePattern(uint8_t* base, uint8_t* pattern, size_t mask) {
		for (; mask; ++base, ++pattern, mask--) {
			if (*pattern != 0xCC && *base != *pattern)
				return false;
		}

		return true;
	}

	IL2CPP_INLINE uint8_t* FindPattern(uint8_t* base, size_t size, uint8_t* pattern, size_t mask) {
		size -= mask;

		for (size_t i = 0; i <= size; ++i) {
			uint8_t* addr = &base[i];

			if (ComparePattern(addr, pattern, mask))
				return addr;
		}

		return nullptr;
	}

	IL2CPP_INLINE uint8_t* Relative32(uint8_t* inst, uint32_t offset) {
		int32_t relativeOffset = *(int32_t*)(inst + offset);

		return (inst + relativeOffset + offset + sizeof(int32_t));
	}
};

#define IL2CPP_FIND_PATTERN( base, size, sig )                                                                         \
	Il2Cpp::FindPattern( ( uint8_t* ) base, size, ( uint8_t* ) sig, sizeof( sig ) - 1 )
