#include "patches.h"
#include "transform_ids.h"
#include "functions.h"

extern s32 D_803815C0;
extern s32 D_803815E4;
extern s32 D_803815EC;
extern s32 D_8036A018[];
extern f32 D_803815C8;
extern f32 D_803815CC;
extern f32 D_803815D0;
extern f32 D_803815D4;
extern f32 D_803815D8;
extern f32 D_803815DC;
extern f32 D_803815E0;
extern s32 D_803815E8;
extern void *D_8036A010;
extern void *D_8036A014;
extern Gfx D_8036A030[];

extern void func_80347FC0(Gfx **gfx, BKSprite *sprite, s32 frame, s32 tmem, s32 rtile, s32 uls, s32 ult, s32 cms, s32 cmt, s32 *width, s32 *height);
extern f32 func_802FDE60(f32 arg0);
extern f32 func_802FB0E4(struct8s *this);
extern s32 func_802FB0D4(struct8s *this);
extern s32 itemPrint_getValue(s32 item_id);
extern s32 getGameMode(void);
extern f32 vtxList_getGlobalNorm(BKVertexList *);

typedef struct {
    u8 pad0[0x14];
    s32 unk14;
    u8 pad18[0x8];
    s32 item_id; //item_id
    s32 model_id; //model_id
    s32 anim_id; //anim_id
    f32 anim_duration; //anim_duration
    f32 unk30;
    f32 unk34;
    f32 unk38;
    f32 unk3C; //scale?
    f32 unk40;
    f32 unk44;
    f32 unk48;
    f32 unk4C;
    f32 unk50;
    f32 unk54;
    BKModelBin *model;
    char value_string[0xC];
    f32 unk68;
    f32 unk6C;
    AnimCtrl *anim_ctrl;
}Struct_core2_79830_0;

