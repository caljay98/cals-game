// CollisionLib.c

// *****INCLUDES*****
#include "CollisionLib.h"
#include "raymath.h"
#include <float.h>

// *****DEFINES*****

// *****TYPEDEFS*****

// *****PRIVATE VARIABLES*****

// *****PRIVATE FUNCTION PROTOTYPES*****
static CollisionInfo checkProjectionCollision(Vector2 aTestEdge[],
    Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces);
static CollisionInfo checkProjectionCollisionPolyCircle(Vector2 aTestEdge[],
    Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2 circleCenter, float circleRadius);
static CollisionInfo checkProjectionCollisionCirclePoly(Vector2 aTestEdge[],
    Vector2 circleCenter, float circleRadius,
    Vector2* aShape2VertexArray, int shape2NumVerteces);
static Projection projectPoly(Vector2* aVertexArray, int numVerteces, Vector2 axis);
static Projection projectCircle(Vector2 circleCenter, float radius, Vector2 axis);
static Vector2 moveVectorInDirection(Vector2 v, Vector2 target, float distance);

// *****PUBLIC FUNCTIONS*****
CollisionInfo SatCollisionPolyPoly(Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces, bool (*testFunc)(Vector2))
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
                // check if this is a valid collision direction based on the function pointer
                if (!testFunc || testFunc(projColInfo.surfaceNormal))
                {
                    retVal = projColInfo;
                }
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
                // invert the direction so it is pointing towards shape1
                projColInfo.surfaceNormal = Vector2Negate(projColInfo.surfaceNormal);

                // check if this is a valid collision direction based on the function pointer
                if (!testFunc || testFunc(projColInfo.surfaceNormal))
                {
                    retVal = projColInfo;
                }
            }
        }
    }

    return retVal;
}

CollisionInfo SatCollisionCirclePoly(Vector2 circleCenter, float circleRadius,
    Vector2* aShape2VertexArray, int shape2NumVerteces, bool (*testFunc)(Vector2))
{
    CollisionInfo retVal = { 0 };
    retVal.colDepth = -FLT_MAX;
    CollisionInfo projColInfo;
    Vector2 testEdge[2];

    // test axis generated from the edges of shape2
    for (int c = 0; c < shape2NumVerteces; c++)
    {
        // pick the edge to check
        testEdge[1] = aShape2VertexArray[(c + 1) % shape2NumVerteces];
        testEdge[0] = aShape2VertexArray[c];

        // check if the shapes are colliding along this axis
        projColInfo = checkProjectionCollisionPolyCircle(testEdge,
            aShape2VertexArray, shape2NumVerteces, circleCenter, circleRadius);

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
                // invert the direction so it is pointing towards shape1
                projColInfo.surfaceNormal = Vector2Negate(projColInfo.surfaceNormal);

                // check if this is a valid collision direction based on the function pointer
                if (!testFunc || testFunc(projColInfo.surfaceNormal))
                {
                    retVal = projColInfo;
                }
            }
        }
    }
    // generate a new "edge" tangent to the circle and perpendicular to the line from the
    // circle center to the nearest vertex on the polygon
    Vector2 closestVertex = { 0 };
    Vector2 tangentPoint;
    Vector2 perpVector;
    float minDist = FLT_MAX;
    float tempFlt;
    for (int c = 0; c < shape2NumVerteces; c++)
    {
        tempFlt = Vector2DistanceSqr(circleCenter, aShape2VertexArray[c]);
        if (tempFlt < minDist)
        {
            minDist = tempFlt;
            closestVertex = aShape2VertexArray[c];
        }
    }
    // generate the edge
    tangentPoint = moveVectorInDirection(circleCenter, closestVertex, circleRadius);
    perpVector = Vector2Subtract(circleCenter, closestVertex);
    tempFlt = perpVector.x;
    perpVector.x = -perpVector.y;
    perpVector.y = tempFlt;
    testEdge[0] = Vector2Add(tangentPoint, perpVector);
    testEdge[1] = Vector2Subtract(tangentPoint, perpVector);

    // test this generated edge for collision
    projColInfo = checkProjectionCollisionCirclePoly(testEdge,
        circleCenter, circleRadius, aShape2VertexArray, shape2NumVerteces);

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
            // check if this is a valid collision direction based on the function pointer
            if (!testFunc || testFunc(projColInfo.surfaceNormal))
            {
                retVal = projColInfo;
            }
        }
    }

    return retVal;
}

