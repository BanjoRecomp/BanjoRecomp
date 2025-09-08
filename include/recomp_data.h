#ifndef __RECOMP_DATA_H__
#define __RECOMP_DATA_H__

namespace recomputil {
    void init_extended_object_data(size_t num_types);
    void clear_all_object_data(size_t type_index);

    void register_data_api_exports();
}

#endif
