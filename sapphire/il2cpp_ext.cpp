#include "il2cpp_ext.hpp"
#include <stdio.h>

#include <winternl.h>
#pragma comment( lib, "ntdll.lib" )

Il2Cpp::Reader::Reader(const char* fileName, uint32_t flags)
	: m_file(INVALID_HANDLE_VALUE), m_mapping(INVALID_HANDLE_VALUE), m_data(nullptr), m_size(0) {
	ANSI_STRING aname{};
	UNICODE_STRING name{};
	OBJECT_ATTRIBUTES oa{};
	IO_STATUS_BLOCK iosb{};

	RtlInitAnsiString(&aname, fileName);
	RtlAnsiStringToUnicodeString(&name, &aname, true);
	InitializeObjectAttributes(&oa, &name, OBJ_CASE_INSENSITIVE, nullptr, nullptr);
	NTSTATUS status =
		NtOpenFile(&m_file, FILE_GENERIC_READ, &oa, &iosb, FILE_SHARE_READ, FILE_NON_DIRECTORY_FILE);
	RtlFreeUnicodeString(&name);

	if (!NT_SUCCESS(status)) {
		return;
	}

	m_size = GetFileSize(m_file, nullptr);

	if (m_size == INVALID_FILE_SIZE || m_size == 0) {
		CloseHandle(m_file);

		return;
	}

	m_mapping = CreateFileMappingA(m_file, nullptr, PAGE_READONLY | flags, 0, m_size, nullptr);

	if (!m_mapping) {
		CloseHandle(m_file);

		return;
	}

	m_data = (uint8_t*)MapViewOfFile(m_mapping, FILE_MAP_COPY, 0, 0, m_size);
}

Il2Cpp::Reader::~Reader() {
	if (m_data) {
		UnmapViewOfFile(m_data);

		m_data = nullptr;
	}

	if (m_mapping != INVALID_HANDLE_VALUE) {
		CloseHandle(m_mapping);
	}

	if (m_file != INVALID_HANDLE_VALUE) {
		CloseHandle(m_file);
	}
}

Il2Cpp::Parser::Parser(const char* assembly, const char* metadata)
	: m_valid(false), m_assembly(assembly, SEC_IMAGE), m_metadata(metadata) {
	if (ParseAssembly() && ParseMetadata())
		m_valid = true;
}

const char* Il2Cpp::Parser::GetString(uint32_t index) {
	if (!m_valid)
		return nullptr;

	if (index == -1 || index > m_stringsSize)
		return nullptr;

	const char* str = &m_strings[index];
	if (!strlen(str))
		return nullptr;

	return str;
}

Il2Cpp::Type* Il2Cpp::Parser::GetType(uint32_t typeIndex) {
	if (typeIndex >= m_typesCount)
		return nullptr;

	return m_types[typeIndex];
}

Il2Cpp::TypeDefinition* Il2Cpp::Parser::GetTypeDef(uint32_t typeDefIndex) {
	if (typeDefIndex >= m_typeDefCount)
		return nullptr;

	return &m_typeDefs[typeDefIndex];
}

Il2Cpp::ImageDefinition* Il2Cpp::Parser::GetTypeDefImage(uint32_t typeDefIndex) {
	if (typeDefIndex >= m_typeDefCount)
		return nullptr;

	for (uint32_t i = 0; i < m_imagesCount; i++) {
		ImageDefinition* image = &m_images[i];

		if (typeDefIndex >= image->typeStart && typeDefIndex < (image->typeStart + image->typeCount))
			return image;
	}

	return nullptr;
}

Il2Cpp::StringLiteral* Il2Cpp::Parser::GetStringLit(uint32_t stringLitIndex) {
	if (stringLitIndex >= m_stringLitCount)
		return nullptr;

	return &m_stringLits[stringLitIndex];
}

Il2Cpp::CodeGenModule* Il2Cpp::Parser::GetCodeGenModule(ImageDefinition* image) {
	const char* imageName = GetString(image->nameIndex);
	if (!imageName)
		return nullptr;

	for (uint32_t i = 0; i < m_codeGenModulesCount; i++) {
		CodeGenModule* codeGenMod = m_codeGenModules[i];

		if (codeGenMod->moduleName && strcmp(codeGenMod->moduleName, imageName) == 0)
			return codeGenMod;
	}

	return nullptr;
}

