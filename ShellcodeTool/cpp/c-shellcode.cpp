#include "peb-lookup.h"

//"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx86\x64\cl" /c /FA /GS- c-shellcode.cpp
//"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx86\x64\ml64" c-shellcode.asm /link /entry:AlignRSP

struct TArray
{
public:
    wchar_t *Data;
    __int32 Count;
    __int32 Max;
};

struct FName
{
public:
    uint32_t ComparisonIndex;
    uint32_t DisplayIndex;
};

int main()
{
    return 0;
}

extern "C" void *maindeez(uint32_t index)
{
    // Stack based strings for libraries and functions the shellcode needs
    wchar_t kernel32_dll_name[] = {'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l', 0};
    char load_lib_name[] = {'L', 'o', 'a', 'd', 'L', 'i', 'b', 'r', 'a', 'r', 'y', 'A', 0};
    char get_proc_name[] = {'G', 'e', 't', 'P', 'r', 'o', 'c', 'A', 'd', 'd', 'r', 'e', 's', 's', 0};
    char user32_dll_name[] = {'u', 's', 'e', 'r', '3', '2', '.', 'd', 'l', 'l', 0};
    char message_box_name[] = {'M', 'e', 's', 's', 'a', 'g', 'e', 'B', 'o', 'x', 'W', 0};

    // stack based strings to be passed to the messagebox win api
    wchar_t msg_content[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', 0};
    wchar_t msg_title[] = {'K', 'e', 'm', 'o', 0};

    // resolve kernel32 image base
    LPVOID base = get_module_by_name((const LPWSTR)kernel32_dll_name);
    if (!base)
    {
        return nullptr;
    }

    // resolve loadlibraryA() address
    LPVOID load_lib = get_func_by_name((HMODULE)base, (LPSTR)load_lib_name);
    if (!load_lib)
    {
        return nullptr;
    }

    // resolve getprocaddress() address
    LPVOID get_proc = get_func_by_name((HMODULE)base, (LPSTR)get_proc_name);
    if (!get_proc)
    {
        return nullptr;
    }

    // loadlibrarya and getprocaddress function definitions
    HMODULE(WINAPI * _LoadLibraryA)
    (LPCSTR lpLibFileName) =
        (HMODULE(WINAPI *)(LPCSTR))load_lib;
    FARPROC(WINAPI * _GetProcAddress)
    (HMODULE hModule, LPCSTR lpProcName) =
        (FARPROC(WINAPI *)(HMODULE, LPCSTR))get_proc;

    // load user32.dll
    LPVOID u32_dll = _LoadLibraryA(user32_dll_name);

    // messageboxw function definition
    int(WINAPI * _MessageBoxW)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType) =
        (int(WINAPI *)(_In_opt_ HWND, _In_opt_ LPCWSTR, _In_opt_ LPCWSTR, _In_ UINT))_GetProcAddress((HMODULE)u32_dll, message_box_name);

    if (_MessageBoxW == NULL)
        return nullptr;

    TArray temp;
    FName test{index, 0};
    void (*FNameToString)(void *_this, TArray &out) = (void (*)(void *_this, TArray &out))0xD1D1D1D1D1D1;
    FNameToString(&test, temp);

    // invoke the message box winapi
    _MessageBoxW(0, temp.Data, msg_title, MB_OK);

    return temp.Data;
}