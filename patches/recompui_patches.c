#include "functions.h"
#include "patches.h"
#include "ui_funcs.h"

extern s32 D_802823AC;
extern f32 D_80282420;
extern f32 D_80282424;

// @recomp Patched to run a callback per frame in a function that always gets called by mainLoop().
RECOMP_PATCH void baMotor_80250C08(void) {
    // @recomp Patched to run UI callbacks.
    recomp_run_ui_callbacks();

    if (D_802823AC != 0) {
        D_80282424 = MIN(D_80282420, D_80282424 + time_getDelta());
    }
}