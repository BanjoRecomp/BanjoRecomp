#include "patches.h"
#include "transform_ids.h"
#include "functions.h"

#define TILE_SIZE 32
#define TILE_COUNT_X 5
#define TILE_COUNT_Y 4
#define IMAGE_WIDTH (TILE_SIZE * TILE_COUNT_X)
#define IMAGE_HEIGHT (TILE_SIZE * TILE_COUNT_Y)
#define FROM_XZ 0
#define FROM_YZ 1

extern f32 D_80368250;
extern f32 D_80368360[3];
extern f32 D_8037DFF0[3];
extern BKModelBin *D_8037DEA8;
extern BKModelBin *D_8037DFE8;
extern BKModelBin *chBottleBonusBookselfModelBin;
extern ActorMarker *D_8037E000;

extern s32 getGameMode(void);
extern s16 *func_8030C704(void);
extern void chBottlesBonus_func_802DD080(Gfx **gfx, Mtx **mtx);
extern Actor *func_802DF160(Gfx **gfx, Mtx **mtx, Vtx **vtx);
extern void actor_postdrawMethod(ActorMarker *);
extern void chBottlesBonusCursor_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
extern void chBottlesBonus_func_802DD158(Gfx **gfx, Mtx **mtx);
extern BKGfxList *model_getDisplayList(BKModelBin *arg0);

void patch_picture_model(BKModelBin *model_bin, s32 min_xy, s32 max_xy, s32 min_z, s32 max_z, u32 from) {
    // Use pad0 to indicate if the model's been patched already.
    if (model_bin->pad0[0] == 0xBA) {
        return;
    }

    model_bin->pad0[0] = 0xBA;

    BKGfxList *gfx_list = model_getDisplayList(model_bin);
    Vtx *vtx_list = model_getVtxList(model_bin)->vtx_18;
    Gfx *cur_gfx = &gfx_list->list[0];
    Gfx *end_gfx = (Gfx *)((char *)model_bin + model_bin->vtx_list_offset_10);
    bool in_framebuffer_tile = FALSE;
    while (cur_gfx < end_gfx) {
        if (cur_gfx->words.w0 >> 24 == G_SETTIMG) {
            in_framebuffer_tile = cur_gfx->words.w1 == 0x04000000;
            if (in_framebuffer_tile) {
                // Patch the settimg and the 6 commands after it, replacing with a load of the framebuffer texture.
                Gfx* g = cur_gfx;
                Gfx* ng = cur_gfx + 1;
                u8 fmt = (ng->words.w0 >> 21) & 0x7;
                gDPLoadTextureTile(g++, 0x04000000, fmt, G_IM_SIZ_16b, 160, 128, 0, 0, 160 - 1, 128 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            }
        }
        else if (cur_gfx->words.w0 >> 24 == G_VTX) {
            // Patch vertices. Get the loaded vertex count for this command.
            u32 cur_vtx_count = (cur_gfx->words.w0 >> 10) & 0b111111;
            
            // Get the vertex address, converting from a segmented address if necessary.
            Vtx *cur_vtx;
            if (cur_gfx->words.w1 >> 24 == 0x01) {
                cur_vtx = (Vtx *)(SEGMENT_OFFSET(cur_gfx->words.w1) + (char *)vtx_list);
            }
            else {
                // Not a segmented address (should never happen, but may help with compatibility for future mods).
                cur_vtx = (Vtx *)cur_gfx->words.w1;
            }

            if (in_framebuffer_tile) {
                if (from == FROM_YZ) {
                    for (u32 i = 0; i < cur_vtx_count; i++) {
                        float xy_frac = (cur_vtx[i].v.ob[1] - (float)min_xy) / (max_xy - min_xy);
                        float z_frac = (cur_vtx[i].v.ob[2] - (float)min_z) / (max_z - min_z);
                        cur_vtx[i].v.tc[0] = z_frac * 160.0f * 64.0f;
                        cur_vtx[i].v.tc[1] = (1.0f - xy_frac) * 128.0f * 64.0f;
                    }
                }
                else if (from == FROM_XZ) {
                    for (u32 i = 0; i < cur_vtx_count; i++) {
                        float xy_frac = (cur_vtx[i].v.ob[0] - (float)min_xy) / (max_xy - min_xy);
                        float z_frac = (cur_vtx[i].v.ob[2] - (float)min_z) / (max_z - min_z);
                        cur_vtx[i].v.tc[0] = xy_frac * 160.0f * 64.0f;
                        cur_vtx[i].v.tc[1] = (1.0f - z_frac) * 128.0f * 64.0f;
                    }
                }
            }
            
            for (u32 i = 0; i < cur_vtx_count; i++) {
                if (cur_vtx[i].v.ob[0] == 568) {
                    cur_vtx[i].v.ob[0] = 567;
                }
            }
        }

        cur_gfx++;
    }
}

// @recomp Patches the model for the puzzle to fix the seams.
RECOMP_PATCH Actor *chBottlesBonus_draw(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    Actor *sp6C;
    f32 sp60[3];
    f32 sp54[3];
    void *sp50;

    sp6C = marker_getActor(marker);
    sp50 = func_8030C704(); //grabs frame as texture?
    if ((sp50 == NULL) || (getGameMode() != GAME_MODE_8_BOTTLES_BONUS))
        return sp6C;

    chBottlesBonus_func_802DD080(gfx, mtx);
    { sp60[0] = 0.0f; sp60[1] = 0.0f; sp60[2] = 0.0f; };
    { sp54[0] = 0.0f; sp54[1] = 0.0f; sp54[2] = 0.0f; };
    modelRender_setDepthMode(MODEL_RENDER_DEPTH_FULL);
    modelRender_draw(gfx, mtx, sp60, NULL, 1.0f, sp54, chBottleBonusBookselfModelBin);
    modelRender_draw(gfx, mtx, sp60, NULL, 1.0f, sp54, D_8037DEA8);

    gDPSetTextureFilter((*gfx)++, G_TF_POINT);
    gDPSetColorDither((*gfx)++, G_CD_DISABLE);
    func_802DF160(gfx, mtx, vtx);
    func_80253190(gfx);

    gDPSetTextureFilter((*gfx)++, G_TF_POINT);
    gSPSegment((*gfx)++, 0x04, osVirtualToPhysical(sp50));
    modelRender_preDraw((GenFunction_1)actor_predrawMethod, (s32)sp6C);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);

    // @recomp Load and patch the puzzle model.
    BKModelBin *model_bin = marker_loadModelBin(marker);
    patch_picture_model(model_bin, 260, 398, -273, -100, FROM_YZ);

    modelRender_draw(gfx, mtx, sp60, NULL, D_80368250, sp54, model_bin);
    gDPSetTextureFilter((*gfx)++, G_TF_BILERP);
    gDPSetColorDither((*gfx)++, G_CD_MAGICSQ);
    chBottlesBonusCursor_draw(gfx, mtx, vtx);
    chBottlesBonus_func_802DD158(gfx, mtx);
    return sp6C;
}

