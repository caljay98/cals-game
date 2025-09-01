// World.c

// *****INCLUDES*****
#include "World.h"

// *****DEFINES*****

// *****TYPEDEFS*****

// *****PRIVATE VARIABLES*****

// *****PRIVATE FUNCTION PROTOTYPES*****
static void checkGroundCollisions(World* pWorld);

// *****PUBLIC FUNCTIONS*****
// UpdateWorld
void UpdateWorld(World* pWorld)
{
    // DEBUG
    Player* p = &pWorld->playerArray[0];
    float speed = 1.0f;
    if (p->inputs.leftInput)
    {
        p->position.x -= speed;
    }
    if (p->inputs.rightInput)
    {
        p->position.x += speed;
    }
    if (p->inputs.jumpInput)
    {
        p->position.y -= speed;
    }
    if (p->inputs.shootInput)
    {
        p->position.y += speed;
    }

    // apply the inputs to the forces on each player
    // TODO includes input forces and resistances
    // TODO resistances will be based on velocity and if the player is on the ground

    // update projectiles, including if new projectiles are needed
    // TODO check if need projectiles are needed
    // TODO check for projectile collision, if so create an explosion or do damage

    // calculate the force and damage needed on players from explosions
    // TODO

    // update player velocities and positions using the forces
    // TODO

    // check for player to player collisions, including grapple collisions
    // TODO need to find a good way to handle grapple collisions
    // TODO use conservation of momentum and collision direction to change velocities
    // TODO update positions so there is no overlap between players

    // check for player to ground collisions, including grapple collisions
    checkGroundCollisions(pWorld);

    // TODO these all need to propogate out to all players that are touching each other
    // TODO remove all forces in the direction of the collision (dot product)
    // TODO remove all velocity in the direction of the collision

    // update player positions so there is no overlap from the player to wall or other
    // players. May need to propogate out
    // TODO
}

// DrawWorld
void DrawWorld(World* pWorld)
{
    int c = 0;
    for (c = 0; c < pWorld->numRectGrounds; c++)
    {
        DrawRectGround(&pWorld->rectGroundArray[c]);
    }

    for (c = 0; c < pWorld->numTriGrounds; c++)
    {
        DrawTriGround(&pWorld->triGroundArray[c]);
    }

    for (c = 0; c < MAX_PLAYERS; c++)
    {
        if (pWorld->playerActive[c] == true)
        {
            DrawPlayer(&pWorld->playerArray[c]);
        }
    }

    // TODO more when they are implemented
}

// *****PRIVATE FUNCTIONS*****
// checkGroundCollisions
//  fills out the groundsTouchedArray for each player
static void checkGroundCollisions(World* pWorld)
{
    for (int c = 0; c < MAX_PLAYERS; c++)
    {
        if (pWorld->playerActive[c] == true)
        {
            Player* p = &pWorld->playerArray[c];
            CollisionInfo colInfo;

            // reset the collision info for this player
            p->numGroundsTouched = 0;

            // rectGround collisions
            for (int k = 0; k < pWorld->numRectGrounds; k++)
            {
                colInfo = TouchingRectGround(p, &pWorld->rectGroundArray[k]);
                if (colInfo.surfaceNormal.x != 0 || colInfo.surfaceNormal.y != 0) // direct comparison to 0 is ok here because no collision will be set to 0
                {
                    if (p->numGroundsTouched < MAX_GROUND_COLLISIONS)
                    {
                        p->groundsTouchedArray[p->numGroundsTouched] = colInfo;
                        p->numGroundsTouched++;
                    }
                }
            }

            // triGround collisions
            for (int k = 0; k < pWorld->numTriGrounds; k++)
            {
                colInfo = TouchingTriGround(p, &pWorld->triGroundArray[k]);
                if (colInfo.surfaceNormal.x != 0 || colInfo.surfaceNormal.y != 0) // direct comparison to 0 is ok here because no collision will be set to 0
                {
                    if (p->numGroundsTouched < MAX_GROUND_COLLISIONS)
                    {
                        p->groundsTouchedArray[p->numGroundsTouched] = colInfo;
                        p->numGroundsTouched++;
                    }
                }
            }
        }
    }
}