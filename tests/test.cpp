#include <gtest/gtest.h>
#include <gmock/gmock.h>
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
    DAL::dal new_dal{};
    std::string file = "../../tests/dal_test";
    DAL::dal *dal = DAL::openFile(file, 4);
    auto p = dal->allocateEmptyPage();
    p->num = dal->freeList->getNextPage();
    p->writeData("asdf");

    EXPECT_THAT(p->num, 1);
    EXPECT_THAT(*(p->data), ElementsAre('a', 's', 'd', 'f'));

    dal->writePage(p);
    DAL::page *pg_clone = dal->readPage(p->num);

    EXPECT_THAT(*pg_clone->data, ElementsAre('a', 's', 'd', 'f'));

    dal->close();
    EXPECT_THAT(dal->file, nullptr);

}