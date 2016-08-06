#pragma once
#ifdef _WIN32
  #define libff7_export __declspec(dllexport)
  #define libff7_import __declspec(dllimport)
#else
  #define libff7_export __attribute__ ((visibility ("default")))
  #define libff7_import __attribute__ ((visibility ("default")))
#endif

#ifndef libff7
  #ifdef STATIC_LIB
    #define libff7
  #else
    #ifdef libff7_source
      #define libff7 libff7_export
    #else
      #define libff7 libff7_import
    #endif
  #endif
#endif

