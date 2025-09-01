// Player.c

// *****INCLUDES*****
#include "Player.h"
#include "raymath.h"
#include <float.h>

// DEBUG
#include <stdio.h>
#include "DebugLib.h"

// *****DEFINES*****
#define NUM_RECT_VERTECIES       (4)
#define MAX_POLYGON_VERTECIES    (4) // increase this if we need to support more complex polygons
#define PLAYER_WIDTH             (40.0f)
#define PLAYER_HEIGHT            (90.0f)
#define PLAYER_ROUNDED_RADIUS    (10.0f) // must be <= 0.5*PLAYER_WIDTH
//#define PLAYER_ROUNDED_RADIUS    (20.0f) // probably best, tuning needed

// vector drawing defines
#define COLLISION_INFO_COLOR     (ORANGE)
#define COLLISION_INFO_LEN       (15.0f)
#define COLLISION_INFO_THICKNESS (4.0f)
#define COLLISION_INFO_RADIUS    (5.0f)

// *****TYPEDEFS*****

// *****PRIVATE VARIABLES*****

// *****PRIVATE FUNCTION PROTOTYPES*****

// *****PUBLIC FUNCTIONS*****
void InitPlayer(Player* pPlayer)
{
    // TODO some basic defaults
}

void UpdatePlayerInput(Player* pPlayer)
{
    // TODO updates the forces vector based on inputs

    // TODO use the GroundTouchedArray to see if jumps and planted running are ok
}

