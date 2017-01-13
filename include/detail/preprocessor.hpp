#pragma once

#define EXSTREAM_PP_STRINGIZE(str) #str

#define EXSTREAM_PP_CONCAT_IMPL(a, b) a ## b
#define EXSTREAM_PP_CONCAT(a, b) EXSTREAM_PP_CONCAT_IMPL(a, b)

#ifdef __COUNTER__
#   define EXSTREAM_ANONYMOUS_VAR(name) EXSTREAM_PP_CONCAT(name, __COUNTER__)
#else
#   define EXSTREAM_ANONYMOUS_VAR(name) EXSTREAM_PP_CONCAT(name, __LINE__)
#endif
