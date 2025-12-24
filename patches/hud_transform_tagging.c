#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "../src/core2/gc/zoombox.h"

#define AIRSCORE_COUNT (6)

extern u8 D_80381E58[5];
extern u16 D_80381620[0xD][5][0x10];
extern u16 gScissorBoxLeft;
extern u16 gScissorBoxRight;
extern u16 gScissorBoxTop;
extern u16 gScissorBoxBottom;
extern s32 D_803815C0;
extern s32 D_803815E4;
extern s32 D_803815EC;
extern s32 D_8036A018[];
extern s32 D_80381EC4;
extern s32 gTotalHealth;
extern f32 D_803815C8;
extern f32 D_803815CC;
extern f32 D_803815D0;
extern f32 D_803815D4;
extern f32 D_803815D8;
extern f32 D_803815DC;
extern f32 D_803815E0;
extern f32 D_80381E54;
extern f32 D_80381E60[5];
extern f32 D_80381E78[5];
extern f32 D_80381EB8;
extern f32 D_80381EBC;
extern f32 D_80381EFC;
extern f32 D_80381F08[8];
extern f32 D_80381F68[AIRSCORE_COUNT];
extern f32 s_texture_scale;
extern f32 s_active_count;
extern s32 D_803815E8;
extern f32 gHealth;
extern void *D_8036A010;
extern void *D_8036A014;
extern Gfx D_8036A030[];
extern Gfx D_8036A228[];
extern Gfx D_8036A278[];
extern Gfx D_8036A918[];
extern Gfx s_fxairscore_context[];
extern void *D_80381EB0[2];
extern char code_78E50_ItemValueString[8];
extern BKSprite *D_80381E40[5];
extern BKSprite *gSpriteHealth;
extern BKSprite *gSpriteRedHealth;
extern BKSprite *s_sprite;
extern struct1Cs_1 D_8036C58C[0xD];

extern void func_80347FC0(Gfx **gfx, BKSprite *sprite, s32 frame, s32 tmem, s32 rtile, s32 uls, s32 ult, s32 cms, s32 cmt, s32 *width, s32 *height);
extern void func_80348044(Gfx **gfx, BKSprite *sprite, s32 frame, s32 tmem, s32 rtile, s32 uls, s32 ult, s32 cms, s32 cmt, s32 *width, s32 *height, s32 *frame_width, s32 *frame_height, s32 *texture_x, s32 *texture_y, s32 *textureCount);
extern f32 func_802FDE60(f32 arg0);
extern f32 func_802FB0E4(struct8s *this);
extern s32 func_802FB0D4(struct8s *this);
extern f32 func_802FB0DC(struct8s *this);
extern void func_802FD360(struct8s *arg0, Gfx **gfx, Mtx **mtx, Vtx **vtx);
extern s32 itemPrint_getValue(s32 item_id);
extern s32 level_get(void);
extern s32 itemscore_noteScores_getTotal(void);
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

extern struct {
    u8          state;
    u8          menu;
    u8          selection; //menu page
    u8          exit_pause : 1;
    u8          unk3_6 : 1; //busy?
    u8          unk3_5 : 1;
    u8          unk3_4 : 1;
    u8          left_joystick_visible : 1;
    u8          right_joystick_visible : 1;
    u8          b_button_visible : 1;
    u8          unk3_0 : 1;
    s8          zoombox_opening_count;
    s8          zoombox_closing_count;
    u8          unk6;
    u8          unk7;
    s8          unk8; //header position
    s8          page;
    s8          joystick_frame;
    u8          joystick_frame_count;
    f32         unkC;
    GcZoombox *zoombox[4];
    f32         unk20;
    BKSprite *joystick_sprite;
    f32         unk28;
    BKSprite *b_button_sprite;
    u8          b_button_frame;
    u8          b_button_frame_count;      //B-button total frames
    s16         b_button_alpha;      //B-button alpha
    s16         left_joystick_alpha;      //left joystick alpha
    s16         right_joystick_alpha;      //right joystick alpha
    u8          page_cnt;
    u8          sns_items;
    u8          sns_visible;
    // u8  pad3B[1];
    s16         sns_alpha; //sns opacity
    s16         unk3E[7];
    s16         unk4C[7];
    // u8 pad5A[0x3];
    BKModelBin *sns_egg_model; //SnS Egg Model
    BKModelBin *ice_key_model; //Ice key model
    u8          pad64[12];
    u32         unk70_31 : 1;
    u32         unk70_30 : 1;
    u32         return_to_lair_disabled : 1;
    u32         pad70_28 : 29;
} D_80383010;

