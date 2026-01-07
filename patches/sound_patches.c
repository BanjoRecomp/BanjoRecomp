#include "patches.h"
#include "music.h"
#include "time.h"
#include "core1/ml.h"
#include "../lib/bk-decomp/src/core1/musicplayer.h"
#include "sound.h"

// @recomp The music player is often used to play sound effects. Therefore, check if track_id is actually music.
bool isMusic(s16 trackId) {
    switch (trackId) {
        case COMUSIC_2_MM:
        case COMUSIC_3_FP_TWINKLIES_TALKING:
        case COMUSIC_4_MMM_CLOCK_VERSION:
        case COMUSIC_5_TTC_VACATION_VERSION:
        case COMUSIC_6_BGS:
        case COMUSIC_7_CC_MUTANT_CRABS:
        case COMUSIC_8_STARTUP_MUSICAL:
        case COMUSIC_F_MMM_ALTERNATIVE:
        case COMUSIC_10_SM:
        case COMUSIC_12_TTC_NIPPER:
        case COMUSIC_13_INSIDE_SANDCASTLE:
        case COMUSIC_1B_MYSTERIOUS_INDOORS:
        case COMUSIC_1C_CC_ALTERNATIVE:
        case COMUSIC_1E_GL_MM_VERSION:
        case COMUSIC_1F_CC_INSIDE_CLANKER:
        case COMUSIC_20_GV_ALTERNATIVE:
        case COMUSIC_21_MMM_INSIDE_MMM_MANSION:
        case COMUSIC_22_MMM:
        case COMUSIC_23_MMM_INSIDE_CHURCH:
        case COMUSIC_24_GV_INSIDE_PYRAMID:
        case COMUSIC_25_USING_GOLD_FEATHERS:
        case COMUSIC_26_GV_SANDYBUTT_DANGER:
        case COMUSIC_27_GV_RUBEES_SONG:
        case COMUSIC_28_SOMETHING_AQUATIC:
        case COMUSIC_29_CCW_SUMMER:
        case COMUSIC_2A_CCW_WINTER:
        case COMUSIC_2E_CCW_FALL_AQUATIC:
        case COMUSIC_2F_CCW_HUBROOM:
        case COMUSIC_32_STARTUP_LOGO_SCENE:
        case COMUSIC_33_RBB_ALTERNATIVE:
        case COMUSIC_34_SNACKER_DANGER:
        case COMUSIC_35_RBB_MASCHINE_ROOM_AQUATIC:
        case COMUSIC_39_DK64_FUNGI_FOREST:
        case COMUSIC_3A_FP_BOGGY_RACE:
        case COMUSIC_41_MUMBOS_HUT:
        case COMUSIC_44_CCW_NABNUT:
        case COMUSIC_45_CCW_NABNUT_ATTIC_A:
        case COMUSIC_46_CCW_SPRING:
        case COMUSIC_47_BGS_INSIDE_TANKTUP:
        case COMUSIC_48_CCW_ALTERNATIVE_A:
        case COMUSIC_49_CCW_ALTERNATIVE_QUICK:
        case COMUSIC_4A_RBB_INSIDE_CONTAINER:
        case COMUSIC_4B_CCW_ZUBBA_FIGHT:
        case COMUSIC_4C_RBB_CREWMATE_CABIN:
        case COMUSIC_50_GL_TTC_VERSION:
        case COMUSIC_51_GL_CCW_VERSION:
        case COMUSIC_52_GL_BGS_RBB_VERSION:
        case COMUSIC_53_GL_FP_VERSION_A:
        case COMUSIC_54_GL_GV_VERSION:
        case COMUSIC_55_BGS_MR_VILE:
        case COMUSIC_56_SM_HANGBRIDGE:
        case COMUSIC_57_TURBO_TRAINERS:
        case COMUSIC_58_WADING_BOOTS:
        case COMUSIC_59_GL_FP_VERSION_B:
        case COMUSIC_5A_FP_IGLOO_SAD:
        case COMUSIC_5B_FP_IGLOO_HAPPY:
        case COMUSIC_5D_GL_MMM_VERSION:
        case COMUSIC_5E_GL_MMM_RBB_VERSION:
        case COMUSIC_5F_CCW_ALTERNATIVE_B:
        case COMUSIC_60_NABNUT_ATTIC_B:
        case COMUSIC_62_RBB_BOOMBOX:
        case COMUSIC_63_GL_FF_VERSION:
        case COMUSIC_64_WORLD_OPENING_A:
        case COMUSIC_65_WORLD_OPENING_B:
        case COMUSIC_66_FP_INSIDE_WOZZAS_CAVE:
        case COMUSIC_68_TWINKLY_MINIGAME:
        case COMUSIC_6A_MMM_TUMBLARS_SHED:
        case COMUSIC_6B_FP_ALTERNATIVE:
        case COMUSIC_6C_INTRO_TOWER_SCENE:
        case COMUSIC_6D_CCW_GNAWTYS_HOUSE:
        case COMUSIC_6E_GAME_SELECT:
        case COMUSIC_6F_PAUSE_SCREEN:
        case COMUSIC_70_MMM_INSIDE_LOGGO:
        case COMUSIC_71_FF:
        case MUSIC_BGS_FLIBBIT_FIGHT:
        case COMUSIC_74_GL_FINAL_SECTION:
        case COMUSIC_7A_BRENTILDA:
        case COMUSIC_83_GV_SNS:
        case COMUSIC_84_TTC_SNS:
        case COMUSIC_85_FP_SNS:
        case COMUSIC_86_MMM_SNS:
        case COMUSIC_87_MMM_SNS_HAPPIER:
        case COMUSIC_88_BIG_SNS_FANFARE:
        case COMUSIC_8E_CREDITS:
        case COMUSIC_94_BBONUS:
        case COMUSIC_95_BBONUS_A:
        case COMUSIC_AC_GOOD_ENDING:
        case MUSIC_MUMBO_BBQ:
        case COMUSIC_A8_KLUNGO_BY_FALLEN_GRUNTY:
        case COMUSIC_AA_BEACH:
        case COMUSIC_91_GRUNTY_FALLING:
        case COMUSIC_80_GAME_OVER_CUTSCENE:
        case COMUSIC_1_FINAL_BATTLE:
            return TRUE;
        default:
            return FALSE;
    }
}

