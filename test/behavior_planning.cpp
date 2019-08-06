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


template <typename CommandT>
class Behavior {
public:
    using Ptr = std::shared_ptr<Behavior>;

    Behavior(const std::string& name = "Behavior") : name_{name} {
    }

    virtual CommandT getCommand() = 0;
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

using DummyCommand = std::string;
class DummyBehavior : public Behavior<DummyCommand> {
public:
    using Ptr = std::shared_ptr<DummyBehavior>;

    DummyBehavior(const bool invocation, const bool commitment, const std::string& name = "DummyBehavior")
            : Behavior(name), invocationCondition_{invocation}, commitmentCondition_{commitment}, loseControlCounter_{
                                                                                                      0} {};

    DummyCommand getCommand() override {
        return name_;
    }
    bool checkInvocationCondition() const override {
        return invocationCondition_;
    }
    bool checkCommitmentCondition() const override {
        return commitmentCondition_;
    }
    virtual void loseControl() override {
        loseControlCounter_++;
    }

    bool invocationCondition_;
    bool commitmentCondition_;
    int loseControlCounter_;
};

class DummyBehaviorTest : public ::testing::Test {
protected:
    DummyBehavior testBehaviorTrue{true, true};

    DummyCommand expected_command{"DummyBehavior"};
};

TEST_F(DummyBehaviorTest, BasicInterface) {
    EXPECT_EQ(expected_command, testBehaviorTrue.getCommand());
    EXPECT_TRUE(testBehaviorTrue.checkCommitmentCondition());
    EXPECT_TRUE(testBehaviorTrue.checkInvocationCondition());
    EXPECT_NO_THROW(testBehaviorTrue.gainControl());
    EXPECT_NO_THROW(testBehaviorTrue.loseControl());
}

template <typename CommandT>
class PriorityArbitrator : public Behavior<CommandT> {
public:
    PriorityArbitrator(const std::string& name = "PriorityArbitrator") : Behavior<CommandT>(name){};

    void addOption(const typename Behavior<CommandT>::Ptr& behavior, const bool interruptable) {
        behaviorOptions_.push_back({behavior, interruptable});
    }

    CommandT getCommand() override {
        bool activeBehaviorCanBeContinued =
            activeBehavior_ && behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();

        if (activeBehavior_ && !activeBehaviorCanBeContinued) {
            behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
            activeBehavior_ = boost::none;
        }

        bool activeBehaviorInterruptable = activeBehavior_ && behaviorOptions_.at(*activeBehavior_).interruptable;

        if (!activeBehavior_ || !activeBehaviorCanBeContinued || activeBehaviorInterruptable) {
            boost::optional<int> bestOption = findBestOption();

            if (bestOption) {
                if (!activeBehavior_) {
                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_).behavior->gainControl();
                } else if (*bestOption != *activeBehavior_) {
                    behaviorOptions_.at(*activeBehavior_).behavior->loseControl();

                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_).behavior->gainControl();
                }
            } else {
                throw std::runtime_error("No behavior with true invocation condition found! Only call getCommand() if "
                                         "checkInvocationCondition() or checkCommitmentCondition() is true!");
            }
        }
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
        typename Behavior<CommandT>::Ptr behavior;
        bool interruptable;
    };

    /*!
     * Find behavior option with highest priority and true invocation condition
     *
     * @return  Applicable option with highest priority (can also be the currently active option)
     */
    boost::optional<int> findBestOption() {
        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            Option& option = behaviorOptions_.at(i);

            bool isActive = activeBehavior_ && (i == *activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();
            if (option.behavior->checkInvocationCondition() || isActiveAndCanBeContinued) {
                return i;
            }
        }
        return boost::none;
    }

    std::vector<Option> behaviorOptions_;
    boost::optional<int> activeBehavior_;
};

