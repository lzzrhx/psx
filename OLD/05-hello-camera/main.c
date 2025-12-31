#include <stdlib.h>
#include <sys/types.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include "globals.h"
#include "display.h"
#include "joypad.h"
#include "camera.h"

#define VIDEO_MODE 0
#define SCREEN_RES_X 320
#define SCREEN_RES_Y 240
#define SCREEN_CENTER_X (SCREEN_RES_X >> 1)
#define SCREEN_CENTER_Y (SCREEN_RES_Y >> 1)
#define SCREEN_Z 320



///////////////////////////////////////////////////////////////////////////////
// Vertices and face indices
///////////////////////////////////////////////////////////////////////////////
typedef struct Cube {
    SVECTOR rotation;
    VECTOR position;
    VECTOR scale;
    VECTOR vel;
    VECTOR acc;
    SVECTOR vertices[8];
    short faces[24];
} Cube;

typedef struct Floor {
    SVECTOR rotation;
    VECTOR position;
    VECTOR scale;
    SVECTOR vertices[4];
    short faces[6];
} Floor;



///////////////////////////////////////////////////////////////////////////////
// Declarations and global variables
///////////////////////////////////////////////////////////////////////////////
POLY_G4 *polyg4;
POLY_G3 *polyg3;

MATRIX worldmat = {0};
MATRIX viewmat = {0};

Camera camera;

Cube cube = {
    {0, 0, 0},
    {0, -400, 1800},
    {ONE, ONE, ONE},
    {0, 0, 0},
    {0, 1, 0},
    {
        { -128, -128, -128},
        {  128, -128, -128},
        {  128, -128,  128},
        { -128, -128,  128},
        { -128,  128, -128},
        {  128,  128, -128},
        {  128,  128,  128},
        { -128,  128,  128},
    },
    {
        3, 2, 0, 1,
        0, 1, 4, 5,
        4, 5, 7, 6,
        1, 2, 5, 6,
        2, 3, 6, 7,
        3, 0, 7, 4,
    },
};

Floor floor = {
    {0, 0, 0},
    {0, 450, 1800},
    {ONE, ONE, ONE},
    {
        {-900, 0, -900},
        {-900, 0,  900},
        { 900, 0, -900},
        { 900, 0,  900},
    },
    {
        0, 1, 2,
        1, 3, 2,
    },
};



///////////////////////////////////////////////////////////////////////////////
// Setup function that is called once at the beginning of the execution
///////////////////////////////////////////////////////////////////////////////
void Setup(void) {
    ScreenInit();
    // Initialize the joypad
    JoyPadInit();
    // Reset next primitive pointer
    ResetNextPrim(GetCurrBuff());    
    // Initializes the camera object
    camera.position.vx = 500;
    camera.position.vy = -1000;
    camera.position.vz = -1500;
    camera.lookat = (MATRIX){0};
}