extern u32 cur_pushed_text_transform_id;
extern u32 cur_pushed_text_transform_origin;

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

        // @recomp Align the honeycomb pieces to the right side of the screen.
        // NOTE: gScissorBoxRight/gScissorBoxTop are incorrectly named in the decompilation and must be swapped.
        gEXPushScissor((*arg2)++);
        gEXSetScissor((*arg2)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_RIGHT, G_EX_ORIGIN_RIGHT, gScissorBoxLeft - gScissorBoxTop, gScissorBoxRight, 0, gScissorBoxBottom);
        gEXSetViewportAlign((*arg2)++, G_EX_ORIGIN_RIGHT, -gScissorBoxTop * 4, 0);

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

        // @recomp Clear the matrix group.
        gEXSetViewportAlign((*arg2)++, G_EX_ORIGIN_NONE, 0, 0);
        gEXPopScissor((*arg2)++);

        gDPPipeSync((*arg2)++);
        gDPSetTextureLUT((*arg2)++, G_TT_NONE);
        gDPPipelineMode((*arg2)++, G_PM_NPRIMITIVE);
        viewport_setRenderViewportAndPerspectiveMatrix(arg2, arg3);
    }
}

// @recomp Patched to tag the Jinjos on the HUD.
RECOMP_PATCH void fxjinjoscore_draw(s32 arg0, struct8s *arg1, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    BKSprite *sprite; // s1
    s32 draw_index; // s5
    s32 texture_width; // sp11C
    s32 texture_height; // sp118
    s32 jinjo_id; // sp114
    f32 center_y; // f14 (sp110)
    f32 center_x; // f20 (sp10C)
    f32 x_offset; // f26 (sp108)
    f32 y_offset; // f28 (sp104)
    f32 pos_x; // f30 (sp100)
    s32 i; // v1 (spFC)
    s32 j; // v0_2 (spF8)

    // @recomp Align the Jinjos to the left side of the screen.
    // NOTE: gScissorBoxRight/gScissorBoxTop are incorrectly named in the decompilation and must be swapped.
    gEXPushScissor((*gfx)++);
    gEXSetScissor((*gfx)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, gScissorBoxLeft, gScissorBoxRight, gScissorBoxTop, gScissorBoxBottom);
    gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_LEFT, 0, 0);

    gSPDisplayList((*gfx)++, D_8036A228);
    viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
    pos_x = 44.0f;
    // Draw all jinjo heads
    for (jinjo_id = 0; jinjo_id < 5; jinjo_id++) {
        s32 jinjo_collected; // spF0 <----
        sprite = D_80381E40[jinjo_id];
        jinjo_collected = (D_80381E58[jinjo_id] != 0) ? 1 : 0;
        if (sprite != NULL) {
            func_80347FC0(gfx, sprite, (s32)D_80381E60[jinjo_id], 0, 0, 0, 0, 2, 2, &texture_width, &texture_height);
            // Load the palette for the corresponding jinjo color
            gDPLoadTLUT_pal16((*gfx)++, 0, D_80381620[(s32)D_80381E60[jinjo_id]][jinjo_id]);
            x_offset = 0.0f;
            y_offset = 0.0f;
            // Draw the jinjo head, once if uncollected and twice if collected
            // If the head is drawn twice then the first draw will be the drop shadow
            for (draw_index = jinjo_collected; draw_index >= 0; draw_index--) {
                gDPPipeSync((*gfx)++);
                // Draw 0 is the jinjo's head, anything else is a shadow
                if (draw_index != 0) {
                    // Use only primitive color as the color input in order to make a solid color shadow
                    gDPSetCombineLERP((*gfx)++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
                    // Set up a translucent black for primitive color to draw the shadow
                    gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0x8C);
                }
                else {
                    // Use the texture as the color input
                    gDPSetCombineLERP((*gfx)++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
                    // If the jinjo is collected then it's drawn fully opaque, otherwise it's drawn with partial alpha
                    gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, jinjo_collected ? 0xFF : 0x6E);
                }
                center_x = pos_x - (f32)gFramebufferWidth / 2 + x_offset;
                center_y = (f32)gFramebufferHeight / 2 + func_802FB0E4(arg1) - 266.0f + 40.0f + y_offset - D_80381E78[jinjo_id];

                // @recomp Assign the matrix group for each Jinjo and its shadow separately.
                gEXMatrixGroupSimpleVerts((*gfx)++, HUD_JINJOSCORE_TRANSFORM_ID_START + jinjo_id * 2 + draw_index, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

                gSPVertex((*gfx)++, *vtx, 4, 0);
                // Set up the positions of the four vertices
                for (i = 0; i < 2; i++) {
                    for (j = 0; j < 2; j++) {
                        (*vtx)->v.ob[0] = ((texture_width * D_80381E54 * j) - (texture_width * D_80381E54 / 2) + center_x) * 4;
                        (*vtx)->v.ob[1] = ((texture_height * D_80381E54 / 2) - (texture_height * D_80381E54 * i) + center_y) * 4;
                        (*vtx)->v.ob[2] = -20;
                        (*vtx)->v.tc[0] = ((texture_width - 1) * j) << 6;
                        (*vtx)->v.tc[1] = ((texture_height - 1) * i) << 6;
                        (*vtx)++;
                    }
                }
                // Draw a quad made of the four vertices
                gSP1Quadrangle((*gfx)++, 0, 1, 3, 2, 0);

                // @recomp Clear the matrix group.
                gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);

                x_offset += -2;
                y_offset += 2;
            }
        }
        // Move the next jinjo head over by 32 pixels
        pos_x += 32.0f;
    }

    // @recomp Clear the matrix group.
    gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
    gEXPopScissor((*gfx)++);

    gDPPipeSync((*gfx)++);
    gDPSetTextureLUT((*gfx)++, G_TT_NONE);
    gDPPipelineMode((*gfx)++, G_PM_NPRIMITIVE);
    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
}

