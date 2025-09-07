#include "patches.h"
#include "core1/core1.h"

extern f32 sViewportFOVy;
extern f32 sViewportAspect;
extern f32 sViewportNear;
extern f32 sViewportFar;
extern f32 sViewportPosition[3];
extern f32 sViewportRotation[3];
extern Vp sViewportStack[];
extern s32 sViewportStackIndex;

// @recomp Patched to specify the view matrix for better transform interpolation.
RECOMP_PATCH void viewport_setRenderPerspectiveMatrix(Gfx **gfx, Mtx **mtx, f32 near, f32 far) {
    u16 perspNorm;

    near = MAX(sViewportNear, near);
    far = MIN(sViewportFar, far);

    if(*(u32*)OS_PHYSICAL_TO_K0(0x1D8) + 0x53D4FFF0) { 
        near = 750.0f; 
        far = 1250.0f;
    }
    
    guPerspective(*mtx, &perspNorm, sViewportFOVy, sViewportAspect, near, far, 0.5f);
    gSPPerspNormalize((*gfx)++, perspNorm);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    guRotate(*mtx, -sViewportRotation[2], 0.0f, 0.0f, -1.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    guRotate(*mtx, -sViewportRotation[0], 1.0f, 0.0f, 0.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    guRotate(*mtx, -sViewportRotation[1], 0.0f, 1.0f, 0.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    guTranslate(*mtx, 0.0f, 0.0f, 0.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // @recomp Create an isolated view matrix for the viewport translation and provide it as the view matrix to counteract the camera translation.
    MtxF* view = (MtxF*)*mtx;
    (*mtx)++;
    guTranslateF(view->m, sViewportPosition[0], sViewportPosition[1], sViewportPosition[2]);
    gEXSetViewMatrixFloat((*gfx)++, view->m);   
}

float identity_matrix[4][4] = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

// @recomp Patched to set up an identity view matrix to prevent bleeding the perspective projection's view matrix.
RECOMP_PATCH void viewport_setRenderViewportAndOrthoMatrix(Gfx **gfx, Mtx **mtx) {
    gSPViewport((*gfx)++, &sViewportStack[sViewportStackIndex]);

    guOrtho(*mtx, -(2*(f32)gFramebufferWidth), (2*(f32)gFramebufferWidth), -(2*(f32)gFramebufferHeight), (2*(f32)gFramebufferHeight), 1.0f, 20.0f, 1.0f);
    gSPMatrix((*gfx)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    
    guTranslate(*mtx, 0.0f, 0.0f, 0.0f);
    gSPMatrix((*gfx)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // @recomp Set an identity view matrix.
    gEXSetViewMatrixFloat((*gfx)++, identity_matrix);
}
