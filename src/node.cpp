//
// Created by Josh on 2023-09-17.
//

#include <cstring>
#include <cstdarg>
#include "node.h"

bool Node::isLeaf() {
    return childNodes.empty();
}

Node * newEmptyNode() {
    return new Node{};
}

Item * newItem(std::vector<BYTE> key, std::vector<BYTE> value) {
    return new Item{key, value};
}

std::vector<BYTE> *Node::serialize(std::vector<BYTE> &buf) {
    buf.resize(pageSize);
    int leftPos{0};
    size_t rightPos{buf.size()};
    uint8_t bitSetVar = this->isLeaf() ? 1 : 0;
    memcpy(buf.data() + leftPos, &bitSetVar, sizeof(uint8_t));
    leftPos += sizeof(bitSetVar);
    uint16_t itemLength{uint16_t(this->items.size())};
    memcpy(buf.data() + leftPos, &itemLength, sizeof(uint16_t));
    leftPos += sizeof(uint16_t);
    for (int i{0}; i < items.size(); ++i) {
        Item *item {items[i]};
        if (!isLeaf()) {
            pgnum childNode {childNodes[i]};
            memcpy(buf.data() + leftPos, &childNode, sizeof(pgnum));
            leftPos += pageNumSize;
        }

        int8_t klen {static_cast<int8_t>(item->key.size())};
        int8_t vlen {static_cast<int8_t>(item->value.size())};

        uint16_t offset = rightPos - klen - vlen - 2;
        memcpy(buf.data() + leftPos, &offset, sizeof(offset));
        leftPos += sizeof(offset);

        rightPos -= vlen;
        memcpy(buf.data() + rightPos, item->value.data(), vlen);
        rightPos -= sizeof(vlen);
        memcpy(buf.data() + rightPos, &vlen, sizeof(vlen));
        rightPos -= klen;
        memcpy(buf.data() + rightPos, item->key.data(), klen);
        rightPos -= sizeof(klen);
        memcpy(buf.data() + rightPos, &klen, sizeof(klen));


    }

    if (!isLeaf()) {
        pgnum childNode {childNodes.back()};
        memcpy(buf.data() + leftPos, &childNode, sizeof(pgnum));
    }

    return &buf;
}

void Node::deserialize(std::vector<BYTE> &buf) {
    int leftPos{0};
    int isLeaf{int(buf[0])};
    uint16_t itemsCount{};
    leftPos += sizeof(uint8_t);
    memcpy(&itemsCount, buf.data() + leftPos, sizeof(itemsCount));
    leftPos += sizeof(uint16_t);

    for (int i{0}; i < itemsCount; ++i) {
        if (!isLeaf) {
            pgnum newPageNum{};
            memcpy(&newPageNum, buf.data() + leftPos, pageNumSize);
            leftPos += pageNumSize;
            childNodes.push_back(newPageNum);
        }

        uint16_t offset{};
        memcpy(&offset, buf.data() + leftPos, sizeof(uint16_t));
        leftPos += sizeof(uint16_t);

        //read key, value from offset
        int8_t klen {};
        memcpy(&klen, buf.data() + offset, sizeof(int8_t));
        offset += sizeof(int8_t);

        std::vector<BYTE> key{buf.data() + offset, buf.data() + offset + klen};
        offset += klen;

        int8_t vlen {};
        memcpy(&vlen, buf.data() + offset, sizeof(int8_t));
        offset += sizeof(int8_t);

        std::vector<BYTE> value{buf.data() + offset, buf.data() + offset + vlen};
        offset += vlen;

        items.push_back(new Item{key, value});
    }

    if (!isLeaf) {
        pgnum newPageNum{};
        memcpy(&newPageNum, buf.data() + leftPos, pageNumSize);
        childNodes.push_back(newPageNum);
    }
}

Node *Node::writeNode(Node *node) {
    return dal->writeNode(node);
}

void Node::writeNodes(std::initializer_list<Node*> nodes) {
    for (auto node: nodes) {
        writeNode(node);
    }
}

Node *Node::getNode(pgnum pageNum) {
    return dal->getNode(pageNum);
}

//returns -1 if l < r, 0 if l == r, and 1 otherwise
int cmp_bytes(std::vector<unsigned char> &l, std::vector<unsigned char> &r) {
    if (l.size() < r.size()) {
        return -1;
    }
    else if (l.size() > r.size()) {
        return 1;
    }
    else {
        return strncmp(reinterpret_cast<char *>(l.data()), reinterpret_cast<char *>(r.data()), l.size());
    }
}

