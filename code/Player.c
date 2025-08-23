// Player.c

// *****INCLUDES*****
#include "Player.h"
#include "raymath.h"
#include <float.h>

// DEBUG
#include <stdio.h>

// *****DEFINES*****
#define NUM_RECT_VERTECIES       (4)
#define MAX_POLYGON_VERTECIES    (4) // increase this if we need to support more complex polygons
#define PLAYER_WIDTH             (40.0f)
#define PLAYER_HEIGHT            (90.0f)
// DEBUG
//#define PLAYER_ROUNDED_RADIUS    (20.0f)
#define PLAYER_ROUNDED_RADIUS    (0.0f)

// vector drawing defines
#define COLLISION_INFO_COLOR     (ORANGE)
#define COLLISION_INFO_LEN       (15.0f)
#define COLLISION_INFO_THICKNESS (4.0f)
#define COLLISION_INFO_RADIUS    (5.0f)

// *****TYPEDEFS*****
typedef struct Projection
{
    float minVal;
    float maxVal;
} Projection;

// *****PRIVATE VARIABLES*****

// *****PRIVATE FUNCTION PROTOTYPES*****
static CollisionInfo satCollision(Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces);
static Projection projectPoly(Vector2* aVertexArray, int numVerteces, Vector2 axis);
static CollisionInfo checkProjectionCollision(Vector2 aTestEdge[],
    Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces);

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
    CollisionInfo retVal = { 0 };

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
    retVal = satCollision(rectVertArray, NUM_RECT_VERTECIES,
        pTriGround->verteces, NUM_TRI_VERTECES);

    // there is a collision, use which edges on the rect/tri are colliding to choose
    // a player corner circle to run the detailed detection on
    // TODO a few cases here

    // DEBUG just use SAT for now and debug

    // DEBUG
    if (retVal.surfaceNormal.x != 0 || retVal.surfaceNormal.y != 0)
    {
        printf("retVal.colDepth: %f\n", retVal.colDepth);
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
static CollisionInfo satCollision(Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces)
{
    // Roughly following https://dyn4j.org/2010/01/sat/. This will brake if the shapes
    // are not convex or edges cross each other
    CollisionInfo retVal = { 0 };
    retVal.colDepth = -FLT_MAX;
    CollisionInfo projColInfo;
    Vector2 testEdge[2];

    // check the edges one by one. Dont do them all first because this will fail most
    // of the time and should be fast. Make sure to track the minimim collision distance
    // and axis in the retVal
    for (int c = 0; c < shape1NumVerteces; c++)
    {
        // pick the edge to use
        testEdge[1] = aShape1VertexArray[(c + 1) % shape1NumVerteces];
        testEdge[0] = aShape1VertexArray[c];

        // check if the shapes are colliding along this axis
        projColInfo = checkProjectionCollision(testEdge,
            aShape1VertexArray, shape1NumVerteces, aShape2VertexArray, shape2NumVerteces);

        if (projColInfo.surfaceNormal.x == 0 && projColInfo.surfaceNormal.y == 0)
        {
            // there is no collision because the two shapes do not overlap when projected
            // along this axis. Return new collision
            retVal.surfaceNormal.x = 0;
            retVal.surfaceNormal.y = 0;

            return retVal;
        }
        else
        {
            // see if this is the collision axis with the smallest depth. Update the retVal
            // if it is
            if (projColInfo.colDepth > retVal.colDepth)
            {
                retVal = projColInfo;
            }
        }
    }
    // Do the same thing for shape2
    for (int c = 0; c < shape2NumVerteces; c++)
    {
        // pick the edge to check
        testEdge[1] = aShape2VertexArray[(c + 1) % shape2NumVerteces];
        testEdge[0] = aShape2VertexArray[c];

        // check if the shapes are colliding along this axis
        projColInfo = checkProjectionCollision(testEdge,
            aShape2VertexArray, shape2NumVerteces, aShape1VertexArray, shape1NumVerteces);

        if (projColInfo.surfaceNormal.x == 0 && projColInfo.surfaceNormal.y == 0)
        {
            // there is no collision because the two shapes do not overlap when projected
            // along this axis. Return new collision
            retVal.surfaceNormal.x = 0;
            retVal.surfaceNormal.y = 0;

            return retVal;
        }
        else
        {
            // see if this is the collision axis with the smallest depth. Update the retVal
            // if it is
            if (projColInfo.colDepth > retVal.colDepth)
            {
                retVal = projColInfo;
            }
        }
    }

    // we are colliding according to SAT. The surface normal will be the one with the smallest
    // collision depth (already being tracked), but the normal direction may need to be flipped
    // so it is pointing towards the center of shape 1
    // TODO
    //retVal.surfaceNormal = Vector2Normalize(retVal.surfaceNormal);

    // find the position of the collision (shape 1 bias)
    // TODO

    return retVal;
}

static Projection projectPoly(Vector2* aVertexArray, int numVerteces, Vector2 axis)
{
    // return the min and max value for an arbitrary convex polygon projected
    // onto the inputed axis

    Projection retVal = { 0 };
    retVal.minVal = FLT_MAX;
    retVal.maxVal = -FLT_MAX;

    if (numVerteces <= 0)
    {
        // numVerteces <= 0 is invalid. Return (0, 0)
        retVal.minVal = 0;
        retVal.maxVal = 0;
    }
    if (numVerteces > 0)
    {
        // dot each point in the vertex array with the axis, storing the min and max
        for (int c = 0; c < numVerteces; c++)
        {
            float p = Vector2DotProduct(aVertexArray[c], axis);
            if (p < retVal.minVal) retVal.minVal = p;
            if (p > retVal.maxVal) retVal.maxVal = p;
        }
    }

    return retVal;
}

static CollisionInfo checkProjectionCollision(Vector2 aTestEdge[],
    Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces)
{
    // aTestEdge always has size 2 (2 points for an edge)
    // aTestEdge must be from the aShape1VertexArray passed into the function

    CollisionInfo retVal = { 0 };
    retVal.colPosition.x = (aTestEdge[0].x + aTestEdge[1].x) / 2.0f; // colPosition is the middle of the tested edge
    retVal.colPosition.y = (aTestEdge[0].y + aTestEdge[1].y) / 2.0f;

    // generate the axis of projection
    Vector2 testAxis = Vector2Subtract(aTestEdge[1], aTestEdge[0]);
    float tempFlt = testAxis.x;
    testAxis.x = -testAxis.y;
    testAxis.y = tempFlt;
    testAxis = Vector2Normalize(testAxis);

    // generate the projections
    Projection proj1 = projectPoly(aShape1VertexArray, shape1NumVerteces, testAxis);
    Projection proj2 = projectPoly(aShape2VertexArray, shape2NumVerteces, testAxis);
    Projection edgeProj = projectPoly(aTestEdge, 2, testAxis); // min and max should be the same
    float edgeProjVal = edgeProj.minVal;

    if ((proj2.maxVal >= proj1.minVal && proj2.maxVal <= proj1.maxVal) ||
        (proj2.minVal >= proj1.minVal && proj2.minVal <= proj1.maxVal) ||
        (proj1.minVal >= proj2.minVal && proj1.minVal <= proj2.maxVal) ||
        (proj1.maxVal >= proj2.minVal && proj1.maxVal <= proj2.maxVal))
    {
        // the two projections overlap. The depth is the distance of the edgeProj to proj2,
        // in the direction of the other end of proj1
        if (fabsf(proj1.maxVal - edgeProjVal) <= fabsf(proj1.minVal - edgeProjVal))
        {
            // edgeProj is closest to proj1.maxVal (should be equal), meaning the colDepth
            // is in the negative direction (towards minVal). We should test the distance
            // of proj2.minVal
            retVal.colDepth = -fabsf(proj2.minVal - edgeProjVal);
        }
        else
        {
            // edgeProj is closest to proj1.minVal (should be equal), meaning the colDepth
            // is in the positive direction (towards maxVal). We should test the distance
            // of proj2.maxVal
            retVal.colDepth = -fabsf(proj2.maxVal - edgeProjVal);
        }
        retVal.surfaceNormal = testAxis;
    }

    return retVal;
}

static Projection projectCircle(Vector2 circleCenter, float radius, Vector2 axis)
{
    Projection retVal = { 0 };

    // TODO

    return retVal;
}
