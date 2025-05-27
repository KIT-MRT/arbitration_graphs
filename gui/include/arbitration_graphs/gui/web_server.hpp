#pragma once

#include "crow_config.hpp"

#include <crow.h>
#include <filesystem>
#include <mutex>
#include <set>

#include <glog/logging.h>


namespace arbitration_graphs::gui {

/**
 * @brief Serves the arbitration_graphs GUI via HTTP and WebSocket using the Crow framework.
 *
 * @param port The TCP port number on which the web server will listen for incoming HTTP/WebSocket connections.
 *             This allows you to specify which port the arbitration_graphs GUI will be accessible from.
 *
 * @param autostart If set to true, the server will automatically start upon construction and run asynchronously.
 *                  If set to false (default), you must manually start/stop the server by calling start()/stop().
 * @param loglevel The logging verbosity for the Crow application (default: crow::LogLevel::Warning).
 *
 * The server serves static GUI files from a directory determined by environment variables or predefined paths.
 * The "/" route serves the main index.html file, while "/<path>" serves other static files.
 * The "/status" WebSocket route allows clients to connect for real-time updates; use broadcast() to send messages to
 * all connected clients.
 *
 * Example usage:
 * @code
 *   WebServer server(8080, true); // Starts server on port 8080 immediately
 *   // or
 *   WebServer server(8080, false);
 *   server.start(); // Start server manually
 *   server.stop();  // Stop server manually
 * @endcode
 */
class WebServer {
public:
    WebServer(int port, bool autostart = false, crow::LogLevel loglevel = crow::LogLevel::Warning)
            : static_directory_{crow::utility::normalize_path(dataDirectory())}, port_{port}, autostart_{autostart} {

        // Set loglevel and turn off Crow's signal handler
        app_.loglevel(loglevel).signal_clear();

        CROW_ROUTE(app_, "/")
        ([this]() {
            crow::response response;
            response.set_static_file_info_unsafe(static_directory_ + "/index.html");
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

        CROW_ROUTE(app_, "/<path>")
        ([this](std::string file_path_partial) {
            crow::response response;
            crow::utility::sanitize_filename(file_path_partial);
            response.set_static_file_info_unsafe(static_directory_ + file_path_partial);
            return response;
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
        CROW_LOG_INFO << "Serving WebApp from " << static_directory_;
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
    std::string dataDirectory() {
        namespace fs = std::filesystem;

        // Check the APP_DIRECTORY environment variable
        if (const char* pathEnv = std::getenv("APP_DIRECTORY")) {
            fs::path dir = fs::path(pathEnv);
            if (fs::exists(dir) && fs::is_directory(dir)) {
                return dir;
            }
        }

        // Check if the install directory works
        if (fs::exists(INSTALL_APP_DIRECTORY) && fs::is_directory(INSTALL_APP_DIRECTORY)) {
            return INSTALL_APP_DIRECTORY;
        }

        // Check if the local source directory works
        if (fs::exists(LOCAL_APP_DIRECTORY) && fs::is_directory(LOCAL_APP_DIRECTORY)) {
            return LOCAL_APP_DIRECTORY;
        }

        // Return an empty path if not found
        CROW_LOG_WARNING << "App data directory not found! Defaulting to \"app/\""
                         << " Searched in these paths: INSTALL_APP_DIRECTORY=" << INSTALL_APP_DIRECTORY
                         << " LOCAL_APP_DIRECTORY=" << LOCAL_APP_DIRECTORY
                         << " APP_DIRECTORY=" << std::getenv("APP_DIRECTORY");
        return "app/";
    }

    crow::SimpleApp app_;
    std::set<crow::websocket::connection*> connections_;
    std::mutex connections_mutex_;
    std::future<void> _f;

    const std::string static_directory_;

    int port_;
    bool autostart_;
};

} // namespace arbitration_graphs::gui
