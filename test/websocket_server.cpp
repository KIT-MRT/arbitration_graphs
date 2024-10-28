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

#include "gui/websocket_server.hpp"

#include "gtest/gtest.h"

#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace arbitration_graphs;

TEST(WebSocketServer, Autostart) {

  // We run the test in a thread, in order to test for timeouts
  auto asyncFuture = std::async(std::launch::async, []() {
    gui::WebSocketServer server{8080, true};

    std::cout << "WebSocketServer set up, sending some messages" << std::endl;

    for (int i = 0; i < 3; i++) {
      std::this_thread::sleep_for(
          std::chrono::seconds(1)); // Simulate some delay
      server.broadcast("Hello from the server! " + std::to_string(i));
    }

    std::cout << "Closing" << std::endl;
  });

  // Make sure that the server shuts down cleanly
  ASSERT_TRUE(asyncFuture.wait_for(std::chrono::seconds(10)) !=
              std::future_status::timeout);
}

TEST(WebSocketServer, CustomStartStop) {

  // We run the test in a thread, in order to test for timeouts
  auto asyncFuture = std::async(std::launch::async, []() {
    gui::WebSocketServer server{8080};
    auto a = server.start();

    std::cout << "WebSocketServer set up, sending some messages" << std::endl;

    for (int i = 0; i < 3; i++) {
      std::this_thread::sleep_for(
          std::chrono::seconds(1)); // Simulate some delay
      server.broadcast("Hello from the server! " + std::to_string(i));
    }

    std::cout << "Closing" << std::endl;
    server.stop();
  });

  // Make sure that the server shuts down cleanly
  ASSERT_TRUE(asyncFuture.wait_for(std::chrono::seconds(10)) !=
              std::future_status::timeout);
}