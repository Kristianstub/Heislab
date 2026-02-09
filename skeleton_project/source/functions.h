#pragma once

#define N_HEADING_STATES 2

typedef enum
{
    HEAD_UP = 0,
    HEAD_DOWN = 1
} HeadingState;


void resetPosition(void);
void floorLight(void);
int checkButtons(void);
void navigateQueue(void);
void goToFloor(int floor);
void addToRequest(int floor, int btn);
void clearQueue(void);
void closeDoor(void);
void openDoor(void);