std::pair<bool, int> Node::findKeyInNode(std::vector<BYTE> key) {
    for (int i{0}; i < items.size(); ++i) {
        int res = cmp_bytes(items[i]->key, key);
        if (res == 0) {
            return std::make_pair(true, i);
        }
        else if (res == 1) {
            return std::make_pair(false, i);
        }
    }
    return std::make_pair(false, items.size());
}

std::pair<Node *, int> findKeyHelper(Node *node, std::vector<BYTE> &key, bool exact, vector<int> &ancestorsIndex) {
    auto [wasFound, index] {node->findKeyInNode(key)};
    if (wasFound) {
        return std::make_pair(node, index);
    }

    if (node->isLeaf()) {
        if (exact) return std::make_pair(nullptr, -1);
        return std::make_pair(node, index);
    }
    ancestorsIndex.push_back(index);
    Node * nextChild = node->getNode(node->childNodes[index]);
    return findKeyHelper(nextChild, key, exact, ancestorsIndex);
}

std::tuple<Node *, int, vector<int>> Node::findKey(std::vector<BYTE> key, bool exact) {
    vector<int> ancestorsIndex {0};
    return std::tuple_cat(findKeyHelper(this, key, exact, ancestorsIndex), std::make_tuple(ancestorsIndex));
}

int Node::elementSize(int index) {
    int size{0};
    size += items[index]->key.size();
    size += items[index]->value.size();
    size += pageNumSize;
    return size;
}

int Node::nodeSize() {
    int size{0};
    size += nodeHeaderSize;
    for (int i{0}; i < items.size(); i++) {
        size += elementSize(i);
    }
    size += pageNumSize;
    return size;
}

void Node::addItem(Item *item, int index) {
    if (index > items.size()) {
        throw std::out_of_range("Index " + std::string{static_cast<char>(index)} + "is out of range of items array of size " + std::string{static_cast<char>(items.size())});
    }
    else if (index == items.size()) {
        items.push_back(item);
    }
    else {
        items.insert(items.begin() + index, item);
    }
}

bool Node::isOverpopulated() {
    return dal->isOverpopulated(this);
}

bool Node::isUnderpopulated() {
    return dal->isUnderpopulated(this);
}

void Node::split(Node *nodeToSplit, int nodeToSplitIndex) {
    int splitIndex{dal->getSplitIndex(nodeToSplit)};
    Item *middleItem{nodeToSplit->items[splitIndex]};
    Node *newNode;

    if (nodeToSplit->isLeaf()) {
        std::vector<Item *> newItems{nodeToSplit->items.begin() + splitIndex + 1, nodeToSplit->items.end()};
        std::vector<pgnum> newChildNodes{};
        newNode = writeNode(dal->newNode(newItems, newChildNodes));
        nodeToSplit->items.resize(splitIndex);
    }
    else {
        std::vector<Item *> newItems{nodeToSplit->items.begin() + splitIndex + 1, nodeToSplit->items.end()};
        std::vector<pgnum> newChildNodes{nodeToSplit->childNodes.begin() + splitIndex + 1, nodeToSplit->childNodes.end()};
        newNode = writeNode(dal->newNode(newItems, newChildNodes));
        nodeToSplit->items.resize(splitIndex);
        nodeToSplit->childNodes.resize(splitIndex + 1);
    }
    addItem(middleItem, nodeToSplitIndex);
    if (childNodes.size() == nodeToSplitIndex + 1) {
        childNodes.push_back(newNode->pageNum);
    }
    else {
        childNodes.insert(childNodes.begin() + nodeToSplitIndex + 1, newNode->pageNum);
    }
    writeNodes({this, nodeToSplit});
}

Node::Node(std::vector<Item *> items, std::vector<pgnum> childNodes, pgnum pageNum, DAL::dal *dal): items(items), childNodes(childNodes), pageNum(pageNum), dal(dal) {};

Node::Node() = default;

void Node::removeItemFromLeaf(int index){
   items.erase(items.begin() + index);
   writeNode(this);
}

