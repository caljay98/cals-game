// TriGround.h

#ifndef TRI_GROUND_H
#define TRI_GROUND_H

// *****INCLUDES*****
#include "raylib.h"

// *****DEFINES*****
#define NUM_TRI_VERTECES 3

// *****TYPEDEFS*****
typedef struct TriGround
{
    Vector2 verteces[NUM_TRI_VERTECES];
    Color color;
} TriGround;

// *****PUBLIC FUNCTION PROTOTYPES*****
void DrawTriGround(TriGround* pTriGround);

#endif