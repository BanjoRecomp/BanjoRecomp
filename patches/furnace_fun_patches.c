#include "patches.h"
#include "functions.h"
#include "../lib/bk-decomp/src/core2/code_C9E70.h"

enum FF_TileType {
    FFTT_0_NIL,
    FFTT_1_BANJO,
    FFTT_2_PICTURE,
    FFTT_3_MUSIC,
    FFTT_4_MINIGAME,
    FFTT_5_GRUNTY,
    FFTT_6_SKULL,
    FFTT_7_JOKER
};

enum FF_Action {
    FFA_0_NIL,
    FFA_1_UNK,
    FFA_2_ON_BOARD_FORGET_MOVES,
    FFA_3_TRIGGER_QUESTION,
    FFA_4_UNK,
    FFA_5_FORGET_MOVES_2,
    FFA_6_TRIGGER_QUESTION_POST_EFFECTS,
    FFA_7_UNK,
    FFA_8_FURNACE_FUN_COMPLETE
};

typedef struct {
    u8 unk0;
    s16 unk2;
    s16 unk4;
    f32 unk8;
} FFQuestionInfo;

extern struct FF_StorageStruct* D_8037DCB8;
extern FFQuestionInfo D_80394354[];

extern void gcquiz_func_80319EA4(void);
extern void func_8038C9D0(void);
extern void* lair_func_8038C5B8(s32);
extern void lair_func_8038C640(s32, Struct_lair_5ED0_0*);
extern void func_8038D670(enum FF_Action);
extern enum ff_question_type_e func_8038DCD4(enum FF_TileType);
extern void func_8038DE34(enum ff_question_type_e);
extern void func_8038DFBC(void);
extern void func_8038E070(void);
extern s32 func_8033F3E8(BKModel*, f32[3], s32, s32);
extern enum map_e map_get(void);
extern bool func_8028F20C(void);
extern bool func_8028EFEC(void);
extern bool func_8028EFC8(void);
extern enum bsgroup_e player_movementGroup(void);
extern bool fileProgressFlag_get(enum file_progress_e);
extern void fileProgressFlag_set(enum file_progress_e, s32);
extern void progressDialog_setAndTriggerDialog_4(enum volatile_flags_e);
extern s32 item_getCount(enum item_e);
extern void item_dec(enum item_e);
extern void func_802FACA4(enum item_e);
extern void func_8028FA14(enum map_e, s32);
extern void func_80324C58(void);
extern void func_803114D0(void);

