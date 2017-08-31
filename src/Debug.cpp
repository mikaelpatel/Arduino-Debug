/**
 * @file Debug.cpp
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
 */

#include "Debug.h"

#define ARCH "AVR"
#define VERSION "1.0a3"

Debug debug __attribute__((weak));

extern int __heap_start, *__brkval;

bool
Debug::begin(Stream* dev,
	     const char* file,
	     int line,
	     const char* func)
{
  if (m_dev != NULL) return (false);
  m_dev = dev;
  DATAEND =(int) &__heap_start;
  DATASIZE = DATAEND - RAMSTART;
  print(F("Arduino Debug (" ARCH ") " VERSION
	  ", Copyright (C) 2015-2016, Mikael Patel\n"
	  "For help, type \"help\".\n"
	  "Debug::begin:"));
  run(file, line, func);
  return (true);
}

void
Debug::assert(const char* file,
	      int line,
	      const char* func,
	      str_P cond)
{
  print(F("Debug::assert"));
  run(file, line, func, cond);
  end();
  exit(0);
}

void
Debug::break_at(const char* file,
		int line,
		const char* func,
		str_P cond)
{
  print(F("Debug::break_at"));
  run(file, line, func, cond);
}

bool
Debug::check_stack(int room)
{
  uint16_t marker = 0xA5A5;
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  int STACKSTART = (int) &marker;
  return (STACKSTART > HEAPEND + room);
}

void
Debug::observe_at(const char* file,
		  int line,
		  const char* func,
		  str_P expr)
{
  UNUSED(file);
  print(F("Debug::observe_at:"));
  print(func);
  print(':');
  print(line);
  print(':');
  print(expr);
  print('=');
}

bool
Debug::end()
{
  if (m_dev == NULL) return (false);
  println(F("Debug::end"));
  delay(1000);
  m_dev = NULL;
  return (true);
}

void
Debug::run(const char* file, int line, const char* func, str_P expr)
{
  if (func != NULL) {
    print(':');
    print(func);
    print(':');
    print(line);
  }
  if (expr != NULL) {
    print(':');
    print(expr);
  }
  if (func != NULL) println();

  uint16_t marker = 0xA5A5;
  str_P prompt = F("(debug) ");

  while (1) {
    const size_t BUF_MAX = 32;
    char buf[BUF_MAX];
    char* bp = buf;
    int c;
    print(prompt);
    while (1) {
      c = read();
      if (c == '\r' || c == '\n') break;
      if (c > 0) {
	*bp++ = c;
	print((char) c);
      }
      else {
	yield();
      }
    }
    println();
    *bp = 0;
    size_t len = bp - buf;
    if (len == 0) continue;

    if (!strncmp_P(buf, PSTR("go"), len)) return;

#if !defined(DEBUG_NO_BACKTRACE)
    if (!strncmp_P(buf, PSTR("backtrace"), len)) {
      print(F("0x"));
      print((int) &marker, HEX);
      print(':');
      println(func);
      do_backtrace(func);
      continue;
    }
#endif

#if !defined(DEBUG_NO_PRINT_DATA)
    if (!strncmp_P(buf, PSTR("data"), len)) {
      do_print_data();
      continue;
    }
#endif

#if !defined(DEBUG_NO_PRINT_HEAP)
    if (!strncmp_P(buf, PSTR("heap"), len)) {
      do_print_heap();
      continue;
    }
#endif

#if !defined(DEBUG_NO_MEMORY_USAGE)
    if (!strncmp_P(buf, PSTR("memory"), len)) {
      do_memory_usage((int) &marker);
      continue;
    }
#endif

#if !defined(DEBUG_NO_PRINT_COMMANDS)
    if (!strncmp_P(buf, PSTR("commands"), len)
	|| !strncmp_P(buf, PSTR("help"), len)) {
      do_print_commands();
      continue;
    }
#endif

#if !defined(DEBUG_NO_PRINT_STACK)
    if (!strncmp_P(buf, PSTR("stack"), len)) {
      do_print_stack((int) &marker);
      continue;
    }
#endif

#if !defined(DEBUG_NO_PRINT_VARIABLES)
    if (!strncmp_P(buf, PSTR("variables"), len)) {
      do_print_variables();
      continue;
    }
#endif

#if !defined(DEBUG_NO_QUIT)
    if (!strncmp_P(buf, PSTR("quit"), len)) {
      end();
      exit(0);
    }
#endif

#if !defined(DEBUG_NO_WHERE)
    if (!strncmp_P(buf, PSTR("where"), len)) {
      print(file);
      print(':');
      print(line);
      print(':');
      println(func);
      continue;
    }
#endif

#if !defined(DEBUG_NO_LOOKUP_VARIABLES)
    if (*buf == '?' || *buf == '@' ) {
      const char* name = buf + 1;
      if (!do_lookup_variables(name, *buf == '@')) {
	print(name);
	println(F(": unknown variable"));
      }
      continue;
    }
#endif
    print(buf);
    println(F(": unknown command"));
  }
}

