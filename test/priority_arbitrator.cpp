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
#include "priority_arbitrator.hpp"

#include "dummy_types.hpp"


using namespace behavior_planning;
using namespace behavior_planning_tests;


class PriorityArbitratorTest : public ::testing::Test {
protected:
    using OptionFlags = PriorityArbitrator<DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "HighPriority");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "MidPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");


    PriorityArbitrator<DummyCommand> testPriorityArbitrator;

    Time time{Clock::now()};
};


TEST_F(PriorityArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time));

    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(0, testBehaviorMidPriority->loseControlCounter_);

    // testBehaviorMidPriority.loseControl(time) should be called within getCommand since commitment condition is false
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(1, testBehaviorMidPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time));

    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    // testBehaviorLowPriority.loseControl(time) should NOT be called within getCommand since commitment condition is
    // true
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
}

TEST_F(PriorityArbitratorTest, Printout) {
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentFalseString + "PriorityArbitrator\n"
                                    "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                                    "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                                    "    3. " + invocationTrueString + commitmentFalseString + "MidPriority\n"
                                    "    4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actual_printout = testPriorityArbitrator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);


    testPriorityArbitrator.gainControl(time);
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        " -> 3. " + invocationTrueString + commitmentFalseString + "MidPriority\n"
                        "    4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    actual_printout = testPriorityArbitrator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(PriorityArbitratorTest, ToYaml) {
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::INTERRUPTABLE);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    YAML::Node yaml = testPriorityArbitrator.toYaml(time);

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

    testPriorityArbitrator.gainControl(time);
    testPriorityArbitrator.getCommand(time);

    yaml = testPriorityArbitrator.toYaml(time);

    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    EXPECT_EQ(2, yaml["activeBehavior"].as<int>());
}


TEST_F(PriorityArbitratorTest, BasicFunctionalityWithInterruptableOptions) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::INTERRUPTABLE);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::INTERRUPTABLE);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time));

    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::INTERRUPTABLE);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::INTERRUPTABLE);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition(time));

    testPriorityArbitrator.gainControl(time);
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition(time));
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));
}

TEST(PriorityArbitrator, SubCommandTypeDiffersFromCommandType) {
    Time time{Clock::now()};

    using OptionFlags = PriorityArbitrator<DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "___HighPriority___");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "__MidPriority__");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "_LowPriority_");

    PriorityArbitrator<DummyCommandInt, DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    testPriorityArbitrator.gainControl(time);

    std::string expected = "__MidPriority__";
    EXPECT_EQ(expected.length(), testPriorityArbitrator.getCommand(time));
}