CollisionInfo TouchingRectGround(Player* pPlayer, RectGround* pRectGround)
{
    CollisionInfo retVal = { 0 };
    float playerLeft = pPlayer->position.x - (PLAYER_WIDTH / 2.0f);
    float playerRight = pPlayer->position.x + (PLAYER_WIDTH / 2.0f);
    float playerTop = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f);
    float playerBot = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f);
    float rectLeft = pRectGround->position.x;
    float rectRight = pRectGround->position.x + pRectGround->width;
    float rectTop = pRectGround->position.y;
    float rectBot = pRectGround->position.y + pRectGround->height;

    // check if the bounding box of the player is within the rect box. If no there
    // must not be a collision
    if (playerRight >= rectLeft && playerLeft <= rectRight &&
        playerTop <= rectBot && playerBot >= rectTop)
    {
        // find the offsets from each edge of the player
        float leftOffset = playerLeft - rectRight;
        float rightOffset = rectLeft - playerRight;
        float topOffset = playerTop - rectBot;
        float botOffset = rectTop - playerBot;

        int offsetsLessThanRadius = 0;
        if (leftOffset > -PLAYER_ROUNDED_RADIUS) offsetsLessThanRadius++;
        if (rightOffset > -PLAYER_ROUNDED_RADIUS) offsetsLessThanRadius++;
        if (topOffset > -PLAYER_ROUNDED_RADIUS) offsetsLessThanRadius++;
        if (botOffset > -PLAYER_ROUNDED_RADIUS) offsetsLessThanRadius++;

        // check if 3 of 4 offsets are more than the corner radii. If so raw rectangular
        // collision can be used. Collision directions will be the 4 cardinal directions
        // and the position will be in line with the player center of mass
        if (offsetsLessThanRadius <= 1)
        {
            // this if order prioritizes touching the order: bottom, left, right, top
            if (botOffset >= leftOffset && botOffset >= rightOffset && botOffset >= topOffset)
            {
                // ground is on the bottom
                retVal.surfaceNormal.x = 0.0f;
                retVal.surfaceNormal.y = -1.0f;
                retVal.colPosition.x = pPlayer->position.x;
                retVal.colPosition.y = rectTop;
                retVal.colDepth = botOffset;
            }
            else if (leftOffset >= rightOffset && leftOffset >= topOffset && leftOffset >= botOffset)
            {
                // ground is on the left
                retVal.surfaceNormal.x = 1.0f;
                retVal.surfaceNormal.y = 0.0f;
                retVal.colPosition.x = rectRight;
                retVal.colPosition.y = pPlayer->position.y;
                retVal.colDepth = leftOffset;
            }
            else if (rightOffset >= leftOffset && rightOffset >= topOffset && rightOffset >= botOffset)
            {
                // ground is on the right
                retVal.surfaceNormal.x = -1.0f;
                retVal.surfaceNormal.y = 0.0f;
                retVal.colPosition.x = rectLeft;
                retVal.colPosition.y = pPlayer->position.y;
                retVal.colDepth = rightOffset;
            }
            else
            {
                // ground is on the top
                retVal.surfaceNormal.x = 0.0f;
                retVal.surfaceNormal.y = 1.0f;
                retVal.colPosition.x = pPlayer->position.x;
                retVal.colPosition.y = rectBot;
                retVal.colDepth = topOffset;
            }
        }
        else
        {
            // we must be touching on a corner. Figure out which corner and calculate based on
            // that one. NOTE: there may be no collision if the corner of the rect is outside the
            // corner radius of the player
            Vector2 circleCenter;
            Vector2 rectCorner;
            Vector2 playerCenter = pPlayer->position;
            Vector2 rectCenter = pRectGround->position;
            rectCenter.x += (pRectGround->width / 2.0f);
            rectCenter.y += (pRectGround->height / 2.0f);

            if (playerCenter.x <= rectCenter.x && playerCenter.y <= rectCenter.y)
            {
                // ground is on the bottom right
                circleCenter.x = (playerCenter.x + PLAYER_WIDTH / 2.0) - PLAYER_ROUNDED_RADIUS;
                circleCenter.y = (playerCenter.y + PLAYER_HEIGHT / 2.0) - PLAYER_ROUNDED_RADIUS;
                rectCorner.x = rectLeft;
                rectCorner.y = rectTop;
            }
            else if (playerCenter.x >= rectCenter.x && playerCenter.y <= rectCenter.y)
            {
                // ground is on the bottom left
                circleCenter.x = (playerCenter.x - PLAYER_WIDTH / 2.0) + PLAYER_ROUNDED_RADIUS;
                circleCenter.y = (playerCenter.y + PLAYER_HEIGHT / 2.0) - PLAYER_ROUNDED_RADIUS;
                rectCorner.x = rectRight;
                rectCorner.y = rectTop;
            }
            else if (playerCenter.x >= rectCenter.x && playerCenter.y >= rectCenter.y)
            {
                // ground is on the top left
                circleCenter.x = (playerCenter.x - PLAYER_WIDTH / 2.0) + PLAYER_ROUNDED_RADIUS;
                circleCenter.y = (playerCenter.y - PLAYER_HEIGHT / 2.0) + PLAYER_ROUNDED_RADIUS;
                rectCorner.x = rectRight;
                rectCorner.y = rectBot;
            }
            else
            {
                // ground is on the top right
                circleCenter.x = (playerCenter.x + PLAYER_WIDTH / 2.0) - PLAYER_ROUNDED_RADIUS;
                circleCenter.y = (playerCenter.y - PLAYER_HEIGHT / 2.0) + PLAYER_ROUNDED_RADIUS;
                rectCorner.x = rectLeft;
                rectCorner.y = rectBot;
            }

            // check if the corner distance is less than the radius
            float circleOffset = Vector2Distance(rectCorner, circleCenter);
            if (circleOffset <= PLAYER_ROUNDED_RADIUS)
            {
                // there is a collision to the corner of the rectangle
                retVal.colPosition = rectCorner;

                // calculate the surface normal by finding two points on the rectangle that
                // collide with the circle. The surfance normal will be perpendicualr to that
                Vector2 rectCornerNorm = Vector2Subtract(rectCorner, circleCenter);
                Vector2 point1;
                Vector2 point2;
                Vector2 surfVector;

                // calculate the circle x of the corner y
                point1.y = rectCornerNorm.y;
                point1.x = sqrtf((PLAYER_ROUNDED_RADIUS * PLAYER_ROUNDED_RADIUS) -
                    (point1.y * point1.y));
                if (rectCornerNorm.x < 0) point1.x = -point1.x;

                // calculate the circle y of the corner x
                point2.x = rectCornerNorm.x;
                point2.y = sqrtf((PLAYER_ROUNDED_RADIUS * PLAYER_ROUNDED_RADIUS) -
                    (point2.x * point2.x));
                if (rectCornerNorm.y < 0) point2.y = -point2.y;

                // calculate the normal from the two points generated
                surfVector = Vector2Normalize(Vector2Subtract(point1, point2));
                if (Vector2Equals(surfVector, Vector2Zero()))
                {
                    // the two points must have been equal. Just return the vector of the
                    // rect corner to the circle center instead of the surface normal
                    retVal.surfaceNormal = Vector2Normalize(Vector2Negate(rectCornerNorm));
                }
                else
                {
                    if ((rectCornerNorm.x < 0 && rectCornerNorm.y < 0) ||
                        (rectCornerNorm.x > 0 && rectCornerNorm.y > 0))
                    {
                        retVal.surfaceNormal.x = surfVector.y;
                        retVal.surfaceNormal.y = -surfVector.x;
                    }
                    else
                    {
                        retVal.surfaceNormal.x = -surfVector.y;
                        retVal.surfaceNormal.y = surfVector.x;
                    }
                }

                // remember to set the collision depth
                retVal.colDepth = circleOffset - PLAYER_ROUNDED_RADIUS;
            }
            // else no collision
        }
    }
    // else no collision, leave the retVal zeroed out

    // DEBUG
    if (retVal.surfaceNormal.x != 0 || retVal.surfaceNormal.y != 0)
    {
        printf("retVal.colDepth: %f\n", retVal.colDepth);
    }

    return retVal;
}

