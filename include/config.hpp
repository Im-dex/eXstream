#pragma once

#ifdef _MSC_VER
#   define STREAM_MSVC
#elif defined __clang__
#   define STREAM_CLANG
#elif defined __GNUC__
#   define STREAM_GCC
#else
#   error "Unsupported conpiler"
#endif

#ifdef NDEBUG
#   define STREAM_RELEASE
#else
#   define STREAM_DEBUG
#endif

#ifdef STREAM_MSVC
#   define STREAM_SUPRESS_ALL_WARNINGS\
        __pragma(warning(push, 0))\
        __pragma(warning(disable:4710))\
        __pragma(warning(disable:4548))\
        __pragma(warning(disable:4711))
#   define STREAM_RESTORE_ALL_WARNINGS __pragma(warning(pop))

#   define STREAM_NO_VTABLE __declspec(novtable)
#   define STREAM_FORCEINLINE __forceinline
#endif

#if defined(STREAM_GCC) || defined(STREAM_CLANG)
#   define STREAM_SUPRESS_ALL_WARNINGS _Pragma("GCC diagnostic push")
#   define STREAM_RESTORE_ALL_WARNINGS _Pragma("GCC diagnostic pop")
#   define STREAM_NO_VTABLE
#   define STREAM_FORCEINLINE __attribute__((always_inline))
#endif

#define STREAM_STRINGIZE(str) #str
