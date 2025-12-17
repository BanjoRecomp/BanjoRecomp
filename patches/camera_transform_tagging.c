#include "patches.h"
#include "transform_ids.h"
#include "core1/core1.h"

s32 cur_perspective_projection_transform_id = 0;
s32 backup_perspective_projection_transform_id = 0;

s32 cur_ortho_projection_transform_id = 0;
s32 backup_ortho_projection_transform_id = 0;

void reset_projection_ids() {
    cur_perspective_projection_transform_id = 0;
    cur_ortho_projection_transform_id = 0;
}

bool perspective_interpolation_skipped = FALSE;

s32 getGameMode(void);

extern f32 sViewportFOVy;
extern f32 sViewportAspect;
extern f32 sViewportNear;
extern f32 sViewportFar;
extern f32 sViewportLookVector[3];
extern f32 sViewportPosition[3];
extern f32 sViewportRotation[3];
extern Vp sViewportStack[];
extern MtxF sViewportMatrix;
extern s32 sViewportStackIndex;

extern f32 sViewportBackupPosition[3];
extern f32 sViewportBackupRotation[3];
extern f32 sViewportBackupFrustumPlanes[4][4];
extern f32 sViewportBackupLookVector[3];
extern MtxF sViewportBackupMatrix;

// @recomp Patched to specify the view matrix for better transform interpolation and set up a matrix group for the new projection.
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

    // @recomp If a perspective projection transform ID is set, apply it as the projection matrix group. Otherwise, use auto as the projection matrix group.
    if (all_interpolation_skipped()) {
        gEXMatrixGroupNoInterpolate((*gfx)++, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
    else if (cur_perspective_projection_transform_id != 0) {
        // Force the projection to not adjust itself for a wider aspect ratio when it's being rendered for the Bottles' bonus puzzle or the Mumbo photo.
        u16 aspect = G_EX_ASPECT_AUTO;
        bool inPictureGameMode = (getGameMode() == GAME_MODE_8_BOTTLES_BONUS) || (getGameMode() == GAME_MODE_A_SNS_PICTURE);
        bool isGameplayTransformId = (cur_perspective_projection_transform_id == PROJECTION_GAMEPLAY_TRANSFORM_ID);
        bool isTransitionTransformId = (cur_perspective_projection_transform_id == PROJECTION_TRANSITION_TRANSFORM_ID);
        if (inPictureGameMode && (isGameplayTransformId || isTransitionTransformId)) {
            aspect = G_EX_ASPECT_STRETCH;
        }

        gEXMatrixGroup((*gfx)++, cur_perspective_projection_transform_id, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, aspect, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
    }
    else {
        gEXMatrixGroupSimpleNormal((*gfx)++, G_EX_ID_AUTO, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
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

    // @recomp If an ortho projection transform ID is set, apply it as the projection matrix group. Otherwise, use auto as the projection matrix group.
    if (all_interpolation_skipped()) {
        gEXMatrixGroupNoInterpolate((*gfx)++, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
    else if (cur_ortho_projection_transform_id != 0) {
        gEXMatrixGroupSimpleNormal((*gfx)++, cur_ortho_projection_transform_id, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
    else {
        gEXMatrixGroupSimpleNormal((*gfx)++, G_EX_ID_AUTO, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
}

// @recomp Patched to also back up the projection matrix transform ID and reset the current one.
RECOMP_PATCH void viewport_backupState(void) {
    s32 i, j;

    viewport_getPosition_vec3f(sViewportBackupPosition);
    viewport_getRotation_vec3f(sViewportBackupRotation);
    viewport_getFrustumPlanes(sViewportBackupFrustumPlanes[0], sViewportBackupFrustumPlanes[1], sViewportBackupFrustumPlanes[2], sViewportBackupFrustumPlanes[3]);
    viewport_getLookVector(sViewportBackupLookVector);

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            sViewportBackupMatrix.m[i][j] = sViewportMatrix.m[i][j];
        }
    }

    // @recomp Back up the projection transform IDs and reset them.
    backup_perspective_projection_transform_id = cur_perspective_projection_transform_id;
    cur_perspective_projection_transform_id = 0;
    backup_ortho_projection_transform_id = cur_ortho_projection_transform_id;
    cur_ortho_projection_transform_id = 0;
}

// @recomp Patched to also restore the backed up projection matrix transform ID.
RECOMP_PATCH void viewport_restoreState(void) {
    s32 i, j;

    viewport_setPosition_vec3f(sViewportBackupPosition);
    viewport_setRotation_vec3f(sViewportBackupRotation);
    viewport_setFrustumPlanes(sViewportBackupFrustumPlanes[0], sViewportBackupFrustumPlanes[1], sViewportBackupFrustumPlanes[2], sViewportBackupFrustumPlanes[3]);
    ml_vec3f_copy(sViewportLookVector, sViewportBackupLookVector);

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            sViewportMatrix.m[i][j] = sViewportBackupMatrix.m[i][j];
        }
    }

    // @recomp Restore the backed up projection transform IDs.
    cur_perspective_projection_transform_id = backup_perspective_projection_transform_id;
    cur_ortho_projection_transform_id = backup_ortho_projection_transform_id;
}
