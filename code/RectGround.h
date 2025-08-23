// RectGround.h

#ifndef RECT_GROUND_H
#define RECT_GROUND_H

// *****INCLUDES*****
#include "raylib.h"

// *****DEFINES*****

// *****TYPEDEFS*****
typedef struct RectGround
{
    Vector2 position; // Position is the top left corner
    float width;
    float height;
    Color color;
} RectGround;

// *****PUBLIC FUNCTION PROTOTYPES*****
void DrawRectGround(RectGround* pRectGround);

#endif
