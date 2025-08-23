// TriGround.c

// *****INCLUDES*****
#include "TriGround.h"

// *****DEFINES*****

// *****TYPEDEFS*****

// *****PRIVATE VARIABLES*****

// *****PRIVATE FUNCTION PROTOTYPES*****

// *****PUBLIC FUNCTIONS*****
void DrawTriGround(TriGround* pTriGround)
{
    // draw the triangle forwards and backwards to garuntee the normal is facing the camera
    DrawTriangle(pTriGround->verteces[0], pTriGround->verteces[1],
        pTriGround->verteces[2], pTriGround->color);
    DrawTriangle(pTriGround->verteces[2], pTriGround->verteces[1],
        pTriGround->verteces[0], pTriGround->color);

    // DEBUG RGB labels for the verteces
    DrawCircle(pTriGround->verteces[0].x, pTriGround->verteces[0].y, 5.0f, RED);
    DrawCircle(pTriGround->verteces[1].x, pTriGround->verteces[1].y, 5.0f, GREEN);
    DrawCircle(pTriGround->verteces[2].x, pTriGround->verteces[2].y, 5.0f, BLUE);

}

// *****PRIVATE FUNCTIONS*****
