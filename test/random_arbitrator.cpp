#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "behavior.hpp"
#include "random_arbitrator.hpp"

#include "dummy_types.hpp"


using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;


class RandomArbitratorTest : public ::testing::Test {
protected:
    using OptionFlags = RandomArbitrator<DummyEnvironmentModel, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorUnavailable = std::make_shared<DummyBehavior>(false, false, "Unavailable");
    DummyBehavior::Ptr testBehaviorHighWeight = std::make_shared<DummyBehavior>(true, false, "HighWeight");
    DummyBehavior::Ptr testBehaviorMidWeight = std::make_shared<DummyBehavior>(true, false, "MidWeight");
    DummyBehavior::Ptr testBehaviorLowWeight = std::make_shared<DummyBehavior>(true, false, "LowWeight");

    RandomArbitrator<DummyEnvironmentModel, DummyCommand> testRandomArbitrator;

    DummyEnvironmentModel environmentModel;

    Time time{Clock::now()};
};


TEST_F(RandomArbitratorTest, BasicFunctionality) {
    // if there are no options yet -> the invocationCondition should be false
    EXPECT_FALSE(testRandomArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testRandomArbitrator.checkCommitmentCondition(time, environmentModel));

    // otherwise the invocationCondition is true if any of the option has true invocationCondition
    testRandomArbitrator.addOption(testBehaviorUnavailable, OptionFlags::NO_FLAGS);
    EXPECT_FALSE(testRandomArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testRandomArbitrator.checkCommitmentCondition(time, environmentModel));

    testRandomArbitrator.addOption(testBehaviorHighWeight, OptionFlags::NO_FLAGS, 2.5);
    testRandomArbitrator.addOption(testBehaviorMidWeight, OptionFlags::NO_FLAGS);
    testRandomArbitrator.addOption(testBehaviorLowWeight, OptionFlags::NO_FLAGS, 0.5);
    double weightSumOfAvailableOptions = 2.5 + 1.0 + 0.5;

    EXPECT_TRUE(testRandomArbitrator.checkInvocationCondition(time, environmentModel));
    EXPECT_FALSE(testRandomArbitrator.checkCommitmentCondition(time, environmentModel));

    testRandomArbitrator.gainControl(time, environmentModel);

    int sampleSize = 1000;
    std::map<std::string, int> commandCounter{
        {"Unavailable", 0}, {"HighWeight", 0}, {"MidWeight", 0}, {"LowWeight", 0}};

    for (int i = 0; i < sampleSize; i++) {
        std::string command = testRandomArbitrator.getCommand(time, environmentModel);
        commandCounter[command]++;
    }

    EXPECT_EQ(0, commandCounter["Unavailable"]);
    EXPECT_NEAR(2.5 / weightSumOfAvailableOptions, commandCounter["HighWeight"] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(1.0 / weightSumOfAvailableOptions, commandCounter["MidWeight"] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(0.5 / weightSumOfAvailableOptions, commandCounter["LowWeight"] / static_cast<double>(sampleSize), 0.1);
}

TEST_F(RandomArbitratorTest, Printout) {
    // Force midWeight behavior by setting all applicable behavior's weights to 0
    testRandomArbitrator.addOption(testBehaviorUnavailable, OptionFlags::NO_FLAGS);
    testRandomArbitrator.addOption(testBehaviorHighWeight, OptionFlags::NO_FLAGS, 0);
    testRandomArbitrator.addOption(testBehaviorHighWeight, OptionFlags::NO_FLAGS, 0);
    testRandomArbitrator.addOption(testBehaviorMidWeight, OptionFlags::NO_FLAGS, 2.5);
    testRandomArbitrator.addOption(testBehaviorLowWeight, OptionFlags::NO_FLAGS, 0);

    // clang-format off
    std::string expected_printout = invocationTrueString + commitmentFalseString + "RandomArbitrator\n"
                                    "    - (weight: 1.000) " + invocationFalseString + commitmentFalseString + "Unavailable\n"
                                    "    - (weight: 0.000) " + invocationTrueString + commitmentFalseString + "HighWeight\n"
                                    "    - (weight: 0.000) " + invocationTrueString + commitmentFalseString + "HighWeight\n"
                                    "    - (weight: 2.500) " + invocationTrueString + commitmentFalseString + "MidWeight\n"
                                    "    - (weight: 0.000) " + invocationTrueString + commitmentFalseString + "LowWeight";
    // clang-format on
    std::string actual_printout = testRandomArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);


    testRandomArbitrator.gainControl(time, environmentModel);
    EXPECT_EQ("MidWeight", testRandomArbitrator.getCommand(time, environmentModel));

    // clang-format off
    expected_printout = invocationTrueString + commitmentTrueString + "RandomArbitrator\n"
                        "    - (weight: 1.000) " + invocationFalseString + commitmentFalseString + "Unavailable\n"
                        "    - (weight: 0.000) " + invocationTrueString + commitmentFalseString + "HighWeight\n"
                        "    - (weight: 0.000) " + invocationTrueString + commitmentFalseString + "HighWeight\n"
                        " -> - (weight: 2.500) " + invocationTrueString + commitmentFalseString + "MidWeight\n"
                        "    - (weight: 0.000) " + invocationTrueString + commitmentFalseString + "LowWeight";
    // clang-format on
    actual_printout = testRandomArbitrator.to_str(time, environmentModel);
    std::cout << actual_printout << std::endl;

    EXPECT_EQ(expected_printout, actual_printout);
}

TEST_F(RandomArbitratorTest, ToYaml) {
    testRandomArbitrator.addOption(testBehaviorUnavailable, OptionFlags::NO_FLAGS);
    testRandomArbitrator.addOption(testBehaviorHighWeight, OptionFlags::NO_FLAGS, 0);
    testRandomArbitrator.addOption(testBehaviorHighWeight, OptionFlags::NO_FLAGS, 0);
    testRandomArbitrator.addOption(testBehaviorMidWeight, OptionFlags::INTERRUPTABLE);
    testRandomArbitrator.addOption(testBehaviorLowWeight, OptionFlags::NO_FLAGS, 0);

    YAML::Node yaml = testRandomArbitrator.toYaml(time, environmentModel);

    EXPECT_EQ("RandomArbitrator", yaml["type"].as<std::string>());
    EXPECT_EQ("RandomArbitrator", yaml["name"].as<std::string>());
    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(false, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(5, yaml["options"].size());
    EXPECT_EQ("Option", yaml["options"][0]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][1]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][2]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][3]["type"].as<std::string>());
    EXPECT_EQ("Option", yaml["options"][4]["type"].as<std::string>());
    EXPECT_EQ("Unavailable", yaml["options"][0]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("HighWeight", yaml["options"][1]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("HighWeight", yaml["options"][2]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("MidWeight", yaml["options"][3]["behavior"]["name"].as<std::string>());
    EXPECT_EQ("LowWeight", yaml["options"][4]["behavior"]["name"].as<std::string>());
    EXPECT_EQ(false, yaml["options"][0]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][1]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][2]["flags"].IsDefined());
    ASSERT_EQ(true, yaml["options"][3]["flags"].IsDefined());
    EXPECT_EQ(false, yaml["options"][4]["flags"].IsDefined());

    EXPECT_EQ(false, yaml["activeBehavior"].IsDefined());

    testRandomArbitrator.gainControl(time, environmentModel);
    testRandomArbitrator.getCommand(time, environmentModel);

    yaml = testRandomArbitrator.toYaml(time, environmentModel);

    EXPECT_EQ(true, yaml["invocationCondition"].as<bool>());
    EXPECT_EQ(true, yaml["commitmentCondition"].as<bool>());

    ASSERT_EQ(true, yaml["activeBehavior"].IsDefined());
    EXPECT_EQ(3, yaml["activeBehavior"].as<int>());
}

