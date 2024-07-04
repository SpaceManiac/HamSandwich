# Use i686-w64-mingw32-gcc.
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_C_COMPILER_TARGET i686-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET ${CMAKE_C_COMPILER_TARGET})
set(CMAKE_C_FLAGS_INIT "-fms-compatibility /imsvc/xwin/crt/include /imsvc/xwin/sdk/include/ucrt /imsvc/xwin/sdk/include/um /imsvc/xwin/sdk/include/shared")
set(CMAKE_CXX_FLAGS_INIT ${CMAKE_C_FLAGS_INIT})
set(CMAKE_LINKER lld-link)
set(CMAKE_MT llvm-mt)  # CMake can find rc=llvm-rc all by itself, but needs to be told mt=llvm-mt
set(CMAKE_EXE_LINKER_FLAGS_INIT "/LIBPATH:/xwin/crt/lib/x86 /LIBPATH:/xwin/sdk/lib/um/x86 /LIBPATH:/xwin/sdk/lib/ucrt/x86")
set(CMAKE_MODULE_LINKER_FLAGS_INIT ${CMAKE_EXE_LINKER_FLAGS_INIT})
set(CMAKE_SHARED_LINKER_FLAGS_INIT ${CMAKE_EXE_LINKER_FLAGS_INIT})
