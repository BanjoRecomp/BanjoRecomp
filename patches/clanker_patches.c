#include "patches.h"
#include "transform_ids.h"

int overlayManagergetLoadedId(void);

extern struct {
    u8 unk0;
    u8 unk1;
    u8 pad2[2];
    void (*init)(void);
    void (*update)(void);
    void (*release)(void);
    void (*draw)(Gfx **, Mtx **, Vtx **);
    void (*unk14)(s32, s32);
} sOverlay;

void recomp_setup_overlay_skinning(int overlay_id);
void recomp_clear_overlay_skinning();

// @recomp: Patched to assign an ID to Clanker.
RECOMP_PATCH void func_80322E64(Gfx **gfx, Mtx **mtx, Vtx **vtx){
    // @recomp If the current level is Clanker's Cavern, set up the model ID for Clanker. 
    u32 prev_transform_id = cur_drawn_model_transform_id;
    if (overlayManagergetLoadedId() == OVERLAY_2_WHALE) {
        cur_drawn_model_transform_id = CLANKER_TRANSFORM_ID_START;
    }

    // @recomp Setup the overlay skinning data.
    recomp_setup_overlay_skinning(overlayManagergetLoadedId());

    if(sOverlay.draw)
        sOverlay.draw(gfx, mtx, vtx);
    
    // @recomp Clear the overlay skinning setup.
    recomp_clear_overlay_skinning();

    // @recomp Reset the current transform ID.
    cur_drawn_model_transform_id = prev_transform_id;
}
