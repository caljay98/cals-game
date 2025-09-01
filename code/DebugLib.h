// DebugLib.h

#ifndef DEBUG_LIB_H
#define DEBUG_LIB_H

// *****INCLUDES*****
#include "raylib.h"

// *****DEFINES*****
#define NUM_DEBUG_FLOATS 6
#define NUM_DEBUG_POINTS 6
#define NUM_DEBUG_EDGES  6

#define DEBUG_FLOAT_TEXT_SIZE 30.0f
#define DEBUG_POINT_RADIUS 2.0f
#define DEBUG_EDGE_THICKNESS 1.0f

// *****TYPEDEFS*****

// *****PUBLIC VARIABLES*****
extern float gDebugFloat[NUM_DEBUG_FLOATS];
extern bool gShowDebugFloat[NUM_DEBUG_FLOATS];
extern Vector2 gDebugPoints[NUM_DEBUG_POINTS];
extern bool gShowDebugPoints[NUM_DEBUG_POINTS];
extern Vector2 gDebugEdges[NUM_DEBUG_EDGES][2];
extern bool gShowDebugEdges[NUM_DEBUG_EDGES];

// *****PUBLIC FUNCTION PROTOTYPES*****
void DrawDebugFloatsOnGui(void);
void DrawDebugPoints(void);
void DrawDebugEdges(void);

#endif
