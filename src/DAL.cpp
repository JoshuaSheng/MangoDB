//
// Created by Josh on 2023-08-19.
//

#include "DAL.h"
#include <iostream>
#include <memory>
#include <cstdint>
#include <vector>
#include <iterator>
#include <filesystem>

using namespace DAL;

freelist::freelist(pgnum initialPage) {
    maxPage = initialPage;
}

void freelist::serialize(std::vector<BYTE> &data) {
    int pos = 0;
    auto released_pages_size = this->releasedPages.size();

    data.resize(sizeof(this->maxPage) + sizeof(size_t));
    memcpy(data.data() + pos, &this->maxPage, sizeof(pgnum));
    pos += sizeof(pgnum);
    memcpy(data.data() + pos, &released_pages_size, sizeof(released_pages_size));
    pos += sizeof(released_pages_size);

    data.resize(data.size() + released_pages_size);
    memcpy(data.data() + pos, this->releasedPages.data(), released_pages_size);
}

void freelist::deserialize(const std::vector<BYTE> &data) {
    int pos = 0;
    size_t released_pages_size = 0;
    memcpy(&this->maxPage, data.data(), sizeof(this->maxPage));

    pos += sizeof(this->maxPage);
    memcpy(&released_pages_size, data.data() + pos, sizeof(released_pages_size));
    pos += sizeof(released_pages_size);

    memcpy(this->releasedPages.data(), data.data() + pos, released_pages_size);
}

pgnum freelist::getNextPage() {
    if (releasedPages.size() != 0) {
        pgnum pageID = releasedPages.back();
        releasedPages.pop_back();
        return pageID;
    }
    maxPage += 1;
    return maxPage;
}

void freelist::releasePage(pgnum page) {
    releasedPages.push_back(page);
}

dal::dal(std::string path, std::fstream *file, Options options) {
    meta = newEmptyMeta();
    freeList = new freelist{0};
    minFillPercent = options.minFillPercent;
    maxFillPercent = options.maxFillPercent;
    pagesize = options.pageSize;
}

page *dal::allocateEmptyPage() {
    return new page {0, this->pagesize, new std::vector<BYTE>{}};
};

page *dal::readPage(pgnum pageNum) {
    file->flush();
    //don't resize on read, because the back inserter will automatically get the correct size
    page *p = allocateEmptyPage();
    auto offset = int(pageNum) * pagesize;
    file->seekg(int64_t(offset));
    //don't eat new lines in binary mode
    file->unsetf(std::ios::skipws);

    copy_n(std::istreambuf_iterator<char>(*file), this->pagesize, back_inserter(*p->data));
    return p;
}

void dal::writePage(page *p) {
    p->data->resize(64);
    auto offset = int(p->num) * pagesize;
    file->seekp(offset);
    file->write((char *)p->data->data(), p->data->size());
    file->flush();
    if (file -> fail()) {
        std::cerr << "Error: path " << path << " failed to write";
    }
}

freelist *dal::readFreeList() {
    try {
        page *p = readPage(meta->freelistPage);
        freelist *fl = new freelist{0};
        fl->deserialize(*p->data);
        return fl;
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
    return nullptr;
}

page *dal::writeFreeList() {
    page *p = allocateEmptyPage();
    p->num = meta->freelistPage;
    freeList->serialize(*p->data);
    writePage(p);
    return p;
}

page *dal::writeMeta(Meta *meta) {
    page *p = allocateEmptyPage();
    p->num = metaPageNum;
    meta->serialize(*p->data);
    try {
        writePage(p);
    }
    catch (std::exception &e) {
        throw e;
    }
    return p;
}

Meta *dal::readMeta() {
    try {
        page *p = readPage(metaPageNum);
        Meta *meta = newEmptyMeta();
        meta->unserialize(*p->data);
        return meta;
    }
    catch (std::exception &e) {
        throw e;
    }
}

void dal::close() {
    if (file != nullptr) {
        file->close();
        if (file->fail()) {
            std::cerr << "Error: path " << path << " failed to close";
        }
        delete file;
        file = nullptr;
    }
    else {
        std::cerr << "Error: path " << path << " failed to close";
    }
}

Node *dal::getNode(pgnum pageNum) {
    page *p {readPage(pageNum)};
    Node *node = newEmptyNode();
    node->deserialize(*p->data);
    node->pageNum = pageNum;
    return node;
}

Node *dal::writeNode(Node *node) {
    page *p = allocateEmptyPage();
    if (node->pageNum == 0) {
        p->num = freeList->getNextPage();
        node->pageNum = p->num;
    }
    else {
        p->num = node->pageNum;
    }
    p->data = node->serialize(*p->data);
    writePage(p);

    return node;
}

void dal::deleteNode(pgnum pageNum) {
    freeList->releasePage(pageNum);
}

double dal::maxThreshold() {
    return maxFillPercent*pagesize;
}

double dal::minThreshold() {
    return minFillPercent*pagesize;
}

bool dal::isOverpopulated(Node *node) {
    return node->nodeSize() > maxThreshold();
}

bool dal::isUnderpopulated(Node *node) {
    return node->nodeSize() < minThreshold();
}

int dal::getSplitIndex(Node *node) {
    int size{nodeHeaderSize};
    for (int i{0}; i < node->items.size(); ++i) {
        size += node->elementSize(i);
        //need to have at least one node on the right to populate the new right child
        if (size > minThreshold() && i < node->items.size() - 1) {
            return i + 1;
        }
    }
    return -1;
}

dal *DAL::openFile(std::string path, const Options &options) {
    try {
        dal *new_dal = new dal{path, nullptr, options};

        if (std::filesystem::exists(path)) {
            std::fstream *new_file = new std::fstream(path, std::ios::binary);
            new_dal->file = new_file;
            new_dal->path = path;
            new_dal->pagesize = options.pageSize;
            new_dal->file->open(path);
            if (new_dal->file->fail()) {
                std::cerr << "Error: path " << path << " failed to open";
            }

            delete new_dal->meta;
            new_dal->meta = new_dal->readMeta();

            delete new_dal->freeList;
            new_dal->freeList = new_dal->readFreeList();
        }
        else {
            std::fstream *new_file = new std::fstream(path, std::ios::binary | std::fstream::in | std::fstream::out | std::fstream::trunc);
            new_dal->file = new_file;
            new_dal->path = path;
            new_dal->pagesize = options.pageSize;
            new_dal->meta->freelistPage = new_dal->freeList->getNextPage();
            new_dal->writeFreeList();
            new_dal->writeMeta(new_dal->meta);
        }

        return new_dal;
    }
    catch (std::exception &e){
        std::cerr << e.what() << std::endl;
        throw e;
    }
}