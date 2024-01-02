//
// Created by Josh on 2023-09-17.
//

#ifndef MAIN_CPP_NODE_H
#define MAIN_CPP_NODE_H

#include <vector>
#include "const.h"
#include "DAL.h"

namespace DAL {
    struct dal;
}

struct Item {
    std::vector<BYTE> key;
    std::vector<BYTE> value;
};

class Node {

public:
        pgnum pageNum;
        Node(std::vector<Item *> items, std::vector<pgnum> childNodes, pgnum pageNum, DAL::dal *dal);
        Node();
        vector<BYTE> * serialize(std::vector<BYTE> &buf);
        void deserialize(std::vector<BYTE> &buf);

        Node *writeNode(Node *node);
        void writeNodes(std::initializer_list<Node *> nodes);
        Node * getNode(pgnum pageNum);
        pair<bool, int> findKeyInNode(std::vector<BYTE> key);
        tuple<Node *, int, vector<int>> findKey(std::vector<BYTE> key, bool exact=true);

        int elementSize(int index);
        int nodeSize();
        void addItem(Item *item, int index);

        bool isOverpopulated();
        bool isUnderpopulated();

        void *split(Node *nodeToSplit, int nodeToSplitIndex);

    bool isLeaf();

    std::vector<pgnum> childNodes;
    std::vector<Item *> items;
    DAL::dal *dal;
};

Node * newEmptyNode();

Item * newItem(std::vector<BYTE> key, std::vector<BYTE> value);

int cmp_bytes(std::vector<unsigned char> &l, std::vector<unsigned char> &r);
#endif //MAIN_CPP_NODE_H
