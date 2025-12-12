#include "patches.h"
#include "core1/core1.h"
#include "functions.h"
#include "misc_funcs.h"

BKSpriteTextureBlock font_base_texture;
u8 font_base_texture_data[0x800];

typedef struct{
    s16 x;
    s16 y;
    s16 unk4;
    s16 unk6;
    u8 fmtString[8];
    f32 unk10;
    u8 *string;
    u8 rgba[4];
} PrintBuffer;

typedef struct font_letter{
    BKSpriteTextureBlock *unk0;//chunkPtr
    void *unk4;//palPtr
} FontLetter;

typedef struct map_font_texture_map{
    s16 mapID;
    s16 assetId;
} MapFontTextureMap;

typedef struct{
    u8 unk0;
    u8 unk1;
    s8 unk2;
    s8 unk3;
}Struct_6DA30_0_s;

extern Struct_6DA30_0_s  D_80369000[];
extern s32 D_80369068[];
extern struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
} D_80369078;

extern MapFontTextureMap D_8036907C[];
extern char D_80369200[];
extern Gfx D_80369238[];

extern char D_80380AB0;
extern BKSprite *D_80380AB8[0x5];

extern FontLetter  *print_sFonts[4];
extern PrintBuffer *print_sPrintBuffer;
extern PrintBuffer *print_sCurrentPtr;
extern s32 D_80380AE8;
extern s32 D_80380AEC;
extern s32 D_80380AF0; //print_sMonospaced
extern s32 D_80380AF4;
extern s32 D_80380AF8;
extern s32 D_80380AFC;
extern s32 D_80380B00;
extern s32 D_80380B04;
extern bool print_sInFontFormatMode;
extern s32 D_80380B0C;
extern s32 D_80380B10;
extern s32 D_80380B14;
extern s32 D_80380B18;
extern s32 D_80380B1C;
extern s8 D_80380B20[0x400];
extern s8 D_80380F20[0x80];

BKSpriteTextureBlock *func_802F5494(s32 letterId, s32 *fontType);
void *func_802F55A8(u8 arg0);

#define SPRITE_TYPE_IA16_MASK (1 << 12)

// Creates an IA16 texture that mimics the behavior of the operation done in func_802F4A24.
// The masking operation is moved to the color combiner, allowing texture packs to replace the individual mask textures
// instead of having to replace every single combination of mask and base texture. 
void unpack_rgba32_to_ia16(BKSpriteTextureBlock *alphaMask) {
    u32 *pxl_in;
    u16 *pxl_out;
    s32 x;
    s32 y;
    
    pxl_in = (u32*)(alphaMask + 1);
    pxl_out = (u16*)(pxl_in);

    int width_padding = (-alphaMask->w) & 0x3;

    recomp_printf("Size: %-3d %-3d %-3d %-3d (%d)\n", alphaMask->w, alphaMask->h, alphaMask->x, alphaMask->y, width_padding);

    for (y = 0; y < alphaMask->h; y++) {
        for (x = 0; x < alphaMask->w; x++) {
            s32 b = (*pxl_in >> 8) & 0xFF;
            s32 a = (*pxl_in >> 0) & 0xFF;
            // The original function divides the intensity by 0x1F, so repeat that here and then undo the division to scale back to 8-bit color depth.
            b /= 0x1F;
            b *= 0x1F;
            *pxl_out = (b << 8) | (a << 0);
            pxl_in++;
            pxl_out++;
        }
        for (int i = 0; i < width_padding; i++) {
            *pxl_out = 0;
            pxl_out++;
        }
    }

    alphaMask->w += width_padding;
}

void darken_blue_channel(BKSpriteTextureBlock *alphaMask) {
    u32 *pxl;
    s32 x;
    s32 y;
    
    pxl = (u32*)(alphaMask + 1);

    for (y = 0; y < alphaMask->h; y++) {
        for (x = 0; x < alphaMask->w; x++) {
            s32 masked_data = *pxl & 0xFFFF00FF;
            s32 b = (*pxl >> 8) & 0xFF;
            // The original function divides the intensity by 0x1F, so repeat that here and then undo the division to scale back to 8-bit color depth.
            b /= 0x1F;
            b *= 0x1F;
            
            *pxl = masked_data | (b << 8);
            pxl++;
        }
    }
}