// @recomp Tag the matrices for each honeycomb piece.
RECOMP_PATCH void fxhoneycarrierscore_draw(s32 arg0, struct8s *arg1, Gfx **arg2, Mtx **arg3, Vtx **arg4) {
    f64 var_f24;
    s32 sp13C;
    s32 sp138;
    s32 sp134;
    f32 sp130;
    f32 sp12C;
    f32 sp128;
    f32 sp124;
    s32 var_v0;
    s32 var_v1;
    u32 sp118;
    f32 pad;
    f32 sp110;

    sp118 = D_803815C0 == 2;
    if (D_8036A010 != 0) {
        func_80347FC0(arg2, (sp118) ? (D_8036A014 != 0) ? D_8036A014 : D_8036A010 : D_8036A010, 0, 0, 0, 0, 0, 2, 2, &sp13C, &sp138);
        viewport_setRenderViewportAndOrthoMatrix(arg2, arg3);
        gSPDisplayList((*arg2)++, D_8036A030);
        for (sp134 = 0; sp134 < ((sp118) ? ((D_8036A014 != 0) ? 2 : 1) : 6); sp134++) {
            sp110 = D_8036A018[sp134] * -0x3C;
            gDPPipeSync((*arg2)++);
            if (sp118) {
                if (sp134 != 0) {
                    func_80347FC0(arg2, D_8036A010, 0, 0, 0, 0, 0, 2, 2, &sp13C, &sp138);
                    gDPSetPrimColor((*arg2)++, 0, 0, 0x00, 0x00, 0x00, (0xFF - D_803815E4));
                }
                else {
                    gDPSetPrimColor((*arg2)++, 0, 0, 0x00, 0x00, 0x00, D_803815E4);
                }
            }
            else {
                if (D_803815D4 <= D_8036A018[sp134]) {
                    gDPSetPrimColor((*arg2)++, 0, 0, 0x00, 0x00, 0x00, 0x50);
                }
                else {
                    if ((D_803815EC != 0) && ((D_803815D4 - 1.0f) == D_8036A018[sp134])) {
                        gDPSetPrimColor((*arg2)++, 0, 0, 0x00, 0x00, 0x00, D_803815E8);
                    }
                    else {
                        gDPSetPrimColor((*arg2)++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
                    }
                }
            }

            // @recomp Set a matrix group.
            gEXMatrixGroupSimpleVerts((*arg2)++, HUD_HONEYCOMB_TRANSFORM_ID_START + sp134, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

            sp128 = (244.0f - ((f32)gFramebufferWidth / 2));
            sp124 = func_802FB0E4(arg1) + ((f32)gFramebufferHeight / 2) - 246.0f;
            guTranslate(*arg3, sp128 * 4.0f, sp124 * 4.0f, 0.0f);
            gSPMatrix((*arg2)++, OS_K0_TO_PHYSICAL((*arg3)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            guRotate(*arg3, func_802FDE60(D_803815D8 + D_803815DC), 0.0f, 0.0f, 1.0f);
            gSPMatrix((*arg2)++, OS_K0_TO_PHYSICAL((*arg3)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            guScale(*arg3, D_803815E0, D_803815E0, D_803815E0);
            gSPMatrix((*arg2)++, OS_K0_TO_PHYSICAL((*arg3)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            guTranslate(*arg3, -sp128 * 4.0f, -sp124 * 4.0f, 0.0f);
            gSPMatrix((*arg2)++, OS_K0_TO_PHYSICAL((*arg3)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            var_f24 = MIN(1.0, MAX(0.0, D_803815C8));
            sp130 = cosf(((D_803815CC + sp110) * 0.017453292519943295)) * (var_f24 * 24.5) * D_803815D0;
            var_f24 = MIN(1.0, MAX(0.0, D_803815C8));
            sp12C = sinf(((D_803815CC + sp110) * 0.017453292519943295)) * (var_f24 * 24.5) * D_803815D0;
            gSPVertex((*arg2)++, *arg4, 4, 0);
            for (var_v1 = 0; var_v1 < 2; var_v1++) {
                for (var_v0 = 0; var_v0 < 2; var_v0++, (*arg4)++) {
                    (*arg4)->v.ob[0] = ((((sp13C * D_803815D0) * var_v0) - ((sp13C * D_803815D0) / 2)) + (s32)(sp130 + sp128)) * 4.0f;
                    (*arg4)->v.ob[1] = ((((sp138 * D_803815D0) / 2) - ((sp138 * D_803815D0) * var_v1)) + (s32)(sp12C + sp124)) * 4.0f;
                    (*arg4)->v.ob[2] = -0x14;
                    (*arg4)->v.tc[0] = (s16)((sp13C - 1) * var_v0 << 9);
                    (*arg4)->v.tc[1] = (s16)((sp138 - 1) * var_v1 << 9);

                }
            }
            gSP1Quadrangle((*arg2)++, 0, 1, 3, 2, 0);

            // @recomp Clear the matrix group.
            gEXPopMatrixGroup((*arg2)++, G_MTX_MODELVIEW);
        }
        gDPPipeSync((*arg2)++);
        gDPSetTextureLUT((*arg2)++, G_TT_NONE);
        gDPPipelineMode((*arg2)++, G_PM_NPRIMITIVE);
        viewport_setRenderViewportAndPerspectiveMatrix(arg2, arg3);
    }
}

#define RM_DEPTH_SET(clk) \
        Z_UPD | CVG_DST_FULL | ALPHA_CVG_SEL | FORCE_BL | G_ZS_PRIM | \
        ZMODE_OPA | \
        GBL_c##clk(G_BL_CLR_BL, G_BL_0, G_BL_CLR_MEM, G_BL_1MA)

// @recomp Tag the matrices for any model using this score type.
RECOMP_PATCH void fxcommon3score_draw(enum item_e item_id, void *arg1, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    Struct_core2_79830_0 *a1 = (Struct_core2_79830_0 *)arg1;
    f32 sp68[3];
    f32 sp5C[3];
    f32 sp50[3];
    f32 sp44[3];
    f32 sp40;
    f32 sp3C;

    sp40 = func_802FB0E4(arg1) * a1->unk54 + a1->unk34;
    if (a1->model != NULL && func_802FB0D4(arg1)) {
        a1->value_string[0] = '\0';
        strIToA(a1->value_string, itemPrint_getValue(item_id));
        print_bold_spaced(a1->unk30 + a1->unk40, sp40 + a1->unk44, a1->value_string);
        sp3C = viewport_transformCoordinate(a1->unk30, sp40, sp5C, sp68);

        sp44[0] = 0.0f;
        sp44[1] = a1->unk38;
        sp44[2] = 0.0f;

        sp50[0] = 0.0f;
        sp50[1] = a1->unk68;
        sp50[2] = 0.0f;
        func_8033A308(sp50);
        if (getGameMode() != GAME_MODE_4_PAUSED) {
            modelRender_setDepthMode(MODEL_RENDER_DEPTH_FULL);
        }
        sp68[0] += a1->unk4C;
        if (a1->unk6C == 0.0f) {
            a1->unk6C = 1.1 * (vtxList_getGlobalNorm(model_getVtxList(a1->model)) * a1->unk3C);
        }

        // @recomp Draw a rectangle that clears the depth and gets interpolated along with the 3D model.
        // This fixes an issue where the rectangle would teleport ahead of the movement of the model in HFR
        // and it also provides a minor performance boost by skipping the need to switch render targets.
        if (getGameMode() != GAME_MODE_4_PAUSED) {
            u32 depth_rect_transform_id = HUD_SCORE3_DEPTH_RECT_TRANSFORM_ID_START + item_id;
            guMtxIdent(*mtx);
            gSPMatrix((*gfx)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gEXMatrixGroupSimpleVerts((*gfx)++, depth_rect_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

            f32 x = a1->unk30 - a1->unk6C;
            f32 y = sp40 - a1->unk6C;
            f32 w = 2 * a1->unk6C;
            f32 h = 2 * a1->unk6C;
            gSPVertex((*gfx)++, *vtx, 4, 0);

            f32 vpos[3];
            f32 vrot[3];
            for (u32 v_y = 0; v_y < 2; v_y++) {
                for (u32 v_x = 0; v_x < 2; v_x++) {
                    viewport_transformCoordinate(x + (w * v_x), y + (h * v_y), vpos, vrot);
                    (*vtx)->v.ob[0] = vpos[0];
                    (*vtx)->v.ob[1] = vpos[1];
                    (*vtx)->v.ob[2] = vpos[2];
                    (*vtx)++;
                }
            }

            gEXPushOtherMode((*gfx)++);
            gEXPushCombineMode((*gfx)++);
            gDPSetRenderMode((*gfx)++, RM_DEPTH_SET(1), RM_DEPTH_SET(2));
            gDPSetCycleType((*gfx)++, G_CYC_1CYCLE);
            gDPSetCombineLERP((*gfx)++, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            gDPSetPrimDepth((*gfx)++, 0xFFFF, 0xFFFF);
            gSP1Quadrangle((*gfx)++, 0, 1, 3, 2, 0);
            gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
            gEXPopOtherMode((*gfx)++);
            gEXPopCombineMode((*gfx)++);
        }

        if (a1->anim_ctrl != NULL) {
            anctrl_drawSetup(a1->anim_ctrl, sp5C, 1);
        }

        // @recomp Set the model transform ID.
        cur_drawn_model_transform_id = HUD_SCORE3_TRANSFORM_ID_START + item_id * MARKER_TRANSFORM_ID_COUNT;

        modelRender_draw(gfx, mtx, sp5C, sp68, a1->unk3C * sp3C, sp44, a1->model);

        // @recomp Clear the model transform ID.
        cur_drawn_model_transform_id = 0;
    }//L80300BA4
}