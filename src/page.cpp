//
// Created by Josh on 2023-08-20.
//

#include "page.h"

void page::writeData(std::string data) {
    if (data.size() > size) {
        std::cerr << "Attempted to write " << data << "of size " << data.size() << " into block of size " << size << std::endl;
        return;
    }
    this->data->assign(data.begin(), data.end());
}