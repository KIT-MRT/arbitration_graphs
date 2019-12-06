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
#include <map>
#include <memory>
#include <string>
#include "gtest/gtest.h"

#include "behavior.hpp"
#include "cost_arbitrator.hpp"

#include "cost_estimator.hpp"
#include "dummy_types.hpp"


using namespace behavior_planning;
using namespace behavior_planning_tests;


class CostArbitratorTest : public ::testing::Test {
protected:
    using OptionFlags = CostArbitrator<DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorLowCost = std::make_shared<DummyBehavior>(false, false, "low_cost");
    DummyBehavior::Ptr testBehaviorMidCost = std::make_shared<DummyBehavior>(true, false, "mid_cost");
    DummyBehavior::Ptr testBehaviorHighCost = std::make_shared<DummyBehavior>(true, true, "high_cost");

    CostEstimatorFromCostMap::CostMap costMap{{"low_cost", 0}, {"mid_cost", 0.5}, {"high_cost", 1}};
    CostEstimatorFromCostMap::Ptr cost_estimator = std::make_shared<CostEstimatorFromCostMap>(costMap);
    CostEstimatorFromCostMap::Ptr cost_estimator_with_activation_costs =
        std::make_shared<CostEstimatorFromCostMap>(costMap, 10);

    CostArbitrator<DummyCommand> testCostArbitrator;
};


TEST_F(CostArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NO_FLAGS, cost_estimator);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorMidCost->loseControlCounter_);

    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(1, testBehaviorMidCost->loseControlCounter_);

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());

    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorHighCost->loseControlCounter_);
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorHighCost->loseControlCounter_);

    // high_cost behavior is not interruptable -> high_cost should stay active
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
}

TEST_F(CostArbitratorTest, Printout) {
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NO_FLAGS, cost_estimator);

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentFalseString + "CostArbitrator\n"
                                    "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                                    "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                                    "    - (cost:  n.a.) " + invocationTrueString + commitmentTrueString + "high_cost\n"
                                    "    - (cost:  n.a.) " + invocationTrueString + commitmentFalseString + "mid_cost";
    // clang-format on
    std::stringstream actual_printout;
    actual_printout << testCostArbitrator;
    std::cout << actual_printout.str() << std::endl;

    EXPECT_EQ(expected_printout, actual_printout.str());


    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "CostArbitrator\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                        "    - (cost: 1.000) " + invocationTrueString + commitmentTrueString + "high_cost\n"
                        " -> - (cost: 0.500) " + invocationTrueString + commitmentFalseString + "mid_cost";
    // clang-format on
    actual_printout.str("");
    actual_printout << testCostArbitrator;
    std::cout << actual_printout.str() << std::endl;

    EXPECT_EQ(expected_printout, actual_printout.str());
}


TEST_F(CostArbitratorTest, BasicFunctionalityWithInterruptableOptionsAndActivationCosts) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.addOption(
        testBehaviorHighCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());

    // high_cost behavior is not interruptable -> high_cost should stay active
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
}


TEST_F(CostArbitratorTest, BasicFunctionalityWithInterruptableOptions) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::INTERRUPTABLE, cost_estimator);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());

    // high_cost behavior is interruptable -> mid_cost should become active again
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
}

TEST(CostArbitrator, SubCommandTypeDiffersFromCommandType) {
    using OptionFlags = CostArbitrator<DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorLowCost = std::make_shared<DummyBehavior>(false, false, "low_cost");
    DummyBehavior::Ptr testBehaviorMidCost = std::make_shared<DummyBehavior>(true, false, "__mid_cost__");
    DummyBehavior::Ptr testBehaviorHighCost = std::make_shared<DummyBehavior>(true, true, "____high_cost____");

    CostEstimatorFromCostMap::CostMap costMap{{"low_cost", 0}, {"__mid_cost__", 0.5}, {"____high_cost____", 1}};
    CostEstimatorFromCostMap::Ptr cost_estimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    CostArbitrator<DummyCommandInt, DummyCommand> testCostArbitrator;

    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::INTERRUPTABLE, cost_estimator);

    testCostArbitrator.gainControl();

    std::string expected = "__mid_cost__";
    EXPECT_EQ(expected.length(), testCostArbitrator.getCommand());
}
