//
// Created by Josh on 2023-08-19.
//

#ifndef MANGODB_DAL_H
#define MANGODB_DAL_H

class Node;

#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>
#include <vector>
#include "const.h"
#include "meta.h"
#include "page.h"
#include "node.h"

namespace DAL {

    struct freelist {
        pgnum maxPage;
        std::vector<pgnum> releasedPages;

        freelist(pgnum initialPage);

        pgnum getNextPage();
        void releasePage(pgnum page);

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

        Node *getNode(pgnum pageNum);

        Node *writeNode(Node *node);

        void deleteNode(pgnum pageNum);
    };

    dal *openFile(std::string path, int pageSize);
}


#endif //MANGODB_DAL_H
