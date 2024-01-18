//
// Created by Josh on 2024-01-02.
//

#ifndef MAIN_CPP_COLLECTION_H
#define MAIN_CPP_COLLECTION_H

#include <vector>
#include "const.h"
#include "DAL.h"
#include "Tx.h"

class Collection {
public:
    Tx *tx;
    std::vector<BYTE> name;
    uint64_t counter;
    pgnum root;

    Collection(std::vector<BYTE> &name, pgnum root, Tx *tx);
    Collection();
    std::vector<Node *>getNodes(std::vector<int> &indexes);

    Item *find(std::vector<BYTE> &key);
    void put(std::vector<BYTE> &key, std::vector<BYTE> &value);

    void put(std::string key, std::string value);

    void remove(std::vector<BYTE> &key);

    Item *serialize();
    void deserialize(Item *item);

    uint64_t getId();

    Item *find(std::string key);

    Item *find(std::vector<BYTE> &&key);
};

Collection *newEmptyCollection();

#endif //MAIN_CPP_COLLECTION_H
