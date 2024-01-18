//
// Created by Josh on 2023-08-20.
//

#ifndef MANGODB_META_H
#define MANGODB_META_H

#include <cstdint>
#include <vector>
#include "const.h"

const int metaPageNum = 0;
typedef uint64_t pgnum;
typedef unsigned char BYTE;


struct Meta {
    pgnum root;
    pgnum freelistPage;

    void serialize(std::vector<BYTE> &buffer);
    void unserialize(std::vector<BYTE> &buffer);
};

Meta *newEmptyMeta();

#endif //MANGODB_META_H
