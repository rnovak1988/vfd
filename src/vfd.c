#include <stdio.h>
#include <pigpio/pigpiod_if2.h>

#include "base.h"
#include "hardware.h"

int main (void) {

  pigpio_start(NULL, NULL);

  return 0;
}

