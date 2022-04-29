#pragma once
#include "framework.h"

class ProcessEX
{
public:
    HANDLE handle;
    DWORD pid;
    HWND hwnd;
    uint64_t moduleBase, modulesize;

    template <typename T>
    T RPM(void* address)
    {
        T buffer = {};
        ReadProcessMemory(handle, address, &buffer, sizeof(T), nullptr);
        return buffer;
    }

    auto WPM(void* address, void* buffer, size_t size)
    {
        return WriteProcessMemory(handle, address, buffer, size, nullptr);
    }

    auto Alloc(size_t size, DWORD allocationType = MEM_COMMIT | MEM_RESERVE, DWORD protection = PAGE_EXECUTE_READWRITE, void* address = nullptr)
    {
        return VirtualAllocEx(this->handle, address, size, allocationType, protection);
    }

    auto CRT(void* address, void* param = nullptr)
    {
        return CreateRemoteThread(this->handle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), param, 0, nullptr);
    }

    auto setCurrModule(const wchar_t* modName)
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->pid);

        if (hSnapShot != INVALID_HANDLE_VALUE)
        {
            MODULEENTRY32 moduleEntry;
            moduleEntry.dwSize = sizeof(moduleEntry);
            if (Module32First(hSnapShot, &moduleEntry))
            {
                do
                {
                    if (!wcscmp(moduleEntry.szModule, modName))
                    {
                        CloseHandle(hSnapShot);
                        modulesize = moduleEntry.modBaseSize;
                        moduleBase = reinterpret_cast<uint64_t>(moduleEntry.modBaseAddr);
                        return;
                    }
                } while (Module32Next(hSnapShot, &moduleEntry));
            }
        }
    }

    ProcessEX()
    {
        this->hwnd = FindWindow((L"UnrealWindow"), (L"Fortnite  "));
        if (!IsWindow(this->hwnd))
        {
            printf("[x] Couldn't find fortnite window, please launch the game with AC off and try again!.\n");
            return;
        }

        GetWindowThreadProcessId(this->hwnd, &this->pid);

        printf("[+] Found the process, PID: %lu\n", this->pid);

        this->handle = OpenProcess(PROCESS_ALL_ACCESS, false, this->pid);

        this->setCurrModule(L"FortniteClient-Win64-Shipping.exe");

        printf("[+] Found the module, base: 0x%p\n", moduleBase);
    }

    uint64_t FindPatternEx(const char* pattern, const char* mask, uint64_t begin, uint64_t end) // https://guidedhacking.com/threads/external-signature-pattern-scan-issues.12618/?view=votes#post-73200
    {
        auto scan = [](const char* pattern, const char* mask, char* begin, unsigned int size) -> char*
        {
            size_t patternLen = strlen(mask);
            for (unsigned int i = 0; i < size - patternLen; i++)
            {
                bool found = true;
                for (unsigned int j = 0; j < patternLen; j++)
                {
                    if (mask[j] != '?' && pattern[j] != *(begin + i + j))
                    {
                        found = false;
                        break;
                    }
                }

                if (found)
                    return (begin + i);
            }
            return nullptr;
        };

        uint64_t match = NULL;
        SIZE_T bytesRead;
        char* buffer = nullptr;
        MEMORY_BASIC_INFORMATION mbi = { 0 };

        uint64_t curr = begin;

        for (uint64_t curr = begin; curr < end; curr += mbi.RegionSize)
        {
            if (!VirtualQueryEx(this->handle, (void*)curr, &mbi, sizeof(mbi)))
                continue;

            if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
                continue;

            buffer = new char[mbi.RegionSize];

            ReadProcessMemory(this->handle, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead);

            char* internalAddr = scan(pattern, mask, buffer, (unsigned int)bytesRead);

            if (internalAddr != nullptr)
            {
                match = curr + (uint64_t)(internalAddr - buffer);
                break;
            }
        }
        delete[] buffer;
        return match;
    }

    uintptr_t FindPattern(const char* sig) // https://guidedhacking.com/threads/universal-pattern-signature-parser.9588/
    {
        char pattern[100];
        char mask[100];

        char lastChar = ' ';
        unsigned int j = 0;

        for (unsigned int i = 0; i < strlen(sig); i++)
        {
            if ((sig[i] == '?' || sig[i] == '*') && (lastChar != '?' && lastChar != '*'))
            {
                pattern[j] = mask[j] = '?';
                j++;
            }

            else if (isspace(lastChar))
            {
                pattern[j] = lastChar = (char)strtol(&sig[i], 0, 16);
                mask[j] = 'x';
                j++;
            }
            lastChar = sig[i];
        }
        pattern[j] = mask[j] = '\0';

        return FindPatternEx(pattern, mask, moduleBase, moduleBase + modulesize);
    }
};
