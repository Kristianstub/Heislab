#pragma once

#define N_HEADING_STATES 2

typedef enum {
    HEAD_UP     = 1,
    HEAD_DOWN   = 0
} HeadingState

bool requests[N_FLOORS][N_HEADING_STATES]; //Fortsett her!!!!

void resetposition(void);
void floorLight(void);