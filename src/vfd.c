#include <stdio.h>
#include <pigpio/pigpiod_if2.h>

#include "base.h"

#define USEC_PER_SECOND   1000000

#define PWM_FREQUENCY     4000

#define DUTY_CYCLE_RANGE  100
#define DUTY_CYCLE_ON     50
#define DUTY_CYLE_OFF     0

#define BASE_FREQUENCY    60

static const int32_t PIN_OUT[] = { 14, 15, 23, 24, 25, 8 };
static const size_t  NUM_PINS  = sizeof(PIN_OUT) / sizeof(int32_t);

static int32_t *_sentinal_;

  // Forward Declarations
void    _handler_   (int32_t);
int32_t _run_       (int32_t);
int32_t _cleanup_   (int32_t);


int32_t main (void) {

  int32_t pi = pigpio_start(NULL, NULL);

  if (pi >= 0) {
    
    int32_t result = _run_(pi);

    if (result != EXIT_SUCCESS) {
      fprintf(stderr, "%s\n", strerror(result));
    } else {
      result = _cleanup_(pi);
    }

    return result;

  } else {
    fprintf(stderr, "%s\n", pigpio_error(pi));
  }

  return EXIT_FAILURE;
}


int32_t _run_ (int32_t pi_handle) {
  
  int pin_index;

  if (pi_handle >= 0) {
    /* Basic initialization. Set mode, make sure the pin is pulled
        low while its off, and set base PWM parameters */
    for (pin_index = 0; pin_index < NUM_PINS; pin_index++) {
      
      set_mode          (pi_handle, PIN_OUT[pin_index], PI_OUTPUT);
      set_pull_up_down  (pi_handle, PIN_OUT[pin_index], PI_PUD_DOWN);

      set_PWM_frequency (pi_handle, PIN_OUT[pin_index], PWM_FREQUENCY);
      set_PWM_range     (pi_handle, PIN_OUT[pin_index], DUTY_CYCLE_RANGE);
      set_PWM_dutycycle (pi_handle, PIN_OUT[pin_index], DUTY_CYCLE_OFF);

    }
  
  }


  return EINVAL;
}

