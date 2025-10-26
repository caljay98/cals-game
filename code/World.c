// World.c

// *****INCLUDES*****
#include "World.h"
#include "raymath.h"

// *****DEFINES*****

// *****TYPEDEFS*****

// *****PRIVATE VARIABLES*****

// *****PRIVATE FUNCTION PROTOTYPES*****
static void checkGroundCollisions(World* pWorld);
static void updatePlayerFromColInfo(Player* pPlayer, CollisionInfo colInfo);

// *****PUBLIC FUNCTIONS*****
// UpdateWorld
void UpdateWorld(World* pWorld)
{
    // apply the inputs to the forces on each player
    for (int c = 0; c < MAX_PLAYERS; c++)
    {
        if (pWorld->playerActive[c])
        {
            CalcInputsAndWorldForces(&pWorld->playerArray[c]);
        }
    }

    // update projectiles, including if new projectiles are needed
    // TODO check if need projectiles are needed
    // TODO check for projectile collision, if so create an explosion or do damage

    // calculate the force and damage needed on players from explosions
    // TODO

    // update player velocities and positions using the forces
    for (int c = 0; c < MAX_PLAYERS; c++)
    {
        if (pWorld->playerActive[c])
        {
            // update velocity
            pWorld->playerArray[c].velocity.x += pWorld->playerArray[c].forces.x;
            pWorld->playerArray[c].velocity.y += pWorld->playerArray[c].forces.y;

            // update position
            pWorld->playerArray[c].position.x += pWorld->playerArray[c].velocity.x;
            pWorld->playerArray[c].position.y += pWorld->playerArray[c].velocity.y;
        }
    }
    
    // check for player to player collisions, including grapple collisions
    // TODO need to find a good way to handle grapple collisions
    // TODO use conservation of momentum and collision direction to change velocities
    // TODO update positions so there is no overlap between players

    // check for player to ground collisions, including grapple collisions. Also update
    // player positions and remove velocity in any direction towards a ground
    checkGroundCollisions(pWorld);

    // TODO any player movements for ground collision reasons need to propogate out to
    // all players that are touching each other
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
            p->groundedNormal = (Vector2){ 0, 0 };

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
                        updatePlayerFromColInfo(p, colInfo);
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
                        updatePlayerFromColInfo(p, colInfo);
                    }
                }
            }
        }
    }
}

static void updatePlayerFromColInfo(Player* pPlayer, CollisionInfo colInfo)
{
    // do not allow any velocities in the direction of this surface
    if (Vector2DotProduct(pPlayer->velocity, colInfo.surfaceNormal) < 0)
    {
        // there is some component of the velocity pointing *into* the surface.
        // Project the velocity onto the surface normal, and subtract that component
        // so only tangential motion remains.

        Vector2 normal = Vector2Normalize(colInfo.surfaceNormal);
        float vDotN = Vector2DotProduct(pPlayer->velocity, normal);

        // Subtract the normal component (since vDotN < 0, this removes inward motion)
        pPlayer->velocity.x -= vDotN * normal.x;
        pPlayer->velocity.y -= vDotN * normal.y;
    }

    // move the player out in the direction of the surface normal scaled to the
    // collision depth
    pPlayer->position = Vector2Add(pPlayer->position,
        Vector2Scale(colInfo.surfaceNormal, -colInfo.colDepth));

    // update the groundedNormal, the normal of all the grounds the player is standing on
    // (below player and a shallow enough angle)
    if (colInfo.surfaceNormal.y <= STANDABLE_GND_Y_THRESH)
    {
        if (colInfo.surfaceNormal.x * pPlayer->groundedNormal.x < 0.0f)
        {
            // there is two shallow normals on either side of the player. The
            // groundedNormal should be vertical
            pPlayer->groundedNormal.x = 0.0f;
            pPlayer->groundedNormal.y = -1.0f;
        }
        else if (colInfo.surfaceNormal.y < pPlayer->groundedNormal.y)
        {
            // always choose the shallowest slope (largest mag neg y)
            pPlayer->groundedNormal = colInfo.surfaceNormal;
        }
    }
} 