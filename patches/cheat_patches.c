#include "patches.h"
#include "input.h"

// @recomp classic cheats - recomp equivalents of GameShark codes.
// each cheat is toggled via the enhancements tab, bitmask from recomp_get_enabled_cheats().

#define CHEAT_INFINITE_HEALTH        (1 << 0)
#define CHEAT_INFINITE_LIVES         (1 << 1)
#define CHEAT_INFINITE_AIR           (1 << 2)
#define CHEAT_INFINITE_EGGS          (1 << 3)
#define CHEAT_INFINITE_RED_FEATHERS  (1 << 4)
#define CHEAT_INFINITE_GOLD_FEATHERS (1 << 5)
#define CHEAT_INFINITE_MUMBO_TOKENS  (1 << 6)
#define CHEAT_HOVER                  (1 << 7)

// item score array, indexed by item_e
extern s32 D_80385F30[];

s32 item_getCount(s32 item);
void baphysics_set_vertical_velocity(f32);
u32 bakey_held(s32 button_indx);

// @recomp called every frame from game_draw(), writes max values into the item score array.
void apply_classic_cheats(void) {
    u32 cheats = recomp_get_enabled_cheats();
    if (cheats == 0) {
        return;
    }

    // @recomp infinite health - GS 803851A3 0008
    if (cheats & CHEAT_INFINITE_HEALTH) {
        s32 max_health = D_80385F30[ITEM_15_HEALTH_TOTAL];
        if (max_health > 0) {
            D_80385F30[ITEM_14_HEALTH] = max_health;
        }
    }

    // @recomp infinite lives - GS 80385F8B 00FF (capped at 9 for HUD)
    if (cheats & CHEAT_INFINITE_LIVES) {
        D_80385F30[ITEM_16_LIFE] = 9;
    }

    // @recomp infinite air - GS 813851AE 0E10 (3600 = full meter)
    if (cheats & CHEAT_INFINITE_AIR) {
        D_80385F30[ITEM_17_AIR] = 3600;
    }

    // @recomp infinite eggs - GS 80385F67 00FF (max 100)
    if (cheats & CHEAT_INFINITE_EGGS) {
        D_80385F30[ITEM_D_EGGS] = 100;
    }

    // @recomp infinite red feathers - GS 80385F6F 00FF (max 50)
    if (cheats & CHEAT_INFINITE_RED_FEATHERS) {
        D_80385F30[ITEM_F_RED_FEATHER] = 50;
    }

    // @recomp infinite gold feathers - GS 80385F73 00FF (max 10)
    if (cheats & CHEAT_INFINITE_GOLD_FEATHERS) {
        D_80385F30[ITEM_10_GOLD_FEATHER] = 10;
    }

    // @recomp infinite mumbo tokens - GS 80385FC6 00FF + 80385FA2 00FF
    if (cheats & CHEAT_INFINITE_MUMBO_TOKENS) {
        D_80385F30[ITEM_1C_MUMBO_TOKEN] = 99;
    }

    // @recomp hover - sets vertical velocity while L is held
    if (cheats & CHEAT_HOVER) {
        if (bakey_held(BUTTON_L)) {
            baphysics_set_vertical_velocity(600.0f);
        }
    }
}
