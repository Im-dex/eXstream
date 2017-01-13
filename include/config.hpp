#pragma once

#ifdef _MSC_VER
#   define EXSTREAM_MSVC
#elif defined __clang__
#   define EXSTREAM_CLANG
#elif defined __GNUC__
#   define EXSTREAM_GCC
#else
#   error "Unsupported conpiler"
#endif

#ifdef NDEBUG
#   define EXSTREAM_RELEASE
#else
#   define EXSTREAM_DEBUG
#endif

#ifdef EXSTREAM_MSVC
#   define EXSTREAM_SUPPRESS_ALL_WARNINGS\
        __pragma(warning(push, 0))\
        __pragma(warning(disable:4710))\
        __pragma(warning(disable:4548))\
        __pragma(warning(disable:4711))
#   define EXSTREAM_RESTORE_ALL_WARNINGS __pragma(warning(pop))

#   define EXSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(warnings)\
        __pragma(warning(push))\
        __pragma(warning(disable: warnings))
#   define EXSTREAM_MSVC_WARNINGS_POP __pragma(warning(pop))

#   define EXSTREAM_FORCEINLINE __forceinline
#endif

#if defined(EXSTREAM_GCC) || defined(EXSTREAM_CLANG)
#   define EXSTREAM_SUPPRESS_ALL_WARNINGS _Pragma("GCC diagnostic push")
#   define EXSTREAM_RESTORE_ALL_WARNINGS _Pragma("GCC diagnostic pop")
#   define EXSTREAM_FORCEINLINE __attribute__((always_inline))
#endif

#define EXSTREAM_UNUSED(var) (void) var;
