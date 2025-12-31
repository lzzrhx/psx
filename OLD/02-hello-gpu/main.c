#include <stdlib.h>
#include <sys/types.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>

#define VIDEO_MODE 0
#define SCREEN_RES_X 320
#define SCREEN_RES_Y 240
#define SCREEN_CENTER_X (SCREEN_RES_X >> 1)
#define SCREEN_CENTER_Y (SCREEN_RES_Y >> 1)
#define SCREEN_Z 400
#define OT_LENGTH 16



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

TILE *tile;
POLY_F3 *polyf3;
POLY_G4 *quadg4;



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
    setRGB0(&screen.draw[0], 63, 0, 127);
    setRGB0(&screen.draw[1], 63, 0, 127);
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
// Update function
///////////////////////////////////////////////////////////////////////////////
void Update(void) {
    ClearOTagR(ot[currbuff], OT_LENGTH); // Clear ordering table
    // Add tile
    tile = (TILE*) nextprim; // Cast next primitive
    setTile(tile); // Initialize the primitve
    setXY0(tile, 50, 32); // Set primitive (x,y) position
    setWH(tile, 64, 64); // Set primitive size
    setRGB0(tile, 0, 255, 0); // Set primitve color
    addPrim(ot[currbuff], tile); // Add & sort the primitve to the OT
    nextprim += sizeof(TILE); // Advance the next primitive pointer
    // Add triangle
    polyf3 = (POLY_F3*) nextprim;
    setPolyF3(polyf3);
    setXY3(polyf3, 64, 100, 200, 150, 50, 220);
    setRGB0(polyf3, 255, 0, 255);
    addPrim(ot[currbuff], polyf3);
    nextprim += sizeof(POLY_F3);
    // Add quad
    quadg4 = (POLY_G4*) nextprim;
    setPolyG4(quadg4);
    setXY4(quadg4, 140, 50, 200, 40, 170, 120, 220, 80);
    setRGB0(quadg4, 255, 0, 0);
    setRGB1(quadg4, 0, 255, 0);
    setRGB2(quadg4, 0, 0, 255);
    setRGB3(quadg4, 255, 255, 0);
    addPrim(ot[currbuff], quadg4);
    nextprim += sizeof(POLY_G4);
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
