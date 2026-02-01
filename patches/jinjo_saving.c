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
bool fileProgressFlag_get(enum file_progress_e index);
enum asset_e __chJinjo_getMeetDialogId(enum marker_e marker_id);
void fileProgressFlag_set(enum file_progress_e index, s32 set);
s32  item_adjustByDiffWithHud(enum item_e item, s32 diff);
typedef s32 (*FuncUnk40)(ActorMarker *, s32, f32[3]);
void commonParticle_add(s32 actorMarker, s32 arg1, FuncUnk40 arg2);
int commonParticle_new(enum common_particle_e particle_id, int arg1);
enum jiggy_e getJiggyId(Actor *this);
enum jiggy_e chjiggy_getJiggyId(Actor *this);
void func_80343DEC(Actor *this);
void chjiggy_updateRotation(Actor *this);
void destroyJiggy(Actor *this, s32 jiggyFlag, s32 timerRunningFlag, s32 cameraId, s32 switchPressedFlag, s32 resetSwitchFlag, enum volatile_flags_e dialogFlag);
s32 levelSpecificFlags_get(s32 i);
s32 func_80341F2C(s32 arg0);
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

extern ActorInfo chJinjoBlue;
extern ActorInfo chJinjoGreen;
extern ActorInfo chJinjoOrange;
extern ActorInfo chJinjoPink;
extern ActorInfo chJinjoYellow;

extern s32 D_80385F30[0x2C];
extern u32 gGlobalTimer;
extern struct
{
    u8 unk0;
    u8 level;
} D_80383300;
extern ActorArray *suBaddieActorArray;


/*********************** Declarations (recomp) */

bool recomp_in_demo_playback_game_mode();


/*********************** Statics */

#define SAVEDJINJO_IDX_ALL_COLLECTED  (5)
#define SAVEDJINJO_NUM_BITS_PER_LEVEL (6)

static vec3f sJinjoJiggySpawnPosition     = {0};
static bool  sJinjoJiggyRestoredForLevel  = FALSE;
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


/*********************** Helpers */

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

static u32 get_jiggy_idx_for_jinjo_jiggy(u32 levelIdx)
{
    // The jinjo jiggy is the first jiggy in each level's list of jiggy IDs
    return ((levelIdx - 1) * 10) + 1;
}

/**
 * We want to have a tightly packed jinjo bitfield, where all bits
 * used are from levels that exist. Since grunty's lair's level idx
 * is right in between all others, we need to "shift" the levelIdx
 * slightly when accessing the bitfield for gets/sets.
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
            // Map to zero-indexed
            return 1;
        case LEVEL_7_GOBIS_VALLEY:
        case LEVEL_8_CLICK_CLOCK_WOOD:
        case LEVEL_9_RUSTY_BUCKET_BAY:
        case LEVEL_A_MAD_MONSTER_MANSION:
            // Map to zero-indexed
            // Level idx needs to be reduced by an additional 1 to be used with the contiguous bitfield
            return 2;
        case LEVEL_6_LAIR:
        case LEVEL_B_SPIRAL_MOUNTAIN:
        case LEVEL_C_BOSS:
        case LEVEL_D_CUTSCENE:
        default:
            // No jinjos on these levels
            return -1;
    }
}

enum JinjoIdx
{
    JINJOIDX_0_BLUE,
    JINJOIDX_1_GREEN,
    JINJOIDX_2_ORANGE,
    JINJOIDX_3_PINK,
    JINJOIDX_4_YELLOW,
};

static s32 jinjo_get_jinjoidx_by_actorId(u32 actorId)
{
    switch (actorId)
    {
        // ordered by their marker idx
        case ACTOR_60_JINJO_BLUE:   return JINJOIDX_0_BLUE;
        case ACTOR_62_JINJO_GREEN:  return JINJOIDX_1_GREEN;
        case ACTOR_5F_JINJO_ORANGE: return JINJOIDX_2_ORANGE;
        case ACTOR_61_JINJO_PINK:   return JINJOIDX_3_PINK;
        case ACTOR_5E_JINJO_YELLOW: return JINJOIDX_4_YELLOW;
        default:
            return -1;
    }
}

/**
 * Will not return proper individualised results if all jinjos are collected.
 * Pass `SAVEDJINJO_IDX_ALL_COLLECTED` for jinjoIdx to get the final "all collected" flag
 */
