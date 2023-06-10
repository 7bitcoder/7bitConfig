#include "Mocks/ConfigurationBuilderMock.hpp"
#include "SevenBit/Config/InMemoryConfiguration.hpp"
#include "SevenBit/Config/Json.hpp"
#include "SevenBit/Config/JsonConfiguration.hpp"
#include "SevenBit/Config/MapConfiguration.hpp"
#include <gtest/gtest.h>
#include <iostream>

class SettingConfigutationTest : public testing::Test
{
  protected:
    ConfigurationBuilderMock mock;

    static void TearUpTestSuite() {}

    SettingConfigutationTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~SettingConfigutationTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(SettingConfigutationTest, ShouldLoadSimpleSettingConfiguration)
{
    auto provider =
        sb::cf::InMemoryConfigurationSource::create({{"yes:yes:inner", sb::cf::JsonArray{1, 2, 3, 4, 5}}})->build(mock);

    provider->load();

    sb::cf::JsonObject expected = {{"yes", {{"yes", {{"inner", sb::cf::JsonArray{1, 2, 3, 4, 5}}}}}}};

    EXPECT_EQ(provider->getConfiguration(), expected);
}
