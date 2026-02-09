#pragma once

#define N_HEADING_STATES 2

typedef enum
{
    HEAD_UP = 0,
    HEAD_DOWN = 1
} HeadingState;


void resetPosition(void);
void floorLight(void);
void checkButtons(void);
void navigateQueue(void);
void goToFloor(int floor);
void addToRequest(int floor, int btn);