uint32_t Il2Cpp::Parser::GetMethodRva(CodeGenModule* codeGenMod, MethodDefinition* methodDef) {
	uint32_t methodToken = methodDef->token;
	uint32_t methodPointerIndex = methodToken & 0x00FFFFFFu;
	uint64_t ptr = codeGenMod->methodPointers[methodPointerIndex - 1];

	if (!ptr)
		return 0;

	return (ptr - m_assemblyBase) & 0xFFFFFFFF;
}

bool Il2Cpp::Parser::GetTypeDefName(TypeDefinition* typeDef, char* typeName) {
	const char* name = GetString(typeDef->nameIndex);
	if (!name)
		return false;

	const char* ns = GetString(typeDef->namespaceIndex);
	if (ns) {
		strcpy(typeName, ns);
		strcat(typeName, "::");
		strcat(typeName, name);
	}
	else {
		strcpy(typeName, name);
	}

	return true;
}

bool Il2Cpp::Parser::GetStringLitValue(StringLiteral* stringLit, char* stringLitValue) {
	uint32_t len = stringLit->length;
	if ((len + 1) > 1024) {
		return false;
	}

	uint8_t* value = &m_stringLitData[stringLit->dataIndex];
	memcpy(stringLitValue, value, len);
	stringLitValue[len] = 0;

	return true;
}

uint32_t Il2Cpp::Parser::FindType(const char* matchName, const char* matchNs, int index) {
	if (!m_valid)
		return -1;

	int localIndex = 0;

	for (uint32_t i = 0; i < m_typeDefCount; i++) {
		TypeDefinition* typeDef = &m_typeDefs[i];

		const char* name = GetString(typeDef->nameIndex);
		if (!name || strcmp(name, matchName) != 0)
			continue;

		if (matchNs) {
			const char* ns = GetString(typeDef->namespaceIndex);

			if (!ns || strcmp(ns, matchNs) != 0) {
				continue;
			}
		}
		else if (typeDef->namespaceIndex == -1) {
			continue;
		}

		if (index != 0) {
			localIndex++;

			if (localIndex == index)
				return i;
			else
				continue;
		}

		return i;
	}

	return -1;
}

