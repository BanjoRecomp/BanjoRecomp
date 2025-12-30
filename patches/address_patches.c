#include "patches.h"
#include "core1/mlmtx.h"

typedef struct {
    s32 cmd_0;
    s32 size_4;
    s16 unk8[];
}GeoCmd5;

typedef struct {
    s32 cmd_0;
    s32 size_4;
    u8  pad8;
    s16 unkA;
}GeoCmd7;

extern s32 D_80370990;
extern BKGfxList *modelRenderDisplayList;

// @recomp Patched to remove unnecessary osVirtualToPhysical use to allow extended addresses.
RECOMP_PATCH void func_80338970(Gfx **gfx, Mtx **mtx, void *arg2){
    GeoCmd5 *cmd = (GeoCmd5 *)arg2;
    int i;

    if(D_80370990){
        // @recomp Remove unnecessary usage of osVirtualToPhysical to allow extended addresses.
        gSPDisplayList((*gfx)++, /*osVirtualToPhysical*/(modelRenderDisplayList->list + cmd->unk8[0]));
    }

    if(D_80370990){
        for(i = 1; cmd->unk8[i]; i++){
            mlMtxApply(*mtx);
            gSPMatrix((*gfx)++, (*mtx)++, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList((*gfx)++, /*osVirtualToPhysical*/(modelRenderDisplayList->list + cmd->unk8[i]));
        }
    }
}


// @recomp Patched to remove unnecessary osVirtualToPhysical use to allow extended addresses.
RECOMP_PATCH void func_80338AE8(Gfx **gfx, Mtx **mtx, void *arg2){
    if(D_80370990){
        // @recomp Remove unnecessary usage of osVirtualToPhysical to allow extended addresses.
        gSPDisplayList((*gfx)++, /*osVirtualToPhysical*/(modelRenderDisplayList->list + ((GeoCmd7*)arg2)->unkA));
    }
}
