#ifndef __SAVE_EXTENSION_H__
#define __SAVE_EXTENSION_H__

typedef struct {
    u8 bytes[32]; // 32*8 = 256 bits per level, enough to account for mods with unused vanilla rooms.
} LevelNotes;

// This struct must be 256 bytes to add up to 1536 bytes, which adds to the original 512 bytes of save data to equal exactly 2048 bytes.
typedef struct {
    LevelNotes level_notes[9];
    u8 padding[32]; // Reserved for future use.
} SaveFileExtensionData;

_Static_assert(sizeof(SaveFileExtensionData) == 320, "SaveExtensionData must be 256 bytes");

typedef struct {
    u8 padding[256];
} SaveGlobalExtensionData;

_Static_assert(sizeof(SaveGlobalExtensionData) == 256, "save_global_extension_data must be 512 bytes");

extern SaveFileExtensionData loaded_file_extension_data;

#endif
