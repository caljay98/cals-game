// World.h

#ifndef WORLD_H
#define WORLD_H

// *****INCLUDES*****
#include "Player.h"
#include "RectGround.h"
#include "TriGround.h"

// *****DEFINES*****
#define MAX_PLAYERS 16
#define MAX_RECT_GROUNDS 1028
#define MAX_TRI_GROUNDS 1028

// *****TYPEDEFS*****
typedef struct World
{
    Player playerArray[MAX_PLAYERS];
    bool playerActive[MAX_PLAYERS]; // true if the player at this index is connected
    TriGround triGroundArray[MAX_TRI_GROUNDS];
    int numTriGrounds;
    RectGround rectGroundArray[MAX_RECT_GROUNDS];
    int numRectGrounds;

    // TODO projectiles
    //bool projActive array
    // TODO entities
} World;

// *****PUBLIC FUNCTION PROTOTYPES*****
void UpdateWorld(World* pWorld);
void DrawWorld(World* pWorld);

#endif
