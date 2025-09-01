// DebugLib.c

// *****INCLUDES*****
#include "DebugLib.h"
#include <stdio.h>

// *****DEFINES*****

// *****TYPEDEFS*****

// *****PUBLIC VARIABLES*****
float gDebugFloat[NUM_DEBUG_FLOATS] = { 0 };
bool gShowDebugFloat[NUM_DEBUG_FLOATS] = { 0 };
Vector2 gDebugPoints[NUM_DEBUG_POINTS] = { 0 };
bool gShowDebugPoints[NUM_DEBUG_POINTS] = { 0 };
Vector2 gDebugEdges[NUM_DEBUG_EDGES][2] = { 0 };
bool gShowDebugEdges[NUM_DEBUG_EDGES] = { 0 };

// *****PRIVATE VARIABLES*****
static Color debugPointColors[NUM_DEBUG_POINTS] = {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    PURPLE
};
static Color debugEdgeColors[NUM_DEBUG_EDGES] = {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    PURPLE
};

// *****PRIVATE FUNCTION PROTOTYPES*****

// *****PUBLIC FUNCTIONS*****
void DrawDebugFloatsOnGui(void)
{
    char text[255];
    int textY = 20;
    sprintf(text, "Frame Time: %2.3fms", GetFrameTime() * 1000.0f);
    DrawText(text, 20, textY, DEBUG_FLOAT_TEXT_SIZE, BLACK);
    textY += DEBUG_FLOAT_TEXT_SIZE;

    for (int c = 0; c < NUM_DEBUG_FLOATS; c++)
    {
        if (gShowDebugFloat[c] == true)
        {
            sprintf(text, "FLOAT%u: %f", c, gDebugFloat[c]);
            DrawText(text, 20, textY, DEBUG_FLOAT_TEXT_SIZE, BLACK);
            textY += DEBUG_FLOAT_TEXT_SIZE;
        }
    }
}

void DrawDebugPoints(void)
{
    for (int c = 0; c < NUM_DEBUG_POINTS; c++)
    {
        if (gShowDebugPoints[c] == true)
        {
            DrawCircleV(gDebugPoints[c], DEBUG_POINT_RADIUS, debugPointColors[c]);
        }
    }
}

void DrawDebugEdges(void)
{
    for (int c = 0; c < NUM_DEBUG_EDGES; c++)
    {
        if (gShowDebugEdges[c] == true)
        {
            DrawLineEx(gDebugEdges[c][0], gDebugEdges[c][1],
                DEBUG_EDGE_THICKNESS, debugEdgeColors[c]);
        }
    }
}

// *****PRIVATE FUNCTIONS*****
