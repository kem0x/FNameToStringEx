#pragma once

#define MAX_FNAME 1024

constexpr const char* FNameToStringSig = "48 89 ? ? ? 56 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? ? 83 79 04";

struct FString
{
    wchar_t* Data;
    __int32 Count;
    __int32 Max;
};

struct FName
{
    uint32_t ComparisonIndex;
    uint32_t DisplayIndex;
};

struct SHELL_CODE_DATA
{
    uint32_t index;
    wchar_t* ret;
};