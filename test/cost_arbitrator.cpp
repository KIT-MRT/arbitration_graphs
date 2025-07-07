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
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "behavior.hpp"
#include "cost_arbitrator.hpp"

#include "cost_estimator.hpp"
#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class CostArbitratorTest : public ::testing::Test {
protected:
    using OptionFlags = CostArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorLowCost = std::make_shared<DummyBehavior>(false, false, "low_cost");
    DummyBehavior::Ptr testBehaviorMidCost = std::make_shared<DummyBehavior>(true, false, "mid_cost");
    DummyBehavior::Ptr testBehaviorHighCost = std::make_shared<DummyBehavior>(true, true, "high_cost");

    CostEstimatorFromCostMap::CostMap costMap{{"low_cost", 0}, {"mid_cost", 0.5}, {"high_cost", 1}};
    CostEstimatorFromCostMap::Ptr cost_estimator = std::make_shared<CostEstimatorFromCostMap>(costMap);
    CostEstimatorFromCostMap::Ptr cost_estimator_with_activation_costs =
        std::make_shared<CostEstimatorFromCostMap>(costMap, 10);

    CostArbitrator<DummyEnvironmentModel, DummyCommand> testCostArbitrator;

    DummyEnvironmentModel environmentModel;

    Time time{Clock::now()};
};


TEST_F(CostArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NO_FLAGS, cost_estimator);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    //! \note This should be 0, if we estimate costs without calling getCommand (and thus gain/loseControl, see c2b2a93)
    EXPECT_EQ(1, testBehaviorMidCost->loseControlCounter_);

    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    //! \note This should be 1, if we estimate costs without calling getCommand (and thus gain/loseControl, see c2b2a93)
    EXPECT_EQ(3, testBehaviorMidCost->loseControlCounter_);

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
    //! \note This should be 0, if we estimate costs without calling getCommand (and thus gain/loseControl, see c2b2a93)
    EXPECT_EQ(3, testBehaviorHighCost->loseControlCounter_);
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
    //! \note This should be 0, if we estimate costs without calling getCommand (and thus gain/loseControl, see c2b2a93)
    EXPECT_EQ(3, testBehaviorHighCost->loseControlCounter_);

    // high_cost behavior is not interruptable -> high_cost should stay active
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
}

TEST_F(CostArbitratorTest, CommandCaching) {
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NO_FLAGS, cost_estimator);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ(0, testBehaviorMidCost->getCommandCounter_);

    testCostArbitrator.gainControl(time, environmentModel);

    // Even though the cost arbitrator needs to compute the command to estimate the costs, the behaviors getCommand
    // should only be called once since the result is cached
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(1, testBehaviorMidCost->getCommandCounter_);
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    // For a second call to getCommand, we can still use the cached command
    EXPECT_EQ(1, testBehaviorMidCost->getCommandCounter_);

    time = time + Duration(1);

    // The cached command should be invalidated after the time has passed
    // Therefore the behavior should be called again once for the new time
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(2, testBehaviorMidCost->getCommandCounter_);
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(2, testBehaviorMidCost->getCommandCounter_);
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
    std::string actual_printout = testCostArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);


    testCostArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "CostArbitrator\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                        "    - (cost: 1.000) " + invocationTrueString + commitmentTrueString + "high_cost\n"
                        " -> - (cost: 0.500) " + invocationTrueString + commitmentFalseString + "mid_cost";
    // clang-format on
    actual_printout = testCostArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(CostArbitratorTest, ToYaml) {
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NO_FLAGS, cost_estimator);

    YAML::Node yaml = testCostArbitrator.toYaml(time, environmentModel);

    //    std::cout << yaml << std::endl << std::endl;

    EXPECT_EQ("CostArbitrator", yaml["type"].as<std::string>());
    EXPECT_EQ("CostArbitrator", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(4, yaml["options"].size());
    EXPECT_EQ("Option", yaml["options"][0]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][1]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][2]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][3]["type"].as<std::string>());
    EXPECT_EQ("low_cost", yaml["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("low_cost", yaml["options"][1]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("high_cost", yaml["options"][2]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("mid_cost", yaml["options"][3]["behavior"]["name"].as<std::string>());
    EXPECT_EQ(false, yaml["options"][0]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][1]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][2]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][3]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][0]["cost"].IsDefined());
    EXPECT_EQ(false, yaml["options"][1]["cost"].IsDefined());
    EXPECT_EQ(false, yaml["options"][2]["cost"].IsDefined());
    EXPECT_EQ(false, yaml["options"][3]["cost"].IsDefined());

    EXPECT_EQ(false, yaml["activeBehavior"].IsDefined());

    testCostArbitrator.gainControl(time, environmentModel);
    testCostArbitrator.getCommand(time, environmentModel);

    yaml = testCostArbitrator.toYaml(time, environmentModel);

    //    std::cout << yaml << std::endl << std::endl;

    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());

    EXPECT_EQ(false, yaml["options"][0]["cost"].IsDefined());
    EXPECT_EQ(false, yaml["options"][1]["cost"].IsDefined());
    ASSERT_EQ(true, yaml["options"][2]["cost"].IsDefined());
    ASSERT_EQ(true, yaml["options"][3]["cost"].IsDefined());
    EXPECT_NEAR(1.0, yaml["options"][2]["cost"].as<double>(), 1e-3);
    EXPECT_NEAR(0.5, yaml["options"][3]["cost"].as<double>(), 1e-3);

    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    EXPECT_EQ(3, yaml["activeBehavior"].as<int>());
}


TEST_F(CostArbitratorTest, BasicFunctionalityWithInterruptableOptionsAndActivationCosts) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.addOption(
        testBehaviorHighCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::INTERRUPTABLE, cost_estimator_with_activation_costs);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));

    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));

    // high_cost behavior is not interruptable -> high_cost should stay active
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
}


TEST_F(CostArbitratorTest, BasicFunctionalityWithInterruptableOptions) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::INTERRUPTABLE, cost_estimator);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand(time, environmentModel));

    // high_cost behavior is interruptable -> mid_cost should become active again
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));
}

TEST(CostArbitrator, SubCommandTypeDiffersFromCommandType) {
    DummyEnvironmentModel environmentModel;
    Time time{Clock::now()};

    using OptionFlags = CostArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorLowCost = std::make_shared<DummyBehavior>(false, false, "low_cost");
    DummyBehavior::Ptr testBehaviorMidCost = std::make_shared<DummyBehavior>(true, false, "__mid_cost__");
    DummyBehavior::Ptr testBehaviorHighCost = std::make_shared<DummyBehavior>(true, true, "____high_cost____");

    CostEstimatorFromCostMap::CostMap costMap{{"low_cost", 0}, {"__mid_cost__", 0.5}, {"____high_cost____", 1}};
    CostEstimatorFromCostMap::Ptr cost_estimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    CostArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand> testCostArbitrator;

    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::INTERRUPTABLE, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::INTERRUPTABLE, cost_estimator);

    testCostArbitrator.gainControl(time, environmentModel);

    std::string expected = "__mid_cost__";
    EXPECT_EQ(expected.length(), testCostArbitrator.getCommand(time, environmentModel));
}
