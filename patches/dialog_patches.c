#include "patches.h"
#include "functions.h"

extern struct {
    u8 unk0;
    char* ptr;
    s32 index;
} s_dialogBin;

extern s32 code94620_func_8031B5B0(void);

static char dialog_bytes[1024];
char* dialog_byte_cursor = NULL;
char* dialog_byte_character_cursor = NULL;

void start_dialog_bin() {
    dialog_byte_cursor = &dialog_bytes[0];
    dialog_byte_character_cursor = dialog_byte_cursor;
    *(dialog_byte_cursor++) = 0;
}

void add_dialog_bin_string(char command, const char* string) {
    u32 length_with_zero = strlen(string) + 1;
    *(dialog_byte_cursor++) = command;
    *(dialog_byte_cursor++) = length_with_zero;
    memcpy(dialog_byte_cursor, string, length_with_zero);
    dialog_byte_cursor += length_with_zero;
    *dialog_byte_character_cursor += 1;
}

void next_dialog_bin_character() {
    dialog_byte_character_cursor = dialog_byte_cursor;
    *(dialog_byte_cursor++) = 0;
}

char *end_dialog_bin() {
    u32 dialog_size = dialog_byte_cursor - dialog_bytes;
    char *alloc = malloc_recomp(dialog_size);
    memcpy(alloc, dialog_bytes, dialog_size);
#if 0
    recomp_printf("[%d]: ", dialog_size);

    for (u32 i = 0; i < dialog_size; i++) {
        recomp_printf("0x%X ", dialog_bytes[i]);
    }
#endif
    return alloc;
}

RECOMP_PATCH char* dialogBin_get(enum asset_e text_id) {
    char* sp1C;
    char* var_v0;
    s32 var_a0; //offset where text starts (normally 0x3)

    //get text_bin from asset cache
    s_dialogBin.ptr = assetcache_get(text_id);
    sp1C = s_dialogBin.ptr + 1;
    sp1C += code94620_func_8031B5B0() * 2;
    var_a0 = *(sp1C++);
    var_a0 += *(sp1C++) << 8;
    if (sp1C);
    var_v0 = s_dialogBin.ptr + var_a0;
    s_dialogBin.index = text_id;

#if 0
    recomp_printf("static char dialog_%d[] = {", text_id);

    char* c = var_v0;
    for (u32 i = 0; i < 2; i++) {
        char count = *(c++);
        recomp_printf("0x%X, ", count);

        for (char j = 0; j < count; j++) {
            char cmd = *(c++);
            char len = *(c++);
            recomp_printf("0x%X, ", cmd);
            recomp_printf("0x%X, ", len);

            for (char k = 0; k < len; k++) {
                if (*c == '\'') {
                    recomp_printf("'\\'', ");
                }
                else if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9') || (*c >= ' ' && *c <= '?')) {
                    recomp_printf("'%c', ", *c);
                }
                else {
                    recomp_printf("0x%X, ", *c);
                }

                c++;
            }
        }
    }

    recomp_printf("}\n");
#endif