vector<int> Node::removeItemFromBranch(int index) {
    std::vector<int> affectedNodes{index};
    //target the left branch of the item
    Node *affectedNode = getNode(childNodes[index]);
    //we want the largest element from the left branch of the item, e.g. the largest element smaller than the current one
    while (!affectedNode->isLeaf()) {
        affectedNodes.push_back(childNodes.size() - 1);
        affectedNode = affectedNode->getNode(affectedNode->childNodes.back());
    }
    items[index] = affectedNode->items.back();
    affectedNode->items.pop_back();
    writeNodes({this, affectedNode});

    return affectedNodes;
}

void Node::rotateRight(Node *leftChild, Node *rightChild, Node *parent, int rightChildIndex) {
    if (leftChild->items.empty()) {
        throw logic_error("Tried to rebalance with empty left child");
    }
    Item *leftChildLastItem = leftChild->items.back();
    leftChild->items.pop_back();

    //Get the item that will be replaced in the parent
    int parentItemIndex = rightChildIndex - 1;
    if (rightChildIndex == 0) {
        parentItemIndex = 0;
    }
    Item *parentItem = parent->items[parentItemIndex];

    //move the left item to the parent and the parent item to the right node
    parent->items[parentItemIndex] = leftChildLastItem;
    rightChild->items.insert(rightChild->items.begin(), parentItem);
    //move child nodes as well if they exist
    if (!leftChild->isLeaf()) {
        pgnum childNodeToShift = leftChild->childNodes.back();
        leftChild->childNodes.pop_back();
        rightChild->childNodes.insert(rightChild->childNodes.begin(), childNodeToShift);
    }
}

void Node::rotateLeft(Node *leftChild, Node *rightChild, Node *parent, int rightChildIndex) {
    if (rightChild->items.empty()) {
        throw logic_error("Tried to rebalance with empty right child");
    }
    Item *rightChildFirstItem = rightChild->items.front();
    rightChild->items.erase(rightChild->items.begin());

    //Get the item that will be replaced in the parent
    int parentItemIndex = rightChildIndex;
    if (parentItemIndex >= parent->items.size()) {
        parentItemIndex = parent->items.size() - 1;
    }
    Item *parentItem = parent->items[parentItemIndex];

    //move the left item to the parent and the parent item to the right node
    parent->items[parentItemIndex] = rightChildFirstItem;
    leftChild->items.push_back(parentItem);
    //move child nodes as well if they exist
    if (!rightChild->isLeaf()) {
        pgnum childNodeToShift = rightChild->childNodes.front();
        rightChild->childNodes.erase(rightChild->childNodes.begin());
        leftChild->childNodes.push_back(childNodeToShift);
    }
}
//merges a child node with its left sibling, removing the node
void Node::merge(Node *node, int index) {
    Node *leftSibling = getNode(index - 1);
    Item *parentNodeItem = items[index-1];
    items.erase(items.begin() + index - 1);
    leftSibling->items.push_back(parentNodeItem);
    leftSibling->items.insert(leftSibling->items.end(), node->items.begin(), node->items.end());
    if (!node->isLeaf()) {
        leftSibling->childNodes.insert(leftSibling->childNodes.end(), node->childNodes.begin(), node->childNodes.end());
    }
    writeNodes({leftSibling, this});
    dal->deleteNode(node->pageNum);
}

inline bool Node::canSpareAnElement() {
    return dal->getSplitIndex(this) != -1;
}

void Node::rebalanceRemove(Node *unbalancedNode, int unbalancedNodeIndex) {
    //rotate left capacity into unbalanced node
    if (unbalancedNodeIndex != 0) {
        Node *leftNode = getNode(childNodes[unbalancedNodeIndex - 1]);
        if (leftNode->canSpareAnElement()) {
            rotateRight(leftNode, unbalancedNode, this, unbalancedNodeIndex);
            writeNodes({leftNode, unbalancedNode, this});
            return;
        }
    }
    //rotate right capacity into unbalanced node
    if (unbalancedNodeIndex != childNodes.size()) {
        Node *rightNode = getNode(childNodes[unbalancedNodeIndex + 1]);
        if (rightNode->canSpareAnElement()) {
            rotateLeft(unbalancedNode, rightNode, this, unbalancedNodeIndex);
            writeNodes({unbalancedNode, rightNode, this});
            return;
        }
    }
    //merge right with unbalanced node
    if (unbalancedNodeIndex == 0) {
        Node *rightNode = getNode(childNodes[unbalancedNodeIndex+1]);
        return merge(rightNode, unbalancedNodeIndex + 1);
    }

    //merge with left
    return merge(unbalancedNode, unbalancedNodeIndex);
}




