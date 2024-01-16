//
// Created by Josh on 2023-08-20.
//

#ifndef MANGODB_CONST_H
#define MANGODB_CONST_H
#include <cstdint>
#include <string>
#include <stdexcept>
#include <vector>


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

constexpr uint32_t dbFileHeader = 0xDEADBEEF;

inline const std::runtime_error writeInsideReadTxErr {"Tried to write inside of a read transaction"};

inline std::vector<BYTE> toVector(std::string s) {
    return std::vector<BYTE>{s.begin(), s.end()};
}


#endif //MANGODB_CONST_H
