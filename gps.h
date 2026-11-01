#pragma once

#include "minmea/minmea.h"

// MINMEA_MAX_SENTENCE_LENGTH is 80 by default, which is fine
#define MAX_GPS_CHARS (MINMEA_MAX_SENTENCE_LENGTH)

#define FP 4000000
#define GPS_BAUDRATE 9600
#define GPS_BRGVAL ((FP/GPS_BAUDRATE)/16)-1

void init_gps_uart();
