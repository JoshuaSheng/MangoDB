//
// Created by Josh on 2023-08-20.
//

#ifndef MANGODB_PAGE_H
#define MANGODB_PAGE_H

#include "const.h"
#include <string>
#include <vector>
#include <iostream>

struct page {
    pgnum num;
    int size;
    std::vector<BYTE> *data;

    void writeData(std::string data);
};


#endif //MANGODB_PAGE_H
