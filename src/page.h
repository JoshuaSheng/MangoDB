//
// Created by Josh on 2023-08-20.
//

#ifndef NOTMYSQL_PAGE_H
#define NOTMYSQL_PAGE_H

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


#endif //NOTMYSQL_PAGE_H
