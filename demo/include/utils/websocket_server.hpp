#pragma once

#include <libwebsockets.h>

#include <string>
#include <vector>

namespace utils {

class WebSocketServer {
public:
    WebSocketServer(int port);
    ~WebSocketServer();

    void run();

    static int callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

    void broadcast(const std::string& message);

private:
    struct lws_context* context;
    std::vector<struct lws*> clients;
    struct lws_protocols protocols[2];
    int port;
};

} // namespace utils
