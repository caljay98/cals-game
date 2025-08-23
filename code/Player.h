// Player.h

#ifndef PLAYER_H
#define PLAYER_H

// *****INCLUDES*****
#include "raylib.h"
#include "RectGround.h"
#include "TriGround.h"

// *****DEFINES*****
// TODO default mass, color?
#define MAX_GROUND_COLLISIONS (16) // some limit to the number of collisions to store

// *****TYPEDEFS*****
typedef struct Inputs
{
    bool leftInput;
    bool rightInput;
    bool jumpInput;
    bool shootInput;
} Inputs;

typedef struct CollisionInfo
{
    Vector2 colPosition;
    Vector2 surfaceNormal; // a surface normal of (0,0) is no collision
    float colDepth; // depth should always be negative
} CollisionInfo;

typedef struct Player
{
    Vector2 position; // position is the center of the player
    Vector2 velocity;
    Vector2 forces;
    float mass;
    
    // array for grounds that are being touched
    CollisionInfo groundsTouchedArray[MAX_GROUND_COLLISIONS];
    int numGroundsTouched;

    // TODO other players that are colliding and the directions for ground propogation

    Color color;
    Inputs inputs;
} Player;

// *****PUBLIC FUNCTION PROTOTYPES*****
void UpdatePlayerInput(Player* player);
CollisionInfo TouchingRectGround(Player* pPlayer, RectGround* pRectGround);
CollisionInfo TouchingTriGround(Player* pPlayer, TriGround* pTriGround);
CollisionInfo TouchingPlayer(Player* pPlayer1, Player* pPlayer2);
void DrawPlayer(Player* player);

#endif
