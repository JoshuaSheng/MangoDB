//
// Created by Josh on 2024-01-14.
//

#ifndef MAIN_CPP_TX_H
#define MAIN_CPP_TX_H

class DB;
class Node;
class Item;

#include <unordered_map>
#include "const.h"
#include "node.h"
#include "db.h"

class Tx {
    std::unordered_map<pgnum, Node *> dirtyNodes;
    std::vector<pgnum> pagesToDelete;
    std::vector<pgnum> allocatedPageNums;

public:
    Tx(DB *db, bool write);
    bool write;

    Node *getNode(pgnum pageNum);
    Node *writeNode(Node *node);
    void deleteNode(pgnum pageNum);

    void rollback();

    void commit();

    DB *db;

    Node *newNode(vector<Item *> items, vector<pgnum> childNodes);
};


#endif //MAIN_CPP_TX_H
