#include <optional>
#include <string>
#include "gtest/gtest.h"

#include "behavior.hpp"
#include "priority_arbitrator.hpp"

#include "dummy_types.hpp"

using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;

TEST(ExceptionHandlingTest, HandleExceptionInPriorityArbitrator) {
    using OptionFlags = PriorityArbitrator<DummyCommand>::Option::Flags;

    BrokenDummyBehavior::Ptr testBehaviorHighPriority =
        std::make_shared<BrokenDummyBehavior>(true, true, "HighPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");

    Time time{Clock::now()};

    PriorityArbitrator<DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    // Since the high priority behavior is broken, we should get the low priority behavior as fallback
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));

    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time)->isOk());
    EXPECT_TRUE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time)->isOk());

    testPriorityArbitrator.loseControl(time);

    testBehaviorLowPriority->invocationCondition_ = false;
    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    // With no fallback, there is no option to call even if the invocation condition is true
    EXPECT_THROW(testPriorityArbitrator.getCommand(time), NoApplicableOptionPassedVerificationError);
}

TEST(ExceptionHandlingTest, HandleExceptionInCommitedBehavior) {
    using OptionFlags = PriorityArbitrator<DummyCommand>::Option::Flags;

    // This behavior will only throw on the 2nd invocation
    BrokenDummyBehavior::Ptr testBehaviorHighPriority =
        std::make_shared<BrokenDummyBehavior>(true, true, "HighPriority", 1);
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");

    Time time{Clock::now()};

    PriorityArbitrator<DummyCommand> testPriorityArbitrator;

    testPriorityArbitrator.addOption(testBehaviorHighPriority, OptionFlags::NO_FLAGS);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, OptionFlags::NO_FLAGS);

    ASSERT_TRUE(testPriorityArbitrator.checkInvocationCondition(time));

    testPriorityArbitrator.gainControl(time);

    // On the first call, the high priority behavior should be selected like it normally would
    EXPECT_EQ("HighPriority", testPriorityArbitrator.getCommand(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    EXPECT_TRUE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time)->isOk());

    // Progress time to not retreive cached commands
    time += Duration(1);

    // On the second call, the high priority behavior will throw an exception
    // The arbitrator should catch the exception and fall back to the low priority behavior
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time));
    ASSERT_TRUE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time));

    EXPECT_FALSE(testPriorityArbitrator.options().at(0)->verificationResult_.cached(time)->isOk());
    EXPECT_TRUE(testPriorityArbitrator.options().at(1)->verificationResult_.cached(time)->isOk());
}
