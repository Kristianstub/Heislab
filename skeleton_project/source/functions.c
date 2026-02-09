#pragma once
#include "driver/elevio.h"
#include <stdbool.h>
#include "functions.h"
#include <time.h>

bool queue[N_FLOORS][N_HEADING_STATES];
int direction = 0;

int doorstate = 0;

void resetPosition(void){
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

void addToRequest(int floor, int btn){
    int currentFloor = elevio_floorSensor();
    switch (btn){
        case 0:
            queue[floor][0] = true;
            break;

        case 1:
            queue[floor][1] = true;
            break;


        case 2:
            if (floor < currentFloor){
                queue[floor][1] = true;
            }

            else if (floor > currentFloor){
                queue[floor][0] = true;
            }
            break;
    }
   
}

void goToFloor(int floor){
    int currentFloor = elevio_floorSensor();
    int motordirection;

    if (direction == 0){
        motordirection = 1;
    }

    else{
        motordirection = -1;
    }

    while (floor != currentFloor){
        if (doorstate == 0){
            elevio_motorDirection(motordirection);
        }
        
        currentFloor = elevio_floorSensor();
        floorLight();
        checkButtons();
        
    }

    elevio_motorDirection(0);
    

}

void navigateQueue(void){
    switch (direction){
        case 0:
            for (int floor = 0; floor < N_FLOORS; floor++){
                if (queue[floor][direction] == true){
                    goToFloor(floor);
                    queue[floor][direction] = false;
                }
            }
            direction = 1;
            break;

            
        case 1:
            for (int floor = N_FLOORS - 1; floor > -1; floor--){
                if (queue[floor][direction] == true){
                    goToFloor(floor);
                    queue[floor][direction] = false;
                }
            }
            direction = 0;
            break;
    }
}

void checkButtons(void){
    if (elevio_stopButton()){

        elevio_motorDirection(0);
        elevio_stopLamp(1);

        bool stop = true;
        nanosleep(&(struct timespec){0, 1000*1000*1000}, NULL);
        while (stop == true){
            if (elevio_stopButton()){
                stop = false;
            }
        }

        elevio_stopLamp(0);

        
    }

    for (int floor = 0; floor < N_FLOORS; floor++){
        for (int btn = 0; btn < N_BUTTONS; btn++){
            if (elevio_callButton(floor,btn)){
                addToRequest(floor,btn);
            }
        }
    }
}