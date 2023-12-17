# ExampleLuaLib

This repository showcases the minimum steps required to create a Lua C library on Windows using Lua for Windows and CMake.
However, the code still attempts to be cross-platform where it can, so few changes are required to compile on other platforms.
The name "mylualib" will be used as an alias for the name of the library you want to create.

## High-Level Steps
1. Create a CMakeLists.txt file with mylualib as a library target.
2. Ensure you have a Lua static library (.lib on Windows, .a on Linux) and a directory with the lua headers.
4. Modify the CMakeLists.txt file to link the library and include the directory of headers.
5. Compile with CMake. Be sure to match the architectures of the compiler and the lua library match.
6. Copy the resulting shared library to the same directory as your Lua files, and require() it.