class PriorityArbitratorTest : public ::testing::Test {
protected:
    DummyBehavior::Ptr testBehaviorHighPriority = std::make_shared<DummyBehavior>(false, false, "HighPriority");
    DummyBehavior::Ptr testBehaviorMidPriority = std::make_shared<DummyBehavior>(true, false, "MidPriority");
    DummyBehavior::Ptr testBehaviorLowPriority = std::make_shared<DummyBehavior>(true, true, "LowPriority");


    PriorityArbitrator<DummyCommand> testPriorityArbitrator;
};


TEST_F(PriorityArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, false);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, false);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.addOption(testBehaviorMidPriority, false);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, false);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.gainControl();

    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorMidPriority->loseControlCounter_);

    // testBehaviorMidPriority.loseControl() should be called within getCommand since commitment condition is false
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(1, testBehaviorMidPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());

    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    // testBehaviorLowPriority.loseControl() should NOT be called within getCommand since commitment condition is true
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorLowPriority->loseControlCounter_);

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
}


TEST_F(PriorityArbitratorTest, BasicFunctionalityWithInterruptableOptions) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testPriorityArbitrator.addOption(testBehaviorHighPriority, true);
    testPriorityArbitrator.addOption(testBehaviorHighPriority, true);
    EXPECT_FALSE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.addOption(testBehaviorMidPriority, true);
    testPriorityArbitrator.addOption(testBehaviorLowPriority, true);

    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testPriorityArbitrator.checkCommitmentCondition());

    testPriorityArbitrator.gainControl();
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());

    testBehaviorMidPriority->invocationCondition_ = false;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("LowPriority", testPriorityArbitrator.getCommand());

    testBehaviorMidPriority->invocationCondition_ = true;
    EXPECT_TRUE(testPriorityArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testPriorityArbitrator.checkCommitmentCondition());
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
    EXPECT_EQ("MidPriority", testPriorityArbitrator.getCommand());
}


template <typename CommandT>
class CostArbitrator : public Behavior<CommandT> {
public:
    struct CostEstimator {
        using Ptr = std::shared_ptr<CostEstimator>;

        virtual double estimateCost(const CommandT& command, const bool isActive) = 0;
    };

    CostArbitrator(const std::string& name = "CostArbitrator") : Behavior<CommandT>(name){};

    void addOption(const typename Behavior<CommandT>::Ptr& behavior,
                   const bool interruptable,
                   const typename CostEstimator::Ptr& costEstimator) {
        behaviorOptions_.push_back({behavior, interruptable, costEstimator});
    }

    CommandT getCommand() override {
        bool activeBehaviorCanBeContinued =
            activeBehavior_ && behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();

        if (activeBehavior_ && !activeBehaviorCanBeContinued) {
            behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
            activeBehavior_ = boost::none;
        }

        bool activeBehaviorInterruptable = activeBehavior_ && behaviorOptions_.at(*activeBehavior_).interruptable;

        if (!activeBehavior_ || !activeBehaviorCanBeContinued || activeBehaviorInterruptable) {
            boost::optional<int> bestOption = findBestOption();

            if (bestOption) {
                if (!activeBehavior_) {
                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_).behavior->gainControl();
                } else if (*bestOption != *activeBehavior_) {
                    behaviorOptions_.at(*activeBehavior_).behavior->loseControl();

                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_).behavior->gainControl();
                }
            } else {
                throw std::runtime_error("No behavior with true invocation condition found! Only call getCommand() if "
                                         "checkInvocationCondition() or checkCommitmentCondition() is true!");
            }
        }
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
        typename Behavior<CommandT>::Ptr behavior;
        bool interruptable;
        typename CostEstimator::Ptr costEstimator;
    };

    std::vector<Option> behaviorOptions_;
    boost::optional<int> activeBehavior_;

    /*!
     * Find behavior option with lowest cost and true invocation condition
     *
     * @return  Applicable option with lowest costs (can also be the currently active option)
     */
    boost::optional<int> findBestOption() {
        double costOfBestOption = std::numeric_limits<double>::max();
        boost::optional<int> bestOption;

        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            Option& option = behaviorOptions_.at(i);

            bool isActive = activeBehavior_ && (i == *activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();
            if (option.behavior->checkInvocationCondition() || isActiveAndCanBeContinued) {
                double cost = option.costEstimator->estimateCost(option.behavior->getCommand(), isActive);

                if (cost < costOfBestOption) {
                    costOfBestOption = cost;
                    bestOption = i;
                }
            }
        }
        return bestOption;
    }
};

