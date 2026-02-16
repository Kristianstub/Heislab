#pragma once
#include "driver/elevio.h"
#include <stdbool.h>
#include "functions.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

bool queue[N_FLOORS][N_HEADING_STATES];
int direction = 0;

int doorstate = 0;
int obstructionstate;
int lastvisitedfloor = 0;
int segment = 0;
float stopfloor = 0;
bool hasstopped = false;

void resetPosition(void)
{
    int floor = elevio_floorSensor();
    if (floor != 0)
    {
        while (elevio_floorSensor() != 0)
        {
            elevio_motorDirection(DIRN_DOWN);
        }
        elevio_motorDirection(DIRN_STOP);
    }
}

void floorLight(void)
{
    if (elevio_floorSensor() != -1)
    {
        elevio_floorIndicator(elevio_floorSensor());
    }
}

void addToRequest(int floor, int btn)
{
    
    
    elevio_buttonLamp(floor, btn, 1);
    

    int currentFloor = elevio_floorSensor();

   /*  if (floor == currentFloor)
    {   
        
        openDoor();
        time_t start_time = time(NULL);
        time_t stop_time = start_time + 3;

        while (time(NULL) < stop_time)
        {
            checkButtons();
        }

        closeDoor();
        elevio_buttonLamp(floor, btn, 0);
    } */

    
    
        switch (btn)
        {
        case 0:
            queue[floor][0] = true;
            break;

        case 1:
            queue[floor][1] = true;
            break;

        case 2:
            if (floor <= currentFloor)
            {
                queue[floor][1] = true;
                queue[floor][0] = true;
            }

            else if (floor > currentFloor)
            {
                queue[floor][0] = true;
                queue[floor][1] = true;
            }
            break;
        }
    
}

void goToFloor(int flr)
{
    int currentFloor = elevio_floorSensor();
    if (currentFloor == -1)
    {
        if (flr > stopfloor)
        {
            currentFloor = (int)floor(stopfloor);
        }

        else
        {
            currentFloor = (int)ceil(stopfloor);
        }
    }

    if (flr == currentFloor)
    {
        // Already at target floor
    }
    else if (flr > currentFloor)
    {
        elevio_motorDirection(DIRN_UP);
    }
    else
    {
        elevio_motorDirection(DIRN_DOWN);
    }
    
    while (flr != currentFloor)
    {
        if (elevio_floorSensor() != -1)
        {
            currentFloor = elevio_floorSensor();

            // Check if this intermediate floor has a request in our travel direction
            int heading = (flr > currentFloor) ? 0 : 1;
            if (currentFloor != flr && queue[currentFloor][heading])
            {
                // Stop and service this floor
                elevio_motorDirection(DIRN_STOP);
                lastvisitedfloor = elevio_floorSensor();

                queue[currentFloor][0] = false;
                queue[currentFloor][1] = false;
                for (int btn = 0; btn < N_BUTTONS; btn++)
                {
                    elevio_buttonLamp(currentFloor, btn, 0);
                }

                openDoor();
                time_t stop_time = time(NULL) + 3;
                while (time(NULL) < stop_time)
                {
                    if (checkButtons() == 1){
                        return;
                    }
                }
                closeDoor();

                // Resume moving toward target
                if (flr > currentFloor)
                {
                    elevio_motorDirection(DIRN_UP);
                }
                else
                {
                    elevio_motorDirection(DIRN_DOWN);
                }
            }
        }
        floorLight();
        if (checkButtons() == 1)
        {
            return;
        }
    }
    hasstopped = false;

    elevio_motorDirection(DIRN_STOP);
    lastvisitedfloor = flr;

    queue[flr][0] = false;
    queue[flr][1] = false;
    for (int btn = 0; btn < N_BUTTONS; btn++)
    {
        elevio_buttonLamp(flr, btn, 0);
    }

    openDoor();
    time_t stop_time = time(NULL) + 3;
    while (time(NULL) < stop_time)
    {
        checkButtons();
    }
    closeDoor();
}

void openDoor(void)
{
    if (elevio_floorSensor() != -1)
    {
        doorstate = 1;
        elevio_doorOpenLamp(1);
    }
    else{
        elevio_doorOpenLamp(0);
    }
}

void closeDoor(void)
{
    while (elevio_obstruction())
    {
        checkButtons();
    }
    doorstate = 0;
    elevio_doorOpenLamp(0);
}

void navigateQueue(void)
{
    switch (direction)
    {
    case 0:
        for (int floor = 0; floor < N_FLOORS; floor++)
        {
            if (queue[floor][direction] == true)
            {
                if (doorstate == 1)
                {
                    while (elevio_obstruction())
                    {
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
        for (int floor = N_FLOORS - 1; floor > -1; floor--)
        {
            if (queue[floor][direction] == true)
            {
                if (doorstate == 1)
                {
                    while (elevio_obstruction())
                    {
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

int stopButton()
{
    elevio_motorDirection(0);
    
    elevio_stopLamp(1);
    clearQueue();

    if (elevio_floorSensor() == -1 && !hasstopped )
    {
        hasstopped = true;
        if (direction == 0)
        {
            stopfloor = lastvisitedfloor + 0.5;
        }

        else
        {
            stopfloor = lastvisitedfloor - 0.5;
        }
    }

    else{
        openDoor();

    }

    while (elevio_stopButton())
    {
    }


    elevio_stopLamp(0);

   
        time_t start_time = time(NULL);
        time_t stop_time = start_time + 3;

        while (time(NULL) < stop_time && elevio_floorSensor() != -1)
        {
            checkButtons();
        }

        
        if (elevio_floorSensor() != -1){
            closeDoor();
        }
    return 1;
}
int checkButtons(void)
{
    if (elevio_stopButton())
    {
        if (stopButton() == 1)
        {
            return 1;
        }
    }

    obstructionstate = elevio_obstruction();

    for (int floor = 0; floor < N_FLOORS; floor++)
    {
        for (int btn = 0; btn < N_BUTTONS; btn++)
        {
            if (elevio_callButton(floor, btn))
            {
                addToRequest(floor, btn);
            }
        }
    }
    return 0;
}

void clearQueue(void)
{
    for (int floor = 0; floor < N_FLOORS; floor++)
    {
        for (int btn = 0; btn < N_HEADING_STATES; btn++)
        {
            queue[floor][btn] = false;
        }
    }

    for (int floor = 0; floor < N_FLOORS; floor++)
    {
        for (int btn = 0; btn < N_BUTTONS; btn++)
        {
            elevio_buttonLamp(floor, btn, 0);
        }
    }
}

void printQueue(void)
{
    
        
}