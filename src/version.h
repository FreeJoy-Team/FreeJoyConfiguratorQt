#ifndef VERSION_H
#define VERSION_H

#define MAJOR_VERSION 1
#define MINOR_VERSION 7
#define PATCH_VERSION 0
#define CONFIGURATOR_VERSION 1

#define str(s) #s
#define xstr(s) str(s)
#define APP_VERSION xstr(MAJOR_VERSION) "." xstr(MINOR_VERSION) "." xstr(PATCH_VERSION) "b" xstr(CONFIGURATOR_VERSION)

// If it is defined by the compiler, then it is a nightly build, and in the YYYYMMDD format.
#ifndef BUILD_VERSION
    #define BUILD_VERSION 0
#endif

#endif // VERSION_H
