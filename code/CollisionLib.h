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
CollisionInfo satCollision(Vector2* aShape1VertexArray, int shape1NumVerteces,
    Vector2* aShape2VertexArray, int shape2NumVerteces);

#endif
