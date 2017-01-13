#pragma once

#define CPPSTREAM_PP_STRINGIZE(str) #str

#define CPPSTREAM_PP_CONCAT_IMPL(a, b) a ## b
#define CPPSTREAM_PP_CONCAT(a, b) CPPSTREAM_PP_CONCAT_IMPL(a, b)

#ifdef __COUNTER__
#   define CPPSTREAM_ANONYMOUS_VAR(name) CPPSTREAM_PP_CONCAT(name, __COUNTER__)
#else
#   define CPPSTREAM_ANONYMOUS_VAR(name) CPPSTREAM_PP_CONCAT(name, __LINE__)
#endif