static bool SavedJinjo_get_collected_bit(u32 levelIdx, u32 jinjoIdx)
{
    s32 levelSubtract = jinjo_get_level_lookup_subtract_amount(levelIdx);
    if (levelSubtract < 0)
        return FALSE;
    levelIdx -= levelSubtract;

    u32 bitIdx = (levelIdx * SAVEDJINJO_NUM_BITS_PER_LEVEL) + jinjoIdx;

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
        if (SavedJinjo_get_collected_bit(levelIdx, i))
            jinjobits |= (1 << i);

    return jinjobits;
}

/**
 * Pass `SAVEDJINJO_IDX_ALL_COLLECTED` for jinjoIdx to set the final "all collected" flag
 */
static void SavedJinjo_set_collected(u32 levelIdx, u32 jinjoIdx, bool collected)
{
    s32 levelSubtract = jinjo_get_level_lookup_subtract_amount(levelIdx);
    if (levelSubtract < 0)
        return;

    // Check to see if getting this jinjo would give us all five.
    // If so, just set the "all collected" flag instead.
    {
        u32 jinjobits = SavedJinjo_get_raw_jinjobits(levelIdx);

        jinjobits |= 1 << jinjoIdx;

        if (jinjobits == 0b11111)
            // Swap out idx to use.
            // This retains the final collected jinjo bit as 0
            jinjoIdx = SAVEDJINJO_IDX_ALL_COLLECTED;
    }

    // We do this after the jinjo all collected check, else we would subtract this multiple times
    levelIdx -= levelSubtract;

    u32 bitIdx = (levelIdx * SAVEDJINJO_NUM_BITS_PER_LEVEL) + jinjoIdx;

    u32 byte = bitIdx >> 3;
    u32 bit  = bitIdx &  7;

    jinjodata_set_byte(
        byte,
        collected
            ? jinjodata_get_byte(byte) |  (1 << bit)
            : jinjodata_get_byte(byte) & ~(1 << bit)
    );
}

/**
 * Returns -1 if there's no spawned jiggy to restore.
 * Else, returns the jinjo idx at which the spawned jiggy should be restored.
 */
