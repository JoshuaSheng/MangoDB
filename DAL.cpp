//
// Created by Josh on 2023-08-19.
//

#include "DAL.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>
#include <vector>
#include <iterator>

using namespace std;

typedef uint64_t pgnum;
typedef unsigned char BYTE;

freelist::freelist(pgnum initialPage) {
    maxPage = initialPage;
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

dal *open(string path, int pageSize) {
    dal *new_dal = new dal{path, pageSize, nullptr, new freelist{0}};
    fstream *new_file = new fstream(path, ios::binary);

    new_dal->file = new_file;
    new_dal->path = path;

    new_dal->file->open(path);
    if (new_dal->file->fail()) {
        cerr << "Error: path " << path << " failed to open";
    }
    return new_dal;
}