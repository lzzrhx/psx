#include "display.h"

static DoubleBuff screen;
static u_short currbuff;



u_short GetCurrBuff(void) {
  return currbuff;
}



///////////////////////////////////////////////////////////////////////////////
// Initialize display mode and setup double buffering
///////////////////////////////////////////////////////////////////////////////
void ScreenInit(void) {
    // Reset GPU
    ResetGraph(0);
    // Set the display area of the first buffer
    SetDefDispEnv(&screen.disp[0], 0,   0, SCREEN_RES_X, SCREEN_RES_Y);
    SetDefDrawEnv(&screen.draw[0], 0, 240, SCREEN_RES_X, SCREEN_RES_Y);
    // Set the display area of the second buffer
    SetDefDispEnv(&screen.disp[1], 0, 240, SCREEN_RES_X, SCREEN_RES_Y);
    SetDefDrawEnv(&screen.draw[1], 0,   0, SCREEN_RES_X, SCREEN_RES_Y);
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
    DrawOTag(GetOTAt(currbuff, OT_LEN - 1));
    // Swap current buffer
    currbuff = !currbuff;
    // Reset next primitive pointer
    ResetNextPrim(currbuff);
}