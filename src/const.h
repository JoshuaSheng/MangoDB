//
// Created by Josh on 2023-08-20.
//

#ifndef MANGODB_CONST_H
#define MANGODB_CONST_H
#include <cstdint>
#include <string>
#include <stdexcept>


inline constexpr int pageNumSize = 8;
inline constexpr int pageSize = 64;
inline constexpr int nodeHeaderSize = 3;
typedef uint64_t pgnum;
typedef unsigned char BYTE;

struct Options {
    int pageSize;
    double minFillPercent;
    double maxFillPercent;
};

constexpr Options defaultOptions {pageSize, 0.5, 0.95};


#endif //MANGODB_CONST_H
