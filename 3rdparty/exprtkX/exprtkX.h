#pragma once

#if defined(EXPRTKX_LIBRARY)
#  if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#    if defined(BUILD_EXPRTKX)
#       define EXPRTKX_EXPORT __declspec(dllexport)
#    else
#       define EXPRTKX_EXPORT __declspec(dllimport)
#    endif

#  else
#    define EXPRTKX_EXPORT __attribute__((visibility("default")))
#  endif

#else
#  define EXPRTKX_EXPORT 
#endif


#if defined(USE_LONG_DOUBLE)
#  define REAL long double
#elif defined(USE_FLOAT)
#  define REAL float
#else
#  define REAL double
#endif
