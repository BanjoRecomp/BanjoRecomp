#include "patches.h"

#include "enums.h"
#include "input.h"

struct Struct_core2_9B180_1;
typedef struct Struct_core2_9B180_1 Struct_core2_9B180_1;
struct struct_core2_9B180_s;

typedef struct struct_core2_9B180_s{
    s16 unk0;
    // u8 pad2[0x2];
    Struct_core2_9B180_1 *unk4;
    void (*unk8)(struct struct_core2_9B180_s *);
    void (*unkC)(struct struct_core2_9B180_s *);
    void (*unk10)(struct struct_core2_9B180_s *);
}Struct_core2_9B180_0;

extern u8 D_80383330;
extern Struct_core2_9B180_0 D_8036DE00[];

void func_80322318(Struct_core2_9B180_0*);
int map_get(void);
void transitionToMap(s32 map, s32 exit, s32 transition);

// @recomp check if the current map is a skippable cutscene.
// MAP_1F excluded - the game's own func_80322318 handles logo skip.
// MAP_7E and MAP_8A excluded - not in cutscenetrigger_update, probably unused.
static bool is_skippable_cutscene_map(void) {
    switch (map_get()) {
    case MAP_1E_CS_START_NINTENDO:
    // @recomp intro cutscenes (new game only, flag 0xC skip in decomp)
    case MAP_85_CS_SPIRAL_MOUNTAIN_3:
    case MAP_7B_CS_INTRO_GL_DINGPOT_1:
    case MAP_81_CS_INTRO_GL_DINGPOT_2:
    case MAP_7D_CS_SPIRAL_MOUNTAIN_1:
    case MAP_7C_CS_INTRO_BANJOS_HOUSE_1:
    case MAP_86_CS_SPIRAL_MOUNTAIN_4:
    case MAP_89_CS_INTRO_BANJOS_HOUSE_2:
    // @recomp lair entry cutscene (first time entering GL during gameplay)
    case MAP_82_CS_ENTERING_GL_MACHINE_ROOM:
    // @recomp furnace fun access cutscene
    case MAP_94_CS_INTRO_SPIRAL_7:
    // @recomp end-game cutscenes
    case MAP_87_CS_SPIRAL_MOUNTAIN_5:
    case MAP_88_CS_SPIRAL_MOUNTAIN_6:
    case MAP_20_CS_END_NOT_100:
    case MAP_95_CS_END_ALL_100:
    case MAP_96_CS_END_BEACH_1:
    case MAP_97_CS_END_BEACH_2:
    case MAP_98_CS_END_SPIRAL_MOUNTAIN_1:
    case MAP_99_CS_END_SPIRAL_MOUNTAIN_2:
        return TRUE;
    default:
        return FALSE;
    }
}

// @recomp skip destination for the current cutscene map.
// all destinations match the decomp's cutscenetrigger_update() in code_956B0.c:89-119.
static bool try_skip_cutscene(int current_map) {
    switch (current_map) {
    // @recomp nintendo logo -> rareware logo
    case MAP_1E_CS_START_NINTENDO:
        transitionToMap(MAP_1F_CS_START_RAREWARE, 0, 1);
        return TRUE;

    // @recomp intro cutscenes -> spiral mountain
    // only play after starting a new game, flag 0xC skip (lines 107-113)
    case MAP_85_CS_SPIRAL_MOUNTAIN_3:
    case MAP_7B_CS_INTRO_GL_DINGPOT_1:
    case MAP_81_CS_INTRO_GL_DINGPOT_2:
    case MAP_7D_CS_SPIRAL_MOUNTAIN_1:
    case MAP_7C_CS_INTRO_BANJOS_HOUSE_1:
    case MAP_86_CS_SPIRAL_MOUNTAIN_4:
    case MAP_89_CS_INTRO_BANJOS_HOUSE_2:
        transitionToMap(MAP_1_SM_SPIRAL_MOUNTAIN, 0x12, 1);
        return TRUE;

    // @recomp lair entry cutscene -> MM lobby (decomp line 97)
    case MAP_82_CS_ENTERING_GL_MACHINE_ROOM:
        transitionToMap(MAP_69_GL_MM_LOBBY, 0x12, 1);
        return TRUE;

    // @recomp furnace fun cutscene -> furnace fun (decomp line 100)
    case MAP_94_CS_INTRO_SPIRAL_7:
        transitionToMap(MAP_8E_GL_FURNACE_FUN, 4, 1);
        return TRUE;

    // @recomp end-game cutscenes -> file select
    // MAP_87/88 are part of the ending chain, not intro
    case MAP_87_CS_SPIRAL_MOUNTAIN_5:
    case MAP_88_CS_SPIRAL_MOUNTAIN_6:
    case MAP_20_CS_END_NOT_100:
    case MAP_95_CS_END_ALL_100:
    case MAP_96_CS_END_BEACH_1:
    case MAP_97_CS_END_BEACH_2:
    case MAP_98_CS_END_SPIRAL_MOUNTAIN_1:
    case MAP_99_CS_END_SPIRAL_MOUNTAIN_2:
        transitionToMap(MAP_91_FILE_SELECT, 0, 1);
        return TRUE;

    default:
        return FALSE;
    }
}

// @recomp state tracking for cutscene skip
static int cutscene_skip_last_map = -1;
static int cutscene_skip_frame_counter = 0;

// @recomp reset on map load
void reset_cutscene_skip_state(void) {
    cutscene_skip_frame_counter = 0;
}

// @recomp patched to allow skipping cutscenes with start button
RECOMP_PATCH void func_80322490(void) {
    Struct_core2_9B180_0 *i_ptr;
    static int introFrameCounter = 0;

    introFrameCounter++;

    // @recomp reset skip counter on map change
    int current_map = map_get();
    if (current_map != cutscene_skip_last_map) {
        cutscene_skip_frame_counter = 0;
        cutscene_skip_last_map = current_map;
    }

    cutscene_skip_frame_counter++;

    // @recomp skip cutscene on start press (with 1 second delay to avoid accidental skips)
    if (is_skippable_cutscene_map() && cutscene_skip_frame_counter > 30) {
        if (recomp_check_cutscene_skip()) {
            if (try_skip_cutscene(current_map)) {
                return;
            }
        }
    }

    if (D_80383330 != 0) {
        for(i_ptr = D_8036DE00; i_ptr != &D_8036DE00[6]; i_ptr++){
            bool should_run = (i_ptr->unk4 != 0);

            // @recomp force the rareware logo skip handler to run (1 sec delay to avoid launcher input)
            if (i_ptr->unkC == func_80322318 && current_map == MAP_1F_CS_START_RAREWARE && introFrameCounter > 30) {
                should_run = TRUE;
            }

            if(should_run && i_ptr->unkC != NULL){
                i_ptr->unkC(i_ptr);
            }
        }
    }
}
