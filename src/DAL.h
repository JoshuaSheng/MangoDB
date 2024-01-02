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

struct Options {
    int pageSize;
    double_t minFillPercent;
    double_t maxFillPercent;
};

constexpr Options defaultOptions {pageSize, 0.5, 0.95};

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
        double_t minFillPercent;
        double_t maxFillPercent;
        std::fstream *file;
        freelist *freeList;
        Meta *meta;

        dal(string path, fstream *file, Options options=defaultOptions);

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

        Node * newNode(std::vector<Item *> items, std::vector<pgnum> childNodes);

        void deleteNode(pgnum pageNum);

        double maxThreshold();

        double minThreshold();

        bool isOverpopulated(Node *node);

        bool isUnderpopulated(Node *node);

        int getSplitIndex(Node *node);
    };

    dal *openFile(std::string path, Options options);
}


#endif //MANGODB_DAL_H
