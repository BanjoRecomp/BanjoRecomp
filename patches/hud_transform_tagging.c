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