# ExampleLuaLib

This repository showcases the minimum steps required to create a Lua C library on Windows using Lua for Windows and CMake.
However, the code still attempts to be cross-platform where it can, so few changes are required to compile on other platforms.
The name "mylualib" will be used as an alias for the name of the library you want to create.

## High-Level Steps
1. Create a CMakeLists.txt file with mylualib as a library target.
2. Ensure you have a Lua static library (.lib on Windows, .a on Linux) and a directory with the lua headers.
   - Precompiled binaries (Lua for Windows, Lua packages on Linux)
   - Follow compilation instructions online
4. Modify the CMakeLists.txt file to link the library and include the directory of headers.
5. Compile with CMake. If using the precompiled binaries from Lua for Windows, compile as x86 (32-bit) for proper linking.
   If you compile Lua some other way, be sure to match the architecture.
6. Copy the resulting shared library to the same directory as your Lua files, and try to require() them.

Note: the CMakeLists.txt file uses absolute Windows paths to find the Lua for Windows binaries and headers.
I am aware that there is a more proper way to find these resources with CMake (find_package), but I have only
been able to get that working on Linux. If you are aware of a way to find these resources with less hard-coding,
please submit a pull request.