CollisionInfo TouchingTriGround(Player* pPlayer, TriGround* pTriGround)
{
    CollisionInfo retVal = { 0 }; // retVal will store the max depth
    retVal.colDepth = FLT_MAX;
    CollisionInfo outerBoxColInfo;
    CollisionInfo tempColInfo;
    Vector2 cornerCirclePos;
    bool centerRectTouching = false;

    // use SAT to see if the rectangle bounding box of the player and the triangle are
    // colliding. Get information about what rect/tri edges are colliding
    Vector2 rectVertArray[NUM_RECT_VERTECIES];
    rectVertArray[0].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f); // top left
    rectVertArray[0].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f);
    rectVertArray[1].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f); // top right
    rectVertArray[1].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f);
    rectVertArray[2].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f); // bottom right
    rectVertArray[2].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f);
    rectVertArray[3].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f); // bottom left
    rectVertArray[3].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f);

    // call the function. Shape1 is the player and shape2 is the ground
    outerBoxColInfo = SatCollisionPolyPoly(rectVertArray, NUM_RECT_VERTECIES,
        pTriGround->verteces, NUM_TRI_VERTECES, NULL);

    // DEBUG
    if (IsKeyDown(KEY_P))
    {
        return outerBoxColInfo;
    }

    // if the outer box is colliding, a more detailed analysis is needed
    if (outerBoxColInfo.surfaceNormal.x != 0 || outerBoxColInfo.surfaceNormal.y != 0)
    {
        // DEBUG
        printf("\nNEW COLLISION TO TEST\n");
        gShowDebugFloat[1] = true;

        // TODO need to make sure the collision location is in the correct spot

        // break the rounded rectangle into 6 shapes (maybe less depending on corner
        // radius). Check each shape using SAT and find the maximum depth where the
        // shapes are still touching. This will be the normal and depth we will use.
        // This may deturmine the player and triangle are not touching

        // wide box
        if (sideRectOnly(outerBoxColInfo.surfaceNormal))
        {
            rectVertArray[0].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f); // top left
            rectVertArray[0].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS;
            rectVertArray[1].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f); // top right
            rectVertArray[1].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS;
            rectVertArray[2].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f); // bottom right
            rectVertArray[2].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS;
            rectVertArray[3].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f); // bottom left
            rectVertArray[3].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS;
            tempColInfo = SatCollisionPolyPoly(rectVertArray, NUM_RECT_VERTECIES,
                pTriGround->verteces, NUM_TRI_VERTECES, &sideRectOnly);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth < retVal.colDepth)
            {
                retVal = tempColInfo;
                centerRectTouching = true;

                // DEBUG
                gDebugFloat[1] = 0;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  wide box depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }
        }

        // tall box
        if (topBotRectOnly(outerBoxColInfo.surfaceNormal))
        {
            rectVertArray[0].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS; // top left
            rectVertArray[0].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f);
            rectVertArray[1].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS; // top right
            rectVertArray[1].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f);
            rectVertArray[2].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS; // bottom right
            rectVertArray[2].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f);
            rectVertArray[3].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS; // bottom left
            rectVertArray[3].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f);
            tempColInfo = SatCollisionPolyPoly(rectVertArray, NUM_RECT_VERTECIES,
                pTriGround->verteces, NUM_TRI_VERTECES, &topBotRectOnly);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth < retVal.colDepth)
            {
                retVal = tempColInfo;
                centerRectTouching = true;

                // DEBUG
                gDebugFloat[1] = 1;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  tall box depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }
        }

        // top left corner circle
        if ((centerRectTouching && downRightOnlyExclusive(outerBoxColInfo.surfaceNormal)) ||
            (!centerRectTouching && downRightOnlyInclusive(outerBoxColInfo.surfaceNormal)))
        {
            // DEBUG
            gDebugFloat[0] = 2;

            cornerCirclePos.x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS;
            cornerCirclePos.y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS;
            tempColInfo = SatCollisionCirclePoly(cornerCirclePos, PLAYER_ROUNDED_RADIUS,
                pTriGround->verteces, NUM_TRI_VERTECES, &downRightOnlyInclusive);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth < retVal.colDepth)
            {
                retVal = tempColInfo;

                // DEBUG
                gDebugFloat[1] = 2;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  TL circle depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }
        }

        // top right corner circle
        if ((centerRectTouching && downLeftOnlyExclusive(outerBoxColInfo.surfaceNormal)) ||
            (!centerRectTouching && downLeftOnlyInclusive(outerBoxColInfo.surfaceNormal)))
        {
            // DEBUG
            gDebugFloat[0] = 3;

            cornerCirclePos.x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS;
            cornerCirclePos.y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS;
            tempColInfo = SatCollisionCirclePoly(cornerCirclePos, PLAYER_ROUNDED_RADIUS,
                pTriGround->verteces, NUM_TRI_VERTECES, &downLeftOnlyInclusive);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth < retVal.colDepth)
            {
                retVal = tempColInfo;

                // DEBUG
                gDebugFloat[1] = 3;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  TR circle depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }
        }

        // bottom right corner circle
        if ((centerRectTouching && upLeftOnlyExclusive(outerBoxColInfo.surfaceNormal)) ||
            (!centerRectTouching && upLeftOnlyInclusive(outerBoxColInfo.surfaceNormal)))
        {
            // DEBUG
            gDebugFloat[0] = 4;

            cornerCirclePos.x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS;
            cornerCirclePos.y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS;
            tempColInfo = SatCollisionCirclePoly(cornerCirclePos, PLAYER_ROUNDED_RADIUS,
                pTriGround->verteces, NUM_TRI_VERTECES, &upLeftOnlyInclusive);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth < retVal.colDepth)
            {
                retVal = tempColInfo;

                // DEBUG
                gDebugFloat[1] = 4;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  BR circle depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }
        }

        // bottom left corner circle
        if ((centerRectTouching && upRightOnlyExclusive(outerBoxColInfo.surfaceNormal)) ||
            (!centerRectTouching && upRightOnlyInclusive(outerBoxColInfo.surfaceNormal)))
        {
            // DEBUG
            gDebugFloat[0] = 5;

            cornerCirclePos.x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS;
            cornerCirclePos.y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS;
            tempColInfo = SatCollisionCirclePoly(cornerCirclePos, PLAYER_ROUNDED_RADIUS,
                pTriGround->verteces, NUM_TRI_VERTECES, &upRightOnlyInclusive);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth < retVal.colDepth)
            {
                retVal = tempColInfo;

                // DEBUG
                gDebugFloat[1] = 5;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  BL circle depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }
        }

        // DEBUG
        if (retVal.surfaceNormal.x != 0 || retVal.surfaceNormal.y != 0)
        {
            printf("retVal.colDepth: %f\n", retVal.colDepth);
        }

        // if none of the corner circles are colliding, double check that neither middle
        // rectangle is touching. This catches an edge case of the trangle going through
        // the middle of the player without any corner circles being touched
        if (retVal.surfaceNormal.x == 0 && retVal.surfaceNormal.y == 0 &&
            outerBoxColInfo.surfaceNormal.x != 0 && outerBoxColInfo.surfaceNormal.y != 0)
        {
            // take the smaller of these two cases
            retVal.colDepth = -FLT_MAX;

            // wide box
            rectVertArray[0].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f); // top left
            rectVertArray[0].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS;
            rectVertArray[1].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f); // top right
            rectVertArray[1].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS;
            rectVertArray[2].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f); // bottom right
            rectVertArray[2].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS;
            rectVertArray[3].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f); // bottom left
            rectVertArray[3].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS;
            tempColInfo = SatCollisionPolyPoly(rectVertArray, NUM_RECT_VERTECIES,
                pTriGround->verteces, NUM_TRI_VERTECES, &sideRectOnly);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth > retVal.colDepth)
            {
                retVal = tempColInfo;

                // DEBUG
                gDebugFloat[1] = 0;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  wide box depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }

            // tall box
            rectVertArray[0].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS; // top left
            rectVertArray[0].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f);
            rectVertArray[1].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS; // top right
            rectVertArray[1].y = pPlayer->position.y - (PLAYER_HEIGHT / 2.0f);
            rectVertArray[2].x = pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS; // bottom right
            rectVertArray[2].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f);
            rectVertArray[3].x = pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS; // bottom left
            rectVertArray[3].y = pPlayer->position.y + (PLAYER_HEIGHT / 2.0f);
            tempColInfo = SatCollisionPolyPoly(rectVertArray, NUM_RECT_VERTECIES,
                pTriGround->verteces, NUM_TRI_VERTECES, &topBotRectOnly);
            if ((tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0) &&
                tempColInfo.colDepth > retVal.colDepth)
            {
                retVal = tempColInfo;

                // DEBUG
                gDebugFloat[1] = 1;
            }

            // DEBUG
            if (tempColInfo.surfaceNormal.x != 0 || tempColInfo.surfaceNormal.y != 0)
            {
                printf("  tall box depth: %f, (%f, %f)\n", tempColInfo.colDepth, tempColInfo.surfaceNormal.x, tempColInfo.surfaceNormal.y);
            }
        }
    }

    return retVal;
}

