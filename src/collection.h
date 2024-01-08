//
// Created by Josh on 2024-01-02.
//

#ifndef MAIN_CPP_COLLECTION_H
#define MAIN_CPP_COLLECTION_H


#include "const.h"
#include "DAL.h"

class Collection {

    std::vector<BYTE> name;
    pgnum root;
    DAL::dal *dal;

public:
    Collection(vector<BYTE> name, pgnum root, DAL::dal *dal);
    vector<Node *>getNodes(vector<int> indexes);

    Item *find(vector<BYTE> key);
    void put(vector<BYTE> key, vector<BYTE> value);
};


#endif //MAIN_CPP_COLLECTION_H
