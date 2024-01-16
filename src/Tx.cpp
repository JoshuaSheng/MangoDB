//
// Created by Josh on 2024-01-14.
//

#include "Tx.h"

Tx::Tx(DB *db, bool write): db(db), write(write){}

void Tx::rollback() {
    if (!write) {
        db->rwlock.unlock_shared();
        return;
    }

    dirtyNodes.clear();
    pagesToDelete.clear();
    for (auto pagenum: allocatedPageNums) {
        db->dal->freeList->releasePage(pagenum);
    }
    allocatedPageNums.clear();
    db->rwlock.unlock();
};

void Tx::commit() {
    if (!write) {
        db->rwlock.unlock_shared();
        return;
    }

    for (auto [_, node]: dirtyNodes) {
        db->dal->writeNode(node);
    }

    for (auto pagenum: pagesToDelete) {
        db->dal->deleteNode(pagenum);
    }

    db->dal->writeFreeList();
    dirtyNodes.clear();
    pagesToDelete.clear();
    allocatedPageNums.clear();
    db->rwlock.unlock();
}

Node *Tx::newNode(std::vector<Item *> items, std::vector<pgnum> childNodes) {
    Node *node = newEmptyNode();
    node->items = items;
    node->childNodes = childNodes;
    node->pageNum = db->dal->freeList->getNextPage();
    node->tx = this;
    allocatedPageNums.push_back(node->pageNum);
    return node;
}

Node *Tx::getNode(pgnum pageNum) {
    if (dirtyNodes.find(pageNum) != dirtyNodes.end()) {
        return dirtyNodes[pageNum];
    }
    Node *node = db->dal->getNode(pageNum);
    node->tx = this;
    return node;
}

Node *Tx::writeNode(Node *node) {
    dirtyNodes.insert({node->pageNum, node});
    node->tx = this;
    return node;
}

void Tx::deleteNode(pgnum pageNum) {
    pagesToDelete.push_back(pageNum);
}

Collection *Tx::getRootCollection() {
    Collection *rootCollection = newEmptyCollection();
    rootCollection->tx = this;
    rootCollection->root = db->dal->meta->root;
    return rootCollection;
}

Collection *Tx::getCollection(std::vector<BYTE> name) {
    Collection *root = getRootCollection();
    Item *item = root->find(name);
    if (item == nullptr) {
        return nullptr;
    }

    Collection *collection = newEmptyCollection();
    collection->deserialize(item);
    collection->tx = this;
    return collection;
}

Collection *Tx::addToRootCollection(Collection *newCollection) {
    newCollection->tx = this;
    Item *collectionBytes = newCollection->serialize();
    Collection *rootCollection = getRootCollection();
    rootCollection->put(newCollection->name, collectionBytes->value);
    return newCollection;
}

Collection *Tx::createCollection(std::vector<BYTE> name) {
    if (!write) {
        throw writeInsideReadTxErr;
    }
    Node * newCollectionPage = db->dal->writeNode(newEmptyNode());
    Collection *newCollection = newEmptyCollection();

    newCollection->name = name;
    newCollection->root = newCollectionPage->pageNum;
    return addToRootCollection(newCollection);
}

void Tx::deleteCollection(std::vector<BYTE> name) {
    if (!write) {
        throw writeInsideReadTxErr;
    }

    Collection *rootCollection = getRootCollection();
    rootCollection->remove(name);
}
