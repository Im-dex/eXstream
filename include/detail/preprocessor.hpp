#pragma once

#define CPPSTREAM_PP_STRINGIZE(str) #str

#define CPPSTREAM_PP_CONCAT_IMPL(a, b) a ## b
#define CPPSTREAM_PP_CONCAT(a, b) CPPSTREAM_PP_CONCAT_IMPL(a, b)
