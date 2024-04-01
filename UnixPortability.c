//
// Created by dustyn on 4/1/24.
//

/*
 * Portability across unix-like systems can be important. One way to do this can be to run checks against the
 * macros defined by the compiler the user is utilizing. It is possible for these macros to not provide correct
 * information however we will ignore this for now. We can utilize pre-processor directives to choose how
 * we want to define our OS macro to let us know which OS the user is one which can be helpful since certain things
 * may differ. Even simple things such as types can differ. Some operating systems may have long values be of differing size
 * compared to others, integers may be different etc. This is generally covered by size-specific types such as int8_t (8 bit signed) uint8_t(8 bit unsigned) , int16_t(16 bit signed)
 * uint16_t (unsigned) etc etc. You use these kinds of pre-processor directives to store information about the user operating system and make sure that
 * any compatability issues are handled. Portability is an important aspect of the entire idea of Unix and posix-compliant systems. We wouldn't want to
 * sacrifice it.
 *
 */

#ifdef __FreeBSD__
    #define OS "FreeBSD"
#elif __OpenBSD__
    #define OS "OpenBSD"
#elif __NetBSD__
    #define OS "NetBSD"
#elif __DragonFly__
    #define OS DragonFly
#elif __linux__
    #define OS "Linux"
#elif __minix
    #define OS "Minix"
#elif sinux
    #define OS "ReliantUnix"
elif __sun
    #define OS "Solaris"
#elif _UNIXWARE7
    #define  OS "UnixWare"
#endif