#ifndef __VFD_HDW__
#define __VFD_HDW__

#include "base.h"

#define RED_positive 0
#define RED_negative 1
#define BLUE_positive 2
#define BLUE_negative 3
#define GREEN_positive 4
#define GREEN_negative 5

typedef int32_t rpi_t;

typedef struct CONFIGURATION_T {
  int32_t pin_out[6];
  int32_t frequency_hz;
} cfg_t;




#endif
