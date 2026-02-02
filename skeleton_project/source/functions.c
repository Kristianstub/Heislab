#pragma once
#include "driver/elevio.h"

void resetposition(void){
    int floor = elevio_floorSensor();
    if(floor != 0){
        while(elevio_floorSensor() != 0){
            elevio_motorDirection(DIRN_DOWN);
        }
        elevio_motorDirection(DIRN_STOP);
    }
}

void floorLight(void){
    if (elevio_floorSensor() != -1) {
        elevio_floorIndicator(elevio_floorSensor());
    }
}

void addToRequest(void){}