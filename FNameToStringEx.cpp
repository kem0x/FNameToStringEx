#include "processex.h"
#include "shellcode.h"

int main()
{
    auto gameProcess = new ProcessEX();

    auto FNameToStringAdd = (void*)gameProcess->FindPattern("48 89 ? ? ? 56 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? ? 83 79 04");

    Shellcode::updatePointer(FNameToStringAdd, Shellcode::POINTER_OFFSET);

    void* pShellcode = gameProcess->Alloc(Shellcode::bytes.size() + 1);

    printf("[+] Allocated Shellcode at: %p\n", pShellcode);

    gameProcess->WPM(pShellcode, Shellcode::bytes.data(), Shellcode::bytes.size() + 1);

    while (true)
    {
        static uint32_t index;

        std::cin >> index;

        HANDLE hThread = gameProcess->CRT(pShellcode, (void*)index);
        if (!hThread)
        {
            printf("Thread creation failed 0x%X\n", GetLastError());
            VirtualFreeEx(gameProcess->handle, pShellcode, 0, MEM_RELEASE);
            return 1;
        }

        WaitForSingleObject(hThread, INFINITE);
    }

    return 0;
}
