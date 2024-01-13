//
// Created by Josh on 2024-01-02.
//

#include "collection.h"

Collection::Collection(std::vector<BYTE> name, pgnum root, DAL::dal* dal): name(name), root(root), dal(dal) {}

Item *Collection::find(vector<BYTE> key) {
    Node *node = dal->getNode(root);
    auto [containingNode, index, _] = node->findKey(key);
    if (index == -1) {
        return nullptr;
    }
    return containingNode->items[index];
}

void Collection::put(vector<BYTE> key, vector<BYTE> value) {
    Item *item = newItem(key, value);
    Node* rootNode;
    if (root == 0) {
        rootNode = dal->writeNode(dal->newNode(vector<Item *>{item}, vector<pgnum>{}));
        root = rootNode->pageNum;
        return;
    }
    else {
        rootNode = dal->getNode(root);
    }

    auto [targetNode, insertionIndex, ancestorsIndexes] = rootNode->findKey(key, false);

    if (targetNode->items.size() > insertionIndex && cmp_bytes(targetNode->items[insertionIndex]->key, key) == 0) {
        targetNode->items[insertionIndex]->value = value;
    } else {
        targetNode->addItem(item, insertionIndex);
    }
    dal->writeNode(targetNode);
    auto ancestors = getNodes(ancestorsIndexes);

    //rebalances nodes from the bottom up, except for root
    for (int i{static_cast<int>(ancestors.size() - 2)}; i >= 0; --i) {
        Node *currNode{ancestors[i + 1]};
        Node *parentNode{ancestors[i]};
        int currIndex{ancestorsIndexes[i+1]};
        if (currNode->isOverpopulated()) parentNode->split(currNode, currIndex);
    }
    //is rootNode being assigned to itself here?
    rootNode = ancestors[0];
    if (rootNode->isOverpopulated()) {
        Node *newRoot = dal->newNode(vector<Item *>{}, vector<pgnum> {rootNode->pageNum});
        newRoot->split(rootNode, 0);

        newRoot = dal->writeNode(newRoot);
        root = newRoot->pageNum;
    }
}

vector<Node *> Collection::getNodes(vector<int> indexes) {
    Node *rootNode = dal->getNode(root);
    vector<Node *>nodes {rootNode};
    Node *currNode{rootNode};
    for (int i{1}; i < indexes.size(); i++) {
        currNode = dal->getNode(currNode->childNodes[indexes[i]]);
        nodes.push_back(currNode);
    }
    return nodes;
}

void Collection::remove(std::vector<BYTE> key) {
    Node *rootNode = dal->getNode(root);
    auto [nodeToRemoveFrom, removeItemIndex, ancestorsIndexes] = rootNode->findKey(key);
    if (removeItemIndex == -1) {
        cout << "Couldn't find key to remove" << endl;
        return;
    }

    if (nodeToRemoveFrom->isLeaf()) {
        nodeToRemoveFrom->removeItemFromLeaf(removeItemIndex);
    }
    else {
        std::vector<int>affectedNodes = nodeToRemoveFrom->removeItemFromBranch(removeItemIndex);
        ancestorsIndexes.insert(ancestorsIndexes.end(), affectedNodes.begin(), affectedNodes.end());
    }
    std::vector<Node *>ancestors = getNodes(ancestorsIndexes);
    for (int i{static_cast<int>(ancestors.size() - 2)}; i >= 0; --i) {
        Node *parent = ancestors[i];
        Node *node = ancestors[i+1];
        if (node->isUnderpopulated()) {
            parent->rebalanceRemove(node, ancestorsIndexes[i+1]);
        }
    }

    rootNode = ancestors[0];
    if (rootNode->items.empty() && !rootNode->childNodes.empty()) {
        root = ancestors[1]->pageNum;
    }
}