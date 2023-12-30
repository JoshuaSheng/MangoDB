//
// Created by Josh on 2023-08-20.
//

#include "meta.h"
#include <cstring>

void Meta::serialize(vector<BYTE> *buffer) {
    constexpr int freelistPageSize = sizeof(pgnum);
    buffer->resize(freelistPageSize*2); // storing meta and root pgnums
    int pos = 0;
    std::memcpy(buffer->data() + pos, &root, freelistPageSize);
    pos += freelistPageSize;
    std::memcpy(buffer->data() + pos, &freelistPage, freelistPageSize);
}

void Meta::unserialize(vector<BYTE> *buffer) {
    int pos = 0;
    std::memcpy(&root, buffer->data() + pos, sizeof(freelistPage));
    pos += sizeof(freelistPage);
    std::memcpy(&freelistPage, buffer->data() + pos, sizeof(freelistPage));
}
Meta *newEmptyMeta() {
    Meta *m = new Meta{};
    return m;
}