// @recomp Patched to turn RGBA32 alpha masks into IA16 ones instead of mixing the texture sprite into the mask.
// This works because the game only uses the blue and alpha channels (as intensity and alpha respectively).
// The texture sprite will be incorporated during rendering by using the color combiner, allowing fewer texture replacements
// for a texture pack while maintaining an identical appearance.
RECOMP_PATCH FontLetter *func_802F4C3C(BKSprite *alphaMask, BKSprite *textureSprite){
    BKSpriteFrame * font = sprite_getFramePtr(alphaMask, 0);
    BKSpriteTextureBlock *chunkPtr;
    FontLetter * sp2C = malloc((font->chunkCnt + 1)*sizeof(FontLetter));
    u8* palDataPtr;
    u8* chunkDataPtr;
    s32 chunkSize;
    s32 i;
    

    switch(alphaMask->type){
        case SPRITE_TYPE_CI8:
            {//L802F4CA8 
                chunkPtr = (BKSpriteTextureBlock *) (font + 1);
                chunkDataPtr = (u8 *)chunkPtr;
                while((s32)chunkDataPtr % 8)
                    chunkDataPtr++;
                
                palDataPtr = chunkDataPtr;
                chunkPtr = (BKSpriteTextureBlock *) (palDataPtr + 2*0x100);
                
                for(i= 0; i < font->chunkCnt; i++){
                    
                    chunkDataPtr = (u8*)(chunkPtr + 1);
                    while((s32)chunkDataPtr % 8)
                        chunkDataPtr++;

                    sp2C[i].unk0 = chunkPtr;
                    sp2C[i].unk4 = palDataPtr;
                    chunkSize = chunkPtr->w*chunkPtr->h;
                    chunkPtr = (BKSpriteTextureBlock *)(chunkDataPtr + chunkSize);
                }
            }
            break;
        case SPRITE_TYPE_RGBA32://L802F4D80
            {
                // @recomp Replace the alpha mask's format with IA16 to match what it will be unpacked as.
                alphaMask->type = SPRITE_TYPE_IA16_MASK;

                // @recomp Copy the texture sprite's data into the font texture data buffer.
                BKSpriteTextureBlock* base_texture = (BKSpriteTextureBlock *)(sprite_getFramePtr(textureSprite, 0) + 1);
                font_base_texture = *base_texture;
                u16* pixels_in = ((u16*)(base_texture + 1));
                memcpy(font_base_texture_data, pixels_in, base_texture->w * base_texture->h * sizeof(u16));

                chunkPtr = (BKSpriteTextureBlock *)(font + 1);
                for( i = 0; i < font->chunkCnt; i++){
                    // @recomp Remove the texture mixing and darken the blue channel of each pixel to mimic the division done in the original pixel math.
                    darken_blue_channel(chunkPtr);
                    // unpack_rgba32_to_ia16(chunkPtr);
                    
                    sp2C[i].unk0 = chunkPtr;
                    chunkSize = chunkPtr->w*chunkPtr->h;
                    chunkDataPtr = (u8*)(chunkPtr + 1);
                    while((s32)chunkDataPtr % 8)
                        chunkDataPtr++;
                    chunkPtr = (BKSpriteTextureBlock *) (chunkDataPtr + chunkSize*4);
                }
            }
            break;
        case SPRITE_TYPE_I4://L802F4E24
            {
                chunkPtr = (BKSpriteTextureBlock *) (font + 1);
                for( i = 0; i < font->chunkCnt; i++){
                    sp2C[i].unk0 = chunkPtr;
                    chunkDataPtr = (u8*)(chunkPtr + 1);
                    chunkSize = chunkPtr->w*chunkPtr->h;
                    while((s32)chunkDataPtr % 8)
                        chunkDataPtr++;
                    chunkPtr = (BKSpriteTextureBlock *) (chunkDataPtr + chunkSize/2);
                }
            }
            break;
        default://L802F4EC0
            {
                chunkPtr = (BKSpriteTextureBlock *)(font + 1);
                for( i = 0; i < font->chunkCnt; i++){
                    chunkDataPtr = (u8*)(chunkPtr + 1);
                    sp2C[i].unk0 = chunkPtr;
                    chunkSize = chunkPtr->w*chunkPtr->h;
                    while((s32)chunkDataPtr % 8)
                        chunkDataPtr++;
                    chunkPtr = (BKSpriteTextureBlock *)(chunkDataPtr + chunkSize);
                }
            }
            break;
    };
    return sp2C;
}

