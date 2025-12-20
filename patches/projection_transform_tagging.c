#include "patches.h"
#include "transform_ids.h"
#include "core1/core1.h"
#include "functions.h"
#include "../src/core2/gc/zoombox.h"

extern u32 D_803835E0;
void func_802BBD2C(f32 *arg0, f32 *arg1);
s32 func_80320708(void);
void spawnQueue_unlock(void);
void sky_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
bool mapModel_has_xlu_bin(void);
void mapModel_opa_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
int game_is_frozen(void);
void func_80322E64(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void player_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void func_80302C94(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void jiggylist_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void func_803500D8(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void func_802F2ED0(Struct64s *arg0, Gfx **gfx, Mtx **mtx, Vtx **vtx);
Struct64s* func_8032994C(void);
void partEmitMgr_drawPass0(Gfx **gdl, Mtx **mptr, Vtx **vptr);
void mapModel_xlu_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void func_8032D3D8(Gfx **gdl, Mtx **mptr, Vtx **vptr);
void partEmitMgr_drawPass1(Gfx **gdl, Mtx **mptr, Vtx **vptr);
void func_8034F6F0(Gfx **gdl, Mtx **mptr, Vtx **vptr);
void func_802D520C(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void partEmitMgr_draw(Gfx **gdl, Mtx **mptr, Vtx **vptr);
void func_80350818(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void func_802BBD0C(Gfx **gdl, Mtx **mptr, Vtx **vptr);
void spawnQueue_lock(void);
s32 getGameMode(void);
void sfx_rand_sync_to_rand(void);
void func_803162B4(GcZoombox *this);
void rand_sync_to_sfx_rand(void);
void func_80338338(s32, s32, s32);
void func_803382FC(s32);
void func_803382E4(s32);
void func_8033687C(Gfx **);
void func_80335D30(Gfx **);
void func_80344090(BKSpriteDisplayData *self, s32 frame, Gfx **gfx);

// @recomp Patched to set the projection transform ID for the main projection.
RECOMP_PATCH void func_80334540(Gfx** gdl, Mtx **mptr, Vtx **vptr) {
    f32 sp44;
    f32 sp40;

    if (D_803835E0 == 0) {
        drawRectangle2D(gdl, 0, 0, gFramebufferWidth, gFramebufferHeight, 0, 0, 0);
        func_802BBD2C(&sp44, &sp40);
        viewport_setNearAndFar(sp44, sp40);

        // @recomp Set the perpsective projection transform ID.
        cur_perspective_projection_transform_id = PROJECTION_GAMEPLAY_TRANSFORM_ID;
        
        viewport_setRenderViewportAndPerspectiveMatrix(gdl, mptr);
        return;
    }
    if (func_80320708() == 0) {
        eeprom_writeBlocks(0, 0, (void*)0x80BC7230, EEPROM_MAXBLOCKS);
    }
    spawnQueue_unlock();
    sky_draw(gdl, mptr, vptr);
    func_802BBD2C(&sp44, &sp40);
    viewport_setNearAndFar(sp44, sp40);
    
    // @recomp Set the perpsective projection transform ID.
    cur_perspective_projection_transform_id = PROJECTION_GAMEPLAY_TRANSFORM_ID;

    viewport_setRenderViewportAndPerspectiveMatrix(gdl, mptr);
    if (mapModel_has_xlu_bin() != 0) {
        mapModel_opa_draw(gdl, mptr, vptr);
        if (game_is_frozen() == 0) {
            func_80322E64(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            player_draw(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            func_80302C94(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            jiggylist_draw(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            func_803500D8(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            func_802F2ED0(func_8032994C(), gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            partEmitMgr_drawPass0(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            mapModel_xlu_draw(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            func_8032D3D8(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            partEmitMgr_drawPass1(gdl, mptr, vptr);
        }
        if (game_is_frozen() == 0) {
            func_8034F6F0(gdl, mptr, vptr);
        }
        func_802D520C(gdl, mptr, vptr);
    } else {
        mapModel_opa_draw(gdl, mptr, vptr);
        func_80322E64(gdl, mptr, vptr);
        func_8034F6F0(gdl, mptr, vptr);
        player_draw(gdl, mptr, vptr);
        func_80302C94(gdl, mptr, vptr);
        func_8032D3D8(gdl, mptr, vptr);
        jiggylist_draw(gdl, mptr, vptr);
        func_803500D8(gdl, mptr, vptr);
        func_802F2ED0(func_8032994C(), gdl, mptr, vptr);
        func_802D520C(gdl, mptr, vptr);
        partEmitMgr_draw(gdl, mptr, vptr);
    }
    if (game_is_frozen() == 0) {
        func_80350818(gdl, mptr, vptr);
    }
    if (game_is_frozen() == 0) {
        func_802BBD0C(gdl, mptr, vptr);
    }
    spawnQueue_lock();
    
    // @recomp Clear the perpsective projection transform ID.
    cur_perspective_projection_transform_id = 0;
}

void actor_predrawMethod(Actor *);
void actor_postdrawMethod(ActorMarker *);
extern bool D_8037DE84;

// @recomp Patched to set the transform ID for the press start projection.
RECOMP_PATCH Actor *chOverlayPressStart_draw(ActorMarker *marker, Gfx **gdl, Mtx **mptr, Vtx **vptr){
    Actor * actor;
    f32 sp58[3];
    f32 sp4C[3];
    f32 sp40[3];
    f32 sp34[3];
    

    actor = marker_getActor(marker);
    if(D_8037DE84)
        return actor;

    modelRender_preDraw((GenFunction_1)actor_predrawMethod, (s32)actor);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);
    viewport_backupState();
    {sp58[0] = 0.0f; sp58[1] = 0.0f; sp58[2] = 1312.5f;};
    {sp4C[0] = 0.0f; sp4C[1] = 0.0f; sp4C[2] = 0.0f;};
    viewport_setPosition_vec3f(sp58);
    viewport_setRotation_vec3f(sp4C);
    viewport_update();

    // @recomp Record the current projection transform, then set the projection transform ID for the press start projection.
    s32 prev_perspective_projection_transform = cur_perspective_projection_transform_id;
    cur_perspective_projection_transform_id = PROJECTION_PRESS_START_TRANSFORM_ID;

    viewport_setRenderViewportAndPerspectiveMatrix(gdl, mptr);
    {sp40[0] = 0.0f; sp40[1] = 0.0f; sp40[2] = 0.0f;};
    {sp34[0] = 0.0f; sp34[1] = 400.0f; sp34[2] = 0.0f;};
    modelRender_draw(gdl, mptr, sp40, 0, 1.0f, sp34, marker_loadModelBin(marker));
    
    // @recomp Restore the previous perpsective projection transform ID.
    cur_perspective_projection_transform_id = prev_perspective_projection_transform;

    viewport_restoreState();
    viewport_setRenderViewportAndPerspectiveMatrix(gdl, mptr);

    return actor;
}

#define ASSET_54D_MODEL_BK_TITLE_LOGO 0x54D
#define ASSET_56C_MODEL_THE_END 0x56C

// @recomp Patched to set the transform ID for the title logo and copyright text projections.
RECOMP_PATCH Actor *func_802DC7E0(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx){
    Actor *this;
    f32 sp58[3];
    f32 sp4C[3];
    f32 sp40[3];
    f32 sp34[3];

    this = marker_getActor(marker);
    modelRender_preDraw( (GenFunction_1)actor_predrawMethod, (s32)this);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);
    viewport_backupState();
    sp58[0] = 0.0f;
    sp58[1] = 0.0f;
    sp58[2] = 860.0f;
    sp4C[0] = 0.0f;
    sp4C[1] = 0.0f;
    sp4C[2] = 0.0f;
    viewport_setPosition_vec3f(sp58);
    viewport_setRotation_vec3f(sp4C);
    viewport_update();

    // @recomp Record the current projection transform, then set the projection transform ID for the projection determined by the model ID.
    s32 prev_perspective_projection_transform = cur_perspective_projection_transform_id;
    if (marker->modelId == ASSET_54D_MODEL_BK_TITLE_LOGO) {
        cur_perspective_projection_transform_id = PROJECTION_BK_LOGO_TRANSFORM_ID;
    }
    else if (marker->modelId == ASSET_54E_MODEL_COPYRIGHT_OVERLAY) {
        cur_perspective_projection_transform_id = PROJECTION_COPYRIGHT_TRANSFORM_ID;
    }

    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
    sp40[0] = 0.0f;
    sp40[1] = 0.0f;
    sp40[2] = 0.0f;
    sp34[0] = 0.0f;
    sp34[1] = -87.0f;
    sp34[2] = 0.0f;
    modelRender_draw(gfx, mtx, sp40, NULL, 1.0f, sp34, marker_loadModelBin(marker));
    
    // @recomp Restore the previous perpsective projection transform ID.
    cur_perspective_projection_transform_id = prev_perspective_projection_transform;
    viewport_restoreState();
    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
    return this;
}

// @recomp Patched to set the transform ID for the game over and THE END projections.
RECOMP_PATCH Actor *func_802DC320(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx){
    Actor *this;
    f32 vp_position[3];
    f32 vp_rotation[3];
    f32 model_position[3];
    f32 sp34[3];

    this = marker_getActor(marker);
    modelRender_preDraw( (GenFunction_1)actor_predrawMethod, (s32)this);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);
    viewport_backupState();
    vp_position[0] = 0.0f;
    vp_position[1] = 0.0f;
    vp_position[2] = 937.5f;
    vp_rotation[0] = 0.0f;
    vp_rotation[1] = 0.0f;
    vp_rotation[2] = 0.0f;
    viewport_setPosition_vec3f(vp_position);
    viewport_setRotation_vec3f(vp_rotation);
    viewport_update();

    // @recomp Record the current projection transform, then set the projection transform ID for the projection determined by the model ID.
    s32 prev_perspective_projection_transform = cur_perspective_projection_transform_id;
    if (marker->modelId == ASSET_54C_MODEL_GAME_OVER) {
        cur_perspective_projection_transform_id = PROJECTION_GAME_OVER_TRANSFORM_ID;
    }
    else if (marker->modelId == ASSET_56C_MODEL_THE_END) {
        cur_perspective_projection_transform_id = PROJECTION_THE_END_TRANSFORM_ID;
    }

    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
    model_position[0] = 0.0f;
    model_position[1] = 0.0f;
    model_position[2] = 0.0f;
    sp34[0] = 0.0f;
    sp34[1] = 137.5f;
    sp34[2] = 0.0f;
    modelRender_draw(gfx, mtx, model_position, NULL, 1.0f, sp34, marker_loadModelBin(marker));
    
    // @recomp Restore the previous perpsective projection transform ID.
    cur_perspective_projection_transform_id = prev_perspective_projection_transform;
    viewport_restoreState();
    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
    return this;
}

// @recomp Patched to set the zoombox's model transform ID.
RECOMP_PATCH void func_803163A8(GcZoombox *this, Gfx **gfx, Mtx **mtx) {
    f32 sp5C[3];
    f32 sp50[3];
    f32 sp44[3];
    f32 sp38[3];
    f32 sp34;

    sp34 = viewport_transformCoordinate(this->unk170, this->unk172, sp50, sp5C);
    if (this->unk1A4_24) {
        sp5C[1] += 180.0f;
        sp5C[0] -= 2*sp5C[0];
    }
    sp38[0] = 0.0f; sp38[1] = 0.0f; sp38[2] = 0.0f;
    sp44[0] = 0.0f; sp44[1] = 0.0f; sp44[2] = 0.0f;
    func_8033A308(sp44);
    modelRender_setDepthMode(MODEL_RENDER_DEPTH_NONE);
    if (this->anim_ctrl != NULL) {
        anctrl_drawSetup(this->anim_ctrl, sp50, 1);
    }

    // @recomp Set the model transform ID. Skip interpolation when the camera does a cut.
    u32 prev_transform_id = cur_drawn_model_transform_id;
    s32 prev_skip = cur_drawn_model_transform_id_skip_interpolation;
    cur_drawn_model_transform_id = ZOOMBOX_TRANSFORM_ID_START + this->portrait_id;
    cur_drawn_model_transform_id_skip_interpolation = perspective_interpolation_skipped();

    modelRender_draw(gfx, mtx, sp50, sp5C, this->unk198 * sp34, sp38, this->model);

    // @recomp Reset the model transform ID.
    cur_drawn_model_transform_id = prev_transform_id;
    cur_drawn_model_transform_id_skip_interpolation = prev_skip;
}

// @recomp Patched to set the zoombox portrait's model and ortho projection transform IDs.
RECOMP_PATCH void func_803164B0(GcZoombox *this, Gfx **gfx, Mtx **mtx, s32 arg3, s32 arg4, BKSpriteDisplayData *arg5, f32 arg6) {
    f32 sp2C[3];
    f32 temp_f12;

    if (this->portrait_id == ZOOMBOX_SPRITE_46_TUMBLAR) {
        arg6 = 0.75f;
    }
    func_80338338(0xFF, 0xFF, 0xFF);
    func_803382FC(this->unk168 * arg6);
    func_803382E4(5);
    func_80335D30(gfx);

    // @recomp Set the ortho projection transform ID for the portrait.
    cur_ortho_projection_transform_id = PROJECTION_PORTRAIT_TRANSFORM_ID_START + this->portrait_id;

    viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);

    // @recomp Clear the current ortho projection transform ID.
    cur_ortho_projection_transform_id = 0;

    mlMtxIdent();
    if (this->unk1A4_24) {
        mlMtxRotYaw(180.0f);
        sp2C[0] = (f32) this->unk170 - ((f32) arg3 * this->unk198);
    } else {
        sp2C[0] = (f32) this->unk170 + ((f32) arg3 * this->unk198);
    }
    sp2C[1] = this->unk172 + ((f32) arg4 * this->unk198);
    sp2C[2] = -10.0f;
    func_80252330((sp2C[0] * 4.0f) - ((f32)gFramebufferWidth * 2), ((f32)gFramebufferHeight * 2) - (sp2C[1] * 4.0f), sp2C[2]);
    temp_f12 = (f32) ((f64) this->unk198 * 0.8);
    mlMtxScale_xyz(temp_f12, temp_f12, 1.0f);
    mlMtxApply(*mtx);
    gSPMatrix((*gfx)++, (*mtx)++, G_MTX_LOAD | G_MTX_MODELVIEW);

    // @recomp Create a matrix group for the portrait's model matrix.
    gEXMatrixGroupDecomposedVerts((*gfx)++, ZOOMBOX_PORTRAIT_TRANSFORM_ID_START + this->portrait_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

    modelRender_setDepthMode(MODEL_RENDER_DEPTH_NONE);
    func_80344090(arg5, this->unk186, gfx);
    func_8033687C(gfx);
    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);

    // @recomp Pop the model matrix group.
    gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
}
