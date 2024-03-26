cmake_minimum_required(VERSION 3.27)
project(c_basics C)

set(CMAKE_C_STANDARD 11)

add_executable(c_basics LinkedList.c
        Pointers.c
        BitwiseOperations.c
FileIO.c
FunctionPointers.c
PreProcessorDirectives.c
PointerArithmetic.c
MemoryManagement.c
        UnixSystemCalls.c
        FetchUnixStreamSocketAddress.c)
