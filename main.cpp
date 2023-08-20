#include <iostream>
#include "src/DAL.h"
#include <filesystem>

using namespace std;

int main() {
    DAL::dal *new_dal = DAL::openFile("../asdf.txt", 4);
    std::cout << "my directory is " << std::filesystem::current_path() << "\n";
    std::fstream f;
    f.open("../asdf.txt");
    std::cout << f.fail() << std::endl;
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