// @recomp Patched to add the ability to load IA16 masks.
RECOMP_PATCH void _printbuffer_draw_letter(char letter, f32* xPtr, f32* yPtr, f32 arg3, Gfx **gfx, Mtx **mtx, Vtx **vtx){
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

    if(!D_80380B04 && !letter){
        D_80380FA0 = 0.0f;
    }//L802F563C

    switch(D_80380AE8){
        case 0: //L802F5678
            if(letter >= '\x21' && letter < '\x5f'){
                sp20C = letter - '\x21';
                t0 = 1;
            }
            break;
        case 1: //L802F56A0
            if(letter < '\x80' && D_80380F20[(unsigned char)letter] >= 0){
                for(i = 0; D_80369000[i].unk0 != 0; i++){
                    if(letter == D_80369000[i].unk1 && D_80380AB0 == D_80369000[i].unk0){
                        t1 = D_80369000[i].unk3;
                        break;
                    }
                }//L802F5710
                sp20C = D_80380F20[(unsigned char)letter];
                t0 = 1;
                D_80380AB0 = letter;
                f28 += (f32)t1*arg3;
            }//L802F5738
            break;
        case 2: //L802F5740
            sp20C = letter;
            if(D_80380B04){
                t0 = 1;
                sp20C += (D_80380B04 << 8) - 0x100;
                D_80380B04 = 0;
            }
            else{//L802F5764
                if(sp20C > 0 && sp20C < 0xfD)
                    t0 = 1;
            }
            break;
    }//L802F5778

    if(!t0 || print_sInFontFormatMode){
        print_sInFontFormatMode = FALSE;
        switch(letter){
            case ' '://802F5818
                *xPtr += ((D_80380AF0) ? D_80369068[D_80380AE8]: D_80369068[D_80380AE8]*0.8) * arg3;
                break;

            case 'b': //L802F5890
                //toggle background
                D_80380B00  = D_80380B00 ^ 1;
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
                if(D_80380AFC == 0){
                    D_80380AFC = 1;
                    D_80380AEC = D_80380AE8;
                    D_80380AE8 = 2;
                    // D_80380AE8 = 2;
                }
                break;

            case 'e': //L802F58FC
                if(D_80380AFC){
                    D_80380AFC = 0;
                    D_80380AE8 = D_80380AEC;
                }
                break;

            case 'p': //L802F5924
                D_80380AF0 = D_80380AF0 ^1;
                break;

            case 'q': //L802F593C
                D_80380B14 = D_80380B14^1;
                if(D_80380B14){
                    gDPSetTextureFilter((*gfx)++, G_TF_POINT);
                }
                else{//L802F5978
                    gDPSetTextureFilter((*gfx)++, G_TF_BILERP);
                }
                break;

            case 'v': //L802F59A0 
                //toggle letter gradient
                D_80380AF4 ^= 1;
                if(D_80380AF4){
                    viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
                    gDPPipeSync((*gfx)++);
                    gDPSetTexturePersp((*gfx)++, G_TP_PERSP);
                    gDPSetPrimColor((*gfx)++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
                    gDPSetCombineLERP((*gfx)++, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0);
                }
                else{//L802F5A44
                    gDPSetCombineMode((*gfx)++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                    gDPSetTexturePersp((*gfx)++, G_TP_NONE);
                }
                break;

            case 'd': //L802F5A8C
                D_80380AF8 ^= 1;
                if(D_80380AF8){
                    gDPPipeSync((*gfx)++);
                    gDPSetCycleType((*gfx)++, G_CYC_2CYCLE);
                    gDPSetRenderMode((*gfx)++, G_RM_PASS, G_RM_XLU_SURF2);
                    gDPSetTextureLOD((*gfx)++, G_TL_TILE);
                    gDPSetCombineLERP((*gfx)++, 0, 0, 0, TEXEL0, TEXEL0, 0, TEXEL1, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED);
                }
                else{//L802F5B48
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
    else{//L802F5C08
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
        
        // @recomp Track the texture scroll offset. Only nonzero if the font is an IA16 mask.
        s32 scroll_x = 0;
        s32 scroll_y = 0;
        
        sp200 += (D_80380FA0 + (sp1F8 - sp214->x) * 0.5);
        f28 -= sp214->h*0.5;
        sp210 = (s32)(sp214 + 1);
        while(sp210 % 8){
            sp210++;
        }
        if (sp1F4 == SPRITE_TYPE_RGBA32) { 
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_RGBA, G_IM_SIZ_32b, sp214->w, sp214->h, 0, 0, sp214->x-1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        } else if (sp1F4 == SPRITE_TYPE_IA8) {
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_IA, G_IM_SIZ_8b, sp214->w, sp214->h, 0, 0, sp214->x-1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        } else if (sp1F4 == SPRITE_TYPE_I8) {
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_I, G_IM_SIZ_8b, sp214->w, sp214->h, 0, 0, sp214->x-1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        } else if (sp1F4 == SPRITE_TYPE_I4) {
            gDPLoadTextureTile_4b((*gfx)++, sp210, G_IM_FMT_I, sp214->w, sp214->h, 0, 0, sp214->x-1, sp214->y-1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        } else if (sp1F4 == SPRITE_TYPE_CI8) {
            void * pal = func_802F55A8(sp20C);
            gDPLoadTLUT_pal256((*gfx)++, pal);
            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_CI, G_IM_SIZ_8b, sp214->w, sp214->h, 0, 0, sp214->x-1, sp214->y-1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gDPSetTextureLUT((*gfx)++, G_TT_RGBA16);
        }//L802F6570
        // @recomp Add a case for handling the new IA16 mask sprite type.
        else if (sp1F4 == SPRITE_TYPE_IA16_MASK) {
            if (D_80380AF8) {
                recomp_error("Tried to use IA16 mask font and two texture font at the same time");
            }

            scroll_x = ((font_base_texture.w - sp214->w) >> 1);
            scroll_y = ((font_base_texture.h - sp214->h) >> 1);

            gDPLoadTextureTile((*gfx)++, sp210, G_IM_FMT_RGBA, G_IM_SIZ_32b, sp214->w, sp214->h, 0, 0, sp214->x-1, sp214->y - 1, NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gDPSetTile((*gfx)++, G_IM_FMT_IA, G_IM_SIZ_16b, (((((sp214->x - 1)-(0)+1) * G_IM_SIZ_16b_TILE_BYTES)+7)>>3), 0x100, 0, 0, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD);
            gDPLoadMultiTile((*gfx)++, font_base_texture_data, 0x00, 1, G_IM_FMT_RGBA, G_IM_SIZ_16b,
                font_base_texture.w, font_base_texture.h,
                0, 0,
                font_base_texture.w - 1, font_base_texture.h - 1,
                0,
                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOLOD);
            gEXPushOtherMode((*gfx)++);
            gEXPushCombineMode((*gfx)++);
            gDPSetCycleType((*gfx)++, G_CYC_2CYCLE);
            gDPSetRenderMode((*gfx)++, G_RM_PASS, G_RM_XLU_SURF2);
            gDPSetTextureLOD((*gfx)++, G_TL_TILE);
            gDPSetCombineLERP((*gfx)++, TEXEL0, 0, TEXEL1, 0,  0, 0, 0, TEXEL0,  0, 0, 0, COMBINED,  0, 0, 0, COMBINED);
            
            gDPSetTileSize((*gfx)++, 0, scroll_x * 4, scroll_y * 4, (scroll_x + sp214->x - 1) * 4, (scroll_y + sp214->y - 1) * 4);
        }
        if (D_80380AF8 != 0) {
            s32 temp_t1;
            s32 phi_a0;
            temp_t1 = ((print_sCurrentPtr->unk4 - print_sCurrentPtr->y) - D_80380B0C) + 1;
            phi_a0 =  - MAX(1 - D_80380B0C, MIN(0, temp_t1));
            
            gDPSetTextureImage((*gfx)++, G_IM_FMT_I, G_IM_SIZ_8b, 32, &D_80380B20);
            gDPSetTile((*gfx)++, G_IM_FMT_I, G_IM_SIZ_8b, (sp214->x + 8) >> 3, 0x0100, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD);
            gDPLoadSync((*gfx)++);
            gDPLoadTile((*gfx)++, G_TX_LOADTILE, 0 << G_TEXTURE_IMAGE_FRAC, (phi_a0) << G_TEXTURE_IMAGE_FRAC, (sp214->x) << G_TEXTURE_IMAGE_FRAC, (D_80380B0C - 1) << G_TEXTURE_IMAGE_FRAC);
            gDPPipeSync((*gfx)++);
            gDPSetTile((*gfx)++, G_IM_FMT_I, G_IM_SIZ_8b, ((sp214->x - 0 + 1)*G_IM_SIZ_8b_LINE_BYTES + 7) >> 3, 0x0100, 1, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD);
            gDPSetTileSize((*gfx)++, 1, 0 << G_TEXTURE_IMAGE_FRAC, (MAX(0, temp_t1) + phi_a0) << G_TEXTURE_IMAGE_FRAC, (sp214->x) << G_TEXTURE_IMAGE_FRAC, (MAX(0, temp_t1) - (1 - D_80380B0C))<<G_TEXTURE_IMAGE_FRAC);
            
            // gDPLoadMultiTile((*gfx)++, &D_80380B20,)
            
        }//L802F677C
        if (D_80380AF4 != 0) {
            f32 temp_f24;
            f32 spD0;
            f32 ix;
            f32 iy;
            f32 temp_f26;
            f32 spC0;

            temp_f24 = (sp214->x - 1.0);
            spD0 = sp214->y - 1.0;
            temp_f26 = (f64) sp200 - (f32) gFramebufferWidth * 0.5;
            spC0 = (f64)f28 - (f32)gFramebufferHeight*0.5 -0.5f;
            gSPVertex((*gfx)++, *vtx, 4, 0);
            for(iy = 0.0f; iy < 2.0; iy+= 1.0){
                for(ix = 0.0f; ix < 2.0; ix += 1.0){
                    s32 s = (ix * temp_f24 * 64.0f);
                    // @recomp Add the texture scroll.
                    s += scroll_x * 64;
                    (*vtx)->v.ob[0] = (s16)(s32)((f64) (temp_f26 + (temp_f24 *  arg3  * ix)) * 4.0);
                    {
                        s32 t = (iy * spD0 * 64.0f);
                        // @recomp Add the texture scroll.
                        t += scroll_y * 64;
                        (*vtx)->v.ob[1] = (s16) (s32) ((f64) (spC0 + (spD0 * arg3 * iy)) * -4.0);
                        (*vtx)->v.ob[2] = -0x14;
                        (*vtx)->v.tc[0] = s;
                        (*vtx)->v.tc[1] = t;
                    }
                    (*vtx)->v.cn[3] =(iy != 0.0f) ? print_sCurrentPtr->unk6 : print_sCurrentPtr->unk4;
                    
                    (*vtx)++;
                }
            }
            
            gSP1Quadrangle((*gfx)++, 0, 1, 3, 2, 0);
        }
        else{
            gSPScisTextureRectangle((*gfx)++, (s32)(sp200*4.0f), (s32)(f28*4.0f), (s32)((sp200 + sp214->x*arg3)*4.0f), (s32)((f28 + sp214->y*arg3)*4.0f), 0, scroll_x * 32, scroll_y * 32, (s32)(1024.0f / arg3), (s32)(1024.0f / arg3));
        }
        // @recomp Reset the othermode after drawing the character.
        if (sp1F4 == SPRITE_TYPE_IA16_MASK) {
            gEXPopOtherMode((*gfx)++);
            gEXPopCombineMode((*gfx)++);
        }
        *xPtr += sp1F8 * arg3;
    }
}

