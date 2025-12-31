#include <stdlib.h>
#include <sys/types.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
//#include "inline_n.h"

#define VIDEO_MODE 0
#define SCREEN_RES_X 320
#define SCREEN_RES_Y 240
#define SCREEN_CENTER_X (SCREEN_RES_X >> 1)
#define SCREEN_CENTER_Y (SCREEN_RES_Y >> 1)
#define SCREEN_Z 320
#define OT_LENGTH 2048


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
typedef struct {
    DRAWENV draw[2];
    DISPENV disp[2];
} DoubleBuff;

DoubleBuff screen;
u_short currbuff;
u_long ot[2][OT_LENGTH]; // Ordering table
char primbuff[2][2048]; // Primitive buffer
char *nextprim; // Next primitive pointer

POLY_G4 *polyg4;
//POLY_F3 *polyf3;

MATRIX world = {0};

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
// Initialize display mode and setup double buffering
///////////////////////////////////////////////////////////////////////////////
void ScreenInit(void) {
    // Reset GPU
    ResetGraph(0);
    // Set the display area of the first buffer
    SetDefDispEnv(&screen.disp[0], 0,   0, 320, 240);
    SetDefDrawEnv(&screen.draw[0], 0, 240, 320, 240);
    // Set the display area of the second buffer
    SetDefDispEnv(&screen.disp[1], 0, 240, 320, 240);
    SetDefDrawEnv(&screen.draw[1], 0,   0, 320, 240);
    // Set the back/drawing buffer
    screen.draw[0].isbg = 1;
    screen.draw[1].isbg = 1;
    // Set the background clear color
    setRGB0(&screen.draw[0], 28, 28, 28);
    setRGB0(&screen.draw[1], 28, 28, 28);
    // Set the current initial buffer
    currbuff = 0;
    PutDispEnv(&screen.disp[currbuff]);
    PutDrawEnv(&screen.draw[currbuff]);
    // Initialize and setup the GTE geometry offsets
    InitGeom();
    SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y);
    SetGeomScreen(SCREEN_Z);
    // Enable display
    SetDispMask(1);
}



///////////////////////////////////////////////////////////////////////////////
// Draw the current frame primitives in the ordering table
///////////////////////////////////////////////////////////////////////////////
void DisplayFrame(void) {
    DrawSync(0);
    VSync(0);
    PutDispEnv(&screen.disp[currbuff]);
    PutDrawEnv(&screen.draw[currbuff]);
    // Draw the ordering table for the current buffer
    DrawOTag(ot[currbuff] + OT_LENGTH-1);
    // Swap current buffer
    currbuff = !currbuff;
    // Reset next primitive pointer
    nextprim = primbuff[currbuff];
}



///////////////////////////////////////////////////////////////////////////////
// Setup function that is called once at the beginning of the execution
///////////////////////////////////////////////////////////////////////////////
void Setup(void) {
    ScreenInit();
    // Reset next primitive pointer
    nextprim = primbuff[currbuff];
}



///////////////////////////////////////////////////////////////////////////////
// Update function called every frame
///////////////////////////////////////////////////////////////////////////////
void Update(void) {
    int i, nclip;
    long otz, p, flg;
    // Clear ordering table
    ClearOTagR(ot[currbuff], OT_LENGTH);
    /*
    // Draw the floor
    RotMatrix(&floor.rotation, &world);
    TransMatrix(&world, &floor.position);
    ScaleMatrix(&world, &floor.scale);
    SetRotMatrix(&world);
    SetTransMatrix(&world);
    for (i = 0; i < 6; i += 3) {
        polyf3 = (POLY_F3*) nextprim;
        setPolyF3(polyf3);
        setRGB0(polyf3, 255, 0, 255);
        *//*
        // Load the 3 vertices into the GTE registers
        gte_ldv0(&floor.vertices[floor.faces[i + 0]]);
        gte_ldv1(&floor.vertices[floor.faces[i + 1]]);
        gte_ldv2(&floor.vertices[floor.faces[i + 2]]);
        // Perform RTPT with the 3 vertices
        gte_rtpt();
        // Perform the Nclip calculation
        gte_nclip();
        gte_stopz(&nclip);
        // Only proceed if the normal of the face is facing the viewer
        if (nclip >= 0) {
            // Store the transformed & projected points into the poly primitive
            gte_stsxy3(&polyf3->x0, &polyf3->x1, &polyf3->x2);

            // Compute the OTz
            gte_avsz3();
            gte_stotz(&otz);

            // Sort the triange in the OT
            if ((otz > 0) && (otz < OT_LENGTH)) {
                addPrim(ot[currbuff][otz], polyf3);
                nextprim += sizeof(POLY_F3);
            }
        }
        *//*
        nclip = RotAverageNclip3(
            &floor.vertices[floor.faces[i]],
            &floor.vertices[floor.faces[i+1]],
            &floor.vertices[floor.faces[i+2]],
            (long*)&polyf3->x0,
            (long*)&polyf3->x1,
            (long*)&polyf3->x2,
            &p, &otz, &flg
        );
        if (nclip <= 0) {
            continue;
        }
        if ((otz > 0) && (otz < OT_LENGTH)) {
            addPrim(ot[currbuff][otz], polyf3);
            nextprim += sizeof(POLY_F3);
        }
        *//*
    }
    // Update the floor rotation
    floor.rotation.vy += 5;
    */
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
    // Draw the cube
    RotMatrix(&cube.rotation, &world);
    TransMatrix(&world, &cube.position);
    ScaleMatrix(&world, &cube.scale);
    SetRotMatrix(&world);
    SetTransMatrix(&world);
    for (i = 0; i < 6 * 4; i += 4) {
        polyg4 = (POLY_G4*) nextprim;
        setPolyG4(polyg4);
        setRGB0(polyg4, 255,   0, 255);
        setRGB1(polyg4, 255, 255,   0);
        setRGB2(polyg4,   0, 255, 255);
        setRGB3(polyg4,   0, 255,   0);
        /*
        // Load the first 3 vertices (the GTE can only perform a maimum of 3 vectors at a time)
        gte_ldv0(&cube.vertices[cube.faces[i + 0]]);
        gte_ldv1(&cube.vertices[cube.faces[i + 1]]);
        gte_ldv2(&cube.vertices[cube.faces[i + 2]]);
        // Perform RTPT with the first 't'hree vertices
        gte_rtpt();
        // We can perform the nclip check here already since all vertices of a quad are coplanar
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip >= 0) {
            // Store/save the transformed projected coord of the first vertex
            gte_stsxy0(&polyg4->x0);
            // Load the last 4th vertex
            gte_ldv0(&cube.vertices[cube.faces[i + 3]]);
            // Perform RTPS with the 's'ingle remaining 4th vertex
            gte_rtps();
            // Store the transformed last vertices
            gte_stsxy3(&polyg4->x1, &polyg4->x2, &polyg4->x3);
            // Compute the OTz
            gte_avsz4();
            gte_stotz(&otz);
            // Sort the quad in the OT
            if ((otz > 0) && (otz < OT_LENGTH)) {
                addPrim(ot[currbuff][otz], polyg4);
                nextprim += sizeof(POLY_G4);
            }
        }
        */
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
        if ((otz > 0) && (otz < OT_LENGTH)) {
            addPrim(ot[currbuff][otz], polyg4);
            nextprim += sizeof(POLY_G4);
        }
    }
    // Update cube rotation
    cube.rotation.vx += 6;
    cube.rotation.vy += 8;
    cube.rotation.vz += 12;
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
