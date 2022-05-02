#include <stdint.h>
#include "../../defs.h"

int main()
{
    return 0;
}

extern "C" void shellMain(SHELL_CODE_DATA* data)
{
    FString ret;
    FName name { data->index, 0 };

    void (*FNameToString)(void* _this, FString& out) = (void (*)(void* _this, FString& out))0xD1D1D1D1D1D1;
    FNameToString(&name, ret);

    data->ret = ret.Data;
}