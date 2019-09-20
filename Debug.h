#pragma once
#include "MemoryFree.h"

#define DEBUG_PRINT

#ifdef DEBUG_PRINT
  #define PRINT(...)          Serial.print(__VA_ARGS__)
  #define PRINTLN(...)        Serial.println(__VA_ARGS__)
  #define WRITE(...)          Serial.write(__VA_ARGS__)
  #define PRINT_FREEMEM(tag)  \
  { \
    Serial.print(F("freemem: ")); \
    Serial.print(tag); \
    Serial.print(F(": ")); \
    Serial.print(freeMemory()); \
    Serial.println(F("b")); \
  }
#else
  #define PRINT(...) 
  #define PRINTLN(...)
  #define WRITE(...)
  #define PRINT_FREEMEM(tag)
#endif
