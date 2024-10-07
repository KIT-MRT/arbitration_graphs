#include "demo/verifier.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {


class VerifierTest : public ::testing::Test {
protected:
    VerifierTest() : environmentModel_(std::make_shared<MockEnvironmentModel>()), verifier_{environmentModel_} {
    }

    MockEnvironmentModel::Ptr environmentModel_;

    Verifier verifier_;
};

TEST_F(VerifierTest, basicVerification) {
    Time time = Clock::now();

    const char str[] = {"###"
                        "#  "
                        "###"};
    environmentModel_->setMaze({3, 3}, str);
    environmentModel_->setPacmanPosition({1, 1});

    auto goodCommand = Command{Direction::RIGHT};
    VerificationResult goodResult = verifier_.analyze(time, goodCommand);
    EXPECT_TRUE(goodResult.isOk());

    auto badCommand = Command{Direction::LEFT};
    VerificationResult badResult = verifier_.analyze(time, badCommand);
    EXPECT_FALSE(badResult.isOk());
}

} // namespace demo
