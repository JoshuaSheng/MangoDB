//
// Created by Josh on 2023-08-19.
//

#ifndef NOTMYSQL_DAL_H
#define NOTMYSQL_DAL_H


#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>
#include <vector>

typedef uint64_t pgnum;
typedef unsigned char BYTE;

struct freelist {
    pgnum maxPage;
    std::vector<pgnum> releasedPages;

    freelist(pgnum initialPage);

    pgnum getNextPage();
};

struct page {
    pgnum num;
    int size;
    std::vector<BYTE> *data;
};

struct dal {
    std::string path;
    int pagesize;
    std::fstream *file;
    freelist *freeList;

    page *allocateEmptyPage();

    page *readPage(pgnum pageNum);

    void writePage(page *p);

    void close();
};

dal *open(std::string path, int pageSize);


#endif //NOTMYSQL_DAL_H
