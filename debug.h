#pragma once

#include <stdarg.h>
#include <stdlib.h>

// MINMEA_MAX
// FP from internal RC divided by 2
#define FP 4000000
// 9600 baud for debug works, 115200 does not
#define DEBUG_BAUDRATE 9600
#define DEBUG_BRGVAL ((FP/DEBUG_BAUDRATE)/16)-1

#define MAX_DEBUG_CHARS (256)

void debug_printf(const char* format, ...);

void init_debug_uart();