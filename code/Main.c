// Main.c
// TODO right now this is mix of server and client functions in order to test
// basic movemnt and collision. This should be broken into Server.c and Client.c
// when ready

// *****INCLUDES*****
#include <stdio.h>
#include "raylib.h"
#include "Player.h"
#include "World.h"

// *****DEFINES*****
#define TARGET_FPS 60
#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080

// *****TYPEDEFS*****

// *****PRIVATE VARIABLES*****
static World world = { 0 };

// *****PRIVATE FUNCTION PROTOTYPES*****
static void init(Camera2D* pCamera);
static void update(void);
static void updateCamera(Camera2D* pCamera);
static void drawGui(void);

// *****PUBLIC FUNCTIONS*****
int main(int argc, char** argv)
{
    static Camera2D camera = {0};
    init(&camera);

    while (!WindowShouldClose())
    {
        update();
        BeginDrawing();
        {
            ClearBackground(LIGHTGRAY);
            BeginMode2D(camera);
            {
                DrawWorld(&world);
            }
            EndMode2D();
            drawGui();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// *****PRIVATE FUNCTIONS*****
static void init(Camera2D* pCamera)
{
    InitWindow(FRAME_WIDTH, FRAME_HEIGHT, "Title");
    // TODO better camera init
    pCamera->target.x = 500;
    pCamera->target.y = 250;
    pCamera->offset = (Vector2){ FRAME_WIDTH/2.0f, FRAME_HEIGHT/2.0f };
    pCamera->rotation = 0.0f;
    pCamera->zoom = 1.5f;

    SetTargetFPS(TARGET_FPS);

    // setup the world, ground and players and such
    world.numRectGrounds = 3;
    world.rectGroundArray[0].position.x = 0;
    world.rectGroundArray[0].position.y = 500;
    world.rectGroundArray[0].width = 1000;
    world.rectGroundArray[0].height = 80;
    world.rectGroundArray[0].color = BLUE;
    
    world.rectGroundArray[1].position.x = 200;
    world.rectGroundArray[1].position.y = 300;
    world.rectGroundArray[1].width = 200;
    world.rectGroundArray[1].height = 190;
    world.rectGroundArray[1].color = BLUE;

    world.rectGroundArray[2].position.x = 600;
    world.rectGroundArray[2].position.y = 300;
    world.rectGroundArray[2].width = 200;
    world.rectGroundArray[2].height = 50;
    world.rectGroundArray[2].color = BLUE;

    world.numTriGrounds = 1;
    world.triGroundArray[0].verteces[0].x = 400;
    world.triGroundArray[0].verteces[0].y = 400;
    world.triGroundArray[0].verteces[1].x = 400;
    world.triGroundArray[0].verteces[1].y = 600;
    world.triGroundArray[0].verteces[2].x = 500;
    world.triGroundArray[0].verteces[2].y = 500;
    world.triGroundArray[0].color = GREEN;

    world.playerArray[0].position.x = 450;
    world.playerArray[0].position.y = 100;
    world.playerArray[0].color = RED;
    world.playerActive[0] = true;
}

static void update(void)
{
    // DEBUG direct player input
    world.playerArray[0].inputs.leftInput = IsKeyDown(KEY_A);
    world.playerArray[0].inputs.rightInput = IsKeyDown(KEY_D);
    world.playerArray[0].inputs.jumpInput = IsKeyDown(KEY_W);
    world.playerArray[0].inputs.shootInput = IsKeyDown(KEY_S); // DEBUG down for now

    // DEBUG control the TriGround to see how they can and cannot be drawn
    static int selectedVert = 0;
    static bool enterPressed = false;
    float changeSpeed = 4.0;
    // enter changes the selected vertex
    if (IsKeyDown(KEY_ENTER) && enterPressed == false)
    {
        enterPressed = true;
        selectedVert++;
        if (selectedVert >= 3) selectedVert = 0;
    }
    if (!IsKeyDown(KEY_ENTER))
    {
        enterPressed = false;
    }

    if (IsKeyDown(KEY_UP))
    {
        world.triGroundArray[0].verteces[selectedVert].y -= changeSpeed;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        world.triGroundArray[0].verteces[selectedVert].y += changeSpeed;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        world.triGroundArray[0].verteces[selectedVert].x -= changeSpeed;
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        world.triGroundArray[0].verteces[selectedVert].x += changeSpeed;
    }

    UpdateWorld(&world);
}

static void updateCamera(Camera2D* pCamera)
{
    // TODO
}

static void drawGui(void)
{
    char text[255];
    sprintf(text, "Frame Time: %2.3fms", GetFrameTime()*1000.0f);
    DrawText(text, 20, 20, 50, BLACK);

}
