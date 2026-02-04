#include "patches.h"
#include "bk_api.h"
#include "misc_funcs.h"
#include "save_extension.h"
#include "jinjo_saving.h"

#include "functions.h"
#include "prop.h"
#include "core2/timedfunc.h"
#include "core2/commonParticle.h"



/*********************** Declarations */

void codeABC00_spawnJiggyAtLocation(enum jiggy_e, f32[3]);
u32 jiggyscore_isCollected(enum jiggy_e jiggy_id);
s32 func_80329904(ActorMarker *, s32, f32 *);
typedef s32 (*FuncUnk40)(ActorMarker *, s32, f32[3]);
void commonParticle_add(s32 actorMarker, s32 arg1, FuncUnk40 arg2);
int commonParticle_new(enum common_particle_e particle_id, int arg1);
enum jiggy_e getJiggyId(Actor *this);
enum jiggy_e chjiggy_getJiggyId(Actor *this);
void chJinjo_update(Actor *this);
f32 func_80309B24(f32[3]);
void __chJinjo_802CDD3C(Actor * this);
void func_8028E964(f32 pos[3]);
void player_getVelocity(f32 dst[3]);
void __chJinjo_802CDC9C(Actor *this, s16 arg1);
void func_802F9EC4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_802FA0B0(s32 arg0, s32 arg1);
void func_8025AABC(enum comusic_e);
void func_8032BB88(Actor *, s32, s32);
void func_802F9D38(s32 arg0);
void func_8025A7DC(enum comusic_e track_id);
int gcdialog_hasCurrentTextId(void);
ActorAnimationInfo chJinjoAnimations[10];
void __chJinjo_802CDBA8(ActorMarker *this, ActorMarker *other);
enum level_e level_get(void);
enum map_e map_get(void);

typedef struct chjiggy_s
{
    bool isHidden;
    u32  id;
} ActorLocal_Jiggy;

extern s32 D_80385F30[0x2C];
extern u32 gGlobalTimer;
extern ActorArray *suBaddieActorArray;


/*********************** Declarations (recomp) */

bool recomp_in_demo_playback_game_mode();


/*********************** Statics */

#define SAVEDJINJO_NUM_BITS_PER_LEVEL (6)
#define SAVEDJINJO_IDX_ALL_COLLECTED  (SAVEDJINJO_NUM_BITS_PER_LEVEL - 1)

static vec3f sJinjoJiggySpawnPosition     = {0};
static bool  sJinjoJiggySpawnedOnMapEnter = FALSE;

// Jinjo saving can only safely be changed while in the lair, so this value is only updated when in the lair.
bool jinjo_saving_enabled_cached = FALSE;
// Override for allowing mods to disable jinjo saving.
bool jinjo_saving_override_disabled = FALSE;

static struct
{
    enum map_e   map;
    enum level_e level;
    u32          counter;
} sMapInitialVars = {0};


/*********************** Exports */

RECOMP_EXPORT void bkrecomp_jinjosaving_force_disabled(bool disabled)
{
    jinjo_saving_override_disabled = disabled;
}

// Jinjos are always saved, but this function controls whether to use the saved data to prevent them from spawning and adjust the jinjo count. 
RECOMP_EXPORT s32 bkrecomp_jinjo_saving_enabled()
{
    return recomp_get_jinjo_saving_enabled();
}

RECOMP_EXPORT s32 bkrecomp_jinjo_saving_active()
{
    return jinjo_saving_enabled_cached;
}


/*********************** Misc helpers */