#if !defined(DEBUG_NO_BACKTRACE)
void
Debug::do_backtrace(const char* func)
{
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) {
    if (func != vp->m_func) {
      func = vp->m_func;
      print(F("0x"));
      print((int) vp, HEX);
      print(':');
      println(vp->m_func);
    }
  }
}
#endif

#if !defined(DEBUG_NO_LOOKUP_VARIABLES)
bool
Debug::do_lookup_variables(const char* name, bool is_pointer)
{
  bool found = false;
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) {
    if (strcmp_P(name, (const char*) vp->m_name) == 0) {
      if (!is_pointer || (vp->m_size == sizeof(void*)))
	vp->print(is_pointer);
      found = true;
    }
  }
  return (found);
}
#endif

#if !defined(DEBUG_NO_MEMORY_USAGE)
void
Debug::do_memory_usage(int marker)
{
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  print(F("data="));
  print(DATASIZE);
  print(F(",heap="));
  print(HEAPEND - (int) &__heap_start);
  print(F(",stack="));
  print(RAMEND - marker + 1);
  print(F(",free="));
  println(marker - HEAPEND);
}
#endif

#if !defined(DEBUG_NO_PRINT_COMMANDS)
void
Debug::do_print_commands()
{
  static const char help[] PROGMEM =
#if !defined(DEBUG_NO_LOOKUP_VARIABLES)
    "?VARIABLE -- Print variable(s)\n"
    "@VARIABLE -- Print pointer variable(s)\n"
#endif
#if !defined(DEBUG_NO_BACKTRACE)
    "backtrace -- Print call stack\n"
#endif
#if !defined(DEBUG_NO_PRINT_DATA)
    "data -- Print data\n"
#endif
    "go -- Return to sketch\n"
#if !defined(DEBUG_NO_PRINT_HEAP)
    "heap -- Print heap\n"
#endif
#if !defined(DEBUG_NO_MEMORY_USAGE)
    "memory -- Print memory usage\n"
#endif
#if !defined(DEBUG_NO_QUIT)
    "quit -- Exit sketch\n"
#endif
#if !defined(DEBUG_NO_PRINT_STACK)
    "stack -- Print stack\n"
#endif
#if !defined(DEBUG_NO_PRINT_REGISTER)
    "variables -- Print variables\n"
#endif
#if !defined(DEBUG_NO_WHERE)
    "where -- Location in source code\n"
#endif
    ;
  print((str_P) help);
}
#endif

#if !defined(DEBUG_NO_PRINT_DATA)
void
Debug::do_print_data()
{
  dump((uint16_t) RAMSTART, (void*) RAMSTART, DATASIZE);
}
#endif

#if !defined(DEBUG_NO_PRINT_HEAP)
void
Debug::do_print_heap()
{
  int HEAPSTART = (int) &__heap_start;
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  int size = HEAPEND - HEAPSTART;
  if (size == 0) return;
  dump((uint16_t) HEAPSTART, (void*) HEAPSTART, size);
}
#endif

#if !defined(DEBUG_NO_PRINT_STACK)
void
Debug::do_print_stack(int marker)
{
  int size = RAMEND - marker + 1;
  if (size == 0) return;
  dump((uint16_t) marker, (void*) marker, size);
}
#endif

#if !defined(DEBUG_NO_PRINT_VARIABLES)
void
Debug::do_print_variables()
{
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) vp->print();
}
#endif

void
Debug::Variable::print(bool is_pointer)
{
  debug.print(m_func);
  debug.print(':');
  debug.print(m_name);
  debug.print('@');
  debug.print(F("0x"));
  debug.print((int) m_ref, HEX);
  if (m_size == sizeof(void*) && is_pointer) {
    int* ptr = *((int**) m_ref);
    debug.print(F("=>"));
    debug.dump((uint16_t) ptr, ptr, 16);
  }
  else {
    if (m_size == 1) {
      debug.print('=');
      debug.print(*((uint8_t*) m_ref));
      debug.print(F(" (0x"));
      debug.print(*((uint8_t*) m_ref), HEX);
      debug.println(')');
    }
    else if (m_size == 2) {
      debug.print('=');
      debug.print(*((int*) m_ref));
      debug.print(F(" (0x"));
      debug.print(*((uint16_t*) m_ref), HEX);
      debug.println(')');
    }
    else {
      debug.print('[');
      debug.print(m_size);
      debug.print(F("]:"));
      if (m_size > 16) debug.println();
      debug.dump((uint16_t) m_ref, m_ref, m_size);
    }
  }
}

void
Debug::dump(uint16_t src, const void *ptr, size_t size)
{
  uint8_t* p = (uint8_t*) ptr;
  uint8_t n = 0;
  print(F("0x"));
  print(src, HEX);
  print(F(": "));
  while (size--) {
    uint8_t d = *p++;
    print(d >> 4, HEX);
    print(d & 0x0F, HEX);
    src += 1;
    if (++n < 16) {
      print(' ');
    }
    else {
      println();
      n = 0;
      if (size > 0) {
	print(F("0x"));
	print(src, HEX);
	print(F(": "));
      }
    }
  }
  if (n != 0) println();
}