bool Il2Cpp::Parser::ParseAssembly() {
	// Check assembly is valid
	uint8_t* assemblyBase = m_assembly.Data();
	if (!assemblyBase)
		return false;

	// Check dos header
	IMAGE_DOS_HEADER* dosHdr = (IMAGE_DOS_HEADER*)assemblyBase;
	if (dosHdr->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	// Check nt header
	IMAGE_NT_HEADERS64* ntHdrs = (IMAGE_NT_HEADERS64*)(assemblyBase + dosHdr->e_lfanew);
	if (ntHdrs->Signature != IMAGE_NT_SIGNATURE)
		return false;

	// Find the data section
	IMAGE_SECTION_HEADER* sections = (IMAGE_SECTION_HEADER*)(uintptr_t(&ntHdrs->OptionalHeader) +
		ntHdrs->FileHeader.SizeOfOptionalHeader);

	IMAGE_SECTION_HEADER* dataSection = nullptr;
	for (uint32_t i = 0; i < ntHdrs->FileHeader.NumberOfSections; i++) {
		IMAGE_SECTION_HEADER* section = &sections[i];

		if (memcmp(section->Name, ".data", 5) == 0) {
			dataSection = section;
			break;
		}
	}

	// Verify the data section
	if (!dataSection || !dataSection->VirtualAddress)
		return false;

	m_assemblyBase = (uint64_t)assemblyBase;
	m_dataOffset = dataSection->VirtualAddress;
	m_data = &assemblyBase[m_dataOffset];
	m_dataSize = max(dataSection->Misc.VirtualSize, dataSection->SizeOfRawData);

	Il2Cpp::MetadataRegistration* metadataReg = nullptr;
	Il2Cpp::CodeRegistration* codeReg = nullptr;

	/*
	.text:00000001801DD1F0                 lea     r8, unk_182ACC918
	.text:00000001801DD1F7                 lea     rdx, g_metadataRegistration
	.text:00000001801DD1FE                 lea     rcx, g_codeRegistration
	.text:00000001801DD205                 jmp     loc_1802F3490
	*/

	// Find the metadata registration
	uint8_t* result = IL2CPP_FIND_PATTERN( assemblyBase,
		m_assembly.Size(),
		"\x4C\x8D\x05\xCC\xCC\xCC\xCC\x48\x8D\x15\xCC\xCC\xCC\xCC\x48"
		"\x8D\x0D\xCC\xCC\xCC\xCC\xE9" );
	if ( result ) {
		metadataReg = ( Il2Cpp::MetadataRegistration* )Relative32( result + 7, 3 );
		codeReg = ( Il2Cpp::CodeRegistration* )Relative32( result + 14, 3 );
	}
	else {
		/*
		.text:00000001804B4597                 lea     rcx, unk_184D743B0
		.text:00000001804B459E                 mov     cs:qword_185FE0868, rcx
		.text:00000001804B45A5                 lea     rax, unk_1855A6820
		.text:00000001804B45AC                 mov     cs:g_metadataRegPtr, rax
		*/

		result = IL2CPP_FIND_PATTERN( assemblyBase, m_assembly.Size(),
			"\x48\x8D\x0D\xCC\xCC\xCC\xCC\x48\x89\x0D\xCC\xCC\xCC\xCC\x48"
			"\x8D\x05\xCC\xCC\xCC\xCC\x48\x89\x05" );

		if ( result ) {
			codeReg = ( Il2Cpp::CodeRegistration* )Relative32( result, 3 );
			metadataReg = ( Il2Cpp::MetadataRegistration* )Relative32( result + 14, 3 );
		}
		else {
			return false;
		}
	}

	// Populate data from registration
	m_types = (Il2Cpp::Type**)metadataReg->types;
	m_typesCount = (uint32_t)metadataReg->typesCount;
	m_fieldOffsets = (uint64_t*)metadataReg->fieldOffsets;
	m_fieldOffsetsCount = (uint32_t)metadataReg->fieldOffsetsCount;

	m_codeGenModules = (Il2Cpp::CodeGenModule**)codeReg->codeGenModules;
	m_codeGenModulesCount = (uint32_t)codeReg->codeGenModulesCount;

	return true;
}

bool Il2Cpp::Parser::ParseMetadata() {
	// Check metadata is valid
	uint8_t* metadataBase = (uint8_t*)m_metadata.Data();
	if (!metadataBase)
		return false;

	// Check header is valid
	Il2Cpp::GlobalMetadataHeader* metadataHdr = (Il2Cpp::GlobalMetadataHeader*)metadataBase;
	if (metadataHdr->sanity != 0xFAB11BAF && metadataHdr->version != 29)
		return false;

	// Populate data from header
	m_strings = (char*)&metadataBase[metadataHdr->stringOffset];
	m_images = (ImageDefinition*)&metadataBase[metadataHdr->imagesOffset];
	m_typeDefs = (TypeDefinition*)&metadataBase[metadataHdr->typeDefinitionsOffset];
	m_fieldDefs = (FieldDefinition*)&metadataBase[metadataHdr->fieldsOffset];
	m_methodDefs = (MethodDefinition*)&metadataBase[metadataHdr->methodsOffset];
	m_stringLits = (StringLiteral*)&metadataBase[metadataHdr->stringLiteralOffset];
	m_stringLitData = (uint8_t*)&metadataBase[metadataHdr->stringLiteralDataOffset];
	m_stringsSize = metadataHdr->stringSize;
	m_imagesCount = metadataHdr->imagesSize / sizeof(ImageDefinition);
	m_typeDefCount = metadataHdr->typeDefinitionsSize / sizeof(TypeDefinition);
	m_fieldDefCount = metadataHdr->fieldsSize / sizeof(FieldDefinition);
	m_methodDefCount = metadataHdr->methodsSize / sizeof(MethodDefinition);
	m_stringLitCount = metadataHdr->stringLiteralSize / sizeof(StringLiteral);
	m_stringLitDataSize = metadataHdr->stringLiteralDataSize;

	return true;
}