// @recomp Patched to tag the pieces of air and also align them to the left side of the screen.
RECOMP_PATCH void fxairscore_draw(enum item_e item_id, struct8s *arg1, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    f32 y;
    f32 x;
    s32 texture_width;
    s32 texture_height;
    s32 i_part;
    s32 var_s6;
    s32 v_x;
    s32 v_y;

    if (s_sprite != 0) {
        gSPDisplayList((*gfx)++, s_fxairscore_context);
        func_80347FC0(gfx, s_sprite, 0, 0, 0, 0, 0, 2, 2, &texture_width, &texture_height);

        // @recomp Align the pieces of air to the left side of the screen.
        // NOTE: gScissorBoxRight/gScissorBoxTop are incorrectly named in the decompilation and must be swapped.
        gEXPushScissor((*gfx)++);
        gEXSetScissor((*gfx)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, gScissorBoxLeft, gScissorBoxRight, gScissorBoxTop, gScissorBoxBottom);
        gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_LEFT, 0, 0);

        viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
        //render all 6 air pieces
        for (i_part = 0; i_part < AIRSCORE_COUNT; i_part++) {
            if ((i_part != 0) && (i_part != 5)) {
                var_s6 = (i_part & 1) ? i_part + 1 : i_part - 1;
            }
            else {
                var_s6 = i_part;
            }
            gDPPipeSync((*gfx)++);
            if ((f32)(5 - i_part) < s_active_count) {
                gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
            }
            else {
                gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0x78);
            }
            x = func_802FB0E4(arg1);
            x = ((-40 + x) + D_80381F68[var_s6]) - ((f32)gFramebufferWidth / 2);
            y = ((78 + (i_part * 15.5)) - ((f32)gFramebufferHeight / 2));

            // @recomp Assign a matrix group to each piece of air.
            gEXMatrixGroupSimpleVerts((*gfx)++, HUD_AIRSCORE_TRANSFORM_ID_START + i_part, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

            //stagger x position
            x = (i_part & 1) ? x + 5.0f : x - 5.0f;
            gSPVertex((*gfx)++, *vtx, 4, 0);
            for (v_y = 0; v_y < 2; v_y++) {
                for (v_x = 0; v_x < 2; v_x++) {
                    (*vtx)->v.ob[0] = (x + (((texture_width * s_texture_scale) * v_x) - ((texture_width * s_texture_scale) / 2))) * 4.0f;
                    (*vtx)->v.ob[1] = (y + (((texture_height * s_texture_scale) / 2) - (texture_height * s_texture_scale) * v_y)) * 4.0f;
                    (*vtx)->v.ob[2] = -0x14;
                    (*vtx)->v.tc[0] = ((texture_width - 1) * v_x) << 6;
                    (*vtx)->v.tc[1] = ((texture_height - 1) * v_y) << 6;
                    (*vtx)++;
                }
            }
            gSP1Quadrangle((*gfx)++, 0, 1, 3, 2, 0);

            // @recomp Clear the matrix group.
            gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
        }

        // @recomp Clear the matrix group.
        gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
        gEXPopScissor((*gfx)++);

        gDPPipeSync((*gfx)++);
        gDPSetTextureLUT((*gfx)++, G_TT_NONE);
        gDPPipelineMode((*gfx)++, G_PM_NPRIMITIVE);
        viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
    }
}

