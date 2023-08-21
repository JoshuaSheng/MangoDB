//
// Created by Josh on 2023-08-20.
//

#include "meta.h"
#include <cstring>

void Meta::serialize(vector<BYTE> *buffer) {
    int freelistPageSize = sizeof(freelistPage);
    buffer->reserve(freelistPageSize);
    std::memcpy(buffer->data(), &freelistPage, freelistPageSize);
}

void Meta::unserialize(vector<BYTE> *buffer) {
    std::memcpy(&freelistPage, buffer->data(), sizeof(freelistPage));
}
Meta *newEmptyMeta() {
    Meta *m = new Meta{};
    return m;
}