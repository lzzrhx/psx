#include "globals.h"

#include "inline_n.h"
#include "display.h"
#include "joypad.h"
#include "camera.h"
#include "object.h"
#include "utils.h"


Camera camera;
Object object;


///////////////////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////////////////
void Setup(void) {
    ScreenInit();
    CdInit();
    JoyPadInit();
    ResetNextPrim(GetCurrBuff());    
    setVector(&camera.position, 500, -1000, -1200);
    camera.lookat = (MATRIX){0};
    LoadObjectPRM(&object, "\\ALLSH.PRM;1");
}

///////////////////////////////////////////////////////////////////////////////
// Update function
///////////////////////////////////////////////////////////////////////////////
void Update(void) {
    EmptyOT(GetCurrBuff());
    JoyPadUpdate();
    if (JoyPadCheck(PAD1_LEFT)) {
        object.rotation.vy -= 15;
    }
    if (JoyPadCheck(PAD1_RIGHT)) {
        object.rotation.vy += 15;
    }
    LookAt(&camera, &camera.position, &object.position, &(VECTOR){0, -ONE, 0});
    RenderObject(&object, &camera);
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
