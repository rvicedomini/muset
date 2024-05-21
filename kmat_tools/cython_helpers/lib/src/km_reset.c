#include "km_reset.h"
#include <stdio.h>
#include <unistd.h>

void km_reset() {
    optind = 0;  // Reset getopt's internal index variable
}
