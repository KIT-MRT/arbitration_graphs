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
#include <boost/optional.hpp>
#include "gtest/gtest.h"

// A google test function (uncomment the next function, add code and
// change the names TestGroupName and TestName)
// TEST(TestGroupName, TestName) {
// TODO: Add your test code here
//}
using Command = std::string;


class Behavior {
public:
    using Ptr = std::shared_ptr<Behavior>;

    Behavior(const std::string& name = "Behavior") : name_{name} {
    }

    virtual Command getCommand() = 0;
    virtual bool checkInvocationCondition() const {
        return false;
    }
    virtual bool checkCommitmentCondition() const {
        return false;
    }
    virtual void gainControl() {
    }
    virtual void loseControl() {
    }

    const std::string name_;
};


class DummyBehavior : public Behavior {
public:
    DummyBehavior(const bool invocation, const bool commitment, const std::string& name = "DummyBehavior")
            : Behavior(name), invocationCondition_{invocation}, commitmentCondition_{commitment} {};

    Command getCommand() override {
        return name_;
    }
    bool checkInvocationCondition() const override {
        return invocationCondition_;
    }
    bool checkCommitmentCondition() const override {
        return commitmentCondition_;
    }


    bool invocationCondition_;
    bool commitmentCondition_;
};

class DummyBehaviorTest : public ::testing::Test {
protected:
    DummyBehavior testBehaviorTrue{true, true};

    Command expected_command{"DummyBehavior"};
};

TEST_F(DummyBehaviorTest, BasicInterface) {
    EXPECT_EQ(expected_command, testBehaviorTrue.getCommand());
    EXPECT_TRUE(testBehaviorTrue.checkCommitmentCondition());
    EXPECT_TRUE(testBehaviorTrue.checkInvocationCondition());
    EXPECT_NO_THROW(testBehaviorTrue.gainControl());
    EXPECT_NO_THROW(testBehaviorTrue.loseControl());
}

class PriorityArbitrator : public Behavior {
public:
    PriorityArbitrator(const std::string& name = "PriorityArbitrator") : Behavior(name){};

    void addOption(const Behavior::Ptr& behavior, const bool interruptable) {
        behaviorOptions_.push_back({behavior, interruptable});
    }

    Command getCommand() override {
        if (activeBehavior_) {
            if (behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition()) {
                return behaviorOptions_.at(*activeBehavior_).behavior->getCommand();
            } else {
                behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
            }
        }

        activateNextBehavior();
        return behaviorOptions_.at(*activeBehavior_).behavior->getCommand();
    }

    bool checkInvocationCondition() const override {
        for (auto& option : behaviorOptions_) {
            if (option.behavior->checkInvocationCondition()) {
                return true;
            }
        }
        return false;
    }
    bool checkCommitmentCondition() const override {
        if (activeBehavior_) {
            if (behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition()) {
                return true;
            } else {
                return checkInvocationCondition();
            }
        }
        return false;
    }

    virtual void gainControl() override {
    }

    virtual void loseControl() override {
        if (activeBehavior_) {
            behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
        }
        activeBehavior_ = boost::none;
    }


private:
    struct Option {
        Behavior::Ptr behavior;
        bool interruptable;
    };

    std::vector<Option> behaviorOptions_;
    boost::optional<int> activeBehavior_;

    void activateNextBehavior() {
        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            Option& option = behaviorOptions_.at(i);
            if (option.behavior->checkInvocationCondition()) {
                activeBehavior_ = i;
                option.behavior->gainControl();
                return;
            }
        }
        if (!activeBehavior_) {
            throw std::runtime_error("No behavior with true invocation condition found!");
        }
    }
};

class PriorityArbitratorTest : public ::testing::Test {
protected:
    Behavior::Ptr testBehaviorFalseFalse = std::make_shared<DummyBehavior>(false, false, "FalseFalse");
    Behavior::Ptr testBehaviorTrueFalse = std::make_shared<DummyBehavior>(true, false, "TrueFalse");
    Behavior::Ptr testBehaviorTrueTrue = std::make_shared<DummyBehavior>(true, true, "TrueTrue");


    PriorityArbitrator testPriorityArbitrator;
};


