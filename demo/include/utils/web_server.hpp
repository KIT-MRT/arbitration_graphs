#pragma once

#include <atomic>
#include <condition_variable>
#include <httplib.h>
#include <mutex>


namespace utils {

class WebServer {
public:
    WebServer(const int port);

    void run();
    void broadcast(const std::string& message);

private:
    void wait_event(httplib::DataSink* sink);
    void send_event(const std::string& message);

    int port_;
    httplib::Server svr_;

    std::mutex m_;
    std::condition_variable cv_;
    std::atomic_int id_{0};
    std::atomic_int cid_{-1};
    std::string message_;
};

} // namespace utils