// @recomp Patched to tag the health bar and align it to the left side of the screen.
RECOMP_PATCH void fxhealthscore_draw(enum item_e item_id, struct8s *arg1, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    int i;
    int tmp_v1;
    s32 honeycomb_width;
    s32 honeycomb_height;
    int tmp_v0;
    f32 f18;
    f32 f14;
    f32 f20;
    s32 is_red_health_initialized = FALSE;
    s32 s6;

    if (gSpriteHealth == NULL) {
        return;
    }

    gSPDisplayList((*gfx)++, D_8036A918);
    func_80347FC0(gfx, gSpriteHealth, 0, 0, 0, 0, 0, 2, 2, &honeycomb_width, &honeycomb_height);

    // @recomp Align the health bar to the left side of the screen.
    // NOTE: gScissorBoxRight/gScissorBoxTop are incorrectly named in the decompilation and must be swapped.
    gEXPushScissor((*gfx)++);
    gEXSetScissor((*gfx)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, gScissorBoxLeft, gScissorBoxRight, gScissorBoxTop, gScissorBoxBottom);
    gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_LEFT, 0, 0);

    viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);

    //loop over each honeycomb piece
    for (i = gTotalHealth - 1; i >= 0; i--) {//L80300E40
        if (i != 0 && (i + 1 != gTotalHealth || gTotalHealth & 1)) {
            s6 = (i & 1) ? i + 1 : i - 1;
        }
        else {//L80300E84
            s6 = i;
        }

        gDPPipeSync((*gfx)++);

        if (gHealth > i) {
            if (0 < (gHealth - 8.0f) && (gHealth - 8.0f) > i) {
                if (!is_red_health_initialized) {
                    func_80347FC0(gfx, gSpriteRedHealth, 0, 0, 0, 0, 0, 2, 2, &honeycomb_width, &honeycomb_height);
                    is_red_health_initialized = TRUE;
                }
            }//L80300F38

            gDPSetPrimColor((*gfx)++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
        }
        else {//L80300F58
            gDPSetPrimColor((*gfx)++, 0, 0, 0xFF, 0xFF, 0xFF, 0x78);
        }

        f20 = 96.0f - (f32)gFramebufferWidth / 2 + (i * 13);
        f14 = (f32)gFramebufferHeight / 2 - func_802FB0E4(arg1) - D_80381F08[s6] - -48.0f;
        f14 = (i & 1) ? f14 + 5.75 : f14 - 5.75;

        // @recomp Assign a matrix group to each piece of health.
        gEXMatrixGroupSimpleVerts((*gfx)++, HUD_HEALTHSCORE_TRANSFORM_ID_START + i, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

        gSPVertex((*gfx)++, *vtx, 4, 0);

        for (tmp_v1 = 0; tmp_v1 < 2; tmp_v1++) {//L8030101C
            for (tmp_v0 = 0; tmp_v0 < 2; tmp_v0++) {//L80301030
                (*vtx)->v.ob[0] = (((honeycomb_width * D_80381EFC) * tmp_v0 - (honeycomb_width * D_80381EFC) / 2) + f20) * 4.0f;
                (*vtx)->v.ob[1] = (((honeycomb_height * D_80381EFC) / 2 - (honeycomb_height * D_80381EFC) * tmp_v1) + f14) * 4.0f;
                (*vtx)->v.ob[2] = -0x14;

                (*vtx)->v.tc[0] = ((honeycomb_width - 1) * tmp_v0) << 6;
                (*vtx)->v.tc[1] = ((honeycomb_height - 1) * tmp_v1) << 6;
                (*vtx)++;
            }
        }

        gSP1Quadrangle((*gfx)++, 0, 1, 3, 2, 0);

        // @recomp Clear the matrix group.
        gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
    }

    // @recomp Clear the matrix group.
    gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
    gEXPopScissor((*gfx)++);

    gDPPipeSync((*gfx)++);
    gDPSetTextureLUT((*gfx)++, G_TT_NONE);
    gDPPipelineMode((*gfx)++, G_PM_NPRIMITIVE);
    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
}

// @recomp Patched to tag the life icon and align it to the left side of the screen. 
RECOMP_PATCH void fxlifescore_draw(enum item_e item_id, struct8s *arg1, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    s32 sp10C;
    Vtx *sp108;
    s32 sp104;
    s32 var_v0;
    s32 var_v1;
    s32 var_s5;
    s32 var_s4;
    s32 spF0;
    s32 spEC;
    s32 spE8;
    s32 spE4;
    s32 spE0;
    s32 spDC;

    sp10C = -1;
    sp108 = *vtx;
    code_78E50_ItemValueString[0] = '\0';
    strIToA(code_78E50_ItemValueString, MIN(9, itemPrint_getValue(item_id)));

    // @recomp Assign an ID to the text and align it to the left side of the screen.
    cur_pushed_text_transform_id = HUD_LIFESCORE_TRANSFORM_PRINT_ID_START;
    cur_pushed_text_transform_origin = G_EX_ORIGIN_LEFT;

    print_bold_spaced(0x4E, (s32)(func_802FB0E4(arg1) + -16.0f + 4.0f), (char *)&code_78E50_ItemValueString);

    // @recomp Clear the ID and alignment for the text.
    cur_pushed_text_transform_id = 0;
    cur_pushed_text_transform_origin = G_EX_ORIGIN_NONE;

    if (1); //fake
    if (D_80381EB0[D_80381EC4] != NULL) {
        // @recomp Align the life icon to the left side of the screen. 
        // NOTE: gScissorBoxRight/gScissorBoxTop are incorrectly named in the decompilation and must be swapped.
        gEXPushScissor((*gfx)++);
        gEXSetScissor((*gfx)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, gScissorBoxLeft, gScissorBoxRight, gScissorBoxTop, gScissorBoxBottom);
        gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_LEFT, 0, 0);

        gSPDisplayList((*gfx)++, D_8036A278);
        viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
        if (gfx);

        // @recomp Assign a matrix group to the life icon.
        gEXMatrixGroupSimpleNormal((*gfx)++, HUD_LIFESCORE_TRANSFORM_ID_START, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

        gDPPipeSync((*gfx)++);
        gDPSetCombineLERP((*gfx)++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
        do {
            func_80348044(gfx, D_80381EB0[D_80381EC4], (s32)D_80381EBC % 4, 0, 0, 0, 0, 2, 2, &spF0, &spEC, &spE8, &spE4, &spE0, &spDC, &sp10C);

            if (((*vtx - sp108) & 0xF) == 0) {
                gSPVertex((*gfx)++, *vtx, MIN(0x10, (1 + sp10C) << 2), 0);
                sp104 = 0;
            }
            else {
                sp104 = sp104 + 4;
            }

            var_s5 = (40.0f - ((f32)gFramebufferWidth / 2)) + spE0;
            var_s4 = (((((f32)gFramebufferHeight / 2) - func_802FB0E4(arg1)) - -16.0f) - spDC);

            // @recomp Assign the vertical translation of the life icon to the matrix.
            guTranslate(*mtx - 1, 0.0f, -func_802FB0E4(arg1) * 4.0f, 0.0f);
            var_s4 += func_802FB0E4(arg1);

            for (var_v1 = 0; var_v1 < 2; var_v1++) {
                for (var_v0 = 0; var_v0 < 2; var_v0++) {
                    (*vtx)->v.ob[0] = (s16)(s32)(((((f32)spF0 * D_80381EB8 * (f32)var_v0) - (((f32)spE8 * D_80381EB8) / 2)) + var_s5) * 4.0f);
                    (*vtx)->v.ob[1] = (s16)(s32)((((((f32)spE4 * D_80381EB8) / 2) - ((f32)spEC * D_80381EB8 * var_v1)) + var_s4) * 4.0f);
                    (*vtx)->v.ob[2] = -0x14;
                    (*vtx)->v.tc[0] = ((spF0 - 1) * var_v0) << 6;
                    (*vtx)->v.tc[1] = ((spEC - 1) * var_v1) << 6;
                    (*vtx)++;
                }
            }
            gSP1Quadrangle((*gfx)++, sp104, sp104 + 1, sp104 + 3, sp104 + 2, 0);
        } while (sp10C != 0);

        // @recomp Clear the matrix group and the viewport alignment.
        gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
        gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
        gEXPopScissor((*gfx)++);

        gDPPipeSync((*gfx)++);
        gDPSetTextureLUT((*gfx)++, G_TT_NONE);
        gDPPipelineMode((*gfx)++, G_PM_NPRIMITIVE);
        viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
    }
}

// @recomp Patch to tag any 2D score elements and align them to the right side of the screen.
RECOMP_PATCH void fxcommon2score_draw(enum item_e item_id, struct8s *arg1, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    f32 pad;
    s32 sp38;
    f32 sp34;


    sp38 = itemPrint_getValue(item_id);
    sp34 = 0.0f;
    if (item_id == ITEM_C_NOTE) {
        if (level_get() == LEVEL_6_LAIR || level_get() == LEVEL_C_BOSS) {
            sp38 = itemscore_noteScores_getTotal();
        }
    }
    if (item_id < 6) {
        sp38 = ((sp38) ? 1 : 0) + sp38 / 60;
    }//L802FDBA8
    if (item_id == ITEM_1B_VILE_VILE_SCORE && 9 < sp38) {
        sp34 = -16.0f;
    }
    if (item_id == ITEM_1C_MUMBO_TOKEN || item_id == ITEM_25_MUMBO_TOKEN_TOTAL) {
        if (sp38 >= 100) {
            sp38 = 99;
        }
    }
    arg1->string_54[0] = 0;
    //convert to string
    strIToA(arg1->string_54, sp38);

    // @recomp Assign an ID to the text and align it to the left or the right side of the screen.
    bool left_alignment = arg1->unk38 < (gFramebufferWidth * 1.0f / 3.0f);
    cur_pushed_text_transform_id = HUD_SCORE2_TRANSFORM_PRINT_ID_START + item_id * HUD_SCORE2_TRANSFORM_PRINT_ID_COUNT;
    cur_pushed_text_transform_origin = left_alignment ? G_EX_ORIGIN_LEFT : G_EX_ORIGIN_RIGHT;

    //print text (blue egg font)
    print_bold_spaced(
        (s32)(func_802FB0DC(arg1) + arg1->unk38 + arg1->unk44 + sp34),
        (s32)(func_802FB0E4(arg1) * arg1->unk4C + (arg1->unk3C + arg1->unk48)),
        arg1->string_54
    );

    // @recomp Clear the ID and alignment for the text.
    cur_pushed_text_transform_id = 0;
    cur_pushed_text_transform_origin = G_EX_ORIGIN_NONE;

    // @recomp Align the score element to either the left or the right side of the screen.
    // NOTE: gScissorBoxRight/gScissorBoxTop are incorrectly named in the decompilation and must be swapped.
    gEXPushScissor((*gfx)++);
    gEXSetScissor((*gfx)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, gScissorBoxRight, 0, gScissorBoxBottom);
    gEXSetViewportAlign((*gfx)++, left_alignment ? G_EX_ORIGIN_LEFT : G_EX_ORIGIN_RIGHT, left_alignment ? 0 : gScissorBoxTop * -4, 0);

    // @recomp Assign a matrix group to the score element.
    gEXMatrixGroupSimpleVerts((*gfx)++, HUD_SCORE2_TRANSFORM_ID_START + item_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

    //draw sprite?
    func_802FD360(arg1, gfx, mtx, vtx);

    // @recomp Clear the matrix group.
    gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
    gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
    gEXPopScissor((*gfx)++);
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

        // @recomp Align the score element's text to the right or use its default alignment.
        bool aligned_to_the_right = a1->unk30 > (gFramebufferWidth * 2 / 3);
        cur_pushed_text_transform_id = HUD_SCORE3_TRANSFORM_PRINT_ID_START + item_id * HUD_SCORE3_TRANSFORM_PRINT_ID_COUNT;
        cur_pushed_text_transform_origin = aligned_to_the_right ? G_EX_ORIGIN_RIGHT : G_EX_ORIGIN_NONE;

        print_bold_spaced(a1->unk30 + a1->unk40, sp40 + a1->unk44, a1->value_string);

        // @recomp Clear the ID and alignment for the text.
        cur_pushed_text_transform_id = 0;
        cur_pushed_text_transform_origin = G_EX_ORIGIN_NONE;

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

        if (a1->anim_ctrl != NULL) {
            anctrl_drawSetup(a1->anim_ctrl, sp5C, 1);
        }

        // @recomp If necessary, align the score element to the right of the screen.
        // NOTE: gScissorBoxRight/gScissorBoxTop are incorrectly named in the decompilation and must be swapped.
        if (aligned_to_the_right) {
            gEXPushScissor((*gfx)++);
            gEXSetScissor((*gfx)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_RIGHT, G_EX_ORIGIN_RIGHT, gScissorBoxLeft - gScissorBoxTop, gScissorBoxRight, 0, gScissorBoxBottom);
            gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_RIGHT, gScissorBoxTop * -4, 0);
        }

        viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);

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

        // @recomp Set the model transform ID.
        cur_drawn_model_transform_id = HUD_SCORE3_TRANSFORM_ID_START + item_id * MARKER_TRANSFORM_ID_COUNT;

        modelRender_draw(gfx, mtx, sp5C, sp68, a1->unk3C * sp3C, sp44, a1->model);

        // @recomp Clear the model transform ID.
        cur_drawn_model_transform_id = 0;

        if (aligned_to_the_right) {
            // @recomp Clear the matrix group.
            gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
            gEXPopScissor((*gfx)++);
        }
    }//L80300BA4
}

// @recomp: Patched to interpolate the header of the totals screen on the pause menu.
RECOMP_PATCH void gcpausemenu_printTotalsHeader(s32 page_id) {
    // @recomp Assign an ID for the totals text.
    cur_pushed_text_transform_id = HUD_TOTALS_PRINT_TRANSFORM_ID_START;

    struct1Cs_1 *v0 = D_8036C58C + page_id;
    print_bold_overlapping(v0->x, D_80383010.unk8, -1.05f, v0->string);

    // @recomp Clear the ID text.
    cur_pushed_text_transform_id = 0;
}