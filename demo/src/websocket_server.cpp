#include "utils/websocket_server.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

namespace utils {

WebSocketServer::WebSocketServer(int port) : port(port), context(nullptr) {
    protocols[0] = {
        "http-only",
        WebSocketServer::callback,
        0,
        1024,
    };
    protocols[1] = {NULL, NULL, 0, 0};
}

WebSocketServer::~WebSocketServer() {
    if (context) {
        lws_context_destroy(context);
    }
}

void WebSocketServer::run() {
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = port;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    info.user = this;

    context = lws_create_context(&info);
    if (!context) {
        std::cerr << "Failed to create context" << std::endl;
        return;
    }

    while (true) {
        lws_service(context, 50);
    }
}

int WebSocketServer::callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    WebSocketServer* server = (WebSocketServer*)lws_context_user(lws_get_context(wsi));
    switch (reason) {
    case LWS_CALLBACK_ESTABLISHED:
        server->clients.push_back(wsi);
        std::cout << "Client connected" << std::endl;
        break;
    case LWS_CALLBACK_RECEIVE:
        std::cout << "Received message: " << std::string((char*)in, len) << std::endl;
        break;
    case LWS_CALLBACK_CLOSED:
        auto it = std::find(server->clients.begin(), server->clients.end(), wsi);
        if (it != server->clients.end()) {
            server->clients.erase(it);
            std::cout << "Client disconnected" << std::endl;
        }
        break;
    }
    return 0;
}

void WebSocketServer::broadcast(const std::string& message) {
    for (auto client : clients) {
        unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + message.size() + LWS_SEND_BUFFER_POST_PADDING];
        unsigned char* p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
        std::memcpy(p, message.c_str(), message.size());
        lws_write(client, p, message.size(), LWS_WRITE_TEXT);
    }
}

} // namespace utils
