# FNameToStringEx

- A PoC for calling [FName::ToString](https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Core/Private/UObject/UnrealNames.cpp#L2278) externally.

## - The pitch

- FNameToStringEx is designed to take a different approach aiming to be more efficient and to avoid the instability caused by games updates, unlike other ways that are used commonly.

- This PoC uses shellcode and was tested on fortnite (it should work on all ue4\5 games with minimal changes), The shellcode is a simple C++ function that calls FName::ToString internally. However, this project was setup with a fully  automated workflow that avoids the need for writing assembly code and instead working with c++ directly.

## - Building

- To build the Main project, use Visual Studio\Code with C++23 support and compile on x64.

- To build the Shellcode tool, Update the paths at [Program.cs](/ShellcodeTool/Program.cs), [build1.bat](/ShellcodeTool/cpp/build1.bat) and [build2.bat](/ShellcodeTool/cpp/build2.bat) to yours, use Visual Studio\Code with C# support, install `Penet` through nuget package installer and compile on x64.

- To build the [shellcode](/ShellcodeTool/cpp/c-shellcode.cpp), Run the Shellcode tool you compiled yourself and it should print that it wrote the header file successfully, then recompile the main project.


## - License

- This project is licensed under the [MIT License](/LICENSE), You are allowed to use this project in any way you want **with credit**.

