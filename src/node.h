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
    DAL::dal *dal;
    std::vector<Item *> items;

public:
        pgnum pageNum;
        vector<BYTE> * serialize(std::vector<BYTE> &buf);
        void deserialize(std::vector<BYTE> &buf);

        Node *writeNode(Node *node);
        void writeNodes(std::vector<Node *> nodes);
        Node * getNode(pgnum pageNum);
        pair<bool, int> findKeyInNode(std::vector<BYTE> key);
        pair<Node *, int> findKey(std::vector<BYTE> key);

    bool isLeaf();

    std::vector<pgnum> childNodes;
};

Node * newEmptyNode();

Item * newItem(std::vector<BYTE> key, std::vector<BYTE> value);

#endif //MAIN_CPP_NODE_H
