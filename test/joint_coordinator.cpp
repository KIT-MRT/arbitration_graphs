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
#include "exceptions.hpp"
#include "joint_coordinator.hpp"

#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class JointCoordinatorTest : public ::testing::Test {
protected:
    using OptionFlags = JointCoordinator<DummyCommand, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorA = std::make_shared<DummyBehavior>(false, false, "A");
    DummyBehavior::Ptr testBehaviorB1 = std::make_shared<DummyBehavior>(true, false, "B");
    DummyBehavior::Ptr testBehaviorC = std::make_shared<DummyBehavior>(true, true, "C");
    DummyBehavior::Ptr testBehaviorB2 = std::make_shared<DummyBehavior>(true, false, "B");


    JointCoordinator<DummyCommand, DummyCommand> testJointCoordinator;

    Time time{Clock::now()};
};

DummyCommand& operator&=(DummyCommand& a, const DummyCommand& b) {
    a += b;
    return a;
}

TEST_F(JointCoordinatorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testJointCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testJointCoordinator.checkCommitmentCondition(time));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testJointCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    EXPECT_FALSE(testJointCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testJointCoordinator.checkCommitmentCondition(time));

    testJointCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);

    EXPECT_TRUE(testJointCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testJointCoordinator.checkCommitmentCondition(time));

    testJointCoordinator.gainControl(time);
    EXPECT_EQ("BC", testJointCoordinator.getCommand(time));

    std::string actual_printout = testJointCoordinator.to_str(time);
    std::cout << actual_printout << std::endl;


    // if any active option has true commitmentCondition and the coordinator is active, i.e. gained control
    // -> the coordinators commitmentCondition should also be true
    EXPECT_TRUE(testJointCoordinator.checkInvocationCondition(time));
    EXPECT_TRUE(testJointCoordinator.checkCommitmentCondition(time));

    EXPECT_EQ("BC", testJointCoordinator.getCommand(time));


    // lets make it a little more complicated...
    testBehaviorA->commitmentCondition_ = true;
    testBehaviorB1->invocationCondition_ = false;
    testBehaviorC->invocationCondition_ = false;
    EXPECT_FALSE(testJointCoordinator.checkInvocationCondition(time));
    EXPECT_TRUE(testJointCoordinator.checkCommitmentCondition(time));

    // A should not be selected:
    // - it has false invocation condition and
    // - was inactive before (thus, its true commitment condition should have no effect)
    // B should not be selected:
    // - it has false invocation condition and
    // - was active before, but has false commitment condition
    // C should be selected:
    // - it has false invocation condition, but
    // - it was active before and has true commitment condition
    EXPECT_EQ("C", testJointCoordinator.getCommand(time));


    // make all options invocationCondition false now -> the coordinators invocationCondition should also be false
    testBehaviorB1->invocationCondition_ = false;
    testBehaviorC->invocationCondition_ = false;
    testBehaviorC->commitmentCondition_ = false;
    EXPECT_FALSE(testJointCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testJointCoordinator.checkCommitmentCondition(time));

    // coordinators cannot hold the same behavior instantiation multiple times
    EXPECT_THROW(testJointCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS),
                 MultipleReferencesToSameInstanceError);
    EXPECT_NO_THROW(testJointCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS));


    // ConjunctiveCoordinators loseControl(time) should also call loseControl(time) for all its sub-behaviors
    testJointCoordinator.loseControl(time);
    EXPECT_EQ(0, testBehaviorA->loseControlCounter_);
    EXPECT_EQ(1, testBehaviorB1->loseControlCounter_);
    EXPECT_EQ(1, testBehaviorC->loseControlCounter_);
}


struct DummyVerifier {
    DummyResult analyze(const Time& /*time*/, const DummyCommand& data) const {
        if (data == wrong_) {
            return DummyResult{false};
        }
        return DummyResult{true};
    };
    std::string wrong_{"B"};
};
struct RejectingVerifier {
    static DummyResult analyze(const Time& /*time*/, const DummyCommand& /*data*/) {
        return DummyResult{false};
    };
};

