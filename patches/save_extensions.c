#include "patches.h"
#include "save.h"
#include "core1/eeprom.h"
#include "save_extension.h"

// Vanilla declarations.
extern SaveData gameFile_saveData[4]; //save_data
extern s8 gameFile_GameIdToFileIdMap[4]; //gamenum to filenum
extern s32 D_80383F04;

s32 savedata_8033CA2C(s32 filenum, SaveData *save_data);
int savedata_8033CC98(s32 filenum, u8 *buffer);
int savedata_8033CCD0(s32 filenum);
void __gameFile_8033CE14(s32 gamenum);
void savedata_clear(SaveData *savedata);
void savedata_update_crc(void *buffer, u32 size);
void saveData_load(SaveData *savedata);
void saveData_create(SaveData *savedata);

void bsStoredState_clear(void);
void func_8031FFAC(void);
void item_setItemsStartCounts(void);
void jiggyscore_clearAll(void);
void honeycombscore_clear(void);
void mumboscore_clear(void);
void volatileFlag_clear(void);
void func_802D6344(void);


// New declarations.

// One entry for each file and the backup.
SaveFileExtensionData save_file_extension_data[4];
SaveFileExtensionData loaded_file_extension_data;

SaveGlobalExtensionData save_global_extension_data;

_Static_assert(sizeof(save_file_extension_data) + sizeof(save_global_extension_data) == 1536, "Save extension data must be 1536 bytes");

// Place save file extensions after the 4K eeprom range.
#define SAVE_FILE_EXTENSION_OFFSET (EEPROM_BLOCK_SIZE * EEPROM_MAXBLOCKS)
// Number of blocks per save file extension slot.
#define SAVE_FILE_EXTENSION_DATA_BLOCK_COUNT (sizeof(SaveFileExtensionData) / EEPROM_BLOCK_SIZE)
// Place the global extension data after teh save file extensions
#define SAVE_GLOBAL_EXTENSION_OFFSET (SAVE_FILE_EXTENSION_OFFSET + sizeof(save_file_extension_data))

_Static_assert(sizeof(SaveFileExtensionData) % EEPROM_BLOCK_SIZE == 0, "SaveFileExtensionData size must be a multiple of EEPROM_BLOCK_SIZE");
_Static_assert(SAVE_GLOBAL_EXTENSION_OFFSET % EEPROM_BLOCK_SIZE == 0, "SAVE_GLOBAL_EXTENSION_OFFSET must be a multiple of EEPROM_BLOCK_SIZE");

#define SAVE_FILE_EXTENSION_OFFSET_BLOCKS (SAVE_FILE_EXTENSION_OFFSET / EEPROM_BLOCK_SIZE)
#define SAVE_GLOBAL_EXTENSION_OFFSET_BLOCKS (SAVE_GLOBAL_EXTENSION_OFFSET / EEPROM_BLOCK_SIZE)

// @recomp Patched to load the corresponding file number's extension data.
RECOMP_PATCH int __gameFile_8033CD90(s32 filenum){
    int i;
    s32 tmp_v1;
    void *save_data_ptr;
    save_data_ptr = &gameFile_saveData[filenum];
    // @recomp Get a pointer to the extension data for this file number.
    SaveFileExtensionData *extension_ptr = &save_file_extension_data[filenum];
    i = 3;
    do{
        // @recomp Also load the extension data.
        eeprom_readBlocks(0, SAVE_FILE_EXTENSION_OFFSET_BLOCKS + SAVE_FILE_EXTENSION_DATA_BLOCK_COUNT * filenum, extension_ptr, SAVE_FILE_EXTENSION_DATA_BLOCK_COUNT);

        // Read save data from eeprom for file
        tmp_v1 = savedata_8033CA2C(filenum, save_data_ptr);

        if(!tmp_v1)
            break;
        i--;
    }while(i != 0);
    if(tmp_v1) {
        savedata_clear(save_data_ptr);
        // @recomp Also clear the extension data.
        bzero(save_data_ptr, sizeof(*save_data_ptr));
    }
    return tmp_v1;
}

