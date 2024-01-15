//
// Created by Josh on 2024-01-02.
//

#ifndef MAIN_CPP_COLLECTION_H
#define MAIN_CPP_COLLECTION_H


#include "const.h"
#include "DAL.h"
#include "Tx.h"

class Collection {

    std::vector<BYTE> name;
    pgnum root;
public:
    Tx *tx;
    Collection(vector<BYTE> name, pgnum root, Tx *tx);
    vector<Node *>getNodes(vector<int> indexes);

    Item *find(vector<BYTE> key);
    void put(vector<BYTE> key, vector<BYTE> value);

    void remove(vector<BYTE> key);
};


#endif //MAIN_CPP_COLLECTION_H
