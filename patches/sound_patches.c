#include "patches.h"
#include "music.h"
#include "time.h"
#include "core1/ml.h"
#include "../lib/bk-decomp/src/core1/musicplayer.h"
#include "sound.h"

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
    alCSPSetVol(&D_80281720[arg0].cseqp, (s16)(arg1 * recomp_get_bgm_volume()));

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
            temp_lo = var_s0 - musicTracks;
            alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));
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
                alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));

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
            alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));

            var_s0->unk12 = 0;
        }
        // @recomp Always set the volume of every music track in each update loop.
        else {
            temp_lo = var_s0 - musicTracks;
            // @recomp Send a volume command to incorporate the current BGM volume if the track's volume wasn't changed.
            alCSPSetVol(&D_80281720[temp_lo].cseqp, (s16)(var_s0->volume * recomp_get_bgm_volume()));
        }
    }
}
