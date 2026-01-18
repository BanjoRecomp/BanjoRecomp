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
enum map_e map_get(void);

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
    s32 game_mode = getGameMode();
    if (game_mode == GAME_MODE_8_BOTTLES_BONUS) {
        // *arg1 = 2;
        demo_frame_divisor = 3;
    }
    // @recomp Lock the frame divisor in the Mumbo SNS pictures based on the loaded map.
    // This makes sure that the dialog doesn't get cut off because the scene runs too quickly.
    if (game_mode == GAME_MODE_A_SNS_PICTURE) {
        s32 cur_map = map_get();

        switch (cur_map) {
            // 20 FPS maps.
            case MAP_7F_FP_WOZZAS_CAVE:
            case MAP_92_GV_SNS_CHAMBER:
                demo_frame_divisor = 3;
                break;
            // Remaining maps run at 30 FPS.
            default:
                demo_frame_divisor = 2;
                break;
        }
    }

    return not_eof;
}

int extraVis = 0;

// @recomp Patched to override the VI frame divisor when the demo frame divisor has been set.
// Also overrides it if a cutscene needs timing compensation.
RECOMP_PATCH s32 viMgr_func_8024BFA0() {
    if (demo_frame_divisor != -1) {
        return demo_frame_divisor;
    }
    return D_802808DC + extraVis;
}

// @recomp Patched to clear lag overrides after viMgr_func_8024BFD8.
RECOMP_PATCH void viMgr_func_8024C1B4(void){
    viMgr_func_8024BFD8(0);
    // @recomp Clear the demo frame divisor.
    demo_frame_divisor = -1;
    dummy_func_8025AFB8();
    // @recomp Clear the lag override for cutscenes.
    extraVis = 0;
}

// @recomp Patched to clear lag overrides after viMgr_func_8024BFD8.
RECOMP_PATCH void viMgr_func_8024C1DC(void){
    viMgr_func_8024BFD8(1);
    // @recomp Clear the demo frame divisor.
    demo_frame_divisor = -1;
    // @recomp Clear the lag override for cutscenes.
    extraVis = 0;
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

// The concert intro cutscene stutters on console, but it does not stutter in recomp.
// The cutscene is timed with the stutters in mind so this causes desyncs with the music and sound effects.
// We have manually analyzed the cutscene and taken note of the exact frames during which it stutters,
// and we lag the game to 15 FPS internally (this cutscene targets 20 FPS) for a few frames when it would
// have stuttered on console in order to keep the cutscene in sync.

// What frames of the cutscene to lag on, and for how many frames.
int concertStuttersStartFrames[] = { 269, 521, 583, 663, 769, 959, 1155, 1182, 1214 };
int concertStutterDurations[] =    { 4,   4,   4,   4,   4,   4,   4,    4,    4    };

// What frames of the cutscene to lag on, and for how many frames.
int lairDingpotStuttersStartFrames[] = { 258, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000 };
int lairDingpotStutterDurations[] =    { 6,   5,   5,   5,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4 };

// These are reset on map load, so that the cutscene can be replayed (such as when saving and exiting)
// See func_803329AC in load_patches.c
int cutsceneCounter = 0;
int cutsceneNextStutter = 0;
int cutsceneLagIndex = 0;

bool should_lag_cutscene(int *stuttersStartFrames, int *stutterDurations, int stuttersStartFramesCount) {
    // No stutters left to compensate for. Exit the function early. 
    if (cutsceneNextStutter == -1) {
        return FALSE;
    }

    //recomp_printf("cutsceneCounter %d\n", cutsceneCounter);

    // First frame of the cutscene. Set the first stutter frame.
    if (cutsceneNextStutter < stuttersStartFrames[0]) {
        cutsceneNextStutter = stuttersStartFrames[0];
        //recomp_printf("Start cutscene with timing corrections. First stutter frame: %d\n", cutsceneNextStutter);
    }

    if (cutsceneCounter >= (cutsceneNextStutter)  && cutsceneCounter < (cutsceneNextStutter + stutterDurations[cutsceneLagIndex])) {
        // A stutter would have occured on console now. Lag the game for a given amount of frames.
        //recomp_printf("LAGGING. Stutter number %d. Frame number %d\n", cutsceneLagIndex, cutsceneCounter);
        return TRUE;
    } else if (cutsceneCounter > (cutsceneNextStutter)) {
        cutsceneLagIndex++;
        if (cutsceneLagIndex >= stuttersStartFramesCount) {
            // That was the last stutter. We're done.
            cutsceneNextStutter = -1;
            //recomp_printf("End cutscene. %d\n", cutsceneNextStutter);
        } else {
            // Set the next stutter frame. 
            cutsceneNextStutter = stuttersStartFrames[cutsceneLagIndex];
            //recomp_printf("Next stutter: %d\n", cutsceneNextStutter);
        }
    }
    return FALSE;
}
// Reset the custom cutscene frame counter and the stutter frame index used to
// correct the timings of the cutscene.
void reset_cutscene_timings_state(void) {
    cutsceneCounter = 0;
    cutsceneNextStutter = 0;
    cutsceneLagIndex = 0;
}

// Return the current cutscene counter.
u32 get_cutscene_counter(void) {
    return cutsceneCounter;
}

// Check the current map to see if it's a cutscene map that requires timing fixes,
// and run the relevant function if so.
void handle_cutscene_timings(void) {
    switch (map_get()) {
        case MAP_1E_CS_START_NINTENDO:
            if (should_lag_cutscene(concertStuttersStartFrames, concertStutterDurations, (int)sizeof(concertStuttersStartFrames) / (int)sizeof(concertStuttersStartFrames[0]))) {
                extraVis = 1;
            }
            cutsceneCounter++;
            break;
        case MAP_7B_CS_INTRO_GL_DINGPOT_1:
            if (should_lag_cutscene(lairDingpotStuttersStartFrames, lairDingpotStutterDurations, (int)sizeof(lairDingpotStuttersStartFrames) / (int)sizeof(lairDingpotStuttersStartFrames[0]))) {
                extraVis = 1;
            }
            cutsceneCounter++;
            break;
        default:
            break;
    }    
}