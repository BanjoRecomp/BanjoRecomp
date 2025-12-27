#ifndef __NOTE_SAVING_H__
#define __NOTE_SAVING_H__

#include "prop.h"

void init_note_saving();
void calculate_map_start_note_indices();

// Notes are always saved, but this function controls whether to use the saved data to prevent notes from spawning and adjust the note score. 
bool note_saving_enabled();

void note_saving_reset_spawned_static_note_count();
void note_saving_handle_static_note(Cube *c, Prop *p);

#endif