static f32 clamp_f32(f32 val, f32 min, f32 max)
{
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

static u32 get_global_timer(void)
{
    return gGlobalTimer;
}

static u32 get_curr_vanilla_jinjobits(void)
{
    return D_80385F30[ITEM_12_JINJOS];
}

static u32 get_jiggy_idx_for_jinjo_jiggy(u32 levelIdx)
{
    // The jinjo jiggy is the first jiggy in each level's list of jiggy IDs
    return ((levelIdx - 1) * 10) + 1;
}


/*********************** Main functions */

/**
 * jinjodata
 * 
 * Needs "9 * (5 + 1)" bits; 9 worlds, 5 bits (one for each jinjo), plus one to
 * say "all collected", while leaving the final collected jinjo bit 0, so we
 * can tell which jinjo to spawn the jiggy at, if not collected yet
 * 
 * nbits: 9*(5+1) = 54
 * when grouped to nearest group of 8 bits: 7 bytes => 56 bits.
 */

static u8 jinjodata_get_byte(u32 idx)
{
    return loaded_file_extension_data.jinjo_data[idx];
}

static void jinjodata_set_byte(u32 idx, u8 value)
{
    loaded_file_extension_data.jinjo_data[idx] = value;
}

/**
 * We want to have a tightly packed jinjo bitfield, where all bits
 * used are from levels that exist.
 *
 * Previously, we wanted to skip Grunty's Lair's level idx, since
 * it's right between other levels in the list, and we "shifted"
 * the idx of the ones that appear after it so we wouldn't needlessly
 * allocate bits for the lair.
 *
 * But some mods have jinjos in the lair (like Jiggies of Time),
 * so it's been moved back to the middle of the list.
 *
 * We also return -1 for levels that shouldn't have jinjos, so we can
 * easily bail in those same functions.
 */
static s32 jinjo_get_level_lookup_subtract_amount(u32 levelIdx)
{
    switch (levelIdx)
    {
        case LEVEL_1_MUMBOS_MOUNTAIN:
        case LEVEL_2_TREASURE_TROVE_COVE:
        case LEVEL_3_CLANKERS_CAVERN:
        case LEVEL_4_BUBBLEGLOOP_SWAMP:
        case LEVEL_5_FREEZEEZY_PEAK:
        // Some mods (like Jiggies of Time) have jinjos in Grunty's Lair. Save them too.
        case LEVEL_6_LAIR:
        case LEVEL_7_GOBIS_VALLEY:
        case LEVEL_8_CLICK_CLOCK_WOOD:
        case LEVEL_9_RUSTY_BUCKET_BAY:
        case LEVEL_A_MAD_MONSTER_MANSION:
            // Map to zero-indexed
            return 1;
        case LEVEL_B_SPIRAL_MOUNTAIN:
        case LEVEL_C_BOSS:
        case LEVEL_D_CUTSCENE:
        default:
            // No jinjos on these levels
            return -1;
    }
}

static bool jinjo_bit_idx_is_valid(s32 jinjoIdx)
{
    // Includes all jinjo indices, plus extra bits
    return jinjoIdx >= 0 && jinjoIdx < SAVEDJINJO_NUM_BITS_PER_LEVEL;
}

static s32 jinjodata_get_bit_idx(u32 levelIdx, s32 jinjoIdx)
{
    if (!jinjo_bit_idx_is_valid(jinjoIdx))
        return -1;

    s32 levelSubtract = jinjo_get_level_lookup_subtract_amount(levelIdx);
    if (levelSubtract < 0)
        return -1;

    levelIdx -= levelSubtract;

    return (levelIdx * SAVEDJINJO_NUM_BITS_PER_LEVEL) + jinjoIdx;
}

static s32 jinjo_get_jinjoidx_by_actorId(u32 actorId)
{
    switch (actorId)
    {
        // ordered by their marker idx
        case ACTOR_60_JINJO_BLUE:   return 0;
        case ACTOR_62_JINJO_GREEN:  return 1;
        case ACTOR_5F_JINJO_ORANGE: return 2;
        case ACTOR_61_JINJO_PINK:   return 3;
        case ACTOR_5E_JINJO_YELLOW: return 4;
        default:
            return -1;
    }
}

/**
 * Returns raw bit, so will not return proper individualised results if all jinjos are collected.
 * 
 * Pass `SAVEDJINJO_IDX_ALL_COLLECTED` for jinjoIdx to get the final "all collected" flag
 */
static bool SavedJinjo_get_collected_raw_bit(u32 levelIdx, s32 jinjoIdx)
{
    s32 bitIdx = jinjodata_get_bit_idx(levelIdx, jinjoIdx);
    if (bitIdx < 0)
        return FALSE;

    u32 byte = bitIdx >> 3;
    u32 bit  = bitIdx &  7;

    return (jinjodata_get_byte(byte) & (1 << bit)) != 0;
}

/**
 * Use if we just want to "copy" the jinjobits from the saved jinjo bitfield.
 * Even if the final jinjo is collected, the bit for it should be 0.
 */
static u32 SavedJinjo_get_raw_jinjobits(u32 levelIdx)
{
    u32 jinjobits = 0;

    for (u32 i = 0; i < 5; i++)
        if (SavedJinjo_get_collected_raw_bit(levelIdx, i))
            jinjobits |= (1 << i);

    return jinjobits;
}

/**
 * Can clear each bit individually, but won't _enable_ the fifth collected
 * jinjo's bit.
 * 
 * i.e. Automatically handles setting the final "all collected" flag if
 * appropriate, but does not automatically clear it.
 */
static void SavedJinjo_set_collected(u32 levelIdx, s32 jinjoIdx, bool collected)
{
    if (!jinjo_bit_idx_is_valid(jinjoIdx))
        return;

    // Check to see if getting this jinjo would give us all five.
    // If so, just set the "all collected" flag instead.
    // (this block is only relevant when collecting a jinjo, not clearing it)
    if (collected)
    {
        u32 jinjobits = SavedJinjo_get_raw_jinjobits(levelIdx);

        jinjobits |= 1 << jinjoIdx;

        if (jinjobits == 0b11111)
            // Swap out idx to use.
            // This retains the final collected jinjo bit as 0
            jinjoIdx = SAVEDJINJO_IDX_ALL_COLLECTED;
    }

    s32 bitIdx = jinjodata_get_bit_idx(levelIdx, jinjoIdx);
    if (bitIdx < 0)
        return;

    u32 byte = bitIdx >> 3;
    u32 bit  = bitIdx &  7;

    jinjodata_set_byte(
        byte,
        collected
            ? jinjodata_get_byte(byte) |  (1 << bit)
            : jinjodata_get_byte(byte) & ~(1 << bit)
    );
}

static bool SavedJinjo_is_jinjo_collected(u32 levelIdx, s32 jinjoIdx)
{
    return SavedJinjo_get_collected_raw_bit(levelIdx, SAVEDJINJO_IDX_ALL_COLLECTED)
        || SavedJinjo_get_collected_raw_bit(levelIdx, jinjoIdx);
}

/**
 * Mark all jinjos as collected. This is "safe" to re-run even if they are
 * already marked as collected, as `SavedJinjo_set_collected` will leave
 * at least one jinjo bit as 0.
 */
static void SavedJinjo_ensure_all_marked_collected_for_level(u32 levelIdx)
{
    for (u32 i = 0; i < SAVEDJINJO_NUM_BITS_PER_LEVEL - 1; i++)
        SavedJinjo_set_collected(levelIdx, i, TRUE);
}

static void SavedJinjo_clear_all_for_level(u32 levelIdx)
{
    for (u32 i = 0; i < SAVEDJINJO_NUM_BITS_PER_LEVEL; i++)
        SavedJinjo_set_collected(levelIdx, i, FALSE);
}

/**
 * Returns the jinjo idx at which the spawned jiggy should be restored.
 * Returns -1 if there's no spawned jiggy to restore.
 */
static s32 SavedJinjo_restore_spawned_jiggy_at_given_jinjo_idx(u32 levelIdx)
{
    if (jiggyscore_isCollected(get_jiggy_idx_for_jinjo_jiggy(levelIdx)))
        // Jinjo jiggy collected, no need to spawn
        return -1;

    // Check if the "all collected" flag is set
    if (!SavedJinjo_get_collected_raw_bit(levelIdx, SAVEDJINJO_IDX_ALL_COLLECTED))
        // Not set, no need to restore
        return -1;

    // Flag set, we need to restore the jiggy. Get the idx

    for (u32 i = 0; i < 5; i++)
        if (!SavedJinjo_get_collected_raw_bit(levelIdx, i))
            // Found one of the jinjo flags set to 0
            // This denotes the idx of the jinjo at which to spawn the jiggy
            return i;

    // Shouldn't happen, but just default to not spawning the jiggy
    return -1;
}


/*********************** Hooks */

/***** Hook jinjo collision
 * Mark as permanently collected in the saved jinjo data
 */
static void jinjo_collision(ActorMarker *this, ActorMarker *other)
{
    Actor *actor = marker_getActor(this);

    if (actor->state < 5)
    {
        // Make sure we're not in a demo before we change any saved jinjo data
        if (!recomp_in_demo_playback_game_mode())
        {
            /**
             * If this is the first jinjo collected in the level, reset saved jinjos.
             * 
             * This addresses a situation where a user may toggle jinjo saving on and
             * off a few times, where the saved jinjo state may not correlate with what
             * the user last remembers.
             *
             * _______________________________________________________________________
             * | An example of the inconsistency at play:
             * |   Step 1: jinjo saving disabled
             * |   Step 2: enter level (run 1)
             * |   Step 3: collect jinjos P then Y then B (not G or O)
             * |   Step 4: leave level
             * |   Step 5: enter level (run 2)
             * |   Step 6: collect jinjos P then G then O (not Y or B)
             * |   Step 7: leave level
             * |   Step 8: jinjo saving enabled
             * |   Step 9: enter level (run 3)
             * |   Result: All jinjos gone, jiggy is at jinjo O (orange)!
             * |
             * | The jiggy spawning at the orange jinjo would be surprising to a player
             * | since on their most recent run, they left behind Y and B. They would
             * | expect the jiggy to be at Y or B, if they expected a jiggy at all.
             * |
             * | But internally, this behaviour is consistent! They have _at some point_
             * | collected all five jinjos, with orange being the 5th they ever collected
             * | for this level on this file.
             * |
             * | This confusion should only take place when the following occurs:
             * |   - a version of BanjoRecompiled with jinjo saving is used, then:
             * |   - user collects jinjos on this level (and on this file) at least once
             * |     (with or without jinjo saving enabled)
             * |   - user leaves (exits game, leaves level, etc)
             * |   - user turns off jinjo saving, and collects >= 1 jinjo again in the
             * |     same level at least once, but in a different order to their previous
             * |     playthrough
             * |   - user leaves
             * |   - user turns on jinjo saving and enters that level again
             * |
             * | Solution:
             * |   This code resets jinjos on run 2 and leaves jinjos Y and B uncollected
             * |   for run 3, with the jiggy spawning once those are both collected.
             * |   This behaviour would feel the most natural for the player.
             * |_______________________________________________________________________
             */
            {
                // Since we run this before the vanilla collision function, we check for a score of 0.
                // This should work regardless of jinjo saving's enabled state.
                if (get_curr_vanilla_jinjobits() == 0)
                    SavedJinjo_clear_all_for_level(level_get());
            }

            /**
             * Now save collected status.
             * 
             * Don't check whether jinjo saving is enabled, collected jinjos should always save.
             * Only _restoring_ them is conditional.
             */
            SavedJinjo_set_collected(level_get(), jinjo_get_jinjoidx_by_actorId(actor->modelCacheIndex), TRUE);
        }
    }

    // Vanilla
    __chJinjo_802CDBA8(this, other);
}

/***** Hook jinjo update
 * 1. Despawn if already collected
 * 2. Spawn the jiggy if it wasn't collected
 * 3. Replace the collision pointer
 */
RECOMP_PATCH
void chJinjo_update(Actor * this)
{
    if (!this->initialized && jinjo_saving_enabled_cached)
    {
        // Check if this jinjo has already been marked as collected before even being born (loaded)

        u32 levelIdx      = level_get();
        s32 currJinjoIdx  = jinjo_get_jinjoidx_by_actorId(this->modelCacheIndex);
        u32 jinjoJiggyIdx = get_jiggy_idx_for_jinjo_jiggy(levelIdx);

        if (jinjo_bit_idx_is_valid(currJinjoIdx) && SavedJinjo_is_jinjo_collected(levelIdx, currJinjoIdx))
        {
            // Check if we need to respawn the jiggy in its place
            if (currJinjoIdx == SavedJinjo_restore_spawned_jiggy_at_given_jinjo_idx(levelIdx))
            {
                // We need to spawn the jiggy at this jinjo's position

                vec3f jiggypos = *(vec3f *)&this->position;

                // Spawn jiggy silently, and start higher
                {
                    sJinjoJiggySpawnPosition = jiggypos;

                    jiggypos.y += 50;

                    // Check if we've just entered the map.
                    sJinjoJiggySpawnedOnMapEnter = get_global_timer() <= sMapInitialVars.counter + 5;

                    codeABC00_spawnJiggyAtLocation(jinjoJiggyIdx, (f32 *)&jiggypos);
                }
            }
        }
        else if (jiggyscore_isCollected(jinjoJiggyIdx))
        {
            /**
             * Odd. The jiggy is collected, but we have an uncollected jinjo.
             * Mark all jinjos as collected, and remember them.
             * 
             * --------------------------------------------------------------
             * 
             * This is a failsafe that restores the collected jinjos if the
             * jinjo jiggy was previously collected using a build that didn't
             * include jinjo saving.
             * 
             * Running this unconditionally for all mods would also show all
             * jinjos collected for mods that DON'T have jinjos in the lair,
             * but have that same jiggy collected.
             * 
             * So we need to conditionally run this only if we know there are
             * jinjos in the level. We ensure this by only running inside the
             * jinjo update function.
             * 
             * This can cause a potential issue where the collected jinjos
             * aren't shown in the pause menu until a map in the level that
             * contains at least one jinjo is entered. But since this failsafe
             * only needs to happen once per-level per-file, and that most
             * entrance maps for levels should contain a jinjo (except e.g. CCW),
             * this should be acceptable.
             * 
             * The item count restore in particular (to D_80385F30) is necessary
             * for the first time this failsafe is triggered, to show the jinjos
             * correctly on pause. From the next level enter, it should be
             * auto-restored with the standard saved jinjo data.
             */
            SavedJinjo_ensure_all_marked_collected_for_level(levelIdx);
            D_80385F30[ITEM_12_JINJOS] = 0b11111;
        }
        else
        {
            // This jinjo should stay, skip despawn. Proceed as normal
            goto vanilla_update;
        }

        // Don't init
        this->initialized = TRUE;

        // Despawn, and don't proceed to vanilla update
        return marker_despawn(this->marker);
    }

    // Vanilla update body
    vanilla_update:
    {
        f32 sp7C[3];
        f32 sp70[3];
        f32 sp6C;
        f32 sp68; //unused??
        s16 sp66;
        s16 sp64;
        s32 sp60;
        // f32 sp5C;
        ActorLocal_Jinjo *local; //= &this->jinjo; //sp34
        f32 sp58 = time_getDelta();
        s32 i;
        int sp50;
        f32 *sp30;
        
        //f32 sp4C; //unused
        f32 sp40[3];
        //s32 sp3C;
        //s32 sp38;
        
        f32 tmp_f0;
        
        
        local = &this->jinjo;
        if(!this->initialized){
            this->initialized = TRUE;
            local->unk0 = 1;
            local->unk4 = 0;
            local->unk8 = (this->position_y < func_80309B24(this->position));

            // @recomp Replace jinjo collision function too
            this->marker->collisionFunc = jinjo_collision;
            // this->marker->collisionFunc = __chJinjo_802CDBA8;

            marker_setFreeMethod(this->marker, __chJinjo_802CDD3C);
            if(volatileFlag_get(VOLATILE_FLAG_C1_IN_FINAL_CHARACTER_PARADE)){
                marker_despawn(this->marker);
            }
        }//L802CDE24
        sp30 = this->position;
        func_8028E964(sp7C);
        func_80257F18(sp30, sp7C, &sp6C);
        sp64 = (this->yaw * 182.04444);
        // sp66 = sp64 - (s32)(sp6C*182.04444);
        sp66 = (s32) (sp6C * 182.04444);
        sp66 = sp64 - sp66;
        sp60 = func_8028AED4(sp30, 55.0f);

        switch(this->state){
            case 1:
                if(randf() < 0.015){
                    if(sp60){
                        subaddie_set_state_with_direction(this, 3, 0.0f, -1);
                    }else{
                        subaddie_set_state_with_direction(this, 2, 0.0f, -1);
                    }
                    actor_playAnimationOnce(this);
                }//L802CDF24
                break;

            case 2: /* 46FA0 802CDF30 3C053F7D */
            case 3:
                if(actor_animationIsAt(this, 0.99f)){
                    subaddie_set_state_with_direction(this, 1, 0.0f, -1);
                }
                break;
            
            case 4: /* 46FD8 802CDF68 3C053F7D */
                if(actor_animationIsAt(this, 0.99f)){
                    subaddie_set_state_with_direction(this, 1, 0.0f, -1);
                }else{//L802CDF9C
                    tmp_f0 = this->yaw;
                    if(sp66 >= 0){
                        tmp_f0 -= 80.0f * sp58;
                    }
                    else{//L802CDFBC
                        tmp_f0 += 80.0f * sp58;
                    }//L802CDFD0
                    if(tmp_f0 >= 360.0f)
                        tmp_f0 -= 360.0f;
                    else if(tmp_f0 < 0.0f)
                        tmp_f0 += 360.0f;

                    this->yaw = tmp_f0;
                }//L802CE018
                break;
            case 6:/* 47094 802CE024 02002025 */
                __chJinjo_802CDC9C(this, sp66);
                if(actor_animationIsAt(this, 0.0f) && --(local->unk0) == 0){
                    subaddie_set_state_with_direction(this, 7, 0.0f, -1);
                    actor_playAnimationOnce(this);
                    if(local->unk4){
                        sp40[0] = this->position_x;
                        sp40[1] = this->position_y;
                        sp40[2] = this->position_z;
                        sp40[1] += 50.0f;
                        jiggy_spawn(10*level_get()-9, sp40);
                    }//L802CE0CC
                    core1_ce60_incOrDecCounter(FALSE);
                    func_8032BB88(this, 0, 4000);
                    if(local->unk4){
                        coMusicPlayer_playMusic(COMUSIC_30_5TH_JINJO_COLLECTED, 28000);
                    }else{
                        coMusicPlayer_playMusic(COMUSIC_A_JINJO_COLLECTED, 28000);
                    }
                }//L802CE114
                break;

            case 7:
            case 8:
                sp50 = this->state == 7;
                if(!sp50 || 0.1 < anctrl_getAnimTimer(this->anctrl)){//L802CE158
                    player_getVelocity(sp70);
                    sp70[0] *= sp58*6.0f;\
                    sp70[1] *= sp58*6.0f;\
                    sp70[2] *= sp58*6.0f;
                    sp70[0] = sp70[0] + sp7C[0];\
                    sp70[1] = sp70[1] + sp7C[1];\
                    sp70[2] = sp70[2] + sp7C[2];
                    sp70[0] -=  this->position_x;\
                    sp70[1] -=  this->position_y;\
                    sp70[2] -=  this->position_z;
                    sp70[0] *= sp58*3.0f;
                    sp70[1] *= sp58*3.0f;
                    sp70[2] *= sp58*3.0f;
                    this->position_x = this->position_x + sp70[0];
                    this->position_y = this->position_y + sp70[1];
                    this->position_z = this->position_z + sp70[2];
                    if(sp50 || anctrl_getAnimTimer(this->anctrl) < 0.8){
                        for(i = 0; i < 4; i++){
                            if(randf() < 0.3){
                                commonParticle_add((s32)this->marker, i + 5, func_80329904);
                                commonParticle_new(8, this->marker->unk14_21);
                            } //L802CE2C4
                        }
                    }//L802CE2D0
                }

                if(sp50){ 
                    if(anctrl_getAnimTimer(this->anctrl) < 0.2)
                        __chJinjo_802CDC9C(this, sp66);

                    if(actor_animationIsAt(this, 0.1f)){
                        local->unkC = func_802F9AA8(SFX_18_BIGBUTT_SLIDE);
                        func_802F9EC4(local->unkC, (s32)sp30, 500, 2000);
                        func_802F9F80(local->unkC, 0.0f, 9e+09, 0.0f);
                        func_802FA0B0(local->unkC, 0);
                        coMusicPlayer_playMusic(COMUSIC_43_ENTER_LEVEL_GLITTER, 0x7FFF);
                        func_8025AABC(COMUSIC_43_ENTER_LEVEL_GLITTER);
                        func_8030E9C4(SFX_C7_SHWOOP, 0.8f, 0x7FFF, sp30, 300.0f, 2000.0f);
                    }//L802CE3C4

                    if(actor_animationIsAt(this, 0.434f)){
                        func_8030E9C4(SFX_C7_SHWOOP, 0.9f, 0x7fff, sp30, 300.0f, 2000.0f);
                    }//L802CE408
                    
                    if(actor_animationIsAt(this, 0.811f)){
                        func_8030E9C4(SFX_C7_SHWOOP, 1.0f, 0x7fff, sp30, 300.0f, 2000.0f);
                    }

                }else{//L802CE450
                    if(actor_animationIsAt(this,0.214f)){
                        func_8030E9C4(SFX_C7_SHWOOP, 1.1f, 0x7fff, sp30, 300.0f, 2000.0f);
                    }//L802CE490

                    if(actor_animationIsAt(this,0.55f)){
                        func_8030E9C4(SFX_53_BANJO_HUIII, 1.5f, 0x7fff, sp30, 300.0f, 2000.0f);
                    }//L802CE4D0

                    if(actor_animationIsAt(this,0.75f)){ 
                        func_80326310(this);
                        if(local->unk4 == 0){
                            func_8032BB88(this, -1, 4000);
                            core1_ce60_incOrDecCounter(TRUE);
                        }
                    }//L802CE518

                    if(actor_animationIsAt(this,0.85f)){ 
                        if(local->unkC){
                            func_802F9D38(local->unkC);
                            local->unkC = 0;
                        }
                        func_8030E4E4(SFX_19_BANJO_LANDING_08);
                        func_8025A7DC(COMUSIC_43_ENTER_LEVEL_GLITTER);
                    }//L802CE558

                    if(local->unk4 && actor_animationIsAt(this,0.95f)){
                        func_8032BB88(this, -1, 4000);
                        core1_ce60_incOrDecCounter(TRUE);
                    }
                }//L802CE598

                if(sp50){
                    if(actor_animationIsAt(this, 0.96f) || actor_animationIsAt(this, 0.99f)){
                        subaddie_set_state_with_direction(this, 8, 0.0f, -1);
                        actor_playAnimationOnce(this);
                    }
                }
                break;
        }//L802CE5F0
        if(this->state < 4 &&  !(((sp66 >= 0)? sp66: -sp66) <= 0x1000)){
            subaddie_set_state_with_direction(this, 4, 0.0f, -1);
            actor_playAnimationOnce(this);
        }//L802CE630
        if(!gcdialog_hasCurrentTextId()){
            switch(chJinjoAnimations[this->state].index){
                case ASSET_31_ANIM_JINJO_JUMP: 
                    if(actor_animationIsAt(this, 0.6f)){
                        if(local->unk8){
                            func_8030E988(SFX_8_BANJO_LANDING_04, 1.8f, 18000, sp30, 120.0f, 1200.0f);
                        }else{
                            sfx_playFadeShorthandDefault(SFX_8_BANJO_LANDING_04, 1.8f, 18000, sp30, 120, 1200);
                        }
                    }
                    break;
                case ASSET_2D_ANIM_JINJO_IDLE: //L802CE6F4
                    if(actor_animationIsAt(this, 0.2f)){
                        if(local->unk8){
                            func_8030E988(SFX_17_JINJO_WHISTLE, 1.0f, 22000, sp30, 120.0f, 1200.0f);
                        }else{
                            sfx_playFadeShorthandDefault(SFX_17_JINJO_WHISTLE, 1.0f, 22000, sp30, 120, 1200);
                        }
                    }
                    break;
                case ASSET_2F_ANIM_JINJO_HELP: //L802CE760
                    if(actor_animationIsAt(this, 0.2f)){
                        if(local->unk8){
                            func_8030E988(SFX_27_JINJO_HI, 1.0f, 22000, sp30, 120.0f, 1200.0f);
                        }else{
                            sfx_playFadeShorthandDefault(SFX_27_JINJO_HI, 1.0f, 22000, sp30, 120, 1200);
                        }
                    }
                    break;
                default: //L802CE7C8
                    break;
            }
        }//L802CE7CC
    }
}

/**
 * Unlike the note saving setting, which can instantly update while
 * the player is in the lair, we support saving jinjos in the lair
 * itself. So we can't cache the setting there either.
 *
 * We need to do it IN-BETWEEN levels.
 */
static void jinjo_saving_enabled_update(void)
{
    jinjo_saving_enabled_cached = jinjo_saving_override_disabled
        ? FALSE
        : bkrecomp_jinjo_saving_enabled();
}

/**
 * Used when restoring jinjo count on level entry
 */
static u32 jinjo_saving_get_counters(enum level_e level)
{
    u32 jinjobits = 0;

    if (jinjo_get_level_lookup_subtract_amount(level) < 0)
    {
        // No jinjos, clear all
        jinjobits = 0b00000;
    }
    else if (SavedJinjo_get_collected_raw_bit(level, SAVEDJINJO_IDX_ALL_COLLECTED))
    {
        // Set all in a single assignment
        // Looping over as in the below loop WON'T work, due to how we save the index of the final collected jinjo
        jinjobits = 0b11111;
    }
    else
    {
        // Loop over the jinjos and get each individually
        // Note that this doesn't work if all 5 are set. That's what the above branch is for
        jinjobits = SavedJinjo_get_raw_jinjobits(level);
    }

    return jinjobits;
}

/**
 * This item reset happens before <jinjo_saving_on_map_load>,
 * on each new level.
 * 
 * So we update enabled state here.
 */
void jinjo_saving_on_item_reset(enum level_e level)
{
    // Update enabled state before checking if we should restore jinjo counts
    jinjo_saving_enabled_update();

    if (jinjo_saving_enabled_cached)
        D_80385F30[ITEM_12_JINJOS] = jinjo_saving_get_counters(level);
}

void jinjo_saving_on_map_load(void)
{
    if (map_get() != sMapInitialVars.map)
    {
        if (level_get() != sMapInitialVars.level)
            // Reset on new level
            sJinjoJiggySpawnedOnMapEnter = FALSE;

        sMapInitialVars.map     = map_get();
        sMapInitialVars.level   = level_get();
        sMapInitialVars.counter = get_global_timer();
    }
}

void jinjo_saving_update(void)
{
    // Iterate over all active objects and clamp the jinjo jiggy's position if required
    if (jinjo_saving_enabled_cached && sJinjoJiggySpawnedOnMapEnter)
    {
        if (suBaddieActorArray != NULL)
        {
            for (s32 i = 0; i < suBaddieActorArray->cnt; i++)
            {
                Actor *this = &suBaddieActorArray->data[i];

                if (this->modelCacheIndex == ACTOR_46_JIGGY)
                {
                    u32 jinjoJiggyIdx = get_jiggy_idx_for_jinjo_jiggy(level_get());
                    u32 jiggyId       = chjiggy_getJiggyId(this);

                    // Check if this is the jinjo jiggy for this level
                    if (jiggyId == jinjoJiggyIdx)
                    {
                        /**
                         * So now we know that this is the jinjo jiggy for this level,
                         * and that it was restored by us.

                         * Restored jinjo jiggies can clip through the floor if out of view
                         * of the camera. This is a bug also present on XBLA.
                         * 
                         * Assume that any jinjo jiggy that spawns at the time of us entering
                         * the map can potentially clip out of bounds. Keep it in place.
                         */

                        this->position[0] = sJinjoJiggySpawnPosition.x;
                        this->position[1] = clamp_f32(this->position[1], sJinjoJiggySpawnPosition.y, sJinjoJiggySpawnPosition.y + 20.f);
                        this->position[2] = sJinjoJiggySpawnPosition.z;
                    }
                }
            }
        }
    }
}


