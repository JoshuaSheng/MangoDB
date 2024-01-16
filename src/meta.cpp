//
// Created by Josh on 2023-08-20.
//

#include "meta.h"
#include <cstring>

void Meta::serialize(std::vector<BYTE> *buffer) {
    constexpr int freelistPageSize = sizeof(pgnum);
    buffer->resize(freelistPageSize*2 + sizeof(dbFileHeader)); // storing meta and root pgnums
    int pos = 0;
    std::memcpy(buffer->data(), &dbFileHeader, sizeof(dbFileHeader));
    pos += sizeof(dbFileHeader);
    std::memcpy(buffer->data() + pos, &root, freelistPageSize);
    pos += freelistPageSize;
    std::memcpy(buffer->data() + pos, &freelistPage, freelistPageSize);
}

void Meta::unserialize(std::vector<BYTE> *buffer) {
    int pos = 0;
    int32_t fileHeader;
    std::memcpy(&fileHeader, buffer->data(), sizeof(fileHeader));
    if (fileHeader != dbFileHeader) {
        throw std::logic_error("Attempted to load file that is not a mangoDB file");
    }
    pos += sizeof(fileHeader);
    std::memcpy(&root, buffer->data() + pos, sizeof(freelistPage));
    pos += sizeof(freelistPage);
    std::memcpy(&freelistPage, buffer->data() + pos, sizeof(freelistPage));
}
Meta *newEmptyMeta() {
    Meta *m = new Meta{};
    return m;
}