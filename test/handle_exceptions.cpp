#include <optional>
#include <string>
#include "gtest/gtest.h"

#include "behavior.hpp"
#include "priority_arbitrator.hpp"

#include "dummy_types.hpp"

using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;

class ExceptionHandlingTest : public ::testing::Test {
protected:
    BrokenDummyBehavior::Ptr testBehaviorHighPriority =
        std::make_shared<BrokenDummyBehavior>(true, true, "HighPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");

    Time time{Clock::now()};
};

TEST_F(ExceptionHandlingTest, HandleException) {
    using OptionFlags = PriorityArbitrator<DummyCommand>::Option::Flags;

    PriorityArbitrator<DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    // Since the high priority behavior is broken, we should get the low priority behavior as fallback
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));

    EXPECT_TRUE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time)->isOk());

    testPriorityArbitrator.loseControl(time);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    // With no fallback, there is no option to call even if the invocation condition is true
    EXPECT_THROW(testPriorityArbitrator.getCommand(time), NoApplicableOptionPassedVerificationError);
}

