#include "patches.h"
#include "transform_ids.h"
#include "bk_api.h"
#include "misc_funcs.h"

RECOMP_DECLARE_EVENT(recomp_on_init());

void core1_init();
void sns_write_payload_over_heap();
void mainLoop();

RECOMP_PATCH void mainThread_entry(void *arg) { 
    // @recomp Register actor extension data and call the init event.
    recomp_on_init();

    core1_init();
    sns_write_payload_over_heap();

    while (1) {
        // @recomp Reset the tracked projection IDs.
        reset_projection_ids();
        mainLoop();
    }
}
