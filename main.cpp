#include <iostream>
#include "src/DAL.h"
#include "collection.h"
#include <filesystem>

using namespace std;

std::vector<BYTE> toVector(std::string s) {
    return std::vector<BYTE>{s.begin(), s.end()};
}

int main() {
    DAL::dal *new_dal = DAL::openFile("../asdf.txt");
    std::string name = "collection1";
    std::vector<BYTE> name_bytes {name.begin(), name.end()};
    Collection c{name_bytes, new_dal->meta->root, new_dal};

    c.put(toVector("key1"), toVector("value1"));
    c.put(toVector("key2"), toVector("value2"));
    c.put(toVector("key3"), toVector("value3"));
    c.put(toVector("key4"), toVector("value4"));
    c.put(toVector("key5"), toVector("value5"));
    c.put(toVector("key6"), toVector("value6"));

    Item *i = c.find(toVector("key4"));
    std::cout << "key is " << std::string{i->key.begin(), i->key.end()} << std::endl;
    std::cout << "value is " << std::string{i->value.begin(), i->value.end()} << std::endl;

    c.remove(toVector("key3"));
    i = c.find(toVector("key3"));
    if (i != nullptr) {
        cout << "Something went wrong" << endl;
    }
    c.put(toVector("key4"), toVector("value4"));

    i = c.find(toVector("key4"));
    std::cout << "key is " << std::string{i->key.begin(), i->key.end()} << std::endl;
    std::cout << "value is " << std::string{i->value.begin(), i->value.end()} << std::endl;
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