// @recomp Patches the model for the background picture to fix the seams.
RECOMP_PATCH Actor *func_802DF160(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    Actor *this;
    void *sp38;

    this = marker_getActor(D_8037E000);
    sp38 = func_8030C704();
    modelRender_setDepthMode(MODEL_RENDER_DEPTH_FULL);
    gDPSetTextureFilter((*gfx)++, G_TF_POINT);
    gSPSegment((*gfx)++, 0x04, osVirtualToPhysical(sp38));
    modelRender_preDraw((GenFunction_1)actor_predrawMethod, (s32)this);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)D_8037E000);

    // @recomp Load and patch the picture model.
    BKModelBin *model_bin = marker_loadModelBin(D_8037E000);
    patch_picture_model(model_bin, 262, 397, -273, -100, FROM_YZ);

    modelRender_draw(gfx, mtx, &D_80368360, NULL, 1.0f, NULL, model_bin);
    gDPSetTextureFilter((*gfx)++, G_TF_BILERP);
    return this;
}

// @recomp Patches the model for the photo to fix the seams.
RECOMP_PATCH Actor *func_802DEC00(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    Actor *this;
    f32 sp58[3];
    f32 sp4C[3];
    void *sp48;


    this = marker_getActor(marker);
    sp48 = func_8030C704();

    if ((sp48 == 0) || (getGameMode() != GAME_MODE_A_SNS_PICTURE))
        return this;

    chBottlesBonus_func_802DD080(gfx, mtx);
    sp58[0] = 0.0f;
    sp58[1] = 0.0f;
    sp58[2] = 50.0f;

    sp4C[0] = 0.0f;
    sp4C[1] = 0.0f;
    sp4C[2] = 0.0f;

    D_8037DFF0[0] = 0.0f;
    D_8037DFF0[1] = 270.0f;
    D_8037DFF0[2] = 0.0f;
    modelRender_setDepthMode(MODEL_RENDER_DEPTH_FULL);
    modelRender_draw(gfx, mtx, sp58, NULL, 1.0f, sp4C, D_8037DFE8);
    gDPSetColorDither((*gfx)++, G_CD_DISABLE);
    func_80253190(gfx);
    gSPSegment((*gfx)++, 0x04, osVirtualToPhysical(sp48));
    modelRender_preDraw((GenFunction_1)actor_predrawMethod, (s32)this);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);

    // @recomp Load and patch the photo model.
    BKModelBin *model_bin = marker_loadModelBin(marker);
    patch_picture_model(model_bin, -55, -5, -6, 34, FROM_XZ);

    modelRender_draw(gfx, mtx, this->position, NULL, 4.5f, sp4C, model_bin);
    gDPSetTextureFilter((*gfx)++, G_TF_BILERP);
    gDPSetColorDither((*gfx)++, G_CD_MAGICSQ);
    chBottlesBonus_func_802DD158(gfx, mtx);
    return this;
}