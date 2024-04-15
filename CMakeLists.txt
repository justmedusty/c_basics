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
        BasicUnixSystemCalls.c
        FetchUnixStreamSocketAddress.c
        EndiannessConversion.c
        UnixStreamSocketServer.c
        UnixStreamSocketClient.c
        UnixDatagramSocketServer.c
        UnixDatagramSocketClient.c
        UnixMutex.c
        UnixSemaphores.c
        UnixFileEventWatching.c
        PosixThreads.c
        UnixSocketsForIPC.c
        TelnetServerPollingFileDescriptorsWithUnixStreamStreamSockets.c
        UnixPortability.c
        UnixSocketSendAllBytes.c
        QuickAndDirtyPortableFloatingPoint.c
        UnixDatagramSocketBroadcaster.c
        UnixStreamTelnetServerWithSelect.c
        Setbuf&Setvbuf.c
        StreamOrientation.c
        Putchar&Ungetc.c
        HeaderIntroduction.c
        HeaderIntroduction.h
        UsingOurHeaderFile.c
        UnixTelnetClient.c
        UnixUserAuthentication.c
        AlarmSignal.c
        Forking.c
        Pipes.c
        FIFO.c
        ProcessGroups.c
        Exec.c
        DontRunThis.c
        UnixSharedMemory.c
        SystemVMessageQueues.c
        StackAllocation.c
        Gotos.c
        MemoryMapping.c)
