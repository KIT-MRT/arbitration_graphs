#include "demo/verifier.hpp"

#include <arbitration_graphs/priority_arbitrator.hpp>
#include <gtest/gtest.h>

#include "dummy_behavior.hpp"
#include "mock_environment_model.hpp"

namespace demo {


class VerifierTest : public ::testing::Test {
protected:
    using Result = arbitration_graphs::verification::Result;

    VerifierTest() {
        const char str[] = {"###"
                            "#  "
                            "###"};
        environmentModel.setMaze({3, 3}, str);
        environmentModel.setPacmanPosition({1, 1});

        verifier = std::make_shared<Verifier>();
    }

    Command goodCommand{Direction::Right};
    Command badCommand{Direction::Left};
    DummyBehavior::Ptr testBehaviorHighPriority =
        std::make_shared<DummyBehavior>(false, false, goodCommand, "HighPriority");
    DummyBehavior::Ptr testBehaviorMidPriority =
        std::make_shared<DummyBehavior>(true, false, badCommand, "MidPriority");
    DummyBehavior::Ptr testBehaviorLowPriority =
        std::make_shared<DummyBehavior>(true, true, goodCommand, "LowPriority");
    MockEnvironmentModel environmentModel;
    Time time{Clock::now()};
    Verifier::Ptr verifier;
};

TEST_F(VerifierTest, basicVerification) {
    Result::Ptr goodResult = verifier->analyze(time, environmentModel, goodCommand);
    EXPECT_TRUE(goodResult->isOk());

    Result::Ptr badResult = verifier->analyze(time, environmentModel, badCommand);
    EXPECT_FALSE(badResult->isOk());
}

TEST_F(VerifierTest, verifierInPriorityArbitrator) {
    using PriorityArbitrator = arbitration_graphs::PriorityArbitrator<EnvironmentModel, Command>;

    PriorityArbitrator testPriorityArbitrator("PriorityArbitrator", verifier);

    testPriorityArbitrator.addOption(testBehaviorHighPriority, PriorityArbitrator::Option::Flags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, PriorityArbitrator::Option::Flags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, PriorityArbitrator::Option::Flags::NoFlags);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, PriorityArbitrator::Option::Flags::NoFlags);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);
    testPriorityArbitrator.getCommand(time, environmentModel);

    const auto yaml = testPriorityArbitrator.toYaml(time, environmentModel);
    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    EXPECT_EQ(3, yaml["activeBehavior"].as<int>());
    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(1)->verificationResult(time));

    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult(time));
    Result::ConstPtr verificationResult2 = testPriorityArbitrator.options().at(2)->verificationResult(time).value();
    EXPECT_FALSE(verificationResult2->isOk());

    ASSERT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult(time));
    Result::ConstPtr verificationResult3 = testPriorityArbitrator.options().at(3)->verificationResult(time).value();
    EXPECT_TRUE(verificationResult3->isOk());

    // clang-format off
    std::string expectedPrintout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    3. " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        " -> 4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actualPrintout = testPriorityArbitrator.toString(time, environmentModel);
    std::cout << actualPrintout << '\n';

    EXPECT_EQ(expectedPrintout, actualPrintout);

    testPriorityArbitrator.loseControl(time, environmentModel);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);

    EXPECT_THROW(testPriorityArbitrator.getCommand(time, environmentModel),
                 arbitration_graphs::NoApplicableOptionPassedVerificationError);
}

} // namespace demo
