#include <stdio.h>
#include <pigpio/pigpiod_if2.h>

#include "base.h"
#include "hardware.h"

  // Boolean flag. Indicates whether or not
  // to continue in the main loop
static int32_t* __cont__ = NULL; 
static pi_t* __pihnd__ = NULL;

  // When we get interrupted, we set the
  // value of the global variable __cont__ to
  // zero, which indicates that the program should 
  // stop running and exit. we then return the signal
  // back to it's default function, and return execution
  // to the normal program
void __interrupted__ (int signum) {
  
  if (__cont__ != NULL) {
    *(__cont__) = 0;
  }

  signal(signum, SIG_DFL);
}

void __cleanup__();


int main (void) {

  pi_t pihnd = pigpio_start(NULL, NULL);
  
  if (pihnd >= 0) {
    __pihnd__ = &pihnd;
    


    

    
    __cleanup__();
    return EXIT_SUCCESS;

  } else if (errno) {
    fprintf(stderr, "%s\n", strerror(errno));
  }

  return EXIT_FAILURE;
}


void __cleanup__ () {
  if (__pihnd__ != NULL && *(__pihnd__) >= 0) {
    pigpio_stop(*(__pihnd__));
  }

    // always return signal back to default action
  signal(SIGINT, SIG_DFL);
}
