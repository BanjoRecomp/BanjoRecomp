#include "patches.h"
#include "transform_ids.h"
#include "functions.h"

typedef struct {
    u8 unk0;
    u8 unk1;
    s8 unk2;
    s8 unk3;
}Struct_6DA30_0_s;

typedef struct {
    s16 x;
    s16 y;
    s16 unk4;
    s16 unk6;
    u8 fmtString[8];
    f32 unk10;
    u8 *string;
    u8 rgba[4];
} PrintBuffer;

extern char D_80380AB0;
extern s8 D_80380F20[0x80];
extern u16 gScissorBoxLeft;
extern u16 gScissorBoxRight;
extern u16 gScissorBoxTop;
extern u16 gScissorBoxBottom;
extern s32 D_80369068[];
extern s32 D_80380B04;
extern s32 D_80380AE8;
extern s32 D_80380AEC;
extern s32 D_80380AF0;
extern s32 D_80380AF4;
extern s32 D_80380AF8;
extern s32 D_80380AFC;
extern s32 D_80380B00;
extern s32 D_80380B0C;
extern s32 D_80380B10;
extern s32 D_80380B14;
extern s8 D_80380B20[0x400];
extern bool print_sInFontFormatMode;
extern Struct_6DA30_0_s D_80369000[];
extern PrintBuffer *print_sPrintBuffer;
extern PrintBuffer *print_sCurrentPtr;
extern Gfx D_80369238[];

extern BKSpriteTextureBlock *func_802F5494(s32 letterId, s32 *fontType);
extern void *func_802F55A8(u8 arg0);
extern f32 func_802F6C90(u8 letter, f32 *xPtr, f32 *yPtr, f32 arg3);

extern struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
} D_80369078;

// @recomp This ID plus the index of the character in the string will be used to tag each letter if it's assigned before printing text.
u32 cur_pushed_text_transform_id = 0;
u32 cur_pushed_text_transform_origin = G_EX_ORIGIN_NONE;
u32 cur_pushed_text_transform_skip_interpolation = FALSE;
u32 cur_drawn_text_transform_id = 0;
u32 cur_drawn_text_transform_skip_interpolation = 0;
Mtx *cur_drawn_text_mtx = NULL;
u32 print_sPrintBufferIDs[0x20];
u32 print_sPrintBufferOrigins[0x20];
u32 print_sPrintBufferSkipInterpolationFlags[0x20];

// @recomp Patched to assign the current ID and origin from the global variables to the print buffer array.
RECOMP_PATCH void _printbuffer_push_new(s32 x, s32 y, u8 *string) {
    for (print_sCurrentPtr = print_sPrintBuffer; print_sCurrentPtr < print_sPrintBuffer + 0x20 && print_sCurrentPtr->string; print_sCurrentPtr++) {
    }
    if (print_sCurrentPtr == print_sPrintBuffer + 0x20) {
        print_sCurrentPtr = NULL;
        return;
    }
    print_sCurrentPtr->x = x;
    print_sCurrentPtr->y = y;
    print_sCurrentPtr->fmtString[0] = (u8)0;
    print_sCurrentPtr->string = string;
    print_sCurrentPtr->unk10 = 1.0f;
    print_sCurrentPtr->rgba[0] = (u8)D_80369078.unk0;
    print_sCurrentPtr->rgba[1] = (u8)D_80369078.unk1;
    print_sCurrentPtr->rgba[2] = (u8)D_80369078.unk2;
    print_sCurrentPtr->rgba[3] = (u8)D_80369078.unk3;

    // @recomp Store the current ID and origin in the arrays that run parallel to the print buffer.
    u32 bufferIndex = print_sCurrentPtr - print_sPrintBuffer;
    print_sPrintBufferIDs[bufferIndex] = cur_pushed_text_transform_id;
    print_sPrintBufferOrigins[bufferIndex] = cur_pushed_text_transform_origin;
    print_sPrintBufferSkipInterpolationFlags[bufferIndex] = cur_pushed_text_transform_skip_interpolation;
}

