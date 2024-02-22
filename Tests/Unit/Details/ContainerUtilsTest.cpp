#include <gtest/gtest.h>
#include <iostream>

#include "SevenBit/Conf/Details/ContainerUtils.hpp"

class ContainerUtilsTest : public testing::Test
{
  protected:
    static void TearUpTestSuite() {}

    ContainerUtilsTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~ContainerUtilsTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(ContainerUtilsTest, ShouldEraseElements)
{
    std::vector vec = {1, 2, 3, 4, 5, 6};

    auto removed = sb::cf::details::ContainerUtils::eraseIf(vec, [](int val) { return val % 2; });

    EXPECT_EQ(removed, 3);
    EXPECT_EQ(vec, (std::vector{2, 4, 6}));
}
