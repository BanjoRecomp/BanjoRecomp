#include "patches.h"

extern s32 getGameMode(void);

// @recomp Check whether the game is currently on a mode that uses demo playback instead of the player's inputs.
bool recomp_in_demo_playback_game_mode() {
    switch (getGameMode()) {
    case GAME_MODE_2_UNKNOWN:
    case GAME_MODE_6_FILE_PLAYBACK:
    case GAME_MODE_7_ATTRACT_DEMO:
    case GAME_MODE_8_BOTTLES_BONUS:
    case GAME_MODE_A_SNS_PICTURE:
    case GAME_MODE_9_BANJO_AND_KAZOOIE:
        return TRUE;
    default:
        return FALSE;
    }
}