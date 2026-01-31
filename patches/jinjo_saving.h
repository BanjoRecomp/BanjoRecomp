#ifndef __JINJO_SAVING_H__
#define __JINJO_SAVING_H__

u32 jinjo_saving_get_counters(enum level_e level);

s32 bkrecomp_jinjo_saving_active();
void init_jinjo_saving(void);
void jinjo_saving_on_map_load(void);
void jinjo_saving_update(void);

#endif
