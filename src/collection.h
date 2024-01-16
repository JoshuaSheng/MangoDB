//
// Created by Josh on 2024-01-02.
//

#ifndef MAIN_CPP_COLLECTION_H
#define MAIN_CPP_COLLECTION_H


#include "const.h"
#include "DAL.h"
#include "Tx.h"

class Collection {
public:
    Tx *tx;
    std::vector<BYTE> name;
    uint64_t counter;
    pgnum root;

    Collection(vector<BYTE> name, pgnum root, Tx *tx);
    Collection();
    vector<Node *>getNodes(vector<int> indexes);

    Item *find(vector<BYTE> key);
    void put(vector<BYTE> key, vector<BYTE> value);

    void put(std::string key, std::string value);

    void remove(vector<BYTE> key);

    Item *serialize();
    void deserialize(Item *item);

    uint64_t getId();
};

Collection *newEmptyCollection();

#endif //MAIN_CPP_COLLECTION_H
