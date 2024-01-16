//
// Created by Josh on 2024-01-13.
//

#ifndef MAIN_CPP_DB_H
#define MAIN_CPP_DB_H

class Tx;

namespace DAL {
    struct dal;
}

struct Options;

#include <shared_mutex>
#include "const.h"
#include "DAL.h"
#include "Tx.h"

class DB {

public:
    DAL::dal *dal;
    explicit DB(DAL::dal *dal);
    ~DB();
    void close();
    Tx *readTx();
    Tx *writeTx();

    std::shared_mutex rwlock;
};

DB *open(std::string path, const Options &options=defaultOptions);



#endif //MAIN_CPP_DB_H
