// teensy_compat.h
#pragma once

#if defined(__cplusplus)
  #include <cstdarg>
  extern "C" {
    int  vdprintf(int, const char*, va_list);
    char* itoa(int, char*, int);
    char* utoa(unsigned, char*, int);
    char* ltoa(long, char*, int);
    char* ultoa(unsigned long, char*, int);
  }
#else
  #include <stdarg.h>
  int  vdprintf(int, const char*, va_list);
  char* itoa(int, char*, int);
  char* utoa(unsigned, char*, int);
  char* ltoa(long, char*, int);
  char* ultoa(unsigned long, char*, int);
#endif