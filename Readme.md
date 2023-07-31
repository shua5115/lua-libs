# Lua Libs

This repository is a collection of Lua libraries in one environment.
This allows for the lua version dependency to be set at the top level,
which then applies to all the libraries.

The lua files precompiled for Windows come from the [love megasource repository](https://github.com/love2d/megasource)
compiled for the x64 architecture. If you need an x86 version,
either install Lua for Windows, which installs a 32-bit version of lua by default,
or compile megasource yourself for your preferred architecture.

## Tips
Let's say you have a library with the name "mylib"
- The name of the function `luaopen_mylib` is very important. It must match the filename of your shared library (mylib.dll or mylib.so).
  In addition, it must be required at the top level of your lua project, meaning that you must have `require "mylib"` as the require statement.
  You can't nest it in folders, then call something like `require "libs.mylib"`, because then the dynamic loader will search for a function called
  "luaopen_libs_mylib", which does not exist. The reason the loader works like this is beyond me.
  Why not just take the final segment of the require path?
- Any library dependencies of your shared library should be packaged alongside it,
  because otherwise your library might not load on machines without your fancy develpment environment, or at all.
