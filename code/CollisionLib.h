// CollisionLib.h

#ifndef COLLISION_LIB_H
#define COLLISION_LIB_H

// *****INCLUDES*****
#include "raylib.h"

// *****DEFINES*****

// *****TYPEDEFS*****
typedef struct CollisionInfo
{
    Vector2 colPosition;
    Vector2 surfaceNormal; // a surface normal of (0,0) is no collision
    float colDepth; // depth should always be negative
} CollisionInfo;

typedef struct Projection
{
    float minVal;
    float maxVal;
} Projection;

// *****PUBLIC FUNCTION PROTOTYPES*****
CollisionInfo SatCollisionPolyPoly(Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces, bool (*testFunc)(Vector2));
CollisionInfo SatCollisionCirclePoly(Vector2 circleCenter, float circleRadius,
    Vector2* aShape2VertexArray, int shape2NumVerteces, bool (*testFunc)(Vector2));

// directionality contitional checks
bool sideRectOnly(Vector2 direction);
bool topBotRectOnly(Vector2 direction);
// exclusive only takes diagnal vectors
bool upLeftOnlyExclusive(Vector2 direction);
bool upRightOnlyExclusive(Vector2 direction);
bool downLeftOnlyExclusive(Vector2 direction);
bool downRightOnlyExclusive(Vector2 direction);
// inclusive counts horizontal and vertical vectors too
bool upLeftOnlyInclusive(Vector2 direction);
bool upRightOnlyInclusive(Vector2 direction);
bool downLeftOnlyInclusive(Vector2 direction);
bool downRightOnlyInclusive(Vector2 direction);

#endif
