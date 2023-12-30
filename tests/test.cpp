#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include "../src/DAL.h"

using namespace testing;

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 41);
}

TEST(TestDal, TestDal) {
    cout << std::filesystem::current_path() << endl;
    std::string file = "../../tests/dal_test";
    DAL::dal *dal = DAL::openFile(file, 64);
    auto p = dal->allocateEmptyPage();
    p->num = dal->freeList->getNextPage();
    p->writeData("asdf");

    EXPECT_THAT(p->num, 2);
    EXPECT_EQ((*p->data)[3], 'f');



    dal->writePage(p);
    dal->writeFreeList();
    page *pg_clone = dal->readPage(p->num);

    EXPECT_EQ((*pg_clone->data)[3], 'f');

    dal->close();
    EXPECT_THAT(dal->file, nullptr);

}