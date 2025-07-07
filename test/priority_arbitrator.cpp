#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "behavior.hpp"
#include "priority_arbitrator.hpp"

#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class PriorityArbitratorTest : public ::testing::Test {
protected:
    using OptionFlags = PriorityArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "HighPriority");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "MidPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");


    PriorityArbitrator<DummyEnvironmentModel, DummyCommand> testPriorityArbitrator;

    DummyEnvironmentModel environmentModel;

    Time time{Clock::now()};
};


TEST_F(PriorityArbitratorTest, BasicFunctionality) {
    DummyEnvironmentModel environmentModel;

    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));

    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NoFlags);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(0, testBehaviorMidPriority->loseControlCounter_);

    // testBehaviorMidPriority.loseControl(time) should be called within getCommand since commitment condition is false
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(1, testBehaviorMidPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));

    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    // testBehaviorLowPriority.loseControl(time) should NOT be called within getCommand since commitment condition is
    // true
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time, environmentModel));
}

TEST_F(PriorityArbitratorTest, Printout) {
    DummyEnvironmentModel environmentModel;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NoFlags);

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentFalseString + "PriorityArbitrator\n"
                                    "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                                    "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                                    "    3. " + invocationTrueString + commitmentFalseString + "MidPriority\n"
                                    "    4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actual_printout = testPriorityArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);


    testPriorityArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        " -> 3. " + invocationTrueString + commitmentFalseString + "MidPriority\n"
                        "    4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    actual_printout = testPriorityArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(PriorityArbitratorTest, ToYaml) {
    DummyEnvironmentModel environmentModel;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::Interruptable);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NoFlags);

    YAML::Node yaml = testPriorityArbitrator.toYaml(time, environmentModel);

    //    std::cout << yaml << std::endl << std::endl;

    EXPECT_EQ("PriorityArbitrator", yaml["type"].as<std::string>());
    EXPECT_EQ("PriorityArbitrator", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(4, yaml["options"].size());
    EXPECT_EQ("Option", yaml["options"][0]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][1]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][2]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][3]["type"].as<std::string>());
    EXPECT_EQ("HighPriority", yaml["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("HighPriority", yaml["options"][1]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("MidPriority", yaml["options"][2]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("LowPriority", yaml["options"][3]["behavior"]["name"].as<std::string>());
    EXPECT_EQ(false, yaml["options"][0]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][1]["flags"].IsDefined());
    ASSERT_EQ(true, yaml["options"][2]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][3]["flags"].IsDefined());

    EXPECT_EQ(false, yaml["activeBehavior"].IsDefined());

    testPriorityArbitrator.gainControl(time, environmentModel);
    testPriorityArbitrator.getCommand(time, environmentModel);

    yaml = testPriorityArbitrator.toYaml(time, environmentModel);

    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    EXPECT_EQ(2, yaml["activeBehavior"].as<int>());
}


TEST_F(PriorityArbitratorTest, BasicFunctionalityWithInterruptableOptions) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::Interruptable);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::Interruptable);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));

    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::Interruptable);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::Interruptable);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time, environmentModel));

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
}

TEST(PriorityArbitrator, SubCommandTypeDiffersFromCommandType) {
    Time time{Clock::now()};
    DummyEnvironmentModel environmentModel;

    using OptionFlags = PriorityArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "___HighPriority___");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "__MidPriority__");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "_LowPriority_");

    PriorityArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NoFlags);

    testPriorityArbitrator.gainControl(time, environmentModel);

    std::string expected = "__MidPriority__";
    EXPECT_EQ(expected.length(), testPriorityArbitrator.getCommand(time, environmentModel));
}
