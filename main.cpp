#include <iostream>
#include <filesystem>
#include "db.h"

using namespace std;

int main() {
    std::string path = "mango.db";
    DB *db = open(path, defaultOptions);
    Tx *tx = db->writeTx();
    std::string collectionName = "collection1";
    Collection *c = tx->createCollection(collectionName);
    std::string key = "key1", value = "value1";
    c->put(key, value);
    tx->commit();
    Item *foundItem = c->find(key);
    std::cout << "key is " << std::string{foundItem->key.begin(), foundItem->key.end()} << std::endl;
    std::cout << "value is " << std::string{foundItem->value.begin(), foundItem->value.end()} << std::endl;

//command-line driver code, WIP

//    while (true) {
//        char in;
//        std::cout << "Enter your choice" << std::endl;
//        std::cin >> in;
//        if (in == 'i') {
//            std::string key, value;
//            std::cin >> key >> value;
//            std::cout << key << ' ' << value << endl;
//        }
//        else if (in == 's') {
//            std::string key;
//            std::cin >> key;
//            std::cout << key << std::endl;
//        }
//        else if (in == 'd') {
//            std::string key;
//            std::cin >> key;
//            std::cout << key << std::endl;
//        }
//        else if (in == 'e') {
//            break;
//        }
//        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//    }
    return 0;
}
