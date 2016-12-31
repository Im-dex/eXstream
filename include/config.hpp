#pragma once

#ifdef _MSC_VER
#   define CPPSTREAM_MSVC
#elif defined __clang__
#   define CPPSTREAM_CLANG
#elif defined __GNUC__
#   define CPPSTREAM_GCC
#else
#   error "Unsupported conpiler"
#endif

#ifdef NDEBUG
#   define CPPSTREAM_RELEASE
#else
#   define CPPSTREAM_DEBUG
#endif

#ifdef CPPSTREAM_MSVC
#   define CPPSTREAM_SUPPRESS_ALL_WARNINGS\
        __pragma(warning(push, 0))\
        __pragma(warning(disable:4710))\
        __pragma(warning(disable:4548))\
        __pragma(warning(disable:4711))
#   define CPPSTREAM_RESTORE_ALL_WARNINGS __pragma(warning(pop))

#   define CPPSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(warnings)\
        __pragma(warning(push))\
        __pragma(warning(disable: warnings))
#   define CPPSTREAM_MSVC_WARNINGS_POP __pragma(warning(pop))

#   define CPPSTREAM_FORCEINLINE __forceinline
#endif

#if defined(CPPSTREAM_GCC) || defined(CPPSTREAM_CLANG)
#   define CPPSTREAM_SUPPRESS_ALL_WARNINGS _Pragma("GCC diagnostic push")
#   define CPPSTREAM_RESTORE_ALL_WARNINGS _Pragma("GCC diagnostic pop")
#   define CPPSTREAM_FORCEINLINE __attribute__((always_inline))
#endif

#define CPPSTREAM_UNUSED(var) (void) var;
