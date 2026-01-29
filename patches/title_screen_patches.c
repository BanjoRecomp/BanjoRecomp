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

// @recomp Returns true if the current map is a cutscene map that can be skipped.
// NOTE: MAP_1F_CS_START_RAREWARE is intentionally excluded — the game's own
// func_80322318 handler already manages logo skip to file select.
static bool is_skippable_cutscene_map(void) {
    switch (map_get()) {
    case MAP_1E_CS_START_NINTENDO:
    case MAP_7B_CS_INTRO_GL_DINGPOT_1:
    case MAP_7C_CS_INTRO_BANJOS_HOUSE_1:
    case MAP_7D_CS_SPIRAL_MOUNTAIN_1:
    case MAP_7E_CS_SPIRAL_MOUNTAIN_2:
    case MAP_81_CS_INTRO_GL_DINGPOT_2:
    case MAP_82_CS_ENTERING_GL_MACHINE_ROOM:
    case MAP_85_CS_SPIRAL_MOUNTAIN_3:
    case MAP_86_CS_SPIRAL_MOUNTAIN_4:
    case MAP_87_CS_SPIRAL_MOUNTAIN_5:
    case MAP_88_CS_SPIRAL_MOUNTAIN_6:
    case MAP_89_CS_INTRO_BANJOS_HOUSE_2:
    case MAP_8A_CS_INTRO_BANJOS_HOUSE_3:
    case MAP_94_CS_INTRO_SPIRAL_7:
    case MAP_20_CS_END_NOT_100:
    case MAP_95_CS_END_ALL_100:
    case MAP_96_CS_END_BEACH_1:
    case MAP_97_CS_END_BEACH_2:
    case MAP_98_CS_END_SPIRAL_MOUNTAIN_1:
    case MAP_99_CS_END_SPIRAL_MOUNTAIN_2:
    // Exclude game-over cutscenes as skipping those could cause issues.
    // case MAP_83_CS_GAME_OVER_MACHINE_ROOM:
    // case MAP_84_CS_UNUSED_MACHINE_ROOM:
        return TRUE;
    default:
        return FALSE;
    }
}

// @recomp Returns the skip destination for the current cutscene map.
// Uses transitionToMap() to directly load the target map.
// Destinations are based on the decomp's actual game flow:
//   Boot → Logo → File Select → New Game → Intro Cutscenes → Spiral Mountain
//   End-game cutscenes → Logo → File Select
// MAP_1F (Rareware logo) is NOT handled here — the game's own func_80322318
// handler already manages that transition to file select.
static bool try_skip_cutscene(int current_map) {
    switch (current_map) {
    // @recomp Nintendo logo: skip to Rareware logo.
    case MAP_1E_CS_START_NINTENDO:
        transitionToMap(MAP_1F_CS_START_RAREWARE, 0, 1);
        return TRUE;

    // @recomp Intro cutscenes: skip to Spiral Mountain.
    // These ONLY play after starting a new game from file select, so a save
    // file IS loaded. Exit 0x12 is the intro entry point (from decomp chain:
    // MAP_89 → MAP_1 exit 0x12).
    case MAP_7B_CS_INTRO_GL_DINGPOT_1:
    case MAP_7C_CS_INTRO_BANJOS_HOUSE_1:
    case MAP_7D_CS_SPIRAL_MOUNTAIN_1:
    case MAP_7E_CS_SPIRAL_MOUNTAIN_2:
    case MAP_81_CS_INTRO_GL_DINGPOT_2:
    case MAP_82_CS_ENTERING_GL_MACHINE_ROOM:
    case MAP_85_CS_SPIRAL_MOUNTAIN_3:
    case MAP_86_CS_SPIRAL_MOUNTAIN_4:
    case MAP_87_CS_SPIRAL_MOUNTAIN_5:
    case MAP_88_CS_SPIRAL_MOUNTAIN_6:
    case MAP_89_CS_INTRO_BANJOS_HOUSE_2:
    case MAP_8A_CS_INTRO_BANJOS_HOUSE_3:
    case MAP_94_CS_INTRO_SPIRAL_7:
        transitionToMap(MAP_1_SM_SPIRAL_MOUNTAIN, 0x12, 1);
        return TRUE;

    // @recomp End-game cutscenes: skip to file select.
    // The game's own flow goes: end cutscenes → Rareware logo → file select.
    // We skip the logo step for cleaner UX.
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

// @recomp State tracking for cutscene skip.
static int cutscene_skip_last_map = -1;
static int cutscene_skip_frame_counter = 0;

// @recomp Called on map load to reset the cutscene skip state.
void reset_cutscene_skip_state(void) {
    cutscene_skip_frame_counter = 0;
}

// @recomp Patched to allow skipping the intro sequence and all cutscenes when the setting is enabled.
RECOMP_PATCH void func_80322490(void) {
    Struct_core2_9B180_0 *i_ptr;
    static int introFrameCounter = 0;

    introFrameCounter++;

    // @recomp Reset cutscene skip frame counter when the map changes.
    int current_map = map_get();
    if (current_map != cutscene_skip_last_map) {
        cutscene_skip_frame_counter = 0;
        cutscene_skip_last_map = current_map;
    }

    cutscene_skip_frame_counter++;

    // @recomp When Start is pressed on a skippable cutscene, use transitionToMap()
    // to directly load the skip destination. This avoids the broken fast-forward
    // approach that forced all actor callbacks and caused invalid game state.
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

            // @recomp Always allow skipping the intro sequence, with a delay of 1 second to prevent
            // issues with accidentally skipping the intro when navigating the launcher with a controller.
            if (i_ptr->unkC == func_80322318 && current_map == MAP_1F_CS_START_RAREWARE && introFrameCounter > 30) {
                should_run = TRUE;
            }

            if(should_run && i_ptr->unkC != NULL){
                i_ptr->unkC(i_ptr);
            }
        }
    }
}