extern MusicTrack D_80281720[6];
extern CoMusic *musicTracks;
extern int D_80276E34;

void func_8024FCE0(u8 arg0, s16 arg1);
bool func_80250074(u8);
void func_8024FC6C(u8 arg0);
void func_8025A7DC(enum comusic_e);
void func_8024FF34(void);
void func_802599B4(CoMusic *this);

// @recomp Patched to incorporate the bgm player volume in the setvol command for music tracks.
RECOMP_PATCH void musicTrack_setVolume(u8 arg0, s16 arg1) {
    D_80281720[arg0].unk0 = arg1;

    if (isMusic(musicTracks[arg0].track_id)) {
        alCSPSetVol(&D_80281720[arg0].cseqp, (s16)(arg1 * recomp_get_bgm_volume()));
    } else {
        alCSPSetVol(&D_80281720[arg0].cseqp, arg1);
    }
    if (D_80281720[arg0].unk3 && arg1) {
        func_8024FCE0(arg0, arg1);
    } else if (!D_80281720[arg0].unk3 && arg1 == 0) {
        if (func_80250074(arg0) == 0) {
            func_8024FC6C(arg0);
        }
    }
}

// @recomp Patched to always set the volume of every music track in each update loop.
RECOMP_PATCH void coMusicPlayer_update(void) {
    s32 temp_lo;
    CoMusic *var_s0;
    f32 dt;

    dt = time_getDelta();

    for (var_s0 = musicTracks; var_s0 < &musicTracks[MAX_MUSIC_STRUCT_COUNT]; var_s0++) {
        if (var_s0->track_id >= 0) {
            temp_lo = var_s0 - musicTracks;
            var_s0->unk4 = ml_min_f(var_s0->unk4 + dt, 600.0f);

            if ((var_s0->unk4 > 1.0f) && func_80250074(temp_lo)) {
                func_8025A7DC(var_s0->track_id);
            }
        }
    }

    func_8024FF34();

    if (!D_80276E34) {
        // @recomp If there are no pending track updates, send a volume command for each track to incorporate the BGM volume.
        for (var_s0 = musicTracks; var_s0 < &musicTracks[6]; var_s0++) {
            if (isMusic(var_s0->track_id)) {
                temp_lo = var_s0 - musicTracks;
                alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));
            }
        }
        return;
    }

    D_80276E34 = FALSE;

    for (var_s0 = musicTracks; var_s0 < &musicTracks[6]; var_s0++) {
        if (var_s0->track_id < 0) {
            continue;
        }

        if (var_s0->unk12 != 0) {
            temp_lo = var_s0 - musicTracks;

            if (var_s0->unk0 > 0.0f) {
                var_s0->unk0 -= time_getDelta();
                D_80276E34 = TRUE;
                
                // @recomp Send a volume command to incorporate the current BGM volume if the track's volume wasn't changed.
                if (isMusic(var_s0->track_id)) {
                    alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));
                }

                continue;
            }

            if (var_s0->unk12 < 0) {
                var_s0->volume += var_s0->unk12;

                if (var_s0->unk15 && (var_s0->unkC == 0) && (var_s0->volume <= 0)) {
                    func_802599B4(var_s0);
                    continue;
                } else {
                    if (var_s0->unkC >= var_s0->volume) {
                        var_s0->volume = var_s0->unkC;
                        var_s0->unk12 = 0;
                    } else {
                        D_80276E34 = TRUE;
                    }
                    musicTrack_setVolume(temp_lo, (s16)var_s0->volume);
                }

                continue;
            }

            if (var_s0->volume < var_s0->unkC) {
                if (var_s0->volume == 0) {
                    var_s0->unk4 = 0.0f;
                }
                var_s0->volume += var_s0->unk12;
                if (var_s0->volume >= var_s0->unkC) {
                    var_s0->volume = var_s0->unkC;
                    var_s0->unk12 = 0;
                } else {
                    D_80276E34 = TRUE;
                }
                musicTrack_setVolume(temp_lo, (s16)var_s0->volume);

                continue;
            }

            // @recomp Send a volume command to incorporate the current BGM volume if the track's volume wasn't changed.
            if (isMusic(var_s0->track_id)) {
                alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));
            }
            

            var_s0->unk12 = 0;
        }
        // @recomp Always set the volume of every music track in each update loop.
        else {
            temp_lo = var_s0 - musicTracks;
            // @recomp Send a volume command to incorporate the current BGM volume if the track's volume wasn't changed.
            if (isMusic(var_s0->track_id)) {
                alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));
            }
        }
    }
}
