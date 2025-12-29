#include "patches.h"
#include "prop.h"

typedef struct demo_input{
    u8 unk0;
    u8 unk1;
    u16 unk2;
    u8 unk4;
    u8 unk5;
}DemoInput;

typedef struct demo_file_header{
    u8 pad0[0x4];
    DemoInput inputs[];
} DemoFileHeader;

extern DemoInput *D_803860D0; //demo_input_ptr
extern DemoFileHeader * D_803860D4; //demo_file_ptr
extern s32 D_803860D8;//current_input
extern s32 D_803860DC;//total_inputs

extern DemoInput D_80371EF0;

enum extra_actors_e {
    ACTOR_19_FRAMERATE_60 = 0x19,
    ACTOR_1A_FRAMERATE_30,
    ACTOR_1B_FRAMERATE_20,
    ACTOR_1C_FRAMERATE_15,
    ACTOR_1D_FRAMERATE_12,
};

typedef struct struct_core2_9B180_s{
    s16 unk0;
    // u8 pad2[0x2];
    NodeProp *unk4;
    void (*unk8)(struct struct_core2_9B180_s *);
    void (*unkC)(struct struct_core2_9B180_s *);
    void (*unk10)(struct struct_core2_9B180_s *);
}Struct_core2_9B180_0;

extern Struct_core2_9B180_0 D_8036DE00[6];

extern volatile s32 D_802808D8;
extern s32 D_802808DC;
extern s32 D_80280E90;

void func_80244A98(s32 arg0);
NodeProp *cubeList_findNodePropByActorIdAndPosition_s32(enum actor_e actor_id, s32 arg1[3]);
void func_8032236C(s32 arg0, s32 arg1, s32 *arg2);
void viMgr_func_8024BF94(s32 arg0);
void viMgr_func_8024BFD8(s32 arg0);
void dummy_func_8025AFB8(void);
s32 getGameMode(void);
f32 time_getDelta(void);
s32 item_adjustByDiffWithHud(enum item_e item, s32 diff);
void func_802FACA4(enum item_e item_id);
bool func_802FAFE8(enum item_e item_id);

s32 demo_frame_divisor = -1;

// @recomp Patched to set a variable to use as the frame divisor when processing demo inputs.
RECOMP_PATCH int demo_readInput(OSContPad* arg0, s32* arg1){
    DemoInput *input_ptr = &D_803860D0[D_803860D8++];
    int not_eof = D_803860D8 < D_803860DC;

    if(!not_eof)
        input_ptr = &D_80371EF0;

    arg0->stick_x = input_ptr->unk0;
    arg0->stick_y = input_ptr->unk1;
    arg0->button = input_ptr->unk2;
    *arg1 = input_ptr->unk4;

    // @recomp Track the frame divisor for later.
    demo_frame_divisor = input_ptr->unk4;
    
    // @recomp Lock the frame divisor to 3 (20 FPS) in Bottles' Bonus.
    // The game generally runs at 20 FPS during Bottles' Bonus on original hardware, which means the minigame timer should last a similar amount of time.
    // Return 2 (30 FPS) into arg1 so that the game doesn't compensate by making the timer run faster.
    if (getGameMode() == GAME_MODE_8_BOTTLES_BONUS) {
        // *arg1 = 2;
        demo_frame_divisor = 3;
    }

    return not_eof;
}

// @recomp Patched to override the VI frame divisor when the demo frame divisor has been set.
RECOMP_PATCH s32 viMgr_func_8024BFA0() {
    if (demo_frame_divisor != -1) {
        return demo_frame_divisor;
    }
    return D_802808DC;
}

// @recomp Patched to clear the demo frame divisor after viMgr_func_8024BFD8.
RECOMP_PATCH void viMgr_func_8024C1B4(void){
    viMgr_func_8024BFD8(0);
    // @recomp Clear the demo frame divisor.
    demo_frame_divisor = -1;
    dummy_func_8025AFB8();
}

// @recomp Patched to clear the demo frame divisor after viMgr_func_8024BFD8.
RECOMP_PATCH void viMgr_func_8024C1DC(void){
    viMgr_func_8024BFD8(1);
    // @recomp Clear the demo frame divisor.
    demo_frame_divisor = -1;
}

// @recomp Patched to use a fixed time delta of 30 FPS when decrementing the hourglass timer during Bottles' Bonus.
// Because the game is running at 20 FPS, this will result in the timer running slightly slower, which is accurate
// to how fast it runs on original hardware.
RECOMP_PATCH void func_80345EB0(enum item_e item){
    if(func_802FAFE8(item)){
        // @recomp Get the time delta and override it if this is the hourglass timer and the gamemode is Bottles' Bonus.
        f32 time_delta = time_getDelta();
        if (item == ITEM_0_HOURGLASS_TIMER && getGameMode() == GAME_MODE_8_BOTTLES_BONUS) {
            time_delta = 1.0f / 30.0f;
        }
        item_adjustByDiffWithHud(item, (s32)(-time_delta*60.0f * 1.1));
    }else{
        func_802FACA4(item);
    }
}
