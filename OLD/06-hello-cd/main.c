#include <stdlib.h>
#include <sys/types.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <libcd.h>
#include "globals.h"
#include "display.h"
#include "joypad.h"
#include "camera.h"
#include "utils.h"
#include "object.h"

#define VIDEO_MODE 0
#define SCREEN_RES_X 320
#define SCREEN_RES_Y 240
#define SCREEN_CENTER_X (SCREEN_RES_X >> 1)
#define SCREEN_CENTER_Y (SCREEN_RES_Y >> 1)
#define SCREEN_Z 320



///////////////////////////////////////////////////////////////////////////////
// Declarations and global variables
///////////////////////////////////////////////////////////////////////////////
POLY_F4 *polyf4;
MATRIX worldmat = {0};
MATRIX viewmat = {0};
Camera camera;
Object object;



///////////////////////////////////////////////////////////////////////////////
// Setup function that is called once at the beginning of the execution
///////////////////////////////////////////////////////////////////////////////
void Setup(void) {
    u_long i;
    u_long b;
    char* bytes;
    u_long length;
    ScreenInit();
    // Initialize the CD subsystem
    CdInit();
    // Initialize the joypad
    JoyPadInit();
    // Reset next primitive pointer
    ResetNextPrim(GetCurrBuff());    
    // Initializes the camera object
    setVector(&camera.position, 500, -1000, -1200);
    camera.lookat = (MATRIX){0};
    // Initialize the object
    setVector(&object.position, 0, 0, 0);
    setVector(&object.rotation, 0, 0, 0);
    setVector(&object.scale, ONE, ONE, ONE);
    // Read bytes from the CD-ROM
    bytes = FileRead("\\MODEL.BIN;1", &length);
    printf("Read %d bytes from MODEL.BIN\n", length);
    b = 0;
    // Read the vertices from the file
    object.numverts = GetShortBE(bytes, &b);
    object.vertices = malloc(object.numverts * sizeof(SVECTOR));
    for (i = 0; i < object.numverts; i++) {
        object.vertices[i].vx = GetShortBE(bytes, &b);
        object.vertices[i].vy = GetShortBE(bytes, &b);
        object.vertices[i].vz = GetShortBE(bytes, &b);
        printf("VERTICES[%d] X=%d, Y=%d, Z=%d\n", i, object.vertices[i].vx, object.vertices[i].vy, object.vertices[i].vz);
    }
    // Read the faces from the file
    object.numfaces = GetShortBE(bytes, &b) * 4;
    object.faces = malloc(object.numverts * sizeof(short));
    for (i = 0; i < object.numfaces; i++) {
        object.faces[i] = GetShortBE(bytes, &b);
        printf("FACES[%d] %d\n", i, object.faces[i]);
    }
    // Read the colors from the file
    object.numcolors = GetChar(bytes, &b);
    object.colors = malloc(object.numcolors * sizeof(CVECTOR));
    for (i = 0; i < object.numcolors; i++) {
        object.colors[i].r = GetChar(bytes, &b);
        object.colors[i].g = GetChar(bytes, &b);
        object.colors[i].b = GetChar(bytes, &b);
        object.colors[i].cd = GetChar(bytes, &b);
        printf("COLORS[%d] R=%d, G=%d, B=%d\n", i, object.colors[i].r, object.colors[i].g, object.colors[i].b);
    }
    free(bytes);
}



///////////////////////////////////////////////////////////////////////////////
// Update function called every frame
///////////////////////////////////////////////////////////////////////////////
void Update(void) {
    int i, q, nclip;
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
    // Compute the camera Lookat matrix for this frame
    LookAt(&camera, &camera.position, &object.position, &(VECTOR){0, -ONE, 0});
    // Draw the object
    RotMatrix(&object.rotation, &worldmat);
    TransMatrix(&worldmat, &object.position);
    ScaleMatrix(&worldmat, &object.scale);
    // Create the View Matrix combining the world matrix & lookat matrix
    CompMatrixLV(&camera.lookat, &worldmat, &viewmat);
    SetRotMatrix(&viewmat);
    SetTransMatrix(&viewmat);
    for (i = 0, q = 0; i < 6 * 4; i += 4, q++) {
        polyf4 = (POLY_F4*) GetNextPrim();
        setPolyF4(polyf4);
        setRGB0(polyf4, object.colors[q].r, object.colors[q].g, object.colors[q].b);
        nclip = RotAverageNclip4(
            &object.vertices[object.faces[i]], 
            &object.vertices[object.faces[i+1]], 
            &object.vertices[object.faces[i+2]], 
            &object.vertices[object.faces[i+3]], 
            (long*)&polyf4->x0, 
            (long*)&polyf4->x1, 
            (long*)&polyf4->x2, 
            (long*)&polyf4->x3, 
            &p, &otz, &flg
        );
        if (nclip <= 0) {
            continue;
        }
        if ((otz > 0) && (otz < OT_LEN)) {
            addPrim(GetOTAt(GetCurrBuff(), otz), polyf4);
            IncrementNextPrim(sizeof(POLY_F4));
        }
    }
    object.rotation.vy += 20;
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
