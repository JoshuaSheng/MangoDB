//
// Created by Josh on 2023-08-19.
//

#ifndef MANGODB_DAL_H
#define MANGODB_DAL_H


#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>
#include <vector>
#include "const.h"
#include "meta.h"
#include "page.h"

namespace DAL {

    struct freelist {
        pgnum maxPage;
        std::vector<pgnum> releasedPages;

        freelist(pgnum initialPage);

        pgnum getNextPage();

        void serialize(vector<BYTE> *data);
        void deserialize(const vector<BYTE> *data);
    };

    struct dal {
        std::string path;
        int pagesize;
        std::fstream *file;
        freelist *freeList;
        Meta *meta;

        dal(string path, int pagesize, fstream *file);

        page *allocateEmptyPage();

        page *readPage(pgnum pageNum);

        void writePage(page *p);

        void close();

        page *writeMeta(Meta *meta);

        Meta *readMeta();

        page *writeFreeList();

        freelist *readFreeList();
    };

    dal *openFile(std::string path, int pageSize);
}


#endif //MANGODB_DAL_H