// @recomp Patched to save the corresponding file number's extension data.
RECOMP_PATCH s32 gameFile_8033CFD4(s32 gamenum){
    s32 next;
    s32 filenum;
    u32 i = 3;
    s32 eeprom_error;
    SaveData *save_data;


    filenum = D_80383F04;
    next = gameFile_GameIdToFileIdMap[gamenum];
    gameFile_GameIdToFileIdMap[gamenum] = D_80383F04;
    bcopy(&gameFile_saveData[next], &gameFile_saveData[filenum], 0xF*8);
    // @recomp Also copy the extension data from the next slot.
    bcopy(&save_file_extension_data[next], &save_file_extension_data[filenum], sizeof(save_file_extension_data[0]));

    save_data = gameFile_saveData + filenum;
    
    // @recomp Get a pointer to the extension data for this file number.
    SaveFileExtensionData *extension_ptr = &save_file_extension_data[filenum];

    save_data->slotIndex = gamenum + 1;
    savedata_update_crc(save_data, sizeof(SaveData));
    for(eeprom_error = 1; eeprom_error && i > 0; i--){//L8033D070
        // @recomp Also save the extension data.
        eeprom_writeBlocks(0, SAVE_FILE_EXTENSION_OFFSET_BLOCKS + SAVE_FILE_EXTENSION_DATA_BLOCK_COUNT * filenum, extension_ptr, SAVE_FILE_EXTENSION_DATA_BLOCK_COUNT);

        eeprom_error = savedata_8033CC98(filenum, save_data);
        if(!eeprom_error){
            __gameFile_8033CE14(gamenum);
        }
    }
    if(!eeprom_error){
        for(i = 3; i > 0; i--){//L8033D070
            eeprom_error = savedata_8033CCD0(next);
            if(!eeprom_error)
                break;
        }
    }
    if(eeprom_error){
        gameFile_GameIdToFileIdMap[gamenum] = next;
    }
    else{
        D_80383F04 = next;
    }
    return eeprom_error;
}

// @recomp Patched to clear extended file data.
RECOMP_PATCH void gameFile_clear(s32 gamenum){
    s32 filenum = gameFile_GameIdToFileIdMap[gamenum];
    savedata_clear(&gameFile_saveData[filenum]);

    // @recomp Clear the extended file data for this file number.
    bzero(&save_file_extension_data[filenum], sizeof(SaveFileExtensionData));
}

// @recomp Patched to load extended file data.
RECOMP_PATCH void gameFile_load(s32 gamenum){
    s32 filenum = gameFile_GameIdToFileIdMap[gamenum];
    saveData_load(&gameFile_saveData[filenum]);
    
    // @recomp Load the extended file data for this file number.
    memcpy(&loaded_file_extension_data, &save_file_extension_data[filenum], sizeof(SaveFileExtensionData));
}

// @recomp Patched to save extended file data.
RECOMP_PATCH void gameFile_save(s32 gamenum){
    s32 filenum = gameFile_GameIdToFileIdMap[gamenum];
    saveData_create(&gameFile_saveData[filenum]);

    // @recomp Save the extended file data for this file number.
    memcpy(&save_file_extension_data[filenum], &loaded_file_extension_data, sizeof(SaveFileExtensionData));
}

// @recomp Patched to clear the current loaded extended file data. 
RECOMP_PATCH void clearScoreStates(void) {
    bsStoredState_clear();
    func_8031FFAC();
    item_setItemsStartCounts();
    jiggyscore_clearAll();
    honeycombscore_clear();
    mumboscore_clear();
    volatileFlag_clear();
    func_802D6344();

    // @recomp Clear the current loaded extended file data.
    bzero(&loaded_file_extension_data, sizeof(loaded_file_extension_data));
}
