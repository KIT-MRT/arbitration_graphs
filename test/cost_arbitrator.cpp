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
    CostEstimatorFromCostMap::Ptr costEstimator = std::make_shared<CostEstimatorFromCostMap>(costMap);
    CostEstimatorFromCostMap::Ptr costEstimatorWithActivationCosts =
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
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NoFlags, costEstimator);

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
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NoFlags, costEstimator);

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
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NoFlags, costEstimator);

    // clang-format off
    std::string expectedPrintout = InvocationTrueString + CommitmentFalseString + "CostArbitrator\n"
                                    "    - (cost:  n.a.) " + InvocationFalseString + CommitmentFalseString + "low_cost\n"
                                    "    - (cost:  n.a.) " + InvocationFalseString + CommitmentFalseString + "low_cost\n"
                                    "    - (cost:  n.a.) " + InvocationTrueString + CommitmentTrueString + "high_cost\n"
                                    "    - (cost:  n.a.) " + InvocationTrueString + CommitmentFalseString + "mid_cost";
    // clang-format on
    std::string actualPrintout = testCostArbitrator.to_str(time, environmentModel);
    std::cout << actualPrintout << '\n';

    EXPECT_EQ(expectedPrintout, actualPrintout);


    testCostArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand(time, environmentModel));

    // clang-format off
    expectedPrintout = InvocationTrueString + CommitmentTrueString + "CostArbitrator\n"
                        "    - (cost:  n.a.) " + InvocationFalseString + CommitmentFalseString + "low_cost\n"
                        "    - (cost:  n.a.) " + InvocationFalseString + CommitmentFalseString + "low_cost\n"
                        "    - (cost: 1.000) " + InvocationTrueString + CommitmentTrueString + "high_cost\n"
                        " -> - (cost: 0.500) " + InvocationTrueString + CommitmentFalseString + "mid_cost";
    // clang-format on
    actualPrintout = testCostArbitrator.to_str(time, environmentModel);
    std::cout << actualPrintout << '\n';

    EXPECT_EQ(expectedPrintout, actualPrintout);
}

TEST_F(CostArbitratorTest, ToYaml) {
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::NoFlags, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::NoFlags, costEstimator);

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
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::Interruptable, costEstimatorWithActivationCosts);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::Interruptable, costEstimatorWithActivationCosts);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::Interruptable, costEstimatorWithActivationCosts);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::Interruptable, costEstimatorWithActivationCosts);

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
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::Interruptable, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::Interruptable, costEstimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition(time, environmentModel));

    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::Interruptable, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::Interruptable, costEstimator);

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
    CostEstimatorFromCostMap::Ptr costEstimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    CostArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand> testCostArbitrator;

    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::Interruptable, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowCost, OptionFlags::Interruptable, costEstimator);
    testCostArbitrator.addOption(testBehaviorHighCost, OptionFlags::Interruptable, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidCost, OptionFlags::Interruptable, costEstimator);

    testCostArbitrator.gainControl(time, environmentModel);

    std::string expected = "__mid_cost__";
    EXPECT_EQ(expected.length(), testCostArbitrator.getCommand(time, environmentModel));
}
