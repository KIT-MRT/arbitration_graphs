#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "behavior.hpp"
#include "sequence_arbitrator.hpp"

#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class SequenceArbitratorTest : public ::testing::Test {
protected:
    using OptionFlags = SequenceArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    // Behaviors: first two have commitment=false (unavailable), remaining have invocation=true
    // commitment=false means the behavior completes immediately (no persistent commitment)
    // commitment=true means the behavior keeps running until told to stop
    DummyBehavior::Ptr behaviorA = std::make_shared<DummyBehavior>(true, false, "BehaviorA");
    DummyBehavior::Ptr behaviorB = std::make_shared<DummyBehavior>(true, false, "BehaviorB");
    DummyBehavior::Ptr behaviorC = std::make_shared<DummyBehavior>(true, true, "BehaviorC");

    SequenceArbitrator<DummyEnvironmentModel, DummyCommand> testSequenceArbitrator;

    DummyEnvironmentModel environmentModel;

    Time time{Clock::now()};
};


TEST_F(SequenceArbitratorTest, InvocationCondition) {
    // No options: IC should be false
    EXPECT_FALSE(testSequenceArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // IC is based on the first sub-behavior's IC only
    DummyBehavior::Ptr unavailableBehavior = std::make_shared<DummyBehavior>(false, false, "Unavailable");
    testSequenceArbitrator.addOption(unavailableBehavior, OptionFlags::NoFlags);
    EXPECT_FALSE(testSequenceArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));
}

