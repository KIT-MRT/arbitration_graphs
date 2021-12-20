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
    static DummyResult analyze(const Time& /*time*/, const DummyCommand& data) {
        if (data == "MidPriority") {
            return DummyResult{false};
        }
        return DummyResult{true};
    };
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
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(0)->verificationResult_);
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(1)->verificationResult_);
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_);
    // LowPriority could have been verified or not, so don't test it here

    EXPECT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_->isOk());
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
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(0)->verificationResult_);
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(1)->verificationResult_);
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_);
    // LowPriority could have been verified or not, so don't test it here

    EXPECT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_->isOk());
}


// Now DummyVerifier classifies the "MidPriority" command as invalid
TEST_F(CommandVerificationTest, DummyVerifierInPriorityArbitrator) {
    using OptionFlags = PriorityArbitrator<DummyCommand, DummyCommand, DummyVerifier>::Option::Flags;

    PriorityArbitrator<DummyCommand, DummyCommand, DummyVerifier> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(0)->verificationResult_);
    EXPECT_EQ(std::nullopt, testPriorityArbitrator.options().at(1)->verificationResult_);
    ASSERT_TRUE(testPriorityArbitrator.options().at(2)->verificationResult_);
    ASSERT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_);

    EXPECT_FALSE(testPriorityArbitrator.options().at(2)->verificationResult_->isOk());
    EXPECT_TRUE(testPriorityArbitrator.options().at(3)->verificationResult_->isOk());


    testPriorityArbitrator.loseControl(time);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    EXPECT_THROW(testPriorityArbitrator.getCommand(time), NoApplicableOptionPassedVerificationError);
}


TEST_F(CommandVerificationTest, DummyVerifierInCostArbitrator) {
    using OptionFlags = CostArbitrator<DummyCommand, DummyCommand, DummyVerifier>::Option::Flags;

    CostArbitrator<DummyCommand, DummyCommand, DummyVerifier> testCostArbitrator;

    CostEstimatorFromCostMap::CostMap costMap{{"HighPriority", 0}, {"MidPriority", 0.5}, {"LowPriority", 1}};
    CostEstimatorFromCostMap::Ptr costEstimator = std::make_shared<CostEstimatorFromCostMap>(costMap);

    testCostArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorMidPriority, OptionFlags::NO_FLAGS, costEstimator);
    testCostArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS, costEstimator);

    ASSERT_TRUE(testCostArbitrator.checkInvocationCondition(time));

    testCostArbitrator.gainControl(time);

    EXPECT_EQ("LowPriority", testCostArbitrator.getCommand(time));
    EXPECT_EQ(std::nullopt, testCostArbitrator.options().at(0)->verificationResult_);
    EXPECT_EQ(std::nullopt, testCostArbitrator.options().at(1)->verificationResult_);
    ASSERT_TRUE(testCostArbitrator.options().at(2)->verificationResult_);
    ASSERT_TRUE(testCostArbitrator.options().at(3)->verificationResult_);

    EXPECT_FALSE(testCostArbitrator.options().at(2)->verificationResult_->isOk());
    EXPECT_TRUE(testCostArbitrator.options().at(3)->verificationResult_->isOk());


    testCostArbitrator.loseControl(time);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testCostArbitrator.checkInvocationCondition(time));

    testCostArbitrator.gainControl(time);

    EXPECT_THROW(testCostArbitrator.getCommand(time), NoApplicableOptionPassedVerificationError);
}