///////////////////////////////////////////////////////////////////////////////
// Update function called every frame
///////////////////////////////////////////////////////////////////////////////
void Update(void) {
    int i, nclip;
    long otz, p, flg;
    // Clear ordering table
    EmptyOT(GetCurrBuff());
    // Update the state of the controller and read input
    JoyPadUpdate();
    if (JoyPadCheck(PAD1_LEFT)) {
        camera.position.vx -= 50;
    }
    if (JoyPadCheck(PAD1_RIGHT)) {
        camera.position.vx += 50;
    }
    if (JoyPadCheck(PAD1_UP)) {
        camera.position.vy -= 50;
    }
    if (JoyPadCheck(PAD1_DOWN)) {
        camera.position.vy += 50;
    }
    if (JoyPadCheck(PAD1_CROSS)) {
        camera.position.vz += 50;
    }
    if (JoyPadCheck(PAD1_CIRCLE)) {
        camera.position.vz -= 50;
    }
    // Update the cube position based on acceleration and velocity
    cube.vel.vx += cube.acc.vx;
    cube.vel.vy += cube.acc.vy;
    cube.vel.vz += cube.acc.vz;
    cube.position.vx += (cube.vel.vx) >> 1;
    cube.position.vy += (cube.vel.vy) >> 1;
    cube.position.vz += (cube.vel.vz) >> 1;
    if (cube.position.vy + 150 > floor.position.vy) {
        cube.vel.vy *= -1;
        //cube.position.vy = ...;
    }
    // Compute the camera Lookat matrix for this frame
    LookAt(&camera, &camera.position, &cube.position, &(VECTOR){0, -ONE, 0});
    // Draw the cube
    RotMatrix(&cube.rotation, &worldmat);
    TransMatrix(&worldmat, &cube.position);
    ScaleMatrix(&worldmat, &cube.scale);
    // Create the View Matrix combining the world matrix & lookat matrix
    CompMatrixLV(&camera.lookat, &worldmat, &viewmat);
    SetRotMatrix(&viewmat);
    SetTransMatrix(&viewmat);
    for (i = 0; i < 6 * 4; i += 4) {
        polyg4 = (POLY_G4*) GetNextPrim();
        setPolyG4(polyg4);
        setRGB0(polyg4, 255,   0, 255);
        setRGB1(polyg4, 255, 255,   0);
        setRGB2(polyg4,   0, 255, 255);
        setRGB3(polyg4,   0, 255,   0);
        nclip = RotAverageNclip4(
            &cube.vertices[cube.faces[i]], 
            &cube.vertices[cube.faces[i+1]], 
            &cube.vertices[cube.faces[i+2]], 
            &cube.vertices[cube.faces[i+3]], 
            (long*)&polyg4->x0, 
            (long*)&polyg4->x1, 
            (long*)&polyg4->x2, 
            (long*)&polyg4->x3, 
            &p, &otz, &flg
        );
        if (nclip <= 0) {
            continue;
        }
        if ((otz > 0) && (otz < OT_LEN)) {
            addPrim(GetOTAt(GetCurrBuff(), otz), polyg4);
            IncrementNextPrim(sizeof(POLY_G4));
        }
    }
    // Draw the floor
    RotMatrix(&floor.rotation, &worldmat);
    TransMatrix(&worldmat, &floor.position);
    ScaleMatrix(&worldmat, &floor.scale);
    // Create the View Matrix combining the world matrix & lookat matrix
    CompMatrixLV(&camera.lookat, &worldmat, &viewmat);
    SetRotMatrix(&viewmat);
    SetTransMatrix(&viewmat);
    for (i = 0; i < 6; i += 3) {
        polyg3 = (POLY_G3*) GetNextPrim();
        setPolyG3(polyg3);
        setRGB0(polyg3, 255, 0, 255);
        setRGB1(polyg3, 0, 255, 255);
        setRGB2(polyg3, 255, 255, 0);
        nclip = RotAverageNclip3(
            &floor.vertices[floor.faces[i]],
            &floor.vertices[floor.faces[i+1]],
            &floor.vertices[floor.faces[i+2]],
            (long*)&polyg3->x0,
            (long*)&polyg3->x1,
            (long*)&polyg3->x2,
            &p, &otz, &flg
        );
        if (nclip <= 0) {
            continue;
        }
        if ((otz > 0) && (otz < OT_LEN)) {
            addPrim(GetOTAt(GetCurrBuff(), otz), polyg3);
            IncrementNextPrim(sizeof(POLY_G3));
        }
    }
    // Update the floor rotation
    floor.rotation.vy += 5;
}



///////////////////////////////////////////////////////////////////////////////
// Render function that invokes the draw of the current frame
///////////////////////////////////////////////////////////////////////////////
void Render(void) {
    DisplayFrame();
}



///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(void) {
    Setup();
    while (1) {
        Update();
        Render();
    }
    return 0;
}
