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

using namespace std;
using namespace DAL;

freelist::freelist(pgnum initialPage) {
    maxPage = initialPage;
}

void freelist::serialize(vector<BYTE> *data) {
    int pos = 0;
    auto released_pages_size = this->releasedPages.size();

    memcpy(data->data() + pos, &this->maxPage, sizeof(pgnum));
    pos += sizeof(pgnum);
    memcpy(data->data() + pos, &released_pages_size, sizeof(released_pages_size));
    pos += sizeof(released_pages_size);
    memcpy(data->data() + pos, this->releasedPages.data(), sizeof(*this));
}

void freelist::deserialize(vector<BYTE> *data) {
    int pos = 0;
    size_t released_pages_size = 0;

    data->resize(sizeof(this->maxPage) + sizeof(size_t));
    memcpy(&this->maxPage, data->data() + pos, sizeof(this->maxPage));
    pos += sizeof(this->maxPage);
    memcpy(&released_pages_size, data->data() + pos, sizeof(released_pages_size));
    pos += sizeof(released_pages_size);

    data->resize(data->size() + released_pages_size);
    memcpy(this->releasedPages.data(), data->data() + pos, released_pages_size);
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

dal::dal(string path, int pagesize, fstream *file) {
    meta = newEmptyMeta();
    freeList = new freelist{0};
}

page *dal::allocateEmptyPage() {
    return new page {0, this->pagesize, new vector<BYTE>{}};
};

page *dal::readPage(pgnum pageNum) {
    page *p = allocateEmptyPage();
    auto offset = int(pageNum) * pagesize;
    file->seekg(int64_t(offset));
    //don't eat new lines in binary mode
    file->unsetf(std::ios::skipws);

    p->data->reserve(p->size);
    p->data->insert(p->data->begin(),
                    istreambuf_iterator<char>(*file),
                    istreambuf_iterator<char>());
    return p;
}

void dal::writePage(page *p) {
    auto offset = int(p->num) * pagesize;
    file->seekg(offset);
    file->write((char *)p->data->data(), p->size);
    if (file -> fail()) {
        cerr << "Error: path " << path << " failed to write";
    }
}

freelist *dal::readFreeList() {
    try {
        page *p = readPage(meta->freelistPage);
        freelist *freelist;
        freelist->deserialize(p->data);
        return freelist;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    return nullptr;
}

page *dal::writeFreeList() {
    page *p = allocateEmptyPage();
    p->num = meta->freelistPage;
    freeList->serialize(p->data);
    writePage(p);
    return p;
}

page *dal::writeMeta(Meta *meta) {
    page *p = allocateEmptyPage();
    p->num = metaPageNum;
    meta->serialize(p->data);
    try {
        writePage(p);
    }
    catch (exception &e) {
        throw e;
    }
    return p;
}

Meta *dal::readMeta() {
    try {
        page *p = readPage(metaPageNum);
        Meta *meta = newEmptyMeta();
        meta->unserialize(p->data);
        return meta;
    }
    catch (exception &e) {
        throw e;
    }
}

void dal::close() {
    if (file != nullptr) {
        file->close();
        if (file->fail()) {
            cerr << "Error: path " << path << " failed to close";
        }
        delete file;
        file = nullptr;
    }
    else {
        cerr << "Error: path " << path << " failed to close";
    }
}

dal *DAL::openFile(string path, int pageSize) {
    try {
        dal *new_dal = new dal{path, pageSize, nullptr};

        if (filesystem::exists(path)) {
            fstream *new_file = new fstream(path, ios::binary);

            new_dal->file = new_file;
            new_dal->path = path;
            new_dal->file->open(path);
            if (new_dal->file->fail()) {
                cerr << "Error: path " << path << " failed to open";
            }

            delete new_dal->meta;
            new_dal->meta = new_dal->readMeta();

            delete new_dal->freeList;
            new_dal->freeList = new_dal->readFreeList();
        }
        else {
            fstream *new_file = new fstream(path, ios::binary | fstream::app);
            new_dal->file = new_file;
            new_dal->path = path;
            new_dal->meta->freelistPage = new_dal->freeList->getNextPage();
            new_dal->writeFreeList();
            new_dal->writeMeta(new_dal->meta);
        }

        return new_dal;
    }
    catch (exception &e){
        cerr << e.what() << endl;
        throw e;
    }
}