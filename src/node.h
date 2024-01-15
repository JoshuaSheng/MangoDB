//
// Created by Josh on 2023-09-17.
//

#ifndef MAIN_CPP_NODE_H
#define MAIN_CPP_NODE_H

class Tx;

#include <vector>
#include "const.h"
#include "DAL.h"
#include "Tx.h"

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
        Node(std::vector<Item *> items, std::vector<pgnum> childNodes, pgnum pageNum, Tx *tx);
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

        void split(Node *nodeToSplit, int nodeToSplitIndex);

    bool isLeaf();

    std::vector<pgnum> childNodes;
    std::vector<Item *> items;
    Tx *tx;

    void removeItemFromLeaf(int index);

    vector<int> removeItemFromBranch(int index);

    void merge(Node *node, int index);

    bool canSpareAnElement();

    void rebalanceRemove(Node *unbalancedNode, int unbalancedNodeIndex);

    void rotateRight(Node *leftChild, Node *rightChild, Node *parent, int rightChildIndex);

    void rotateLeft(Node *leftChild, Node *rightChild, Node *parent, int rightChildIndex);
};

Node * newEmptyNode();

Item * newItem(std::vector<BYTE> key, std::vector<BYTE> value);

int cmp_bytes(std::vector<unsigned char> &l, std::vector<unsigned char> &r);
#endif //MAIN_CPP_NODE_H
