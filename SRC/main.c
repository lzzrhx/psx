#include "globals.h"

#include "inline_n.h"
#include "display.h"
#include "joypad.h"
#include "camera.h"
#include "object.h"
#include "utils.h"
#include "track.h"


Camera camera;
Object* ships;
Object* ship;
Track track;
//Object* sceneobjs;


///////////////////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////////////////
void Setup(void) {
    u_short shipsstarttexture, scenestarttexture, trackstarttexture;
    ScreenInit();
    CdInit();
    JoyPadInit();
    ResetNextPrim(GetCurrBuff());
    shipsstarttexture = GetTextureCount();
    LoadTextureCMP("\\ALLSH.CMP;1", NULL);
    scenestarttexture = GetTextureCount();
    LoadTextureCMP("\\TRACK02\\SCENE.CMP;1", NULL);
    ships = LoadObjectPRM("\\ALLSH.PRM;1", shipsstarttexture);
    //sceneobjs = LoadObjectPRM("\\TRACK02\\SCENE.PRM;1", scenestarttexture);
    trackstarttexture = GetTextureCount();
    LoadTextureCMP("\\TRACK02\\LIBRARY.CMP;1", "\\TRACK02\\LIBRARY.TTF;1");
    LoadTrackVertices(&track, "\\TRACK02\\TRACK.TRV;1");
    LoadTrackFaces(&track, "\\TRACK02\\TRACK.TRF;1", trackstarttexture);
    LoadTrackSections(&track, "\\TRACK02\\TRACK.TRS;1");
    ship = GetObjectByIndex(ships, 1);
    setVector(&ship->position, 32599, -347, -45310);
    setVector(&camera.position, ship->position.vx, ship->position.vy - 350, ship->position.vz - 1200);
    camera.rotmat = (MATRIX){0};
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
        camera.position.vx -= 15;
    }
    if (JoyPadCheck(PAD1_RIGHT)) {
        camera.position.vx += 15;
    }
    if (JoyPadCheck(PAD1_UP)) {
        camera.position.vz += 100;
        ship->position.vz += 100;
    }
    if (JoyPadCheck(PAD1_DOWN)) {
        camera.position.vz -= 100;
        ship->position.vz -= 100;
    }
    LookAt(&camera, &camera.position, &ship->position, &(VECTOR){0, -ONE, 0});
    //RenderSceneObjects(sceneobjs, &camera);
    RenderTrack(&track, &camera);
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
