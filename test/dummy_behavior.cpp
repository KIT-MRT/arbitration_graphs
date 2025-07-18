#include <string>

#include <gtest/gtest.h>

#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class DummyBehaviorTest : public ::testing::Test {
protected:
    DummyBehavior testBehaviorTrue{true, true};
    DummyEnvironmentModel environmentModel;

    DummyCommand expectedCommand{"DummyBehavior"};

    Time time{Clock::now()};
};

TEST_F(DummyBehaviorTest, BasicInterface) {
    EXPECT_EQ(0, environmentModel.accessCounter);
    EXPECT_EQ(expectedCommand, testBehaviorTrue.getCommand(time, environmentModel));
    EXPECT_EQ(1, environmentModel.accessCounter);
    EXPECT_TRUE(testBehaviorTrue.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ(2, environmentModel.accessCounter);
    EXPECT_TRUE(testBehaviorTrue.checkInvocationCondition(time, environmentModel));
    EXPECT_EQ(3, environmentModel.accessCounter);
    EXPECT_NO_THROW(testBehaviorTrue.gainControl(time, environmentModel));
    EXPECT_EQ(4, environmentModel.accessCounter);
    EXPECT_NO_THROW(testBehaviorTrue.loseControl(time, environmentModel));
    EXPECT_EQ(5, environmentModel.accessCounter);
}

TEST_F(DummyBehaviorTest, Printout) {
    const std::string expectedPrintout = InvocationTrueString + CommitmentTrueString + "DummyBehavior";
    std::string actualPrintout = testBehaviorTrue.toString(time, environmentModel);
    std::cout << actualPrintout << '\n';

    EXPECT_EQ(expectedPrintout, actualPrintout);
}

TEST_F(DummyBehaviorTest, ToYaml) {
    YAML::Node yaml = testBehaviorTrue.toYaml(time, environmentModel);

    EXPECT_EQ("Behavior", yaml["type"].as<std::string>());
    EXPECT_EQ("DummyBehavior", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());
}
