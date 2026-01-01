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
u_short shipindex = 0;
u_short angle = 0;

int holdingright = 0;
int holdingleft = 0;


///////////////////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////////////////
void Setup(void) {
    u_short shipstarttexture;
    //u_short rescuestarttexture;
    ScreenInit();
    CdInit();
    JoyPadInit();
    ResetNextPrim(GetCurrBuff());    
    setVector(&camera.position, 500, -1000, -1200);
    camera.lookat = (MATRIX){0};
    shipstarttexture = GetTextureCount();
    LoadTextureCMP("\\ALLSH.CMP;1");
    ships = LoadObjectPRM("\\ALLSH.PRM;1", shipstarttexture);
}


///////////////////////////////////////////////////////////////////////////////
// Update function
///////////////////////////////////////////////////////////////////////////////
void Update(void) {
    u_short i;
    EmptyOT(GetCurrBuff());
    JoyPadUpdate();
    if (JoyPadCheck(PAD1_LEFT)) {
        if (shipindex > 0 && !holdingleft) {
            shipindex--;
        }
        holdingleft = 1;
    } else {
        holdingleft = 0;
    }
    if (JoyPadCheck(PAD1_RIGHT)) {
        if (shipindex < 7 && !holdingright) {
            shipindex++;
        }
        holdingright = 1;
    } else {
        holdingright = 0;
    }
    ship = ships;
    for (i = 0; i < shipindex; i++) {
        ship = ship->next;
    }
    angle += 10;
    ship->rotation.vy = angle;
    LookAt(&camera, &camera.position, &ship->position, &(VECTOR){0, -ONE, 0});
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
