/**
 * @file DebugDemo.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015-2016, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * Arduino Debug demonstration.
 *
 * @section Footprint
 * Arduino 1.6.7, Arduino Mega2560
 *
 * Debug statements:
 * ASSERT()			2
 * BREAKPOINT/IF()		4
 * CHECK_STACK()		1
 * DEBUG_STREAM()		1
 * OBSERVE/IF()			2
 * REGISTER()			7
 * Total:			17
 *
 * Program storage space and dynamic memory.
 * Without debug statements;	3630/259 bytes
 * With debug statements:	7836/417 bytes
 * Increased size:		4206/158 bytes
 *
 * NB: Increased size includes debug shell and all file and
 * function name strings. Arduino preprocessor uses full path
 * of file name (see debug shell command where).
 */

// Define to remove debug statements
// #define NDEBUG

#include <Debug.h>

// Define to allow debug shell on Serial1 (Arduino Mega2560)
// #define USE_SERIAL1

namespace A {

  // Single breakpoint
  void a()
  {
    BREAKPOINT();
  }

  // Multiple breakpoints with call between
  void b()
  {
    BREAKPOINT();
    a();
    BREAKPOINT();
  }

  // Register variable, conditional observation and breakpoint
  void c()
  {
    static int i = 0;
    REGISTER(i);
    OBSERVE_IF(i < 1, i);
    OBSERVE_IF(i > 5, i);
    BREAK_IF(i == 10);
    i += 1;
    ASSERT(i < 15);
  }

  long e(int i)
  {
    REGISTER(i);
    CHECK_STACK();
    OBSERVE(i);
    if (i > 0) return (e(i - 1) * i);
    return (1);
  }
};

const size_t BUF_MAX = 128;
char* buf = NULL;

void setup()
{
  // Set the debug stream
  Serial.begin(57600);
  while (!Serial);
  Serial.println(F("DebugDemo::started"));

#if defined(ARDUINO_AVR_MEGA2560) && defined(USE_SERIAL1)
  Serial1.begin(57600);
  DEBUG_STREAM(Serial1);
#else
  DEBUG_STREAM(Serial);
#endif

  // Register global data (for this scope)
  Serial.println(F("setup running"));
  REGISTER(buf);

  // Contains a breakpoint. Check the memory. No heap used
  A::a();

  // Allocate from heap
  buf = (char*) malloc(BUF_MAX);
  memset(buf, 0xa5, BUF_MAX);

  // Contains several breakpoints. Check the memory again. Heap is now used
  A::b();

  // Free allocated buffer. Check memory and heap again
  free(buf);
  A::a();
}

void loop()
{
  // Local variable
  static uint8_t i = 0;

  // Register all the variables the debug handler should know about
  REGISTER(buf);
  REGISTER(BUF_MAX);
  REGISTER(Serial);
  REGISTER(i);

  Serial.println(F("loop running"));

  // Contains both breakpoints and observations. Check variables
  A::c();

  // Leak memory and run function with memory check
  buf = (char*) malloc(BUF_MAX);
  ASSERT(buf != NULL);
  memset(buf, 0xa5, BUF_MAX);
  Serial.print(i++);
  Serial.print(':');
  Serial.println(A::e(5));

  // Keep up with the user
  delay(500);
}
