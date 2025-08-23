// RectGround.c

// *****INCLUDES*****
#include "RectGround.h"

// *****DEFINES*****

// *****TYPEDEFS*****

// *****PRIVATE VARIABLES*****

// *****PRIVATE FUNCTION PROTOTYPES*****

// *****PUBLIC FUNCTIONS*****
void DrawRectGround(RectGround* pRectGround)
{
    DrawRectangle(pRectGround->position.x, pRectGround->position.y,
        pRectGround->width, pRectGround->height, pRectGround->color);
}

// *****PRIVATE FUNCTIONS*****