TEST_F(JointCoordinatorTest, DummyVerification) {
    using OptionFlags = JointCoordinator<DummyCommand, DummyCommand, DummyVerifier, DummyResult>::Option::Flags;
    JointCoordinator<DummyCommand, DummyCommand, DummyVerifier, DummyResult> verifyingJointCoordinator;

    verifyingJointCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    verifyingJointCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    verifyingJointCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    verifyingJointCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    testBehaviorA->invocationCondition_ = true;

    EXPECT_TRUE(verifyingJointCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(verifyingJointCoordinator.checkCommitmentCondition(time));

    verifyingJointCoordinator.gainControl(time);

    // B1, C and B2 are invocable, A is not, but B1 and B2 fail verification
    EXPECT_EQ("AC", verifyingJointCoordinator.getCommand(time));

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentTrueString + "JointCoordinator\n"
                        " -> - " + invocationTrueString + commitmentFalseString + "A\n"
                        "    - " + strikeThroughOn
                                 + invocationTrueString + commitmentFalseString + "B"
                                 + strikeThroughOff + "\n"
                        " -> - " + invocationTrueString + commitmentTrueString + "C\n"
                        "    - " + strikeThroughOn
                                 + invocationTrueString + commitmentFalseString + "B"
                                 + strikeThroughOff;
    // clang-format on
    std::string actual_printout = verifyingJointCoordinator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(JointCoordinatorTest, RejectingVerification) {
    using OptionFlags = JointCoordinator<DummyCommand, DummyCommand, RejectingVerifier>::Option::Flags;
    JointCoordinator<DummyCommand, DummyCommand, RejectingVerifier> verifyingJointCoordinator;

    verifyingJointCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    verifyingJointCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    verifyingJointCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    verifyingJointCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    EXPECT_TRUE(verifyingJointCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(verifyingJointCoordinator.checkCommitmentCondition(time));

    verifyingJointCoordinator.gainControl(time);

    EXPECT_THROW(verifyingJointCoordinator.getCommand(time), NoApplicableOptionPassedVerificationError);
}


TEST_F(JointCoordinatorTest, Printout) {
    testJointCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentFalseString + "JointCoordinator\n"
                                    "    - " + invocationFalseString + commitmentFalseString + "A\n"
                                    "    - " + invocationTrueString + commitmentFalseString + "B\n"
                                    "    - " + invocationTrueString + commitmentTrueString + "C\n"
                                    "    - " + invocationTrueString + commitmentFalseString + "B";
    // clang-format on
    std::string actual_printout = testJointCoordinator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);

    testJointCoordinator.gainControl(time);
    EXPECT_EQ("BCB", testJointCoordinator.getCommand(time));

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "JointCoordinator\n"
                        "    - " + invocationFalseString + commitmentFalseString + "A\n"
                        " -> - " + invocationTrueString + commitmentFalseString + "B\n"
                        " -> - " + invocationTrueString + commitmentTrueString + "C\n"
                        " -> - " + invocationTrueString + commitmentFalseString + "B";
    // clang-format on
    actual_printout = testJointCoordinator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(JointCoordinatorTest, ToYaml) {
    testJointCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    YAML::Node yaml = testJointCoordinator.toYaml(time);

    EXPECT_EQ("JointCoordinator", yaml["type"].as<std::string>());
    EXPECT_EQ("JointCoordinator", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(4, yaml["options"].size());
    EXPECT_EQ("Option", yaml["options"][0]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][1]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][2]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][3]["type"].as<std::string>());
    EXPECT_EQ("A", yaml["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("B", yaml["options"][1]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("C", yaml["options"][2]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("B", yaml["options"][3]["behavior"]["name"].as<std::string>());
    EXPECT_EQ(false, yaml["options"][0]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][1]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][2]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][3]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["options"][0]["behavior"]["commitmentCondition"].as<bool>());
    EXPECT_EQ(false, yaml["options"][1]["behavior"]["commitmentCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][2]["behavior"]["commitmentCondition"].as<bool>());
    EXPECT_EQ(false, yaml["options"][3]["behavior"]["commitmentCondition"].as<bool>());
    EXPECT_EQ(false, yaml["options"][0]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][1]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][2]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][3]["flags"].IsDefined());

    testBehaviorA->invocationCondition_ = true;
    testJointCoordinator.gainControl(time);
    testJointCoordinator.getCommand(time);

    yaml = testJointCoordinator.toYaml(time);

    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(4, yaml["options"].size());
    EXPECT_EQ(true, yaml["options"][0]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][1]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][2]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][3]["behavior"]["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["options"][0]["behavior"]["commitmentCondition"].as<bool>());
    EXPECT_EQ(false, yaml["options"][1]["behavior"]["commitmentCondition"].as<bool>());
    EXPECT_EQ(true, yaml["options"][2]["behavior"]["commitmentCondition"].as<bool>());
    EXPECT_EQ(false, yaml["options"][3]["behavior"]["commitmentCondition"].as<bool>());
}

TEST_F(JointCoordinatorTest, SubCommandTypeDiffersFromCommandType) {
    Time time{Clock::now()};

    using OptionFlags = JointCoordinator<DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorA = std::make_shared<DummyBehavior>(false, false, "A");
    DummyBehavior::Ptr testBehaviorB1 = std::make_shared<DummyBehavior>(true, false, "B");
    DummyBehavior::Ptr testBehaviorC = std::make_shared<DummyBehavior>(true, true, "C");
    DummyBehavior::Ptr testBehaviorB2 = std::make_shared<DummyBehavior>(true, false, "B");

    JointCoordinator<DummyCommandInt, DummyCommand> testJointCoordinator;

    testJointCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    testJointCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    testBehaviorA->invocationCondition_ = true;
    testJointCoordinator.gainControl(time);

    std::string expected = "ABCB";
    EXPECT_EQ(expected.length(), testJointCoordinator.getCommand(time));
}
