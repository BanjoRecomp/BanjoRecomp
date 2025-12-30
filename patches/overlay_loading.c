#include "patches.h"
#include "overlay_loading.h"
#include "misc_funcs.h"

struct Overlay {
    void *start;
    void *end;
};

#define MAKE_OVERLAYS() \
    OVERLAY(core2,0) \
    OVERLAY(emptyLvl,1) \
    OVERLAY(CC,2) \
    OVERLAY(MMM,3) \
    OVERLAY(GV,4) \
    OVERLAY(TTC,5) \
    OVERLAY(MM,6) \
    OVERLAY(BGS,7) \
    OVERLAY(RBB,8) \
    OVERLAY(FP,9) \
    OVERLAY(CCW,10) \
    OVERLAY(SM,11) \
    OVERLAY(cutscenes,12) \
    OVERLAY(lair,13) \
    OVERLAY(fight,14)

extern struct Overlay gOverlayTable[];

#define OVERLAY(ovl, _) \
    extern u8 boot_##ovl##_rzip_ROM_START[]; \
    extern u8 boot_##ovl##_rzip_ROM_END[];
MAKE_OVERLAYS()
#undef OVERLAY

#define OVERLAY(ovl, _) \
    extern u8 ovl##_ROM_START[]; \
    extern u8 ovl##_ROM_END[]; \
    extern u8 ovl##_VRAM[]; \
    extern u8 ovl##_VRAM_END[]; \
    extern u8 ovl##_TEXT_START[]; \
    extern u8 ovl##_TEXT_END[]; \
    extern u8 ovl##_DATA_START[]; \
    extern u8 ovl##_DATA_END[]; \
    extern u8 ovl##_RODATA_START[]; \
    extern u8 ovl##_RODATA_END[]; \
    extern u8 ovl##_BSS_START[]; \
    extern u8 ovl##_BSS_END[];
OVERLAY(core1, 0)
MAKE_OVERLAYS()
#undef OVERLAY


#define OVERLAY_START(ovl) boot_##ovl##_rzip_ROM_START
#define OVERLAY_END(ovl) boot_##ovl##_rzip_ROM_END

// @recomp Patched to load overlays, prefixed with boot_ to account for the decomp build system.
RECOMP_PATCH void boot___osInitialize_common(void) {
    // Nothing of the original function needs to happen, so the body can be empty besides the overlay load.

    // @recomp Load core1.
    recomp_load_overlays_by_rom((u32)core1_ROM_START, core1_VRAM, core1_ROM_END - core1_ROM_START);
}

typedef struct{
    u32 unk0;
    u32 unk4;
}struct49s;

extern struct49s D_803FFE10[];
extern u8  D_8000E800;
extern u8  D_8002D500;
extern u32 D_8027BF2C;
extern u32 D_8027BF30;

void func_80254008(void);
void piMgr_read(void* vaddr, s32 devaddr, s32 size);
void rarezip_uncompress(u8 **srcPtr, u8 **dstPtr);

RECOMP_PATCH void overlay_load(
    s32 overlay_id, u32 ram_start, u32 ram_end, u32 rom_start, u32 rom_end, 
    u32 code_start, u32 code_end, u32 data_start, u32 data_end, u32 bss_start, u32 bss_end
){
    // @recomp Track the original input values for later.
    u32 uncompressed_rom_start = rom_start;
    u32 uncompressed_rom_size = rom_end - rom_start;
    u32 overlay_vram_start = ram_start;
    u8* sp34;
    u32 sp30;
    u32 sp2C;
    u32 *tmp;

    osWriteBackDCacheAll();
    osInvalDCache((void*)ram_start, ram_end - ram_start);
    osInvalICache((void*)ram_start, ram_end - ram_start);

    if(bss_start){
        osInvalDCache((void*)bss_start, bss_end - bss_start);
    }

    rom_start = D_803FFE10[overlay_id].unk0;
    rom_end = D_803FFE10[overlay_id].unk4;

    if(overlay_id){
        func_80254008();
        sp34 = &D_8000E800;
    }
    else{
        sp34 = &D_8002D500;
    }
    piMgr_read(sp34, rom_start, rom_end - rom_start);
    rarezip_uncompress(&sp34, (u8**)&ram_start);
    sp2C = D_8027BF2C;
    sp30 = D_8027BF30;
    rarezip_uncompress(&sp34, (u8**)&ram_start);

    if(bss_start){
        bzero((void*)bss_start, bss_end - bss_start);
        osWriteBackDCacheAll();
        tmp = (u32*) bss_start;
        tmp[0] = sp2C;
        tmp[1] = sp30;
        tmp[2] = D_8027BF2C;
        tmp[3] = D_8027BF30;
    }
    
    // @recomp Load the overlay.
    recomp_load_overlays_by_rom(uncompressed_rom_start, (void*)overlay_vram_start, uncompressed_rom_size);
}

#define UCODE_SIZE 256

extern u8 sUcodeData[UCODE_SIZE];
extern s32 D_80283380;
extern s32 D_80283384;
extern s32 D_80283388;

RECOMP_PATCH void ucode_load(void) {
    D_80283384 = 0;
    D_80283388 = (D_80283384) ? 1 :0;
    D_80283380 = 0;
    D_80283388 |= (D_80283380) ? 2 :0;
    if (D_80283388 == 0) {
        piMgr_read(&sUcodeData, 0xB0000B70, UCODE_SIZE);
    }
}

typedef struct Animation_s Animation;

typedef struct animctrl_s{
    Animation *animation;
    f32     timer;
    f32     subrange_start;
    f32     subrange_end;
    f32     animation_duration;
    f32     transition_duration;
    float   start;
    s32     index;
    u8      playback_type;
    u8      playback_direction;
    u8      smooth_transition;
    u8      unk23;
    u8      unk24;
    u8      default_start;
    u8      pad26[2];
} AnimCtrl;

RECOMP_PATCH void anctrl_setDuration(AnimCtrl *this, f32 arg1){
    this->animation_duration = arg1;
}
