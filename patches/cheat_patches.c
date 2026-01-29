#include "patches.h"
#include "input.h"

// @recomp Classic Cheats - proper recomp implementations of GameShark cheats.
// Each cheat is toggled independently via the Enhancements settings tab.
// The bitmask returned by recomp_get_enabled_cheats() has one bit per cheat.

#define CHEAT_INFINITE_HEALTH        (1 << 0)
#define CHEAT_INFINITE_LIVES         (1 << 1)
#define CHEAT_INFINITE_AIR           (1 << 2)
#define CHEAT_INFINITE_EGGS          (1 << 3)
#define CHEAT_INFINITE_RED_FEATHERS  (1 << 4)
#define CHEAT_INFINITE_GOLD_FEATHERS (1 << 5)
#define CHEAT_INFINITE_MUMBO_TOKENS  (1 << 6)
#define CHEAT_HOVER                  (1 << 7)

// Item score array: D_80385F30[0x2C], indexed by enum item_e values.
extern s32 D_80385F30[];

s32 item_getCount(s32 item);
void baphysics_set_vertical_velocity(f32);
u32 bakey_held(s32 button_indx);

// @recomp Called every frame from game_draw() to apply enabled classic cheats.
// Each cheat writes max values directly into the item score array,
// mirroring what the original GameShark codes did via raw memory writes.
void apply_classic_cheats(void) {
    u32 cheats = recomp_get_enabled_cheats();
    if (cheats == 0) {
        return;
    }

    // @recomp Infinite Health: set current health to max health.
    // Only applies when health total is > 0 (i.e. player is in-game).
    // GameShark: 803851A3 0008
    if (cheats & CHEAT_INFINITE_HEALTH) {
        s32 max_health = D_80385F30[ITEM_15_HEALTH_TOTAL];
        if (max_health > 0) {
            D_80385F30[ITEM_14_HEALTH] = max_health;
        }
    }

    // @recomp Infinite Lives: keep lives at 9.
    // GameShark: 80385F8B 00FF (we cap at 9 for HUD display correctness).
    if (cheats & CHEAT_INFINITE_LIVES) {
        D_80385F30[ITEM_16_LIFE] = 9;
    }

    // @recomp Infinite Air: keep air at maximum (3600 units = full meter).
    // GameShark: 813851AE 0E10
    if (cheats & CHEAT_INFINITE_AIR) {
        D_80385F30[ITEM_17_AIR] = 3600;
    }

    // @recomp Infinite Eggs: keep egg count at 100.
    // GameShark: 80385F67 00FF (we use the proper max of 100).
    if (cheats & CHEAT_INFINITE_EGGS) {
        D_80385F30[ITEM_D_EGGS] = 100;
    }

    // @recomp Infinite Red Feathers: keep count at 50.
    // GameShark: 80385F6F 00FF (we use the proper max of 50).
    if (cheats & CHEAT_INFINITE_RED_FEATHERS) {
        D_80385F30[ITEM_F_RED_FEATHER] = 50;
    }

    // @recomp Infinite Gold Feathers: keep count at 10.
    // GameShark: 80385F73 00FF (we use the proper max of 10).
    if (cheats & CHEAT_INFINITE_GOLD_FEATHERS) {
        D_80385F30[ITEM_10_GOLD_FEATHER] = 10;
    }

    // @recomp Infinite Mumbo Tokens: keep current level tokens at 99.
    // GameShark: 80385FC6 00FF + 80385FA2 00FF
    if (cheats & CHEAT_INFINITE_MUMBO_TOKENS) {
        D_80385F30[ITEM_1C_MUMBO_TOKEN] = 99;
    }

    // @recomp Hover: hold L to levitate upward.
    // Sets vertical velocity each frame while L is held, working with the
    // physics system instead of fighting it. Releasing L lets gravity resume.
    if (cheats & CHEAT_HOVER) {
        if (bakey_held(BUTTON_L)) {
            baphysics_set_vertical_velocity(600.0f);
        }
    }
}
