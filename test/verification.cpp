#include <memory>
#include <optional>
#include <string>

#include <gtest/gtest.h>

#include "behavior.hpp"
#include "cost_arbitrator.hpp"
#include "priority_arbitrator.hpp"

#include "cost_estimator.hpp"
#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;

using DummyPlaceboVerifier = verification::PlaceboVerifier<DummyEnvironmentModel, DummyCommand>;

struct DummyVerifier : public verification::Verifier<DummyEnvironmentModel, DummyCommand> {
    verification::Result::Ptr analyze(const Time& /*time*/,
                                      const DummyEnvironmentModel& /*environmentModel*/,
                                      const DummyCommand& command) const override {
        if (command == wrong_) {
            return std::make_shared<DummyResult>(false);
        }
        return std::make_shared<DummyResult>(true);
    };
    std::string wrong_{"MidPriority"};
};

class CommandVerificationTest : public ::testing::Test {
protected:
    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "HighPriority");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "MidPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");

    DummyEnvironmentModel environmentModel;

    Time time{Clock::now()};
};


// The default verifyier should be the PlaceboVerifier
TEST_F(CommandVerificationTest, DefaultVerifier) {
    using OptionFlags = PriorityArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    PriorityArbitrator<DummyEnvironmentModel, DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    // LowPriority could have been verified or not, so don't test it here

    EXPECT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time).value()->isOk());
}


// Same as for DefaultVerification
TEST_F(CommandVerificationTest, PlaceboVerifier) {
    using OptionFlags = PriorityArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    PriorityArbitrator<DummyEnvironmentModel, DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    // LowPriority could have been verified or not, so don't test it here

    EXPECT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time).value()->isOk());
}


// Now DummyVerifier classifies the "MidPriority" command as invalid
TEST_F(CommandVerificationTest, DummyVerifierInPriorityArbitrator) {
    using OptionFlags = PriorityArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    PriorityArbitrator<DummyEnvironmentModel, DummyCommand> testPriorityArbitrator("PriorityArbitrator",
                                                                                   std::make_shared<DummyVerifier>());

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);

    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time));

    EXPECT_FALSE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time).value()->isOk());
    EXPECT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time).value()->isOk());

    std::cout << "verificationResult for " << testPriorityArbitrator.options().at(2)->behavior_->name() << ": "
              << testPriorityArbitrator.options().at(2)->verificationResult_.cached(time).value() << std::endl;
    std::cout << "verificationResult for " << testPriorityArbitrator.options().at(3)->behavior_->name() << ": "
              << testPriorityArbitrator.options().at(3)->verificationResult_.cached(time).value() << std::endl;

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    3. " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        " -> 4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actual_printout = testPriorityArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);


    YAML::Node yaml = testPriorityArbitrator.toYaml(time, environmentModel);
    EXPECT_FALSE(yaml["options"][0]["verificationResult"].IsDefined());
    EXPECT_FALSE(yaml["options"][1]["verificationResult"].IsDefined());
    ASSERT_TRUE(yaml["options"][2]["verificationResult"].IsDefined());
    ASSERT_TRUE(yaml["options"][3]["verificationResult"].IsDefined());

    EXPECT_EQ("failed", yaml["options"][2]["verificationResult"].as<std::string>());
    EXPECT_EQ("passed", yaml["options"][3]["verificationResult"].as<std::string>());


    testPriorityArbitrator.loseControl(time, environmentModel);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);

    EXPECT_THROW(testPriorityArbitrator.getCommand(time, environmentModel), NoApplicableOptionPassedVerificationError);
}


// Two "MidPriority" options, the second one defined as fallback option, so it should be selected, even if it is invalid
TEST_F(CommandVerificationTest, DummyVerifierInPriorityArbitratorWithFallback) {
    using OptionFlags = PriorityArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    PriorityArbitrator<DummyEnvironmentModel, DummyCommand> testPriorityArbitrator("PriorityArbitrator",
                                                                                   std::make_shared<DummyVerifier>());

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::FALLBACK);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time, environmentModel));

    testPriorityArbitrator.gainControl(time, environmentModel);

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time, environmentModel));
    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(4)->verificationResult_.cached(time));

    EXPECT_FALSE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time).value()->isOk());
    EXPECT_FALSE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time).value()->isOk());

    std::cout << "verificationResult for " << testPriorityArbitrator.options().at(2)->behavior_->name() << ": "
              << testPriorityArbitrator.options().at(2)->verificationResult_.cached(time).value() << std::endl;
    std::cout << "verificationResult for " << testPriorityArbitrator.options().at(3)->behavior_->name() << ": "
              << testPriorityArbitrator.options().at(3)->verificationResult_.cached(time).value() << std::endl;

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    3. " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        " -> 4. " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        "    5. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actual_printout = testPriorityArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);


    YAML::Node yaml = testPriorityArbitrator.toYaml(time, environmentModel);
    EXPECT_FALSE(yaml["options"][0]["verificationResult"].IsDefined());
    EXPECT_FALSE(yaml["options"][1]["verificationResult"].IsDefined());
    ASSERT_TRUE(yaml["options"][2]["verificationResult"].IsDefined());
    ASSERT_TRUE(yaml["options"][3]["verificationResult"].IsDefined());
    EXPECT_FALSE(yaml["options"][4]["verificationResult"].IsDefined());

    EXPECT_EQ("failed", yaml["options"][2]["verificationResult"].as<std::string>());
    EXPECT_EQ("failed", yaml["options"][3]["verificationResult"].as<std::string>());
}


TEST_F(CommandVerificationTest, DummyVerifierInCostArbitrator) {
    using OptionFlags = CostArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    CostArbitrator<DummyEnvironmentModel, DummyCommand> testCostArbitrator("CostArbitrator",
                                                                           std::make_shared<DummyVerifier>());

    CostEstimatorFromCostMap::CostMap costMap{{"HighPriority", 0}, {"MidPriority", 0.5}, {"LowPriority", 1}};
    CostEstimatorFromCostMap::Ptr costEstimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    testCostArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS, costEstimator);

    ASSERT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));

    testCostArbitrator.gainControl(time, environmentModel);

    EXPECT_EQ("LowPriority", testCostArbitrator.getCommand(time, environmentModel));
    EXPECT_FALSE(testCostArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_FALSE(testCostArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testCostArbitrator.options().at(2)->verificationResult_.cached(time));
    ASSERT_TRUE(testCostArbitrator.options().at(3)->verificationResult_.cached(time));

    EXPECT_FALSE(testCostArbitrator.options().at(2)->verificationResult_.cached(time).value()->isOk());
    EXPECT_TRUE(testCostArbitrator.options().at(3)->verificationResult_.cached(time).value()->isOk());

    // clang-format off
    std::string expectedPrintout = invocationTrueString + commitmentTrueString + "CostArbitrator\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    - (cost:  n.a.) " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        " -> - (cost: 1.000) " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actualPrintout = testCostArbitrator.to_str(time, environmentModel);
    std::cout << actualPrintout << std::endl;

    EXPECT_EQ(expectedPrintout, actualPrintout);


    testCostArbitrator.loseControl(time, environmentModel);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testCostArbitrator.checkInvocationCondition(time, environmentModel));

    testCostArbitrator.gainControl(time, environmentModel);

    EXPECT_THROW(testCostArbitrator.getCommand(time, environmentModel), NoApplicableOptionPassedVerificationError);
}
