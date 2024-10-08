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
#include "conjunctive_coordinator.hpp"
#include "exceptions.hpp"

#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class ConjunctiveCoordinatorTest : public ::testing::Test {
protected:
    using OptionFlags = ConjunctiveCoordinator<DummyCommand, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorA = std::make_shared<DummyBehavior>(false, false, "A");
    DummyBehavior::Ptr testBehaviorB1 = std::make_shared<DummyBehavior>(true, false, "B");
    DummyBehavior::Ptr testBehaviorC = std::make_shared<DummyBehavior>(true, true, "C");
    DummyBehavior::Ptr testBehaviorB2 = std::make_shared<DummyBehavior>(true, false, "B");


    ConjunctiveCoordinator<DummyCommand, DummyCommand> testConjunctiveCoordinator;

    Time time{Clock::now()};
};

DummyCommand& operator&=(DummyCommand& a, const DummyCommand& b) {
    a += b;
    return a;
}

TEST_F(ConjunctiveCoordinatorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testConjunctiveCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testConjunctiveCoordinator.checkCommitmentCondition(time));

    // otherwise the invocationCondition is true if all of the option have true invocationCondition
    testConjunctiveCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    EXPECT_FALSE(testConjunctiveCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testConjunctiveCoordinator.checkCommitmentCondition(time));

    testConjunctiveCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);

    EXPECT_FALSE(testConjunctiveCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testConjunctiveCoordinator.checkCommitmentCondition(time));

    // make all options invocationCondition true now -> the coordinators invocationCondition should also be true
    testBehaviorA->invocationCondition_ = true;
    EXPECT_TRUE(testConjunctiveCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testConjunctiveCoordinator.checkCommitmentCondition(time));

    // coordinators cannot hold the same behavior instantiation multiple times
    EXPECT_THROW(testConjunctiveCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS),
                 MultipleReferencesToSameInstanceError);
    EXPECT_NO_THROW(testConjunctiveCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS));

    // if any option has true commitmentCondition and the coordinator is active, i.e. gained control
    // -> the coordinators commitmentCondition should also be true
    testConjunctiveCoordinator.gainControl(time);
    EXPECT_TRUE(testConjunctiveCoordinator.checkInvocationCondition(time));
    EXPECT_TRUE(testConjunctiveCoordinator.checkCommitmentCondition(time));

    // the ConjunctiveCoordinator joins all sub-commands using the "& operator"
    EXPECT_EQ("ABCB", testConjunctiveCoordinator.getCommand(time));
    EXPECT_EQ(0, testBehaviorA->loseControlCounter_);
    EXPECT_EQ(0, testBehaviorB1->loseControlCounter_);
    EXPECT_EQ(0, testBehaviorC->loseControlCounter_);

    // loseControl(time) should never be called within getCommand of the ConjunctiveCoordinator as the sub-behaviors
    // gain control with the ConjunctiveCoordinator altogether ...
    EXPECT_EQ("ABCB", testConjunctiveCoordinator.getCommand(time));
    EXPECT_EQ(0, testBehaviorA->loseControlCounter_);
    EXPECT_EQ(0, testBehaviorB1->loseControlCounter_);
    EXPECT_EQ(0, testBehaviorC->loseControlCounter_);

    // ... even if all of the sub-behaviors commitmentCondition are false
    testBehaviorC->commitmentCondition_ = false;
    EXPECT_EQ("ABCB", testConjunctiveCoordinator.getCommand(time));
    EXPECT_EQ(0, testBehaviorA->loseControlCounter_);
    EXPECT_EQ(0, testBehaviorB1->loseControlCounter_);
    EXPECT_EQ(0, testBehaviorC->loseControlCounter_);

    EXPECT_TRUE(testConjunctiveCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(testConjunctiveCoordinator.checkCommitmentCondition(time));

    // ConjunctiveCoordinators loseControl(time) should also call loseControl(time) for all its sub-behaviors
    testConjunctiveCoordinator.loseControl(time);
    EXPECT_EQ(1, testBehaviorA->loseControlCounter_);
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

TEST_F(ConjunctiveCoordinatorTest, Verification) {
    using OptionFlags = ConjunctiveCoordinator<DummyCommand, DummyCommand, DummyVerifier, DummyResult>::Option::Flags;
    ConjunctiveCoordinator<DummyCommand, DummyCommand, DummyVerifier, DummyResult> verifyingConjunctiveCoordinator;

    verifyingConjunctiveCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    verifyingConjunctiveCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    verifyingConjunctiveCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    verifyingConjunctiveCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    testBehaviorA->invocationCondition_ = true;

    EXPECT_TRUE(verifyingConjunctiveCoordinator.checkInvocationCondition(time));
    EXPECT_FALSE(verifyingConjunctiveCoordinator.checkCommitmentCondition(time));

    verifyingConjunctiveCoordinator.gainControl(time);

    EXPECT_THROW(verifyingConjunctiveCoordinator.getCommand(time), ApplicableOptionFailedVerificationError);
}


TEST_F(ConjunctiveCoordinatorTest, Printout) {
    testConjunctiveCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    // clang-format off
    std::string expected_printout = invocationFalseString + commitmentFalseString + "ConjunctiveCoordinator\n"
                                    "    - " + invocationFalseString + commitmentFalseString + "A\n"
                                    "    - " + invocationTrueString + commitmentFalseString + "B\n"
                                    "    - " + invocationTrueString + commitmentTrueString + "C\n"
                                    "    - " + invocationTrueString + commitmentFalseString + "B";
    // clang-format on
    std::string actual_printout = testConjunctiveCoordinator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);

    testBehaviorA->invocationCondition_ = true;
    testConjunctiveCoordinator.gainControl(time);
    EXPECT_EQ("ABCB", testConjunctiveCoordinator.getCommand(time));

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "ConjunctiveCoordinator\n"
                        " -> - " + invocationTrueString + commitmentFalseString + "A\n"
                        " -> - " + invocationTrueString + commitmentFalseString + "B\n"
                        " -> - " + invocationTrueString + commitmentTrueString + "C\n"
                        " -> - " + invocationTrueString + commitmentFalseString + "B";
    // clang-format on
    actual_printout = testConjunctiveCoordinator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(ConjunctiveCoordinatorTest, ToYaml) {
    testConjunctiveCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    YAML::Node yaml = testConjunctiveCoordinator.toYaml(time);

    EXPECT_EQ("ConjunctiveCoordinator", yaml["type"].as<std::string>());
    EXPECT_EQ("ConjunctiveCoordinator", yaml["name"].as<std::string>());
    EXPECT_EQ(false, yaml["invocationCondition"].as<bool>());
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
    testConjunctiveCoordinator.gainControl(time);
    testConjunctiveCoordinator.getCommand(time);

    yaml = testConjunctiveCoordinator.toYaml(time);

    //    std::cout << yaml << std::endl << std::endl;

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

TEST(ConjunctiveCoordinator, SubCommandTypeDiffersFromCommandType) {
    Time time{Clock::now()};

    using OptionFlags = ConjunctiveCoordinator<DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorA = std::make_shared<DummyBehavior>(false, false, "A");
    DummyBehavior::Ptr testBehaviorB1 = std::make_shared<DummyBehavior>(true, false, "B");
    DummyBehavior::Ptr testBehaviorC = std::make_shared<DummyBehavior>(true, true, "C");
    DummyBehavior::Ptr testBehaviorB2 = std::make_shared<DummyBehavior>(true, false, "B");

    ConjunctiveCoordinator<DummyCommandInt, DummyCommand> testConjunctiveCoordinator;

    testConjunctiveCoordinator.addOption(testBehaviorA, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorB1, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorC, OptionFlags::NO_FLAGS);
    testConjunctiveCoordinator.addOption(testBehaviorB2, OptionFlags::NO_FLAGS);

    testBehaviorA->invocationCondition_ = true;
    testConjunctiveCoordinator.gainControl(time);

    std::string expected = "ABCB";
    EXPECT_EQ(expected.length(), testConjunctiveCoordinator.getCommand(time));
}
