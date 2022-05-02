#include <stdint.h>
#include "../../defs.h"

//No external dependencies, No global\static variables, only stack based strings unless you know what you're doing.

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