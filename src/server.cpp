#include <netinet/in.h>
#include <sys/socket.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

#include <unistd.h>
#include <string>
#include <map>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <csignal>
#include <ctime>

#include "utils.h"


const std::string protocol = "HTTP/1.1";

const std::string rootDir = "DOCUMENT_ROOT";
const auto rootPath = std::filesystem::current_path().parent_path().append(rootDir);


std::string getCodeMeaning(const short code) {
    return codes.at(code);
}

struct plainResponse {
    short responseCode;
    std::filesystem::path path;
    std::string data;
    size_t dataLength;
};

class Response {
private:
    short responseCode;
    std::stringstream headers;
    std::string body;

public:
    Response(const plainResponse &response) {
        responseCode = response.responseCode;
        body = response.data;
        headers << "Server: test-libevent\r\n" << "Date: " << getCurrentTime() << "Connection: Closed";

        if (response.path.string().length()) {
            headers << "\r\nContent-Type: " <<
                    extensions.at(response.path.extension().string().length() ?
                                  response.path.extension().string() : "text/plain");
        }
        headers <<
                "\r\nContent-Length: " << response.dataLength;
    }

    std::string get() const {
        return protocol + " " +
               std::to_string(responseCode) + " " +
               getCodeMeaning(responseCode) + "\r\n" +
               headers.str() + "\r\n\r\n" +
               body;
    }
};

class Handler {
private:
    std::string method;
    std::string requestPath;

    bool checkMethod() {
        if (method == "GET" || method == "HEAD") {
            return true;
        } else {
            return false;
        }
    }

public:
    explicit Handler(const std::string &request) {
        method = request.substr(0, request.find(' '));
        requestPath = request.substr(method.length() + 1,
                                     request.rfind(' ') - (method.length() + 1));
    }

    ~Handler() = default;

    plainResponse handle() {
        if (!checkMethod()) {
            return {405, {}, {}, {}};
        }

        if (requestPath.find("/../") != std::string::npos) {
            return {400, {}, {}, {}};
        }

        auto path = rootPath;
        path.append(rootPath.string() + requestPath);

        if (!path.has_filename()) {
            path.replace_filename("index.html");
        }

        if (!std::filesystem::exists(path)) {
            path.assign(urlDecode(path.string()));

            path.assign(path.string().substr(0, path.string().find('?')));
            path.assign(path.string().substr(0, path.string().find('#')));

            if (!std::filesystem::exists(path)) {
                if (requestPath.find('.') != std::string::npos) {
                    return {404, {}, {}, {}};
                } else {
                    return {403, {}, {}, {}};
                }
            }
        }

        if (std::filesystem::is_directory(path)) {
            return {403, {}, {}, {}};
        }

        std::ifstream input(path.string(), std::ios::binary);
        std::string str = {std::istreambuf_iterator<char>(input), {}};

        if (method == "HEAD") {
            return {200, {}, {}, str.length()};
        }

        return {200, path, str, str.length()};
    }
};

const short MAX_LINE = 16384;
const char CHUNK_SIZE = 64;

std::string read(struct bufferevent *bev) {
    char *getRequest = new char[CHUNK_SIZE];
    std::string request;

    while (bufferevent_read(bev, getRequest, CHUNK_SIZE)) {
        if (const auto pos = std::string(getRequest).find('\n'); pos != -1) {
            request += std::string(getRequest).substr(0, pos);
            break;
        }
        request += getRequest;
    }
    delete[] getRequest;
    return request;
}

void readcb(struct bufferevent *bev, void *ctx) {
    Handler handler(read(bev));
    Response response(handler.handle());

    const auto strResponse = response.get();

    bufferevent_write(bev, strResponse.c_str(), strResponse.length());
}


void errorcb(struct bufferevent *bev, short error, void *ctx) {
    if (error & BEV_EVENT_EOF) {
        std::cerr << "connection has been closed" << std::endl;
    } else if (error & BEV_EVENT_ERROR) {
        std::cerr << "BEV_EVENT_ERROR: " << std::to_string(error) << std::endl;
    } else if (error & BEV_EVENT_TIMEOUT) {
        std::cerr << "BEV_EVENT_TIMEOUT" << std::endl;
    }
    bufferevent_free(bev);
}

void writecb(struct bufferevent *bev, void *user_data) {
    struct evbuffer const *output = bufferevent_get_output(bev);
    if (!evbuffer_get_length(output)) {
        bufferevent_free(bev);
    } else {
        std::cerr << "Error bufferevent_free; " << std::endl;
    }
}

void
do_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int sockLength, void *arg) {
    auto *base = static_cast<event_base *>(arg);

    if (fd < 0) {
        std::cerr << "accept; ";
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        evutil_make_socket_nonblocking(fd);
        struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        if (bev) {
            bufferevent_setcb(bev, readcb, writecb, errorcb, base);
            bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
            bufferevent_enable(bev, EV_READ);
        } else {
            std::cerr << "do_accept+bev; ";
            event_base_loopbreak(base);
            return;
        }
    }
}

void SignalCallback(evutil_socket_t sig, short events, void *user_data) {
    auto *callbackBase = static_cast<event_base *>(user_data);
    struct timeval delay = {4, 4};
    std::cerr << "Caught an interrupt signal; exiting cleanly in two seconds; \n";
    event_base_loopexit(callbackBase, &delay);
}

void run() {
    for (int i = 1; i < 4; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            break;
        } else if (pid < 0) {
            std::cerr << "fork error; \n";
            return;
        }
    }

    struct event_base *base = event_base_new();
    if (!base) {
        std::cerr << "base; ";
        return;
    }

    struct event *listener_event = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, SignalCallback,
                                             base);
    if (!listener_event) {
        std::cerr << "event_new; ";
        return;
    }

    struct sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(9080);

    struct evconnlistener *listener = evconnlistener_new_bind(base, do_accept, (void *) base,
                                                              LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE |
                                                              LEV_OPT_REUSEABLE_PORT, -1,
                                                              (struct sockaddr *) &sin,
                                                              sizeof(sin));

    if (!listener) {
        std::cerr << "listener; ";
        return;
    }
    event_reinit(base);
    std::cout << "up; ";
    event_base_dispatch(base);

    std::cout << "down; ";
    evconnlistener_free(listener);
    event_free(listener_event);
    event_base_free(base);
//    libevent_global_shutdown();
}