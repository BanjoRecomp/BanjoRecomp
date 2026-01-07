#include "patches.h"
#include "ultra_extensions.h"
#include "misc_funcs.h"
#include "note_saving.h"
#include "core1/core1.h"
#include "core1/vimgr.h"

extern struct{
    s32 unk0;
    s32 game_mode; //game_mode
    f32 unk8; 
    s32 unkC; //freeze_scene_flag (used for pause menu)
    f32 unk10;
    u8 transition;
    u8 map;
    u8 exit;
    u8 unk17; //reset_on_map_load
    u8 unk18;
    u8 unk19;
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
} D_8037E8E0;

void func_802E39D0(Gfx **gdl, Mtx **mptr, Vtx **vptr, s32 framebuffer_idx, s32 arg4);

// 16x the original sizes.
#define GFX_BUFFER_COUNT 37000
#define MTX_BUFFER_COUNT 7000
#define VTX_BUFFER_COUNT 4300

struct {
    Gfx gfx[2][GFX_BUFFER_COUNT];
    Mtx mtx[2][MTX_BUFFER_COUNT];
    Vtx vtx[2][VTX_BUFFER_COUNT];
} ExtendedBuffers;

extern Gfx *sGfxStack[2];
extern Mtx *sMtxStack[2];
extern Vtx *sVtxStack[2];
extern s32 sStackSelector;
extern s32 gTextureFilterPoint;
extern u32 heap_occupiedBytes;
extern u32 dynamic_camera_target_index;

extern void recomp_reset_skinning_stack();
extern void recomp_reset_map_model_skinning();
extern void recomp_advance_dynamic_camera_targets();

// @recomp Patched to not free anything.
RECOMP_PATCH void graphicsCache_release(void) {
    if (sGfxStack[0]) {
        // @recomp Nothing to free, as the extended buffers are statically allocated.
        sGfxStack[0] = NULL;
    }
}

// @recomp Patched to use the extended buffers for the graphics stacks.
RECOMP_PATCH void graphicsCache_init(void){
    if(sGfxStack[0] == NULL){
        // @recomp Point the stacks to the extended buffers. 
        sGfxStack[0] = ExtendedBuffers.gfx[0];
        sGfxStack[1] = ExtendedBuffers.gfx[1];
        sMtxStack[0] = ExtendedBuffers.mtx[0];
        sMtxStack[1] = ExtendedBuffers.mtx[1];
        sVtxStack[0] = ExtendedBuffers.vtx[0];
        sVtxStack[1] = ExtendedBuffers.vtx[1];
        dummy_func_80254464();
    }
    sStackSelector = 0;
    gTextureFilterPoint = 0;
}

// @recomp Patched to check for graphics stack overflow after processing a frame.
// Also patched to wait for a message when the displaylist is completed immediately after queueing it to solve vertex modification race conditions.
RECOMP_PATCH void game_draw(s32 arg0){
    Gfx *gfx;
    Gfx *gfx_start;
    Gfx *sp2C;
    Mtx *mtx;
    Vtx *vtx;

    if(arg0) {
        scissorBox_setDefault();
    }

    getGraphicsStacks(&gfx, &mtx, &vtx);

    if(D_8037E8E0.unkC == 1){
        getGraphicsStacks(&gfx, &mtx, &vtx);
    }

    // @recomp Reset the high precision position skinning stacks.
    recomp_reset_skinning_stack();
    recomp_reset_map_model_skinning();

    // @recomp Advance the frame used as reference by the dynamic camera target changes for analog camera.
    recomp_advance_dynamic_camera_targets();

    // @recomp Update note saving state.
    note_saving_update();

    // @recomp Track the original values.
    Mtx* mtx_start = mtx;
    Vtx* vtx_start = vtx;

    gfx_start = gfx;

    // @recomp Enable the extended gbi.
    gEXEnable(gfx++);
    gEXSetRefreshRate(gfx++, 60 / viMgr_func_8024BFA0()); // Input framerate is equal to 60 Hz divided by the frame divisor

    // @recomp Turn off nearclipping (i.e. turn on depth clamp) to prevent the camera from clipping through lots of geometry in ultrawide aspect ratios.
    gEXSetNearClipping(gfx++, FALSE);

    func_802E39D0(&gfx, &mtx, &vtx, getActiveFramebuffer(), arg0);

    // @recomp Check for graphics stack overflow
    u32 gfx_count = gfx - gfx_start;
    u32 mtx_count = mtx - mtx_start;
    u32 vtx_count = vtx - vtx_start;

    // recomp_printf("gfx: %-5u mtx: %-5u vtx: %-5u (%06X / %06X)\n", gfx_count, mtx_count, vtx_count, heap_occupiedBytes, 0x210520);

    if (gfx_count >= GFX_BUFFER_COUNT) {
        recomp_error("Gfx buffer overflow\n");
    }

    if (mtx_count >= MTX_BUFFER_COUNT) {
        recomp_error("Mtx buffer overflow\n");
    }

    if (vtx_count >= VTX_BUFFER_COUNT) {
        recomp_error("Vtx buffer overflow\n");
    }

    if(D_8037E8E0.unkC == 0){
        sp2C = gfx;
        viMgr_func_8024C1DC();

        // @recomp Set up the message queue and event for waiting on DL completion.
        OSMesgQueue dl_complete_queue;
        OSMesg dl_complete_queue_buf;
        osCreateMesgQueue(&dl_complete_queue, &dl_complete_queue_buf, 1);
        osExQueueDisplaylistEvent(&dl_complete_queue, NULL, gfx_start, OS_EX_DISPLAYLIST_EVENT_PARSED);

        func_80253EC4(gfx_start, sp2C);
        
        // @recomp Wait for the displaylist to be completed after submitting it. This removes the race condition
        // that exists with vertex modifications for texture scrolling.
        osRecvMesg(&dl_complete_queue, NULL, OS_MESG_BLOCK);

        if(arg0) {
            scissorBox_setDefault();
        }
    }

    // Allow interpolation for the next frame.
    set_all_interpolation_skipped(FALSE);
}