static s32 SavedJinjo_restore_spawned_jiggy_at_given_jinjo_idx(u32 levelIdx)
{
    u32 jinjoJiggyIdx = get_jiggy_idx_for_jinjo_jiggy(levelIdx);

    bool jinjoJiggyCollected = jiggyscore_isCollected(jinjoJiggyIdx);
    if (jinjoJiggyCollected)
        // Collected, no need to spawn
        return -1;

    // Check if the "all collected" flag is set
    if (!SavedJinjo_get_collected_bit(levelIdx, SAVEDJINJO_IDX_ALL_COLLECTED))
        // Not set, no need to restore
        return -1;

    // Flag set, we need to restore the jiggy. Get the idx

    for (u32 i = 0; i < 5; i++)
        if (!SavedJinjo_get_collected_bit(levelIdx, i))
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
        // Make sure we're not in a demo
        if (!recomp_in_demo_playback_game_mode())
        {
            // Save collected status
            // Don't check whether jinjo saving is enabled, collected jinjos should always save.
            // Only _restoring_ them is conditional.

            SavedJinjo_set_collected(
                level_get(),
                jinjo_get_jinjoidx_by_actorId(actor->modelCacheIndex),
                TRUE
            );
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
        // Check normal jinjo counters

        u32 levelIdx = level_get();
        s32 jinjoIdx = jinjo_get_jinjoidx_by_actorId(this->modelCacheIndex);

        bool collected =
            SavedJinjo_get_collected_bit(levelIdx, SAVEDJINJO_IDX_ALL_COLLECTED)
            ||
            SavedJinjo_get_collected_bit(levelIdx, jinjoIdx)
        ;

        if (collected)
        {
            // Check if we need to spawn the jiggy
            {
                s32 jinjoIdxToSpawnJiggyAt = SavedJinjo_restore_spawned_jiggy_at_given_jinjo_idx(levelIdx);
                if (jinjoIdxToSpawnJiggyAt == jinjoIdx)
                {
                    // We need to spawn the jiggy at our own position (i.e. this jinjo's position)

                    u32 jinjoJiggyIdx = get_jiggy_idx_for_jinjo_jiggy(levelIdx);

                    vec3f jiggypos = *(vec3f *)&this->position;

                    // Spawn jiggy silently
                    {
                        // Bounce up high, and start higher too

                        sJinjoJiggySpawnPosition    = jiggypos;
                        sJinjoJiggyRestoredForLevel = TRUE;

                        // Check if we've just entered the map.
                        sJinjoJiggySpawnedOnMapEnter = get_global_timer() <= sMapInitialVars.counter + 20;

                        jiggypos.y += 50;

                        codeABC00_spawnJiggyAtLocation(jinjoJiggyIdx, (f32 *)&jiggypos);
                    }
                }
            }

            // Despawn, and don't proceed
            this->initialized = TRUE;

            return marker_despawn(this->marker);
        }
    }

    // Vanilla body
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
 * Used when restoring jinjo count on level entry
 */
u32 jinjo_saving_get_counters(enum level_e level)
{
    u32 jinjobits = 0;

    if (jinjo_get_level_lookup_subtract_amount(level) < 0)
    {
        // No jinjos, clear all
        jinjobits = 0b00000;
    }
    else if (SavedJinjo_get_collected_bit(level, SAVEDJINJO_IDX_ALL_COLLECTED))
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

void jinjo_saving_on_map_load(void)
{
    if (map_get() != sMapInitialVars.map)
    {
        if (level_get() != sMapInitialVars.level)
        {
            // Reset on new level
            sJinjoJiggyRestoredForLevel  = FALSE;
            sJinjoJiggySpawnedOnMapEnter = FALSE;
        }

        sMapInitialVars.map     = map_get();
        sMapInitialVars.level   = level_get();
        sMapInitialVars.counter = get_global_timer();
    }
}

/**
 * Only update our internal "is jinjo saving enabled" variable outside main levels.
 * Same logic as `note_saving_update`.
 */
void jinjo_saving_update(void)
{
    // When in the lair or file select, update the cached jinjo saving enabled state.
    if (level_get() == LEVEL_6_LAIR || map_get() == MAP_91_FILE_SELECT)
    {
        jinjo_saving_enabled_cached = jinjo_saving_override_disabled
            ? FALSE
            : bkrecomp_jinjo_saving_enabled();
    }

    // Iterate over all active objects and clamp the jinjo jiggy's position if required
    if (jinjo_saving_enabled_cached && sJinjoJiggyRestoredForLevel && sJinjoJiggySpawnedOnMapEnter)
    {
        if (suBaddieActorArray != NULL)
        {
            s32 nObjs = suBaddieActorArray->cnt;

            for (s32 i = 0; i < nObjs; i++)
            {
                Actor *this = &suBaddieActorArray->data[i];

                u32 jinjoJiggyIdx = get_jiggy_idx_for_jinjo_jiggy(level_get());
                u32 jiggyId       = chjiggy_getJiggyId(this);

                // Check if this is the jinjo jiggy for this level
                if (jiggyId == jinjoJiggyIdx)
                {
                    /**
                     * So now we know that this is the jinjo jiggy for this level,
                     * and that it was restored by us.
                     * 
                     * It's not possible for `sJinjoJiggyRestoredForLevel` to be TRUE
                     * if we didn't spawn the jiggy ourselves, since after it's set,
                     * the game doesn't spawn this same jiggy naturally, and we cannot
                     * reach this part of the jiggy update function for it.
                     * 
                     * ---
                     * 
                     * Restored jinjo jiggies can clip through the floor if out of view
                     * of the camera.
                     * 
                     * Assume that any jinjo jiggy that spawns at the time of us entering
                     * the map can potentially clip out of bounds. Keep it in place.
                     */

                    this->position[0] = clamp_f32(this->position[0], sJinjoJiggySpawnPosition.x, sJinjoJiggySpawnPosition.x + 2.f);
                    this->position[1] = clamp_f32(this->position[1], sJinjoJiggySpawnPosition.y, sJinjoJiggySpawnPosition.y + 10.f);
                    this->position[2] = clamp_f32(this->position[2], sJinjoJiggySpawnPosition.z, sJinjoJiggySpawnPosition.z + 2.f);
                }
            }
        }
    }
}