// directionality contitional checks

bool SideRectOnly(Vector2 direction)
{
    // used for rectangle collision, only the sides should be counted
    return (direction.x != 0 && direction.y == 0);
}

bool TopBotRectOnly(Vector2 direction)
{
    // used for rectangle collision, only the top and bottom should be counted
    return (direction.x == 0 && direction.y != 0);
}

bool UpLeftOnlyExclusive(Vector2 direction)
{
    // vector going up and to the left
    return (direction.x < 0 && direction.y < 0);
}

bool UpRightOnlyExclusive(Vector2 direction)
{
    // vector going up and to the right
    return (direction.x > 0 && direction.y < 0);
}

bool DownLeftOnlyExclusive(Vector2 direction)
{
    // vector going down and to the left
    return (direction.x < 0 && direction.y > 0);
}

bool DownRightOnlyExclusive(Vector2 direction)
{
    // vector going down and to the right
    return (direction.x > 0 && direction.y > 0);
}

bool UpLeftOnlyInclusive(Vector2 direction)
{
    // vector going up and to the left
    return (direction.x <= 0 && direction.y <= 0);
}

bool UpRightOnlyInclusive(Vector2 direction)
{
    // vector going up and to the right
    return (direction.x >= 0 && direction.y <= 0);
}

bool DownLeftOnlyInclusive(Vector2 direction)
{
    // vector going down and to the left
    return (direction.x <= 0 && direction.y >= 0);
}

bool DownRightOnlyInclusive(Vector2 direction)
{
    // vector going down and to the right
    return (direction.x >= 0 && direction.y >= 0);
}

// *****PRIVATE FUNCTIONS*****
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

            // switch the normal vector direction to point inwards towards the shape
            testAxis.x = -testAxis.x;
            testAxis.y = -testAxis.y;
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

static CollisionInfo checkProjectionCollisionPolyCircle(Vector2 aTestEdge[],
    Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2 circleCenter, float circleRadius)
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
    Projection proj2 = projectCircle(circleCenter, circleRadius, testAxis);
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

            // switch the normal vector direction to point inwards towards the shape
            testAxis.x = -testAxis.x;
            testAxis.y = -testAxis.y;
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

static CollisionInfo checkProjectionCollisionCirclePoly(Vector2 aTestEdge[],
    Vector2 circleCenter, float circleRadius,
    Vector2* aShape2VertexArray, int shape2NumVerteces)
{
    // aTestEdge always has size 2 (2 points for an edge)
    // aTestEdge must be from the tangent to the circle passed in

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
    Projection proj1 = projectCircle(circleCenter, circleRadius, testAxis);
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

            // switch the normal vector direction to point inwards towards the shape
            testAxis.x = -testAxis.x;
            testAxis.y = -testAxis.y;
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

static Projection projectCircle(Vector2 circleCenter, float radius, Vector2 axis)
{
    Projection retVal = { 0 };
    float p;

    p = Vector2DotProduct(circleCenter, axis);
    retVal.minVal = p - radius;
    retVal.maxVal = p + radius;

    return retVal;
}

static Vector2 moveVectorInDirection(Vector2 v, Vector2 target, float distance)
{
    Vector2 result = { 0 };
    Vector2 dir;

    dir.x = target.x - v.x;
    dir.y = target.y - v.y;

    dir = Vector2Normalize(dir);

    result.x = v.x + dir.x * distance;
    result.y = v.y + dir.y * distance;

    return result;
}