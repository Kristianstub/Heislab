#pragma once
#include "driver/elevio.h"
#include <stdbool.h>
#include "functions.h"
#include <time.h>
#include <stdio.h>


bool queue[N_FLOORS][N_HEADING_STATES];
int direction = 0;

int doorstate = 0;
int obstructionstate;
int lastvisitedfloor = 0;

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
    elevio_buttonLamp(floor, btn, 1);
    
    int currentFloor = elevio_floorSensor();
    
    if (floor == currentFloor){
        openDoor();
        time_t start_time = time(NULL);
        time_t stop_time = start_time + 3;

        while (time(NULL) < stop_time){
            checkButtons();
        }
        
        closeDoor();
        elevio_buttonLamp(floor, btn, 0);

    }

    else{
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
}

void goToFloor(int floor){
    int currentFloor = elevio_floorSensor();
    if (currentFloor == -1) {
        currentFloor = lastvisitedfloor;
    }

    if (floor == currentFloor) {
        // Already at target floor
    } else if (floor > currentFloor) {
        elevio_motorDirection(DIRN_UP);
    } else {
        elevio_motorDirection(DIRN_DOWN);
    }

    while (floor != currentFloor) {
        if (elevio_floorSensor() != -1) {
            currentFloor = elevio_floorSensor();
        }
        floorLight();
        if (checkButtons() == 1) {
            return;
        }
    }

    elevio_motorDirection(DIRN_STOP);
    lastvisitedfloor = floor;

    queue[floor][0] = false;
    queue[floor][1] = false;
    for (int btn = 0; btn < N_BUTTONS; btn++) {
        elevio_buttonLamp(floor, btn, 0);
    }

    openDoor();
    time_t stop_time = time(NULL) + 3;
    while (time(NULL) < stop_time) {
        checkButtons();
    }
    closeDoor();
}

void openDoor(void){
    if (elevio_floorSensor() != -1){
        doorstate = 1;
        elevio_doorOpenLamp(1);
    }
    
}

void closeDoor(void){
    while (elevio_obstruction()) {
        checkButtons();
    }
    doorstate = 0;
    elevio_doorOpenLamp(0);
}

void navigateQueue(void){
    switch (direction){
        case 0:
            for (int floor = 0; floor < N_FLOORS; floor++){
                if (queue[floor][direction] == true){
                    if (doorstate == 1){
                        while (elevio_obstruction()){

                        }
                        closeDoor();
                    }
                    goToFloor(floor);
                    elevio_buttonLamp(floor, 0, 0);
                    queue[floor][direction] = false;
                }
            }
            direction = 1;
            break;

            
        case 1:
            for (int floor = N_FLOORS - 1; floor > -1; floor--){
                if (queue[floor][direction] == true){
                    if (doorstate == 1){
                        while (elevio_obstruction()){

                        }
                        closeDoor();
                    }
                    goToFloor(floor);
                    elevio_buttonLamp(floor, 1, 0);
                    queue[floor][direction] = false;
                }
            }
            direction = 0;
            break;
    }
}

int checkButtons(void){
    if (elevio_stopButton()){

        elevio_motorDirection(0);
        elevio_stopLamp(1);
        clearQueue();
        
        while(elevio_stopButton()){

        }

        elevio_stopLamp(0);
        return 1;

        
    }

    obstructionstate = elevio_obstruction();
    

    for (int floor = 0; floor < N_FLOORS; floor++){
        for (int btn = 0; btn < N_BUTTONS; btn++){
            if (elevio_callButton(floor,btn)){
                addToRequest(floor,btn);
            }
        }
    }
    return 0;
}


void clearQueue(void){
    for (int floor = 0; floor < N_FLOORS; floor++){
        for (int btn = 0; btn < N_HEADING_STATES; btn++){
            queue[floor][btn] = false;
        }
    }

     for (int floor = 0; floor < N_FLOORS; floor++){
        for (int btn = 0; btn < N_BUTTONS; btn++){
            elevio_buttonLamp(floor, btn, 0);
        }
    }
}

void printQueue(void){
    for (int floor = 0; floor < N_FLOORS; floor++){
        for (int btn = 0; btn < N_HEADING_STATES; btn++){
            printf("queue[%d][%d] = %d\n", floor, btn, queue[floor][btn]);
        }
    }
}