TEST_F(SequenceArbitratorTest, BasicFunctionality) {
    // IC is based on the first behavior's IC
    testSequenceArbitrator.addOption(behaviorA, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(behaviorB, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(behaviorC, OptionFlags::NoFlags);

    EXPECT_TRUE(testSequenceArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    testSequenceArbitrator.gainControl(time, environmentModel);

    // Sequence has started: CC should be true (A is not committed but B and C remain)
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // First call: gainControl A + getCommand A → "BehaviorA"
    EXPECT_EQ("BehaviorA", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(0, behaviorA->loseControlCounter);

    // A has CC=false but B remains → sequence is still committed
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // Second call: loseControl A, gainControl B + getCommand B → "BehaviorB"
    EXPECT_EQ("BehaviorB", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(1, behaviorA->loseControlCounter);
    EXPECT_EQ(0, behaviorB->loseControlCounter);

    // B has CC=false but C remains → sequence is still committed
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // Third call: loseControl B, gainControl C + getCommand C → "BehaviorC"
    EXPECT_EQ("BehaviorC", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(1, behaviorB->loseControlCounter);
    EXPECT_EQ(0, behaviorC->loseControlCounter);

    // C has CC=true → sequence is still committed
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // Continue executing C
    EXPECT_EQ("BehaviorC", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(0, behaviorC->loseControlCounter);
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // When C's commitment becomes false, the sequence is complete
    behaviorC->commitmentCondition = false;
    EXPECT_FALSE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));
}

TEST_F(SequenceArbitratorTest, SequentialExecution) {
    // All behaviors with commitment=true to control the flow manually
    DummyBehavior::Ptr step1 = std::make_shared<DummyBehavior>(true, true, "Step1");
    DummyBehavior::Ptr step2 = std::make_shared<DummyBehavior>(true, true, "Step2");
    DummyBehavior::Ptr step3 = std::make_shared<DummyBehavior>(true, true, "Step3");

    testSequenceArbitrator.addOption(step1, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(step2, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(step3, OptionFlags::NoFlags);

    testSequenceArbitrator.gainControl(time, environmentModel);

    // Execute step1 multiple times
    EXPECT_EQ("Step1", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("Step1", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // Step1 is done → advance to step2
    step1->commitmentCondition = false;
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("Step2", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(1, step1->loseControlCounter);

    // Execute step2 multiple times
    EXPECT_EQ("Step2", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // Step2 is done → advance to step3
    step2->commitmentCondition = false;
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("Step3", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(1, step2->loseControlCounter);

    // Execute step3
    EXPECT_EQ("Step3", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));

    // Step3 is done → sequence complete
    step3->commitmentCondition = false;
    EXPECT_FALSE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));
}

TEST_F(SequenceArbitratorTest, LoseControlResetsSequence) {
    DummyBehavior::Ptr step1 = std::make_shared<DummyBehavior>(true, true, "Step1");
    DummyBehavior::Ptr step2 = std::make_shared<DummyBehavior>(true, true, "Step2");

    testSequenceArbitrator.addOption(step1, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(step2, OptionFlags::NoFlags);

    testSequenceArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("Step1", testSequenceArbitrator.getCommand(time, environmentModel));

    // Advance to step2
    step1->commitmentCondition = false;
    EXPECT_TRUE(testSequenceArbitrator.checkCommitmentCondition(time, environmentModel));
    EXPECT_EQ("Step2", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ(1, step1->loseControlCounter);

    // loseControl resets the sequence
    testSequenceArbitrator.loseControl(time, environmentModel);
    EXPECT_EQ(1, step2->loseControlCounter);
    EXPECT_FALSE(testSequenceArbitrator.isActive());

    // Sequence can be restarted from the beginning
    step1->commitmentCondition = true;
    step2->commitmentCondition = true;
    testSequenceArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("Step1", testSequenceArbitrator.getCommand(time, environmentModel));
}

TEST_F(SequenceArbitratorTest, Printout) {
    testSequenceArbitrator.addOption(behaviorA, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(behaviorB, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(behaviorC, OptionFlags::NoFlags);

    // clang-format off
    std::string expectedPrintout = InvocationTrueString + CommitmentFalseString + "SequenceArbitrator\n"
                                    "    1. " + InvocationTrueString + CommitmentFalseString + "BehaviorA\n"
                                    "    2. " + InvocationTrueString + CommitmentFalseString + "BehaviorB\n"
                                    "    3. " + InvocationTrueString + CommitmentTrueString + "BehaviorC";
    // clang-format on
    std::string actualPrintout = testSequenceArbitrator.toString(time, environmentModel);
    std::cout << actualPrintout << '\n';

    EXPECT_EQ(expectedPrintout, actualPrintout);

    testSequenceArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("BehaviorA", testSequenceArbitrator.getCommand(time, environmentModel));

    // clang-format off
    expectedPrintout = InvocationTrueString + CommitmentTrueString + "SequenceArbitrator\n"
                        " -> 1. " + InvocationTrueString + CommitmentFalseString + "BehaviorA\n"
                        "    2. " + InvocationTrueString + CommitmentFalseString + "BehaviorB\n"
                        "    3. " + InvocationTrueString + CommitmentTrueString + "BehaviorC";
    // clang-format on
    actualPrintout = testSequenceArbitrator.toString(time, environmentModel);
    std::cout << actualPrintout << '\n';

    EXPECT_EQ(expectedPrintout, actualPrintout);

    // Advance through B to C
    EXPECT_EQ("BehaviorB", testSequenceArbitrator.getCommand(time, environmentModel));
    EXPECT_EQ("BehaviorC", testSequenceArbitrator.getCommand(time, environmentModel));

    // clang-format off
    expectedPrintout = InvocationTrueString + CommitmentTrueString + "SequenceArbitrator\n"
                        "    1. " + InvocationTrueString + CommitmentFalseString + "BehaviorA\n"
                        "    2. " + InvocationTrueString + CommitmentFalseString + "BehaviorB\n"
                        " -> 3. " + InvocationTrueString + CommitmentTrueString + "BehaviorC";
    // clang-format on
    actualPrintout = testSequenceArbitrator.toString(time, environmentModel);
    std::cout << actualPrintout << '\n';

    EXPECT_EQ(expectedPrintout, actualPrintout);
}

TEST_F(SequenceArbitratorTest, ToYaml) {
    testSequenceArbitrator.addOption(behaviorA, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(behaviorB, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(behaviorC, OptionFlags::NoFlags);

    YAML::Node yaml = testSequenceArbitrator.toYaml(time, environmentModel);

    EXPECT_EQ("SequenceArbitrator", yaml["type"].as<std::string>());
    EXPECT_EQ("SequenceArbitrator", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(3, yaml["options"].size());
    EXPECT_EQ("Option", yaml["options"][0]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][1]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][2]["type"].as<std::string>());
    EXPECT_EQ("BehaviorA", yaml["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("BehaviorB", yaml["options"][1]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("BehaviorC", yaml["options"][2]["behavior"]["name"].as<std::string>());

    EXPECT_EQ(false, yaml["activeBehavior"].IsDefined());

    testSequenceArbitrator.gainControl(time, environmentModel);
    testSequenceArbitrator.getCommand(time, environmentModel);

    yaml = testSequenceArbitrator.toYaml(time, environmentModel);

    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    // After the first getCommand, BehaviorA (index 0) is active
    EXPECT_EQ(0, yaml["activeBehavior"].as<int>());
}

TEST(SequenceArbitrator, SubCommandTypeDiffersFromCommandType) {
    Time time{Clock::now()};
    DummyEnvironmentModel environmentModel;

    using OptionFlags = SequenceArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand>::Option::Flags;

    // DummyCommandInt is constructed from DummyCommand (string) by taking the string's length.
    // The underscores in the names are intentional: they ensure the string lengths differ,
    // so we can verify the correct behavior (step2) is executing.
    DummyBehavior::Ptr step1 = std::make_shared<DummyBehavior>(true, false, "__Step1__");
    DummyBehavior::Ptr step2 = std::make_shared<DummyBehavior>(true, true, "____Step2____");

    SequenceArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand> testSequenceArbitrator;
    testSequenceArbitrator.addOption(step1, OptionFlags::NoFlags);
    testSequenceArbitrator.addOption(step2, OptionFlags::NoFlags);

    testSequenceArbitrator.gainControl(time, environmentModel);

    // First call: gainControl step1 + getCommand step1
    std::string step1Name = "__Step1__";
    EXPECT_EQ(step1Name.length(), testSequenceArbitrator.getCommand(time, environmentModel));

    // Second call: step1.CC=false → advance to step2 (loseControl step1, gainControl step2, getCommand step2)
    std::string expected = "____Step2____";
    EXPECT_EQ(expected.length(), testSequenceArbitrator.getCommand(time, environmentModel));
}
