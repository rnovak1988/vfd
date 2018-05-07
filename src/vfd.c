#include <stdio.h>
#include <pigpio/pigpiod_if2.h>
#include <time.h>

#include "base.h"

#define NSEC_PER_USEC     1000
#define USEC_PER_SECOND   1000000
#define NSEC_PER_SECOND   1000000000

#define PWM_FREQUENCY     4000

#define DUTY_CYCLE_RANGE  100
#define DUTY_CYCLE_ON     50
#define DUTY_CYLE_OFF     0

#define BASE_FREQUENCY    60

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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

int32_t _cleanup_ (int32_t pi_handle) {
  if (pi_handle >= 0) {
    int pin_index = 0;
    
    while (pin_index < NUM_PINS) {
       set_PWM_dutycycle(pi_handle, PIN_OUT[pin_index++], 0);
    }
    
    pigpio_stop(pi_handle);
    signal(SIGINT, SIG_DFL);

    return EXIT_SUCCESS;
  }
  return EINVAL;
}

int32_t _is_it_time_yet_ (const struct timespec* then, const struct timespec* now) {
  if (then != NULL && now != NULL) {
    return (now->tv_sec >= then->tv_sec) && (now->tv_nsec > then->tv_nsec);
  }
  return FALSE;
}

int32_t _calculate_next_ (const struct timespec* base, struct timespec* resultant, int64_t frequency_hertz) {

  if (base != NULL && resultant != NULL && 0 < frequency_hertz && frequency_hertz <= 200) {
    
    int64_t nsec_result = base->tv_nsec + (int64_t)((1.0 * NSEC_PER_SECOND) / (1.0 * frequency_hertz));

    resultant->tv_nsec = nsec_result % NSEC_PER_SECOND;
    resultant->tv_sec = base->tv_sec + (nsec_result / NSEC_PER_SECOND);

    return EXIT_SUCCESS;

  }

  return EINVAL;
}


int32_t _run_ (int32_t pi_handle) {
  
  int pin_index;

  struct timespec current, next;


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

    _sentinal_ = (int32_t*) malloc (sizeof(int32_t));
    if (_sentinal_ != NULL) {

      signal(SIGINT, _handler_);

      *_sentinal_ = 1;
      
      memset(&current, 0, sizeof(struct timespec));
      memset(&next,    0, sizeof(struct timespec));
  
        // determine current time and then the next
        // time phase needs to change
      clock_gettime(CLOCK_REALTIME, &current);
      _calculate_next_(&current, &next, 60);

      int32_t phase = 0;

      while (*_sentinal_) {

          while (*_sentinal_ && !_is_it_time_yet_(next, current)) {
            usleep(1);
            clock_gettime(CLOCK_REALTIME, &current);
          }

          if (*_sentinal_  && _is_it_time_yet_(next, current)) {
            
              // update next time phase needs to change  
            _calculate_next_(&current, &next, 60);


            
            switch(phase) {
               case 0:
                  set_PWM_dutycycle(pi_handle, PIN_OUT[0], DUTY_CYCLE_ON);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[1], DUTY_CYCLE_OFF);

                  set_PWM_dutycycle(pi_handle, PIN_OUT[2], DUTY_CYCLE_OFF);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[3], DUTY_CYCLE_ON);

                  set_PWM_dutycycle(pi_handle, PIN_OUT[4], DUTY_CYCLE_OFF);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[5], DUTY_CYCLE_OFF);
               break;
               case 1:
                  set_PWM_dutycycle(pi_handle, PIN_OUT[0], DUTY_CYCLE_OFF);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[1], DUTY_CYCLE_OFF);

                  set_PWM_dutycycle(pi_handle, PIN_OUT[2], DUTY_CYCLE_ON);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[3], DUTY_CYCLE_OFF);

                  set_PWM_dutycycle(pi_handle, PIN_OUT[4], DUTY_CYCLE_OFF);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[5], DUTY_CYCLE_ON);
               break;
               case 2:
                  set_PWM_dutycycle(pi_handle, PIN_OUT[0], DUTY_CYCLE_OFF);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[1], DUTY_CYCLE_ON);

                  set_PWM_dutycycle(pi_handle, PIN_OUT[2], DUTY_CYCLE_OFF);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[3], DUTY_CYCLE_OFF);

                  set_PWM_dutycycle(pi_handle, PIN_OUT[4], DUTY_CYCLE_ON);
                  set_PWM_dutycycle(pi_handle, PIN_OUT[5], DUTY_CYCLE_OFF);
               break;
               default:
                  fprintf(stderr, "%s\n", "unexpected, only 3 phases should exist");\
                  *_sentinal_ = 0;
                  break;
            }

            phase = (phase + 1) % 3;


          }

      }

      free(_sentinal_);
    } else {
      if (errno) {
        fprintf(stderr, "%s\n", strerror(errno));
      }
      return _cleanup_(pi_handle);
    }

  
  }


  return EINVAL;
}


void _handler_ (int32_t signum) {
  *_sentinal_ = 0;
  signal(signum, SIG_DFL);
}

