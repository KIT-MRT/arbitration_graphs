// google test docs
// wiki page: https://code.google.com/p/googletest/w/list
// primer: https://code.google.com/p/googletest/wiki/V1_7_Primer
// FAQ: https://code.google.com/p/googletest/wiki/FAQ
// advanced guide: https://code.google.com/p/googletest/wiki/V1_7_AdvancedGuide
// samples: https://code.google.com/p/googletest/wiki/V1_7_Samples
//
// List of some basic tests fuctions:
// Fatal assertion                      Nonfatal assertion Verifies /
// Description
//-------------------------------------------------------------------------------------------------------------------------------------------------------
// ASSERT_EQ(expected, actual);         EXPECT_EQ(expected, actual); expected ==
// actual ASSERT_NE(val1, val2);               EXPECT_NE(val1, val2); val1 !=
// val2 ASSERT_LT(val1, val2);               EXPECT_LT(val1, val2); val1 < val2
// ASSERT_LE(val1, val2);               EXPECT_LE(val1, val2); val1 <= val2
// ASSERT_GT(val1, val2);               EXPECT_GT(val1, val2); val1 > val2
// ASSERT_GE(val1, val2);               EXPECT_GE(val1, val2); val1 >= val2
//
// ASSERT_FLOAT_EQ(expected, actual);   EXPECT_FLOAT_EQ(expected, actual);   the
// two float values are almost equal (4 ULPs) ASSERT_DOUBLE_EQ(expected,
// actual); EXPECT_DOUBLE_EQ(expected, actual);  the two double values are
// almost equal (4 ULPs) ASSERT_NEAR(val1, val2, abs_error);  EXPECT_NEAR(val1,
// val2, abs_error); the difference between val1 and val2 doesn't exceed the
// given absolute error
//
// Note: more information about ULPs can be found here:
// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
//
// Example of two unit test:
// TEST(Math, Add) {
//    ASSERT_EQ(10, 5+ 5);
//}
//
// TEST(Math, Float) {
//	  ASSERT_FLOAT_EQ((10.0f + 2.0f) * 3.0f, 10.0f * 3.0f + 2.0f * 3.0f)
//}
//=======================================================================================================================================================
#include <string>

#include <gtest/gtest.h>

#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class DummyBehaviorTest : public ::testing::Test {
protected:
    DummyBehavior testBehaviorTrue{true, true};
    DummyEnvironmentModel environmentModel;

    DummyCommand expected_command{"DummyBehavior"};

    Time time{Clock::now()};
};

TEST_F(DummyBehaviorTest, BasicInterface) {
    EXPECT_EQ(0, environmentModel.accessCounter);
    EXPECT_EQ(expected_command, testBehaviorTrue.getCommand(time, environmentModel));
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
    const std::string expected_printout = invocationTrueString + commitmentTrueString + "DummyBehavior";
    std::string actual_printout = testBehaviorTrue.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(DummyBehaviorTest, ToYaml) {
    YAML::Node yaml = testBehaviorTrue.toYaml(time, environmentModel);

    EXPECT_EQ("Behavior", yaml["type"].as<std::string>());
    EXPECT_EQ("DummyBehavior", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());
}