// Patched to trigger the The Furnace Fun skull-panel warning with 0 lives left instead of 1.
RECOMP_PATCH void lair_func_8038E0B0(void) {
    s32 sp48[6];
    s32 temp_v0;
    // controller_copySideButtons() writes Z/L/R, so it needs dst[3], but
    // the decomp declares sp3C[2]. Harmless in the original stack frame, but once
    // recompiled, the R button write spills into the A button slot, so the Furnace Fun
    // panels can only be activated with R instead of A. Sizing it [3] fixes this.
    s32 sp3C[3];
    s32 sp38;
    s32 sp28;

    if ((map_get() == MAP_8E_GL_FURNACE_FUN)
        && (D_8037DCB8 != NULL)
        && (D_8037DCB8->unk0 != NULL)) {
        gcquiz_func_80319EA4();
        func_8038C9D0();
        controller_copyFaceButtons(0, sp48);
        controller_copySideButtons(0, sp3C);
        if (D_8037DCB8->currFfMode < 3) {
            player_getPosition(D_8037DCB8->playerPosition);
            temp_v0 = func_8033F3E8(D_8037DCB8->unk0, D_8037DCB8->playerPosition, 0x191, 0x1F0);
            if ((temp_v0 != D_8037DCB8->unk8) && (D_8037DCB8->unk8 != 0)) {
                if (D_8037DCB8->unk4->unk9 == 2) {
                    D_8037DCB8->unk4->unk9 = 0U;
                }
            }
            D_8037DCB8->unk8 = temp_v0;
            D_8037DCB8->unk4 = lair_func_8038C5B8(D_8037DCB8->unk8);
        }
        sp38 = MIN((D_8037DCB8->unk8 != 0) ? D_8037DCB8->unk4->unk8 : -1, FFTT_7_JOKER);
        if ((D_8037DCB8->unk8 != 0) && (D_8037DCB8->unk4->unk9 == 0) && func_8028F20C()) {
            D_8037DCB8->unk4->unk9 = 2;
            if (D_8037DCB8->unk11) {
                switch (sp38) {
                    case FFTT_6_SKULL:
                        comusic_playTrack(COMUSIC_7B_STEP_ON_SKULL_TILE);
                        break;

                    case FFTT_5_GRUNTY:
                        comusic_playTrack(COMUSIC_7C_STEP_ON_GRUNTY_TILE);
                        break;

                    case FFTT_1_BANJO:
                        comusic_playTrack(COMUSIC_7D_STEP_ON_BK_TILE);
                        break;

                    case FFTT_7_JOKER:
                        comusic_playTrack(COMUSIC_7E_STEP_ON_MINIGAME_TILE);
                        break;

                    case FFTT_3_MUSIC:
                        comusic_playTrack(COMUSIC_7F_STEP_ON_JOKER_TILE);
                        break;

                    case FFTT_2_PICTURE:
                        func_8030E6D4(SFX_144_DOUBLE_CAMERA_CLICK);
                        break;

                    case FFTT_4_MINIGAME:
                        func_8038DFBC();
                        break;
                }
                D_8037DCB8->unk11 = FALSE;
            }
        } else {
            D_8037DCB8->unk11 = TRUE;
        }

        if ((D_8037DCB8->currFfMode >= 2) && (D_8037DCB8->currFfMode < 8)
            && (item_getCount(ITEM_27_JOKER_CARD) != 0)) {
            func_802FACA4(ITEM_27_JOKER_CARD);
        }
        func_8028FA14(MAP_8E_GL_FURNACE_FUN, 2);
        switch (D_8037DCB8->currFfMode) {
            case 1:
                if (D_8037DCB8->unk8 != 0) {
                    func_80347A14(0);
                    func_8038D670(FFA_2_ON_BOARD_FORGET_MOVES);
                }
                break;

            case 2:
                if (D_8037DCB8->unk8 == 0) {
                    func_8038D670(FFA_1_UNK);
                    break;
                }
                func_802FACA4(ITEM_14_HEALTH);
                func_802FACA4(ITEM_16_LIFE);
                if (sp38 != FFTT_0_NIL) {
                    sp28 = sp38 - 1 + FILEPROG_55_FF_BK_SQUARE_INSTRUCTIONS;
                    if (!fileProgressFlag_get(sp28) && gcdialog_showDialog(sp38 + 0x101E, 0, NULL, NULL, NULL, NULL)) {
                        fileProgressFlag_set(sp28, TRUE);
                    }

                    // Warning message belongs at 0 lives (last life), not 1. 
                    if ((sp38 == FFTT_6_SKULL) && (item_getCount(ITEM_16_LIFE) == 0)) {
                        progressDialog_setAndTriggerDialog_4(VOLATILE_FLAG_AB_LAST_LIFE_ON_SKULL);
                    } else if (item_getCount(ITEM_14_HEALTH) == 1) {
                        progressDialog_setAndTriggerDialog_4(VOLATILE_FLAG_AA_FF_LOW_HEALTH);
                    }
                    if ((D_8037DCB8->unk4->unk9 == 2) && (player_movementGroup() == BSGROUP_0_NONE)) {
                        if (func_8028EFEC() && (sp48[FACE_BUTTON(BUTTON_A)] == 1)) {
                            func_803114D0();
                            player_getRotation(D_8037DCB8->playerRotation);
                            D_8037DCB8->ffQuestionType = func_8038DCD4(sp38);
                            func_8038DE34(D_8037DCB8->ffQuestionType);
                            func_8038D670(FFA_3_TRIGGER_QUESTION);
                            return;
                        }
                        if (func_8028EFC8() && (sp48[FACE_BUTTON(BUTTON_B)] == 1)) {
                            if ((item_getCount(ITEM_27_JOKER_CARD) > 0) && (sp28 < 0x5B)) {
                                lair_func_8038C640(D_8037DCB8->unk8, D_8037DCB8->unk4);
                                item_dec(ITEM_27_JOKER_CARD);
                                func_8030E6D4(SFX_3EA_BANJO_GUH_HUH);
                                progressDialog_setAndTriggerDialog_4(VOLATILE_FLAG_A9_FF_USED_JOKER);
                                if (D_8037DCB8->unk8 == 0x1EF) {
                                    func_8038D670(FFA_8_FURNACE_FUN_COMPLETE);
                                }
                            } else {
                                comusic_playTrack(COMUSIC_2C_BUZZER);
                            }
                        }
                    }
                } else {
                    if (D_8037DCB8->unk4->unk9 == 2) {
                        lair_func_8038C640(D_8037DCB8->unk8, D_8037DCB8->unk4);
                    }
                }
                break;

            case 3:
                if ((D_8037DCB8->ffQuestionType == 2) && D_80394354[D_8037DCB8->unkC].unk0 == 2) {
                    gczoombox_update(D_8037DCB8->unk20);
                }
                if ((D_8037DCB8->unk12 == 0) && func_8028EFC8() && (sp48[FACE_BUTTON(BUTTON_B)] == 1)) {
                    func_80324C58();
                    func_8038D670(FFA_4_UNK);
                }
                break;

            case 4:
                if (volatileFlag_get(VOLATILE_FLAG_1)) {
                    volatileFlag_set(VOLATILE_FLAG_1, 0);
                    func_8038E070();
                    func_8025A55C(6000, 500, 0xA);
                }
                break;

            case 5:
                if (volatileFlag_get(VOLATILE_FLAG_2_FF_IN_MINIGAME)) {
                    if (volatileFlag_get(VOLATILE_FLAG_4)) {
                        func_8038E070();
                        D_8037DCB8->unkF = volatileFlag_get(VOLATILE_FLAG_5_FF_MINIGAME_WON);
                        func_8038D670(FFA_6_TRIGGER_QUESTION_POST_EFFECTS);
                    } else {
                        func_8038D670(FFA_1_UNK);
                    }
                    volatileFlag_set(VOLATILE_FLAG_2_FF_IN_MINIGAME, FALSE);
                    volatileFlag_set(VOLATILE_FLAG_4, FALSE);
                }
                break;

            case 6:
                if ((D_8037DCB8->unk8 == 0x1EF) && (D_8037DCB8->unkF == 1)) {
                    func_8038D670(FFA_8_FURNACE_FUN_COMPLETE);
                } else {
                    func_8038D670(FFA_2_ON_BOARD_FORGET_MOVES);
                }
                break;

            case 9:
                if (!func_8025AD7C(0x78)) {
                    mapSpecificFlags_set(6, TRUE);
                    func_8038D670(FFA_0_NIL);
                }
                break;
        }
    }
}
