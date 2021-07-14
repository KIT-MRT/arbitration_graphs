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
#include <thread>
#include "gtest/gtest.h"

#include "cache.hpp"

using namespace behavior_planning;
using namespace std::chrono_literals;

TEST(CacheTest, BasicTest) {
    Cache<double> cache;
    Time time1 = std::chrono::system_clock::now();
    EXPECT_FALSE(cache.cached());
    EXPECT_FALSE(cache.cachedWithinOneWorkingStep(time1, 10));
    EXPECT_FALSE(cache.lastCachedTime());

    cache.cache(1., time1);
    EXPECT_TRUE(cache.cached());
    EXPECT_DOUBLE_EQ(*cache.cached(), 1.);
    EXPECT_TRUE(cache.lastCachedTime());
    EXPECT_EQ(*cache.lastCachedTime(), time1);

    Time time2 = std::chrono::system_clock::now();
    EXPECT_TRUE(cache.cachedWithinOneWorkingStep(time2, 10));
    EXPECT_DOUBLE_EQ(*cache.cachedWithinOneWorkingStep(time2, 10), 1.);

    std::this_thread::sleep_for(200ms);
    Time time3 = std::chrono::system_clock::now();
    EXPECT_FALSE(cache.cachedWithinOneWorkingStep(time3, 10));
}
