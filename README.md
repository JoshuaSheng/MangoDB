# MangoDB

A key-value store built from the ground up in C++.

## Installing:

Detailed installation is WIP, however you can clone the repository and run cmake to try it yourself.

### Example:

The below code can also be found in main.cpp

```c++
    std::string path = "mango.db";
    DB *db = open(path, defaultOptions);
    Tx *tx = db->writeTx();
    std::string collectionName = "collection1";
    Collection *c = tx->createCollection(collectionName);
    std::string key = "key1", value = "value1";
    c->put(key, value);
    tx->commit();
```

## Features:

### Transactions:

Transactions are how changes are applied to the database. You can create a transaction with `db->writeTx()`, and either `commit()` or `rollback()` to save or erase the changes.

MangoDB supports both multiple read-only transactions or one write transaction at a time.

### Collections:

Collections are groupings of key-value pairs. MangoDB currently supports create, get, and delete operations on Collections.

```c++
tx->createCollection("collection1");
tx->commit();
tx->getCollection("collection1");
tx->deleteCollection("collection1");
```

### Items:

Key-value pairs are stored as items inside collections. MangoDB supports create, get, and delete operations on Items.

```c++
Collection * collection = tx->getCollection("collection1");
collection->put("key1", "value1");
collection->find("key1"); //returns value1
collection->remove("key1");
```

### ACID:

**Atomicity** will be supported by a write-ahead log. This will be implemented in a future update.

**Consistency** is vacuously guaranteed since MangoDB does not currently support integrity constraints.

**Isolation** is guaranteed since MangoDB is serializable and supports concurrent reads or a single write thread.

**Durability** comes from MangoDB's disk-based persistence.

### Acknowledgements

This project was heavily inspired by [LibraDB](https://github.com/amit-davidson/LibraDB).

Special thanks to [learncpp.com](https://www.learncpp.com/cpp-tutorial/introduction-to-these-tutorials/) for getting me up to speed on modern C++.