TEST_F(PriorityArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorFalseFalse, true);
    testPriorityArbitrator.addOption(testBehaviorFalseFalse, true);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());

    testPriorityArbitrator.addOption(testBehaviorTrueFalse, true);
    testPriorityArbitrator.addOption(testBehaviorTrueTrue, true);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.gainControl();
    EXPECT_EQ("TrueFalse", testPriorityArbitrator.getCommand());
    EXPECT_EQ("TrueFalse", testPriorityArbitrator.getCommand());

    dynamic_cast<DummyBehavior*>(testBehaviorTrueFalse.get())->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("TrueTrue", testPriorityArbitrator.getCommand());
    EXPECT_EQ("TrueTrue", testPriorityArbitrator.getCommand());

    dynamic_cast<DummyBehavior*>(testBehaviorTrueFalse.get())->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("TrueTrue", testPriorityArbitrator.getCommand());
    EXPECT_EQ("TrueTrue", testPriorityArbitrator.getCommand());
}


class CostArbitrator : public Behavior {
public:
    struct CostEstimator {
        using Ptr = std::shared_ptr<CostEstimator>;

        virtual double estimateCost(const Command& command) {
            return std::numeric_limits<double>::max();
        }
    };

    CostArbitrator(const std::string& name = "CostArbitrator") : Behavior(name){};

    void addOption(const Behavior::Ptr& behavior, const bool interruptable, const CostEstimator::Ptr& costEstimator) {
        behaviorOptions_.push_back({behavior, interruptable, costEstimator});
    }

    Command getCommand() override {
        if (activeBehavior_) {
            if (behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition()) {
                return behaviorOptions_.at(*activeBehavior_).behavior->getCommand();
            } else {
                behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
            }
        }

        activateNextBehavior();
        return behaviorOptions_.at(*activeBehavior_).behavior->getCommand();
    }

    bool checkInvocationCondition() const override {
        for (auto& option : behaviorOptions_) {
            if (option.behavior->checkInvocationCondition()) {
                return true;
            }
        }
        return false;
    }
    bool checkCommitmentCondition() const override {
        if (activeBehavior_) {
            if (behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition()) {
                return true;
            } else {
                return checkInvocationCondition();
            }
        }
        return false;
    }

    virtual void gainControl() override {
    }

    virtual void loseControl() override {
        if (activeBehavior_) {
            behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
        }
        activeBehavior_ = boost::none;
    }


private:
    struct Option {
        Behavior::Ptr behavior;
        bool interruptable;
        CostEstimator::Ptr costEstimator;
    };

    std::vector<Option> behaviorOptions_;
    boost::optional<int> activeBehavior_;

    void activateNextBehavior() {
        double costOfBestOption = std::numeric_limits<double>::max();
        boost::optional<int> bestOption;

        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            Option& option = behaviorOptions_.at(i);
            if (option.behavior->checkInvocationCondition()) {

                double cost = option.costEstimator->estimateCost(option.behavior->getCommand());

                if (cost < costOfBestOption) {
                    costOfBestOption = cost;
                    bestOption = i;
                }
            }
        }

        if (!bestOption) {
            throw std::runtime_error("No behavior with true invocation condition found!");
        }

        activeBehavior_ = bestOption;
        behaviorOptions_.at(*activeBehavior_).behavior->gainControl();
    }
};

struct CostEstimatorFromCostMap : public CostArbitrator::CostEstimator {
    using CostMap = std::map<Command, double>;

    CostEstimatorFromCostMap(const CostMap& costMap) : costMap_{costMap} {};
    virtual double estimateCost(const Command& command) override {
        return costMap_.at(command);
    }

private:
    CostMap costMap_;
};

class CostArbitratorTest : public ::testing::Test {
protected:
    Behavior::Ptr testBehaviorLowCost = std::make_shared<DummyBehavior>(false, false, "low_cost");
    Behavior::Ptr testBehaviorMidCost = std::make_shared<DummyBehavior>(true, false, "mid_cost");
    Behavior::Ptr testBehaviorHighCost = std::make_shared<DummyBehavior>(true, true, "high_cost");

    CostEstimatorFromCostMap::CostMap costMap{{"low_cost", 0}, {"mid_cost", 1}, {"high_cost", 2}};

    CostArbitrator testCostArbitrator;
};


TEST_F(CostArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, true, std::make_shared<CostEstimatorFromCostMap>(costMap));
    testCostArbitrator.addOption(testBehaviorLowCost, true, std::make_shared<CostEstimatorFromCostMap>(costMap));
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());

    testCostArbitrator.addOption(testBehaviorHighCost, true, std::make_shared<CostEstimatorFromCostMap>(costMap));
    testCostArbitrator.addOption(testBehaviorMidCost, true, std::make_shared<CostEstimatorFromCostMap>(costMap));

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    dynamic_cast<DummyBehavior*>(testBehaviorMidCost.get())->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());

    // high_cost behavior is interruptable -> mid_cost should become active again
    dynamic_cast<DummyBehavior*>(testBehaviorMidCost.get())->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
}
