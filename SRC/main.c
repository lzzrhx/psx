#include "globals.h"

#include "inline_n.h"
#include "display.h"
#include "joypad.h"
#include "camera.h"
#include "object.h"
#include "utils.h"


Camera camera;
Object* ships;
Object* ship;
Object* sceneobjs;
u_short shipindex = 0;


///////////////////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////////////////
void Setup(void) {
    u_short shipsstarttexture, scenestarttexture;
    ScreenInit();
    CdInit();
    JoyPadInit();
    ResetNextPrim(GetCurrBuff());
    shipsstarttexture = GetTextureCount();
    LoadTextureCMP("\\ALLSH.CMP;1");
    scenestarttexture = GetTextureCount();
    LoadTextureCMP("\\TRACK02\\SCENE.CMP;1");
    ships = LoadObjectPRM("\\ALLSH.PRM;1", shipsstarttexture);
    sceneobjs = LoadObjectPRM("\\TRACK02\\SCENE.PRM;1", scenestarttexture);
    ship = GetObjectByIndex(ships, shipindex);
    setVector(&ship->position, 32599, -347, -45310);
    setVector(&camera.position, ship->position.vx, ship->position.vy - 100, ship->position.vz - 1000);
    camera.lookat = (MATRIX){0};
}


///////////////////////////////////////////////////////////////////////////////
// Update function
///////////////////////////////////////////////////////////////////////////////
void Update(void) {
    u_short i;
    EmptyOT(GetCurrBuff());
    JoyPadUpdate();
    if (JoyPadCheck(PAD1_LEFT)) {
        camera.position.vx -= 100;
    }
    if (JoyPadCheck(PAD1_RIGHT)) {
        camera.position.vx += 100;
    }
    if (JoyPadCheck(PAD1_UP)) {
        camera.position.vy -= 100;
    }
    if (JoyPadCheck(PAD1_DOWN)) {
        camera.position.vy += 100;
    }
    LookAt(&camera, &camera.position, &ship->position, &(VECTOR){0, -ONE, 0});
    RenderSceneObjects(sceneobjs, &camera);
    RenderObject(ship, &camera);
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
