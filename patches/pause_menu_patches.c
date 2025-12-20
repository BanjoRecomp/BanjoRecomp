#include "patches.h"
#include "core1/core1.h"

extern Gfx D_8036C630[];
extern Gfx D_8036C690[];

// @recomp Patched to fix artifacts in the pause menu background when using wide aspect ratios. Rewritten to draw rows instead of tiles to avoid issues when upscaling the effect.
RECOMP_PATCH void func_80314BB0(Gfx **gfx, Mtx **mtx, Vtx **vtx, void *frame_buffer_1, void *frame_buffer_2) {
    s32 x;
    s32 y;

    gSPDisplayList((*gfx)++, D_8036C630);
    gDPSetColorImage((*gfx)++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gFramebufferWidth, OS_PHYSICAL_TO_K0(frame_buffer_1));

#if 1
    // @recomp New implementation that uses rows instead of tiles.
    for (y = 0; y < gFramebufferHeight / 7 + 1; y++) {
        gDPLoadTextureTile((*gfx)++, osVirtualToPhysical(frame_buffer_2), G_IM_FMT_RGBA, G_IM_SIZ_16b, gFramebufferWidth, gFramebufferHeight,
            0, y * 7, gFramebufferWidth - 1, MIN((y + 1) * 7, gFramebufferHeight) - 1,
            NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, 0, 0
        );
        gSPScisTextureRectangle((*gfx)++, 0, (y * 7) * 4, gFramebufferWidth * 4, MIN((y + 1) * 7, gFramebufferHeight) * 4,
            G_TX_RENDERTILE, 0, (y * 7) << 5, 0x400, 0x400
        );
    }
#else
    for (y = 0; y < gFramebufferHeight / 32 + 1; y++) {
        for (x = 0; x < gFramebufferWidth / 32 + 1; x++) {
            gDPLoadTextureTile((*gfx)++, osVirtualToPhysical(frame_buffer_2), G_IM_FMT_RGBA, G_IM_SIZ_16b, gFramebufferWidth, gFramebufferHeight,
                0x20 * x, 0x20 * y, 0x20 * (x + 1) - 1, 0x20 * (y + 1) - 1,
                NULL, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, 0, 0
            );
            gSPScisTextureRectangle((*gfx)++, (0x20 * x) * 4, (0x20 * y) * 4, 0x20 * (x + 1) * 4, (0x20 * (y + 1) * 4),
                G_TX_RENDERTILE, (0x20 * x) << 5, (0x20 * y) << 5, 0x400, 0x400
            );
        }
    }
#endif

    gSPDisplayList((*gfx)++, D_8036C690);
    gDPSetColorImage((*gfx)++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gFramebufferWidth, OS_PHYSICAL_TO_K0(gFramebuffers[getActiveFramebuffer()]));
}
