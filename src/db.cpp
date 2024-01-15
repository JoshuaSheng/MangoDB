//
// Created by Josh on 2024-01-13.
//

#include "db.h"
#include "Tx.h"

DB *open(std::string path, const Options &options) {
    DAL::dal *dal = DAL::openFile(path, options);
    DB *database = new DB(dal);
    return database;
}

DB::DB(DAL::dal *dal): dal(dal) {};

void DB::close() {
    return dal->close();
}

DB::~DB() {
    close();
}

Tx *DB::readTx() {
    rwlock.lock_shared();
    return new Tx(this, false);
}

Tx *DB::writeTx() {
    rwlock.lock();
    return new Tx(this, true);
}