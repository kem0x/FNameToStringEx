#include "ShellcodeTool/bin/shellcode.h"
#include "defs.h"
#include "processex.h"

auto FNameToStringEx(ProcessEX* proc, uint32_t index)
{
    static void* Shellcode;
    static SHELL_CODE_DATA scData;
    static void* scDataGame;

    std::wstring ret;

    if (!Shellcode && !scDataGame)
    {
        // Prepare and allocate shellcode only once
        auto FNameToStringAdd = (void*)proc->FindPattern(FNameToStringSig);
        Shellcode::updatePointer(FNameToStringAdd, Shellcode::POINTER_OFFSET);

        Shellcode = proc->Alloc(Shellcode::bytes.size() + 1);
        printf("[+] Allocated Shellcode at: 0x%p\n", Shellcode);

        proc->WPM(Shellcode, Shellcode::bytes.data(), Shellcode::bytes.size() + 1);

        scDataGame = proc->Alloc(sizeof(scData));
        printf("[+] Allocated Shellcode Data at: 0x%p\n", scDataGame);
    }

    scData.index = index;
    proc->WPM(scDataGame, &scData, sizeof(scData));

    HANDLE hThread = proc->CRT(Shellcode, scDataGame);
    if (!hThread)
    {
        if (auto error = GetLastError(); error == ERROR_INVALID_HANDLE || error == ERROR_ACCESS_DENIED)
            printf("[x] Couldn't create thread, please make sure you ran this tool as admin.\n");
        else
            printf("[x] Thread creation failed, Error code: 0x%X\n", error);
            
        proc->Free(Shellcode, Shellcode::bytes.size() + 1);
        return ret;
    }

    WaitForSingleObject(hThread, INFINITE);

    scData = proc->RPM<SHELL_CODE_DATA>(scDataGame);

    if (wchar_t buffer[MAX_FNAME]; proc->RPM(scData.ret, &buffer, sizeof(buffer)))
    {
        ret = buffer;
    }

    return ret;
}

int main()
{
    auto gameProcess = new ProcessEX();

    while (true)
    {
        // Try 2488

        printf("[=] Enter index: ");
        uint32_t index;
        std::cin >> index;

        if (index == -1)
            break;

        if (auto name = FNameToStringEx(gameProcess, index); !name.empty())
        {
            printf("[+] Name: %ls\n", name.c_str());
        }
    }

    Sleep(-1);

    return 0;
}
