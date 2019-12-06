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
};


TEST_F(PriorityArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.gainControl();

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorMidPriority->loseControlCounter_);

    // testBehaviorMidPriority.loseControl() should be called within getCommand since commitment condition is false
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(1, testBehaviorMidPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());

    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    // testBehaviorLowPriority.loseControl() should NOT be called within getCommand since commitment condition is true
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
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
    std::stringstream actual_printout;
    actual_printout << testPriorityArbitrator;
    std::cout << actual_printout.str() << std::endl;

    EXPECT_EQ(expected_printout, actual_printout.str());


    testPriorityArbitrator.gainControl();
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        " -> 3. " + invocationTrueString + commitmentFalseString + "MidPriority\n"
                        "    4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    actual_printout.str("");
    actual_printout << testPriorityArbitrator;
    std::cout << actual_printout.str() << std::endl;

    EXPECT_EQ(expected_printout, actual_printout.str());
}


TEST_F(PriorityArbitratorTest, BasicFunctionalityWithInterruptableOptions) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::INTERRUPTABLE);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::INTERRUPTABLE);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::INTERRUPTABLE);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::INTERRUPTABLE);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.gainControl();
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
}

TEST(PriorityArbitrator, SubCommandTypeDiffersFromCommandType) {
    using OptionFlags = PriorityArbitrator<DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "___HighPriority___");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "__MidPriority__");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "_LowPriority_");

    PriorityArbitrator<DummyCommandInt, DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    testPriorityArbitrator.gainControl();

    std::string expected = "__MidPriority__";
    EXPECT_EQ(expected.length(), testPriorityArbitrator.getCommand());
}
