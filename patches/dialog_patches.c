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
#define DIALOGUE_MUMBO_HEAD 0x84
#define DIALOGUE_COMMAND_END 0x4

    switch (text_id) {
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
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            next_dialog_bin_character();
            add_dialog_bin_string(DIALOGUE_BANJO_HEAD, "SOUNDS EXCITING. WHAT COULD IT BE?");
            add_dialog_bin_string(DIALOGUE_KAZOOIE_HEAD, "YEAH SHOW US YOUR SECRETS.");
            add_dialog_bin_string(DIALOGUE_COMMAND_END, "");
            dialog_3717 = end_dialog_bin();
        }

        return dialog_3717;
    default:
        break;
    }
    
    return var_v0;
}