#include "patches.h"
#include "note_saving.h"
#include "transform_ids.h"
#include "bk_api.h"
#include "misc_funcs.h"

#include "core1/core1.h"
#include "core1/main.h"
#include "core1/vimgr.h"

RECOMP_DECLARE_EVENT(recomp_on_init());

// @recomp Patched to perform some initialization after core2 has been loaded.
RECOMP_PATCH void dummy_func_8025AFB0(void) {
    // @recomp Initialize note saving data before the init event is run.
    // This will allow mods to override it as necessary.
    init_note_saving();

    // @recomp Register actor extension data and call the init event.
    recomp_on_init();

    // @recomp Calculate the note start indices for each map after the init event.
    // This allows the start indices to account for any changes made by mods. 
    calculate_map_start_note_indices();
}