// @recomp Patched to force orthographic projections to be used for all letters.
RECOMP_PATCH void _printbuffer_draw_letter(char letter, f32 *xPtr, f32 *yPtr, f32 arg3, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    static f32 D_80380FA0;

    // u8 letter = arg0;
    BKSpriteTextureBlock *sp214;
    s32 sp210;
    s32 sp20C;
    s32 t0;
    s8 t1;
    f32 sp200;
    f32 f28;
    f32 sp1F8;
    s32 sp1F4; //font_type;

    int i;



    t0 = 0;
    sp200 = *xPtr;
    f28 = *yPtr;
    t1 = 0;

    if (!D_80380B04 && !letter) {
        D_80380FA0 = 0.0f;
    }//L802F563C

    switch (D_80380AE8) {
    case 0: //L802F5678
        if (letter >= '\x21' && letter < '\x5f') {
            sp20C = letter - '\x21';
            t0 = 1;
        }
        break;
    case 1: //L802F56A0
        if (letter < '\x80' && D_80380F20[letter] >= 0) {
            for (i = 0; D_80369000[i].unk0 != 0; i++) {
                if (letter == D_80369000[i].unk1 && D_80380AB0 == D_80369000[i].unk0) {
                    t1 = D_80369000[i].unk3;
                    break;
                }
            }//L802F5710
            sp20C = D_80380F20[letter];
            t0 = 1;
            D_80380AB0 = letter;
            f28 += (f32)t1 * arg3;
        }//L802F5738
        break;
    case 2: //L802F5740
        sp20C = letter;
        if (D_80380B04) {
            t0 = 1;
            sp20C += (D_80380B04 << 8) - 0x100;
            D_80380B04 = 0;
        }
        else {//L802F5764
            if (sp20C > 0 && sp20C < 0xfD)
                t0 = 1;
        }
        break;
    }//L802F5778

    if (!t0 || print_sInFontFormatMode) {
        print_sInFontFormatMode = FALSE;
        switch (letter) {
        case ' '://802F5818
            *xPtr += ((D_80380AF0) ? D_80369068[D_80380AE8] : D_80369068[D_80380AE8] * 0.8) * arg3;
            break;

        case 'b': //L802F5890
            //toggle background
            D_80380B00 = D_80380B00 ^ 1;
            break;

        case 'f': //L802F58A8
            D_80380AEC = D_80380AE8 = D_80380AE8 ^ 1;
            break;

        case 'l': //L802F58BC
            D_80380B10 = 0;
            break;

        case 'h': //L802F58C8
            D_80380B10 = 1;
            break;

        case 'j': //L802F58D4
            if (D_80380AFC == 0) {
                D_80380AFC = 1;
                D_80380AEC = D_80380AE8;
                D_80380AE8 = 2;
                // D_80380AE8 = 2;
            }
            break;

        case 'e': //L802F58FC
            if (D_80380AFC) {
                D_80380AFC = 0;
                D_80380AE8 = D_80380AEC;
            }
            break;

        case 'p': //L802F5924
            D_80380AF0 = D_80380AF0 ^ 1;
            break;

        case 'q': //L802F593C
            D_80380B14 = D_80380B14 ^ 1;
            if (D_80380B14) {
                gDPSetTextureFilter((*gfx)++, G_TF_POINT);
            }
            else {//L802F5978
                gDPSetTextureFilter((*gfx)++, G_TF_BILERP);
            }
            break;

        case 'v': //L802F59A0 
            //toggle letter gradient
            D_80380AF4 ^= 1;
            if (D_80380AF4) {
                // @recomp Comment these lines out, as the viewport is now set in the parent function.
                //viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
                //gDPPipeSync((*gfx)++);
                //gDPSetTexturePersp((*gfx)++, G_TP_PERSP);
                gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
                gDPSetCombineLERP((*gfx)++, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0);
            }
            else {//L802F5A44
                gDPSetCombineMode((*gfx)++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                // @recomp Comment this line out, as texture rectangle letters are no longer possible.
                //gDPSetTexturePersp((*gfx)++, G_TP_NONE);
            }
            break;

        case 'd': //L802F5A8C
            D_80380AF8 ^= 1;
            if (D_80380AF8) {
                gDPPipeSync((*gfx)++);
                gDPSetCycleType((*gfx)++, G_CYC_2CYCLE);
                gDPSetRenderMode((*gfx)++, G_RM_PASS, G_RM_XLU_SURF2);
                gDPSetTextureLOD((*gfx)++, G_TL_TILE);
                gDPSetCombineLERP((*gfx)++, 0, 0, 0, TEXEL0, TEXEL0, 0, TEXEL1, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED);
            }
            else {//L802F5B48
                gDPPipeSync((*gfx)++);
                gDPSetCombineMode((*gfx)++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                gDPSetCycleType((*gfx)++, G_CYC_1CYCLE);
                gDPSetTextureLOD((*gfx)++, G_TL_LOD);
                gDPSetRenderMode((*gfx)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
            }
            break;

        case 0xfd: //L802F5BEC
            print_sInFontFormatMode = TRUE;
            break;

        case 0xfe://L802F5BF4
            D_80380B04 = 1;
            break;

        case 0xff://L802F5BFC
            D_80380B04 = 2;
            break;
        default:
            break;
        }
    }
    else {//L802F5C08
        sp214 = func_802F5494(sp20C, &sp1F4);
        if (D_80380B10 != 0) {
            sp200 += randf2(-2.0f, 2.0f);
            f28 += randf2(-2.0f, 2.0f);
        }
        sp1F8 = (D_80380AF0 != 0) ? D_80369068[D_80380AE8] : sp214->x;

        // temp_f2 = D_80380FA0;
        // phi_f2 = temp_f2;
        if (D_80380FA0 == 0.0f) {
            D_80380FA0 = -sp1F8 * 0.5;
        }

        sp200 += (D_80380FA0 + (sp1F8 - sp214->x) * 0.5);
        f28 -= sp214->h * 0.5;
        sp210 = (s32)(sp214 + 1);
        while (sp210 % 8) {
            sp210++;
        }
        if (sp1F4 == SPRITE_TYPE_RGBA32) {
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_RGBA, G_IM_SIZ_32b, sp214->w, sp214->h, 0, 0, sp214->x - 1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        }
        else if (sp1F4 == SPRITE_TYPE_IA8) {
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_IA, G_IM_SIZ_8b, sp214->w, sp214->h, 0, 0, sp214->x - 1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        }
        else if (sp1F4 == SPRITE_TYPE_I8) {
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_I, G_IM_SIZ_8b, sp214->w, sp214->h, 0, 0, sp214->x - 1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        }
        else if (sp1F4 == SPRITE_TYPE_I4) {
            gDPLoadTextureTile_4b((*gfx)++, sp210, G_IM_FMT_I, sp214->w, sp214->h, 0, 0, sp214->x - 1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        }
        else if (sp1F4 == SPRITE_TYPE_CI8) {
            void *pal = func_802F55A8(sp20C);
            gDPLoadTLUT_pal256((*gfx)++, pal);
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_CI, G_IM_SIZ_8b, sp214->w, sp214->h, 0, 0, sp214->x - 1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gDPSetTextureLUT((*gfx)++, G_TT_RGBA16);
        }//L802F6570
        if (D_80380AF8 != 0) {
            s32 temp_t1;
            s32 phi_a0;
            temp_t1 = ((print_sCurrentPtr->unk4 - print_sCurrentPtr->y) - D_80380B0C) + 1;
            phi_a0 = -MAX(1 - D_80380B0C, MIN(0, temp_t1));

            gDPSetTextureImage((*gfx)++, G_IM_FMT_I, G_IM_SIZ_8b, 32, &D_80380B20);
            gDPSetTile((*gfx)++, G_IM_FMT_I, G_IM_SIZ_8b, (sp214->x + 8) >> 3, 0x0100, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD);
            gDPLoadSync((*gfx)++);
            gDPLoadTile((*gfx)++, G_TX_LOADTILE, 0 << G_TEXTURE_IMAGE_FRAC, (phi_a0) << G_TEXTURE_IMAGE_FRAC, (sp214->x) << G_TEXTURE_IMAGE_FRAC, (D_80380B0C - 1) << G_TEXTURE_IMAGE_FRAC);
            gDPPipeSync((*gfx)++);
            gDPSetTile((*gfx)++, G_IM_FMT_I, G_IM_SIZ_8b, ((sp214->x - 0 + 1) * G_IM_SIZ_8b_LINE_BYTES + 7) >> 3, 0x0100, 1, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD);
            gDPSetTileSize((*gfx)++, 1, 0 << G_TEXTURE_IMAGE_FRAC, (MAX(0, temp_t1) + phi_a0) << G_TEXTURE_IMAGE_FRAC, (sp214->x) << G_TEXTURE_IMAGE_FRAC, (MAX(0, temp_t1) - (1 - D_80380B0C)) << G_TEXTURE_IMAGE_FRAC);

            // gDPLoadMultiTile((*gfx)++, &D_80380B20,)

        }//L802F677C
        
        // @recomp Force orthographic projection rectangles to be used for all letters as it allows them to be interpolated.
        if (TRUE) { //if (D_80380AF4 != 0) {
            f32 temp_f24;
            f32 spD0;
            f32 ix;
            f32 iy;
            f32 temp_f26;
            f32 spC0;

            temp_f24 = (sp214->x - 1.0);
            spD0 = sp214->y - 1.0;
            temp_f26 = (f64)sp200 - (f32)gFramebufferWidth * 0.5;
            spC0 = (f64)f28 - (f32)gFramebufferHeight * 0.5 - 0.5f;

            // @recomp Assign a unique matrix and group to each letter drawn if an ID is currently assigned.
            if (cur_drawn_text_transform_id != 0) {
                gSPMatrix((*gfx)++, OS_K0_TO_PHYSICAL(cur_drawn_text_mtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                if (cur_drawn_text_transform_skip_interpolation) {
                    gEXMatrixGroupSkipAll((*gfx)++, cur_drawn_text_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
                }
                else {
                    gEXMatrixGroupSimpleVerts((*gfx)++, cur_drawn_text_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
                }

                cur_drawn_text_transform_id++;
            }

            gSPVertex((*gfx)++, *vtx, 4, 0);
            for (iy = 0.0f; iy < 2.0; iy += 1.0) {
                for (ix = 0.0f; ix < 2.0; ix += 1.0) {
                    s32 s = (ix * temp_f24 * 64.0f);
                    (*vtx)->v.ob[0] = (s16)(s32)((f64)(temp_f26 + (temp_f24 * arg3 * ix)) * 4.0);
                    {
                        s32 t = (iy * spD0 * 64.0f);
                        (*vtx)->v.ob[1] = (s16)(s32)((f64)(spC0 + (spD0 * arg3 * iy)) * -4.0);
                        (*vtx)->v.ob[2] = -0x14;
                        (*vtx)->v.tc[0] = s;
                        (*vtx)->v.tc[1] = t;
                    }
                    (*vtx)->v.cn[3] = (iy != 0.0f) ? print_sCurrentPtr->unk6 : print_sCurrentPtr->unk4;

                    (*vtx)++;
                }
            }

            gSP1Quadrangle((*gfx)++, 0, 1, 3, 2, 0);

            // @recomp Clear the matrix group.
            if (cur_drawn_text_transform_id != 0) {
                gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
            }
        }
        else {
            gSPScisTextureRectangle((*gfx)++, (s32)(sp200 * 4.0f), (s32)(f28 * 4.0f), (s32)((sp200 + sp214->x * arg3) * 4.0f), (s32)((f28 + sp214->y * arg3) * 4.0f), 0, 0, 0, (s32)(1024.0f / arg3), (s32)(1024.0f / arg3));
        }
        *xPtr += sp1F8 * arg3;
    }
}

// @recomp Patched to set up an orthographic projection for each print and align them to their corresponding side on the screen.
RECOMP_PATCH void printbuffer_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    static f32 D_80380FA8[0x20];

    s32 j;
    f32 _x;
    f32 _y;
    f32 width;

    gSPDisplayList((*gfx)++, D_80369238);

    // @recomp Set up a scissor that covers the entire screen.
    gEXPushScissor((*gfx)++);
    gEXSetScissor((*gfx)++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, gScissorBoxRight, 0, gScissorBoxBottom);

    // @recomp Set up an orthographic projection that will be used for all letters by default.
    u32 curOrigin = G_EX_ORIGIN_NONE;
    gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
    gEXSetRectAlign((*gfx)++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
    cur_drawn_text_mtx = (*mtx - 1);

    // @recomp Set up some drawing parameters that were set up when changing between rectangle and orthographic mode.
    gDPPipeSync((*gfx)++);
    gDPSetTexturePersp((*gfx)++, G_TP_PERSP);
    gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    gDPSetCombineLERP((*gfx)++, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0);

    for (print_sCurrentPtr = print_sPrintBuffer; print_sCurrentPtr < print_sPrintBuffer + 0x20; print_sCurrentPtr++) {
        if (print_sCurrentPtr->string != 0) {
            // @recomp When the origin changes, set up a new viewport with the alignment specified for the print buffer.
            u32 bufferIndex = print_sCurrentPtr - print_sPrintBuffer;
            cur_drawn_text_transform_id = print_sPrintBufferIDs[bufferIndex];
            cur_drawn_text_transform_skip_interpolation = print_sPrintBufferSkipInterpolationFlags[bufferIndex];
            if (curOrigin != print_sPrintBufferOrigins[bufferIndex]) {
                curOrigin = print_sPrintBufferOrigins[bufferIndex];
                s32 viewportOffset = (gScissorBoxTop * curOrigin * -4) / G_EX_ORIGIN_RIGHT;
                gEXSetViewportAlign((*gfx)++, curOrigin, viewportOffset, 0);
                gEXSetRectAlign((*gfx)++, curOrigin, curOrigin, viewportOffset, 0, viewportOffset, 0);
                viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
            }

            _x = (f32)print_sCurrentPtr->x;
            _y = (f32)print_sCurrentPtr->y;
            //toggle on string format modifiers
            for (j = 0; print_sCurrentPtr->fmtString[j] != 0; j++) {
                _printbuffer_draw_letter(0xFD, &_x, &_y, 1.0f, gfx, mtx, vtx);
                _printbuffer_draw_letter(print_sCurrentPtr->fmtString[j], &_x, &_y, 1.0f, gfx, mtx, vtx);
            }
            if (D_80380B00 != 0) {
                width = (strlen(print_sCurrentPtr->string) - 1) * D_80369068[D_80380AE8];
                gDPPipeSync((*gfx)++);
                gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0x64);
                gDPSetCombineMode((*gfx)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                gDPScisFillRectangle((*gfx)++, _x - D_80369068[D_80380AE8] / 2 - 1.0f, _y - D_80369068[D_80380AE8] / 2 - 1.0f, _x + width + D_80369068[D_80380AE8] / 2, _y + D_80369068[D_80380AE8] / 2 + 1.0f);
                gDPPipeSync((*gfx)++);

            }//L802F73E8
            // @recomp Ignore the condition for not setting up these parameters while on orthographic mode.
            if (D_80380AF8 == 0) { //if ((D_80380AF8 == 0) && (D_80380AF4 == 0)) {
                if (D_80380AE8 != 0) {
                    gDPSetCombineMode((*gfx)++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                    gDPSetPrimColor((*gfx)++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                }
                else {
                    gDPSetCombineMode((*gfx)++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor((*gfx)++, 0, 0, print_sCurrentPtr->rgba[0], print_sCurrentPtr->rgba[1], print_sCurrentPtr->rgba[2], print_sCurrentPtr->rgba[3]);
                }
            }
            if ((D_80380AE8 == 1) && ((f64)print_sCurrentPtr->unk10 < 0.0)) {
                for (j = 0; print_sCurrentPtr->string[j]; j++) {
                    D_80380FA8[j] = func_802F6C90(print_sCurrentPtr->string[j], &_x, &_y, -print_sCurrentPtr->unk10);
                }
                while (j >= 0) {
                    _x = D_80380FA8[j];
                    _printbuffer_draw_letter(print_sCurrentPtr->string[j], &_x, &_y, -print_sCurrentPtr->unk10, gfx, mtx, vtx);
                    j--;
                }
            }
            else {
                for (j = 0; (print_sCurrentPtr->string[j] != 0) || (D_80380B04 != 0); j++) {
                    _printbuffer_draw_letter(print_sCurrentPtr->string[j], &_x, &_y, print_sCurrentPtr->unk10, gfx, mtx, vtx);
                }
            }
            //toggle off string format modifiers
            for (j = 0; print_sCurrentPtr->fmtString[j] != 0; j++) {
                _printbuffer_draw_letter(0xFD, &_x, &_y, 1.0f, gfx, mtx, vtx);
                _printbuffer_draw_letter(print_sCurrentPtr->fmtString[j], &_x, &_y, 1.0f, gfx, mtx, vtx);
            }
            _printbuffer_draw_letter(0, &_x, &_y, 1.0f, gfx, mtx, vtx);
            print_sCurrentPtr->string = NULL;
        }
    }

    // @recomp Clear the alignment and the scissor. Also clear the assigned transform group for the letters.
    gEXSetViewportAlign((*gfx)++, G_EX_ORIGIN_NONE, 0, 0);
    gEXSetRectAlign((*gfx)++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    gEXPopScissor((*gfx)++);
    cur_drawn_text_transform_id = 0;

    gDPPipeSync((*gfx)++);
    gDPSetTexturePersp((*gfx)++, G_TP_PERSP);
    gDPSetTextureFilter((*gfx)++, G_TF_BILERP);
    viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
}
