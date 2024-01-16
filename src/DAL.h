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

struct Item;

namespace DAL {

    struct freelist {
        pgnum maxPage;
        std::vector<pgnum> releasedPages;

        freelist(pgnum initialPage);

        pgnum getNextPage();
        void releasePage(pgnum page);

        void serialize(std::vector<BYTE> *data);
        void deserialize(const std::vector<BYTE> *data);
    };

    struct dal {
        std::string path;
        int pagesize;
        double minFillPercent;
        double maxFillPercent;
        std::fstream *file;
        freelist *freeList;
        Meta *meta;

        dal(std::string path, std::fstream *file, Options options=defaultOptions);

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

        double maxThreshold();

        double minThreshold();

        bool isOverpopulated(Node *node);

        bool isUnderpopulated(Node *node);

        int getSplitIndex(Node *node);
    };

    dal *openFile(std::string path, const Options &options=defaultOptions);
}


#endif //MANGODB_DAL_H
