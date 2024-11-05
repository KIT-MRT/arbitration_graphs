#pragma once

#include "crow_config.hpp"

#include <crow.h>
#include <mutex>
#include <set>

#include <glog/logging.h>

namespace arbitration_graphs::gui {

class WebServer {
public:
    WebServer(int port, bool autostart = false, crow::LogLevel loglevel = crow::LogLevel::Warning)
            : port_{port}, autostart_{autostart} {

        app_.loglevel(loglevel);

        CROW_ROUTE(app_, "/")
        ([]() {
            crow::response response;
            response.set_static_file_info_unsafe(std::string{CROW_STATIC_DIRECTORY} + "/index.html");
            return response;
        });

        CROW_WEBSOCKET_ROUTE(app_, "/status")
            .onopen([this](crow::websocket::connection& conn) {
                std::lock_guard<std::mutex> guard(connections_mutex_);
                connections_.insert(&conn);
                CROW_LOG_INFO << "New WebSocket connection opened!";
            })
            .onclose([this](crow::websocket::connection& conn, const std::string& reason) {
                std::lock_guard<std::mutex> guard(connections_mutex_);
                connections_.erase(&conn);
                CROW_LOG_INFO << "WebSocket connection closed: " << reason;
            })
            .onmessage([this](crow::websocket::connection& conn, const std::string& message, bool is_binary) {
                CROW_LOG_DEBUG << "Received message: " << message;
            });

        if (autostart_) {
            // We need to store the std::future to run Crow asynchronously
            _f = start();
        }
    }

    ~WebServer() {
        if (autostart_) {
            stop();
        }
    }

    // Function to start the server
    std::future<void> start() {
        CROW_LOG_INFO << "Serving WebApp from " << CROW_STATIC_DIRECTORY;
        return app_.port(port_).multithreaded().run_async();
    }

    // Function to stop the server
    void stop() {
        app_.stop();
    }

    // Function to send a message to all connected clients
    void broadcast(const std::string& message) {
        std::lock_guard<std::mutex> guard(connections_mutex_);
        for (auto* conn : connections_) {
            conn->send_text(message);
        }

        CROW_LOG_DEBUG << "Message sent to all clients: " << message;
    }

    void loglevel(crow::LogLevel level) {
        app_.loglevel(level);
    }

private:
    crow::SimpleApp app_;
    std::set<crow::websocket::connection*> connections_;
    std::mutex connections_mutex_;
    std::future<void> _f;

    int port_;
    bool autostart_;
};

} // namespace arbitration_graphs::gui
