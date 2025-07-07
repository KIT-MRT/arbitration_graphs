#include <memory>
#include <string>

#include <gtest/gtest.h>

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
    CostEstimatorFromCostMap::Ptr costEstimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    CostArbitratorT::Ptr testCostArbitrator = std::make_shared<CostArbitratorT>();
    PriorityArbitratorT::Ptr testPriorityArbitrator = std::make_shared<PriorityArbitratorT>();

    PriorityArbitratorT::Ptr testRootPriorityArbitrator =
        std::make_shared<PriorityArbitratorT>("root priority arbitrator");

    DummyEnvironmentModel environmentModel;

    Time time{Clock::now()};
};

TEST_F(NestedArbitratorsTest, Printout) {
    testRootPriorityArbitrator->addOption(testCostArbitrator, PriorityOptionFlags::NoFlags);
    testRootPriorityArbitrator->addOption(testPriorityArbitrator, PriorityOptionFlags::NoFlags);

    testCostArbitrator->addOption(testBehaviorLowCost, CostOptionFlags::NoFlags, costEstimator);
    testCostArbitrator->addOption(testBehaviorHighCost, CostOptionFlags::NoFlags, costEstimator);

    testPriorityArbitrator->addOption(testBehaviorHighPriority, PriorityOptionFlags::NoFlags);
    testPriorityArbitrator->addOption(testBehaviorLowPriority, PriorityOptionFlags::NoFlags);


    // clang-format off
    std::string expectedPrintout = InvocationTrueString + CommitmentFalseString + "root priority arbitrator\n"
                                    "    1. " + InvocationTrueString + CommitmentFalseString + "CostArbitrator\n"
                                    "        - (cost:  n.a.) " + InvocationFalseString + CommitmentFalseString + "low_cost\n"
                                    "        - (cost:  n.a.) " + InvocationTrueString + CommitmentTrueString + "high_cost\n"
                                    "    2. " + InvocationTrueString + CommitmentFalseString + "PriorityArbitrator\n"
                                    "        1. " + InvocationFalseString + CommitmentFalseString + "HighPriority\n"
                                    "        2. " + InvocationTrueString + CommitmentTrueString + "LowPriority";
    // clang-format on

    // 1. test to_str()
    EXPECT_EQ(expectedPrintout, testRootPriorityArbitrator->to_str(time, environmentModel));

    // 1. test to_stream()
    std::stringstream actualPrintoutStream;
    testRootPriorityArbitrator->to_stream(actualPrintoutStream, time, environmentModel);
    EXPECT_EQ(expectedPrintout, actualPrintoutStream.str());

    std::cout << actualPrintoutStream.str() << std::endl;


    testPriorityArbitrator->gainControl(time, environmentModel);
    EXPECT_EQ("high_cost", testRootPriorityArbitrator->getCommand(time, environmentModel));

    // clang-format off
    expectedPrintout = InvocationTrueString + CommitmentTrueString + "root priority arbitrator\n"
                        " -> 1. "  + InvocationTrueString + CommitmentTrueString + "CostArbitrator\n"
                        "        - (cost:  n.a.) " + InvocationFalseString + CommitmentFalseString + "low_cost\n"
                        "     -> - (cost: 1.000) " + InvocationTrueString + CommitmentTrueString + "high_cost\n"
                        "    2. " + InvocationTrueString + CommitmentFalseString + "PriorityArbitrator\n"
                        "        1. " + InvocationFalseString + CommitmentFalseString + "HighPriority\n"
                        "        2. " + InvocationTrueString + CommitmentTrueString + "LowPriority";
    // clang-format on
    std::string actualPrintout = testRootPriorityArbitrator->to_str(time, environmentModel);
    EXPECT_EQ(expectedPrintout, actualPrintout);

    std::cout << actualPrintout << std::endl;
}


TEST_F(NestedArbitratorsTest, ToYaml) {
    testRootPriorityArbitrator->addOption(testCostArbitrator, PriorityOptionFlags::NoFlags);
    testRootPriorityArbitrator->addOption(testPriorityArbitrator, PriorityOptionFlags::NoFlags);

    testCostArbitrator->addOption(testBehaviorLowCost, CostOptionFlags::NoFlags, costEstimator);
    testCostArbitrator->addOption(testBehaviorHighCost, CostOptionFlags::NoFlags, costEstimator);

    testPriorityArbitrator->addOption(testBehaviorHighPriority, PriorityOptionFlags::NoFlags);
    testPriorityArbitrator->addOption(testBehaviorLowPriority, PriorityOptionFlags::NoFlags);

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