TEST(PriorityArbitrator, SubCommandTypeDiffersFromCommandType) {
    Time time{Clock::now()};
    DummyEnvironmentModel environmentModel;

    using OptionFlags = RandomArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand>::Option::Flags;

    DummyBehavior::Ptr testBehaviorHighWeight = std::make_shared<DummyBehavior>(false, false, "___HighWeight___");
    DummyBehavior::Ptr testBehaviorMidWeight = std::make_shared<DummyBehavior>(true, false, "__MidWeight__");
    DummyBehavior::Ptr testBehaviorLowWeight = std::make_shared<DummyBehavior>(true, true, "_LowWeight_");

    RandomArbitrator<DummyEnvironmentModel, DummyCommandInt, DummyCommand> testRandomArbitrator;

    testRandomArbitrator.addOption(testBehaviorHighWeight, OptionFlags::NO_FLAGS);
    testRandomArbitrator.addOption(testBehaviorMidWeight, OptionFlags::NO_FLAGS);
    testRandomArbitrator.addOption(testBehaviorLowWeight, OptionFlags::NO_FLAGS, 0);

    testRandomArbitrator.gainControl(time, environmentModel);

    std::string expected = "__MidWeight__";
    EXPECT_EQ(expected.length(), testRandomArbitrator.getCommand(time, environmentModel));
}
