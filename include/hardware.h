#ifndef __VFD_HDW__
#define __VFD_HDW__

#include "base.h"

typedef int32_t pi_t; // Handle to a Raspberry Pi via the pigpiod_if2 mechanism


  // Phase States, these correspond to
  // a modified three phase setup, wherein
  // one poll is off while the other two are
  // onn. This allows for us to not have
  // magnets opposing the direction of travel

typedef enum PI_PHASE_STATE {
  RED_POS_BLUE_NEG_GREEN_OFF,
  RED_NEG_BLUE_OFF_GREEN_POS,
  RED_OFF_BLUE_POS_GREEN_NEG
} phase_t;


  // Structure representing the Output layout
  // with regard to which pin (numbered via the 
  // schematic for GPIO pins), corresponds to
  // which logical switch

typedef struct PINS_T {
  uint16_t RED_p; // Positive Red
  uint16_t RED_n; // Negative Red

  uint16_t BLUE_p; // Positive Blue
  uint16_t BLUE_n; // Negative Blue

  uint16_t GREEN_p; // Positive Green
  uint16_t GREEN_n; // Negative Green
} pin_def_t;




#endif
