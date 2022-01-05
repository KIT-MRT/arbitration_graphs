#include <map>
#include <memory>
#include <optional>
#include <string>
#include "gtest/gtest.h"

#include "behavior.hpp"
#include "cost_arbitrator.hpp"
#include "priority_arbitrator.hpp"

#include "cost_estimator.hpp"
#include "dummy_types.hpp"


using namespace behavior_planning;
using namespace behavior_planning_tests;

using DummyPlaceboVerifier = verification::PlaceboVerifier<DummyCommand>;

struct DummyResult {
    bool isOk() const {
        return isOk_;
    };

    bool isOk_;
};
struct DummyVerifier {
    // This could be made static here, but we want to challenge the compiler in deducing VerificationResultT.
    // Unfortunately in such non-static cases VerificationResultT cannot be deduced
    // and has to be passed on as template argument, see e.g. the DummyVerifierInPriorityArbitrator test
    DummyResult analyze(const Time& /*time*/, const DummyCommand& data) const {
        if (data == wrong_) {
            return DummyResult{false};
        }
        return DummyResult{true};
    };
    std::string wrong_{"MidPriority"};
};

class CommandVerificationTest : public ::testing::Test {
protected:
    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "HighPriority");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "MidPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");

    Time time{Clock::now()};
};


// The default verifyier should be the PlaceboVerifier
TEST_F(CommandVerificationTest, DefaultVerifier) {
    using OptionFlags = PriorityArbitrator<DummyCommand>::Option::Flags;

    PriorityArbitrator<DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    // LowPriority could have been verified or not, so don't test it here

    EXPECT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time)->isOk());
}


// Same as for DefaultVerification
TEST_F(CommandVerificationTest, PlaceboVerifier) {
    using OptionFlags = PriorityArbitrator<DummyCommand, DummyCommand, DummyPlaceboVerifier>::Option::Flags;

    PriorityArbitrator<DummyCommand, DummyCommand, DummyPlaceboVerifier> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    // LowPriority could have been verified or not, so don't test it here

    EXPECT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time)->isOk());
}


// Now DummyVerifier classifies the "MidPriority" command as invalid
TEST_F(CommandVerificationTest, DummyVerifierInPriorityArbitrator) {
    using OptionFlags = PriorityArbitrator<DummyCommand, DummyCommand, DummyVerifier, DummyResult>::Option::Flags;

    PriorityArbitrator<DummyCommand, DummyCommand, DummyVerifier, DummyResult> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time));

    EXPECT_FALSE(testPriorityArbitrator.options().at(2)->verificationResult_.cached(time)->isOk());
    EXPECT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_.cached(time)->isOk());

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentTrueString + "PriorityArbitrator\n"
                        "    1. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    2. " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    3. " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        " -> 4. " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actual_printout = testPriorityArbitrator.to_str(time);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);


    YAML::Node yaml = testPriorityArbitrator.toYaml(time);
    EXPECT_FALSE(yaml["options"][0]["verificationResult"].IsDefined());
    EXPECT_FALSE(yaml["options"][1]["verificationResult"].IsDefined());
    ASSERT_TRUE(yaml["options"][2]["verificationResult"].IsDefined());
    ASSERT_TRUE(yaml["options"][3]["verificationResult"].IsDefined());

    EXPECT_EQ("failed", yaml["options"][2]["verificationResult"].as<std::string>());
    EXPECT_EQ("passed", yaml["options"][3]["verificationResult"].as<std::string>());


    testPriorityArbitrator.loseControl(time);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_THROW(testPriorityArbitrator.getCommand(time), NoApplicableOptionPassedVerificationError);
}


TEST_F(CommandVerificationTest, DummyVerifierInCostArbitrator) {
    using OptionFlags = CostArbitrator<DummyCommand, DummyCommand, DummyVerifier, DummyResult>::Option::Flags;

    CostArbitrator<DummyCommand, DummyCommand, DummyVerifier, DummyResult> testCostArbitrator;

    CostEstimatorFromCostMap::CostMap costMap{{"HighPriority", 0}, {"MidPriority", 0.5}, {"LowPriority", 1}};
    CostEstimatorFromCostMap::Ptr costEstimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    testCostArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS, costEstimator);

    ASSERT_TRUE(testCostArbitrator.checkInvocationCondition(time));

    testCostArbitrator.gainControl(time);

    EXPECT_EQ("LowPriority", testCostArbitrator.getCommand(time));
    EXPECT_EQ(std::nullopt, testCostArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_EQ(std::nullopt, testCostArbitrator.options().at(1)->verificationResult_.cached(time));
    ASSERT_TRUE(testCostArbitrator.options().at(2)->verificationResult_.cached(time));
    ASSERT_TRUE(testCostArbitrator.options().at(3)->verificationResult_.cached(time));

    EXPECT_FALSE(testCostArbitrator.options().at(2)->verificationResult_.cached(time)->isOk());
    EXPECT_TRUE(testCostArbitrator.options().at(3)->verificationResult_.cached(time)->isOk());

    // clang-format off
    std::string expectedPrintout = invocationTrueString + commitmentTrueString + "CostArbitrator\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    - (cost:  n.a.) " + invocationFalseString + commitmentFalseString + "HighPriority\n"
                        "    - (cost: 0.500) " + strikeThroughOn
                                  + invocationTrueString + commitmentFalseString + "MidPriority"
                                  + strikeThroughOff + "\n"
                        " -> - (cost: 1.000) " + invocationTrueString + commitmentTrueString + "LowPriority";
    // clang-format on
    std::string actualPrintout = testCostArbitrator.to_str(time);
    std::cout << actualPrintout << std::endl;

    EXPECT_EQ(expectedPrintout, actualPrintout);


    testCostArbitrator.loseControl(time);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testCostArbitrator.checkInvocationCondition(time));

    testCostArbitrator.gainControl(time);

    EXPECT_THROW(testCostArbitrator.getCommand(time), NoApplicableOptionPassedVerificationError);
}
