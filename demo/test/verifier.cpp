#include "demo/verifier.hpp"

#include <arbitration_graphs/priority_arbitrator.hpp>
#include <gtest/gtest.h>

#include "dummy_behavior.hpp"
#include "mock_environment_model.hpp"

namespace demo {


class VerifierTest : public ::testing::Test {
protected:
    VerifierTest() : environmentModel(std::make_shared<MockEnvironmentModel>()), verifier{environmentModel} {

        const char str[] = {"###"
                            "#  "
                            "###"};
        environmentModel->setMaze({3, 3}, str);
        environmentModel->setPacmanPosition({1, 1});
    }

    Command goodCommand{Direction::RIGHT};
    Command badCommand{Direction::LEFT};
    DummyBehavior::Ptr testBehaviorHighPriority =
        std::make_shared<DummyBehavior>(false, false, goodCommand, "HighPriority");
    DummyBehavior::Ptr testBehaviorMidPriority =
        std::make_shared<DummyBehavior>(true, false, badCommand, "MidPriority");
    DummyBehavior::Ptr testBehaviorLowPriority =
        std::make_shared<DummyBehavior>(true, true, goodCommand, "LowPriority");
    MockEnvironmentModel::Ptr environmentModel;
    Time time{Clock::now()};
    Verifier verifier;
};

TEST_F(VerifierTest, basicVerification) {
    VerificationResult goodResult = verifier.analyze(time, goodCommand);
    EXPECT_TRUE(goodResult.isOk());

    VerificationResult badResult = verifier.analyze(time, badCommand);
    EXPECT_FALSE(badResult.isOk());
}

TEST_F(VerifierTest, verifierInPriorityArbitrator) {
    using PriorityArbitrator = arbitration_graphs::PriorityArbitrator<Command, Command, Verifier, VerificationResult>;

    PriorityArbitrator testPriorityArbitrator("PriorityArbitrator", verifier);

    testPriorityArbitrator.addOption(testBehaviorHighPriority, PriorityArbitrator::Option::Flags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, PriorityArbitrator::Option::Flags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, PriorityArbitrator::Option::Flags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, PriorityArbitrator::Option::Flags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);
    testPriorityArbitrator.getCommand(time);

    const auto yaml = testPriorityArbitrator.toYaml(time);
    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    EXPECT_EQ(3, yaml["activeBehavior"].as<int>());
    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time));

    EXPECT_FALSE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time)->isOk());
    EXPECT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time)->isOk());

    // clang-format off
    std::string expectedPrintout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    3. " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        " -> 4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actualPrintout = testPriorityArbitrator.to_str(time);
    std::cout << actualPrintout << std::endl;

    EXPECT_EQ(expectedPrintout, actualPrintout);

    testPriorityArbitrator.loseControl(time);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_THROW(testPriorityArbitrator.getCommand(time),
                 arbitration_graphs::NoApplicableOptionPassedVerificationError);
}

} // namespace demo
