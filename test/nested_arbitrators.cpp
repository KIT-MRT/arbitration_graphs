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
#include "gtest/gtest.h"

#include "behavior.hpp"
#include "cost_arbitrator.hpp"
#include "priority_arbitrator.hpp"

#include "cost_estimator.hpp"
#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class NestedArbitratorsTest : public ::testing::Test {
protected:
    using PriorityOptionFlags = PriorityArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;
    using CostOptionFlags = CostArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    using CostArbitratorT = CostArbitrator<DummyEnvironmentModel, DummyCommand>;
    using PriorityArbitratorT = PriorityArbitrator<DummyEnvironmentModel, DummyCommand>;

    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "HighPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");

    DummyBehavior::Ptr testBehaviorLowCost = std::make_shared<DummyBehavior>(false, false, "low_cost");
    DummyBehavior::Ptr testBehaviorMidCost = std::make_shared<DummyBehavior>(true, false, "mid_cost");
    DummyBehavior::Ptr testBehaviorHighCost = std::make_shared<DummyBehavior>(true, true, "high_cost");

    CostEstimatorFromCostMap::CostMap costMap{{"low_cost", 0}, {"mid_cost", 0.5}, {"high_cost", 1}};
    CostEstimatorFromCostMap::Ptr cost_estimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    CostArbitratorT::Ptr testCostArbitrator = std::make_shared<CostArbitratorT>();
    PriorityArbitratorT::Ptr testPriorityArbitrator = std::make_shared<PriorityArbitratorT>();

    PriorityArbitratorT::Ptr testRootPriorityArbitrator =
        std::make_shared<PriorityArbitratorT>("root priority arbitrator");

    DummyEnvironmentModel environmentModel;

    Time time{Clock::now()};
};

TEST_F(NestedArbitratorsTest, Printout) {
    testRootPriorityArbitrator->addOption(testCostArbitrator, PriorityOptionFlags::NO_FLAGS);
    testRootPriorityArbitrator->addOption(testPriorityArbitrator, PriorityOptionFlags::NO_FLAGS);

    testCostArbitrator->addOption(testBehaviorLowCost, CostOptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator->addOption(testBehaviorHighCost, CostOptionFlags::NO_FLAGS, cost_estimator);

    testPriorityArbitrator->addOption(testBehaviorHighPriority, PriorityOptionFlags::NO_FLAGS);
    testPriorityArbitrator->addOption(testBehaviorLowPriority, PriorityOptionFlags::NO_FLAGS);


    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentFalseString + "root priority arbitrator\n"
                                    "    1. " + invocationTrueString + commitmentFalseString + "CostArbitrator\n"
                                    "        - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                                    "        - (cost:  n.a.) " + invocationTrueString + commitmentTrueString + "high_cost\n"
                                    "    2. " + invocationTrueString + commitmentFalseString + "PriorityArbitrator\n"
                                    "        1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                                    "        2. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on

    // 1. test to_str()
    EXPECT_EQ(expected_printout, testRootPriorityArbitrator->to_str(time, environmentModel));

    // 1. test to_stream()
    std::stringstream actual_printout_stream;
    testRootPriorityArbitrator->to_stream(actual_printout_stream, time, environmentModel);
    EXPECT_EQ(expected_printout, actual_printout_stream.str());

    std::cout << actual_printout_stream.str() << std::endl;


    testPriorityArbitrator->gainControl(time, environmentModel);
    EXPECT_EQ("high_cost", testRootPriorityArbitrator->getCommand(time, environmentModel));

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "root priority arbitrator\n"
                        " -> 1. "  + invocationTrueString + commitmentTrueString + "CostArbitrator\n"
                        "        - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "low_cost\n"
                        "     -> - (cost: 1.000) " + invocationTrueString + commitmentTrueString + "high_cost\n"
                        "    2. " + invocationTrueString + commitmentFalseString + "PriorityArbitrator\n"
                        "        1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "        2. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actual_printout = testRootPriorityArbitrator->to_str(time, environmentModel);
    EXPECT_EQ(expected_printout, actual_printout);

    std::cout << actual_printout << std::endl;
}


TEST_F(NestedArbitratorsTest, ToYaml) {
    testRootPriorityArbitrator->addOption(testCostArbitrator, PriorityOptionFlags::NO_FLAGS);
    testRootPriorityArbitrator->addOption(testPriorityArbitrator, PriorityOptionFlags::NO_FLAGS);

    testCostArbitrator->addOption(testBehaviorLowCost, CostOptionFlags::NO_FLAGS, cost_estimator);
    testCostArbitrator->addOption(testBehaviorHighCost, CostOptionFlags::NO_FLAGS, cost_estimator);

    testPriorityArbitrator->addOption(testBehaviorHighPriority, PriorityOptionFlags::NO_FLAGS);
    testPriorityArbitrator->addOption(testBehaviorLowPriority, PriorityOptionFlags::NO_FLAGS);

    YAML::Node yaml = testRootPriorityArbitrator->toYaml(time, environmentModel);

    //    std::cout << yaml << std::endl << std::endl;

    EXPECT_EQ("PriorityArbitrator", yaml["type"].as<std::string>());
    EXPECT_EQ("root priority arbitrator", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(2, yaml["options"].size());
    EXPECT_EQ("Option", yaml["options"][0]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][1]["type"].as<std::string>());
    EXPECT_EQ("CostArbitrator", yaml["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("PriorityArbitrator", yaml["options"][1]["behavior"]["name"].as<std::string>());
    EXPECT_EQ(false, yaml["options"][0]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][1]["flags"].IsDefined());

    ASSERT_EQ(2, yaml["options"][0]["behavior"]["options"].size());
    EXPECT_EQ(false, yaml["options"][0]["behavior"]["activeBehavior"].IsDefined());
    EXPECT_EQ("low_cost", yaml["options"][0]["behavior"]["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("high_cost", yaml["options"][0]["behavior"]["options"][1]["behavior"]["name"].as<std::string>());

    ASSERT_EQ(2, yaml["options"][1]["behavior"]["options"].size());
    EXPECT_EQ(false, yaml["options"][1]["behavior"]["activeBehavior"].IsDefined());
    EXPECT_EQ("HighPriority", yaml["options"][1]["behavior"]["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("LowPriority", yaml["options"][1]["behavior"]["options"][1]["behavior"]["name"].as<std::string>());

    EXPECT_EQ(false, yaml["activeBehavior"].IsDefined());

    testPriorityArbitrator->gainControl(time, environmentModel);
    testRootPriorityArbitrator->getCommand(time, environmentModel);

    yaml = testRootPriorityArbitrator->toYaml(time, environmentModel);

    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    EXPECT_EQ(0, yaml["activeBehavior"].as<int>());

    ASSERT_EQ(true, yaml["options"][0]["behavior"]["activeBehavior"].IsDefined());
    EXPECT_EQ(1, yaml["options"][0]["behavior"]["activeBehavior"].as<int>());
}
