#pragma once

struct HiddenValue
{
	uint8_t pad_0x10[ 0x10 ]{ };
	uint32_t AccessCount;
	uint64_t Key;
};