#define DIALOGUE_BANJO_HEAD 0x80
#define DIALOGUE_KAZOOIE_HEAD 0x81
#define DIALOGUE_BOTTLES_HEAD 0x83
#define DIALOGUE_MUMBO_HEAD 0x84
#define DIALOGUE_COMMAND_END 0x4

    switch (text_id) {
    case 3599:
        static char* dialog_3599;
        if (dialog_3599 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "WOW! THAT LOOKS SO SMOOTH!");
            add_dialog_bin_string(DIALOGUE_MUMBO_HEAD, "CAN RUN AT 60, 120 OR WHATEVER FPS YOU WANT. MUMBO'S MAGIC PC DOES 360 FPS.");
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "STOP SHOWING OFF SHAMAN BOY. WHAT ABOUT ON MY ULTRAWIDE MONITOR?");
            add_dialog_bin_string(DIALOGUE_MUMBO_HEAD, "RUDE BIRD NEED NOT WORRY. WIDESCREEN AND MORE SUPPORTED.");
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "IT RUNS WELL TOO. BANJO, EVEN THAT PC YOU GOT OUT OF THE DUMPSTER CAN PROBABLY RUN IT.");
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "KAZOOIE!!!");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "HEY GUYS! CHECK OUT HOW COOL MY MINIGAME LOOKS IN THIS PORT!");
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "OOOH! LOOKS GREAT! CAN'T WAIT TO TRY IT!");
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "I DON'T CARE ABOUT YOUR PUZZLES BOTTLE BRAIN. WHAT ABOUT NEW FEATURES?");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "I'M GLAD YOU ASKED, TAKE A LOOK!");
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "NOTE SAVING? WHAT DOES THAT DO?");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "WHEN YOU USE IT, YOU WON'T LOSE YOUR NOTES EVERY TIME YOU EXIT A WORLD. NEAT, ISN'T IT?");
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "AWESOME! DO WE GET ANY OTHER NEW FEATURES?");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "YES! IF YOU WANT, YOU CAN USE THE RIGHT ANALOG STICK FOR SMOOTHER CAMERA CONTROLS.");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "YOU CAN ALSO USE HIGH DEFINITION TEXTURE PACKS AND CUSTOM MODS!");
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "NEAT! WHAT KIND OF MODS CAN PEOPLE MAKE FOR THIS PORT?");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "PRETTY MUCH ANYTHING! NEW ASSETS, GAMEPLAY CHANGES, YOU NAME IT.");
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "YAWN. DOOM. HOW ORIGINAL. GOT ANYTHING WORTH PLAYING SOIL BRAIN?");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "SURE! MAYBE THESE ONES ARE MORE TO YOUR LIKING...");
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "WOAH! THESE LOOK COOL! ARE THESE NEW ADVENTURES FOR US TO PLAY?");
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "HEY, I THOUGHT OCARINA OF TIME WASN'T READY YET?");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "NO FEATHER BRAIN. IT'S JIGGIES OF TIME BY KURKO MODS, WHICH YOU CAN PLAY IN THIS PORT!");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "WE MADE TOOLS FOR MODDERS TO CONVERT THEIR COMPATIBLE ROMHACKS INTO RECOMP MODS!");
            add_dialog_bin_string(DIALOGUE_BOTTLES_HEAD, "NOSTALGIA 64 IS ALSO AVAILABLE, AND MORE MODS WILL BE RELEASED IN THE FUTURE!");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3599 = end_dialog_bin();
        }

        return dialog_3599;
    case 3705:
        static char* dialog_3705;
        if (dialog_3705 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_MUMBO_HEAD, "HEY...MUMBO GOT SECRET PICTURES!");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3705 = end_dialog_bin();
        }

        return dialog_3705;
    case 3706:
        static char* dialog_3706;
        if (dialog_3706 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "NICE ONE, BONE BRAIN! WHAT'S ON THEM?");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3706 = end_dialog_bin();
        }

        return dialog_3706;
    case 3707:
        static char* dialog_3707;
        if (dialog_3707 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_MUMBO_HEAD, "GOT PICTURES OF NEXT PORT. WISEGUY GAVE THEM TO ME.");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3707 = end_dialog_bin();
        }

        return dialog_3707;
    case 3708:
        static char* dialog_3708;
        if (dialog_3708 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "OOOH...DID YOU HEAR THAT KAZOOIE? THERE'S GOING TO BE ANOTHER RECOMP!"); 
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "IS IT THE ELF BOY AGAIN? HE ALWAYS GETS ALL THE ATTENTION.");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3708 = end_dialog_bin();
        }

        return dialog_3708;
    case 3709:
        static char* dialog_3709;
        if (dialog_3709 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_MUMBO_HEAD, "NO OCARINA. TAKING FOREVER. OTHER INSTRUMENTS FIRST.");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3709 = end_dialog_bin();
        }

        return dialog_3709;
    case 3717:
        static char* dialog_3717;
        if (dialog_3717 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "SOUNDS EXCITING. WHAT COULD IT BE?");
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "YEAH, SHOW US!");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3717 = end_dialog_bin();
        }

        return dialog_3717;
    case 3719:
        static char* dialog_3719;
        if (dialog_3719 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "THAT WAS GREAT! SO WHEN CAN WE PLAY THIS PORT?");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3719 = end_dialog_bin();
        }

        return dialog_3719;
    case 3720:
        static char* dialog_3720;
        if (dialog_3720 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_MUMBO_HEAD, "YOU CAN PLAY IT...NOW!");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3720 = end_dialog_bin();
        }

        return dialog_3720;
    case 3721:
        static char* dialog_3721;
        if (dialog_3721 == NULL) {
            start_dialog_bin();
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "REALLY?! COME ON BANJO, I WANT TO GO HOME AND PLAY IT!");
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "WAHEY! SOUNDS LIKE A PLAN!");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3721 = end_dialog_bin();
        }

        return dialog_3721;
    default:
        break;
    }
    
    return var_v0;
}

