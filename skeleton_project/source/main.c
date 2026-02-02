#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include "functions.h"

int main()
{
    elevio_init();

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
    resetposition();

    while (1)
    {
        floorLight();

        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}
