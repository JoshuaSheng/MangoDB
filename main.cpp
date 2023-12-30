#include <iostream>
#include "src/DAL.h"
#include <filesystem>

using namespace std;

int main() {
    std::cout << "my directory is " << std::filesystem::current_path() << "\n";
    DAL::dal *new_dal = DAL::openFile("../asdf.txt", 64);
    return 0;
}