CollisionInfo TouchingPlayer(Player* pPlayer1, Player* pPlayer2)
{
    CollisionInfo retVal = { 0 };

    // TODO not implemented

    return retVal;
}

void DrawPlayer(Player* pPlayer)
{
    // tall rectangle
    DrawRectangle(pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS,
        pPlayer->position.y - (PLAYER_HEIGHT / 2.0f),
        PLAYER_WIDTH - (2 * PLAYER_ROUNDED_RADIUS), PLAYER_HEIGHT, pPlayer->color);

    // wide rectangle
    DrawRectangle(pPlayer->position.x - (PLAYER_WIDTH / 2.0f),
        pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS,
        PLAYER_WIDTH, PLAYER_HEIGHT - 2 * PLAYER_ROUNDED_RADIUS, pPlayer->color);

    // top left corner circle
    DrawCircle(pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS,
        pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS,
        PLAYER_ROUNDED_RADIUS, pPlayer->color);

    // top right corner circle
    DrawCircle(pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS,
        pPlayer->position.y - (PLAYER_HEIGHT / 2.0f) + PLAYER_ROUNDED_RADIUS,
        PLAYER_ROUNDED_RADIUS, pPlayer->color);

    // bottom left corner circle
    DrawCircle(pPlayer->position.x - (PLAYER_WIDTH / 2.0f) + PLAYER_ROUNDED_RADIUS,
        pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS,
        PLAYER_ROUNDED_RADIUS, pPlayer->color);

    // bottom right corner circle
    DrawCircle(pPlayer->position.x + (PLAYER_WIDTH / 2.0f) - PLAYER_ROUNDED_RADIUS,
        pPlayer->position.y + (PLAYER_HEIGHT / 2.0f) - PLAYER_ROUNDED_RADIUS,
        PLAYER_ROUNDED_RADIUS, pPlayer->color);

    // draw all of the collision data as lines for collisions direction and dots
    // for collision position
    for (int c = 0; c < pPlayer->numGroundsTouched; c++)
    {
        CollisionInfo* info = &pPlayer->groundsTouchedArray[c];
        DrawLineEx(info->colPosition,
            Vector2Add(info->colPosition, Vector2Scale(info->surfaceNormal, COLLISION_INFO_LEN)),
            COLLISION_INFO_THICKNESS, COLLISION_INFO_COLOR);

        DrawCircleV(info->colPosition, COLLISION_INFO_RADIUS, COLLISION_INFO_COLOR);
    }
}

// *****PRIVATE FUNCTIONS*****