struct CostEstimatorFromCostMap : public CostArbitrator<DummyCommand>::CostEstimator {
    using CostMap = std::map<DummyCommand, double>;

    CostEstimatorFromCostMap(const CostMap& costMap, const double activationCosts = 0)
            : costMap_{costMap}, activationCosts_{activationCosts} {};

    virtual double estimateCost(const DummyCommand& command, const bool isActive) override {
        if (isActive) {
            return costMap_.at(command);
        } else {
            return costMap_.at(command) + activationCosts_;
        }
    }

private:
    CostMap costMap_;
    double activationCosts_;
};

class CostArbitratorTest : public ::testing::Test {
protected:
    DummyBehavior::Ptr testBehaviorLowCost = std::make_shared<DummyBehavior>(false, false, "low_cost");
    DummyBehavior::Ptr testBehaviorMidCost = std::make_shared<DummyBehavior>(true, false, "mid_cost");
    DummyBehavior::Ptr testBehaviorHighCost = std::make_shared<DummyBehavior>(true, true, "high_cost");

    CostEstimatorFromCostMap::CostMap costMap{{"low_cost", 0}, {"mid_cost", 1}, {"high_cost", 2}};
    CostEstimatorFromCostMap::Ptr cost_estimator = std::make_shared<CostEstimatorFromCostMap>(costMap);
    CostEstimatorFromCostMap::Ptr cost_estimator_with_activation_costs =
        std::make_shared<CostEstimatorFromCostMap>(costMap, 10);

    CostArbitrator<DummyCommand> testCostArbitrator;
};


TEST_F(CostArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, false, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, false, cost_estimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.addOption(testBehaviorHighCost, false, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, false, cost_estimator);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorMidCost->loseControlCounter_);

    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(1, testBehaviorMidCost->loseControlCounter_);

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());

    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorHighCost->loseControlCounter_);
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ(0, testBehaviorHighCost->loseControlCounter_);

    // high_cost behavior is not interruptable -> high_cost should stay active
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
}


TEST_F(CostArbitratorTest, BasicFunctionalityWithInterruptableOptionsAndActivationCosts) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, true, cost_estimator_with_activation_costs);
    testCostArbitrator.addOption(testBehaviorLowCost, true, cost_estimator_with_activation_costs);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.addOption(testBehaviorHighCost, true, cost_estimator_with_activation_costs);
    testCostArbitrator.addOption(testBehaviorMidCost, true, cost_estimator_with_activation_costs);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());

    // high_cost behavior is not interruptable -> high_cost should stay active
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
}


TEST_F(CostArbitratorTest, BasicFunctionalityWithInterruptableOptions) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testCostArbitrator.addOption(testBehaviorLowCost, true, cost_estimator);
    testCostArbitrator.addOption(testBehaviorLowCost, true, cost_estimator);
    EXPECT_FALSE(testCostArbitrator.checkInvocationCondition());
    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.addOption(testBehaviorHighCost, true, cost_estimator);
    testCostArbitrator.addOption(testBehaviorMidCost, true, cost_estimator);

    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());

    EXPECT_FALSE(testCostArbitrator.checkCommitmentCondition());

    testCostArbitrator.gainControl();
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());

    testBehaviorMidCost->invocationCondition_ = false;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("high_cost", testCostArbitrator.getCommand());

    // high_cost behavior is interruptable -> mid_cost should become active again
    testBehaviorMidCost->invocationCondition_ = true;
    EXPECT_TRUE(testCostArbitrator.checkInvocationCondition());
    EXPECT_TRUE(testCostArbitrator.checkCommitmentCondition());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
    EXPECT_EQ("mid_cost", testCostArbitrator.getCommand());
}
