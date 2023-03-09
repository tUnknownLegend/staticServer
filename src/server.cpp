//#include "net.h"

#include <netinet/in.h>
#include <sys/socket.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <map>
#include <iostream>

#include <filesystem>

#include <coroutine>
#include <fstream>

const std::string protocol = "HTTP/1.1";

const std::string rootDir = "DOCUMENT_ROOT";
const auto rootPath = std::filesystem::current_path().parent_path().append(rootDir);

//const std::string rootDir = std::filesystem::current_path().
//        string().substr(0,
//                        (std::filesystem::current_path().string().rfind('/'))) +
//                            "/DOCUMENT_ROOT";

const std::map<std::string, std::string> extensions = {
        {".html", "text/html"},
        {".css",  "text/css"},
        {".js",   "application/javascript"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png",  "image/png"},
        {".gif",  "image/gif"},
        {".swf",  "application/x-shockwave-flash"}
};

const std::map<short, std::string> codes = {
        {200, "OK"},
        {403, "Forbidden"},
        {404, "NotFound"},
        {405, "MethodNotAllowed"}
};

std::string getContentType(const std::string &fileName) {
    return extensions.at(fileName.substr(fileName.find(':')));
}

std::string getCodeMeaning(const short code) {
    return codes.at(code);
}

struct plainResponse {
    short responseCode;
    std::filesystem::path path;
    std::string data;
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
        headers << "Connection: close"
                << "Content type: " + response.path.extension().string();
    }

    std::string get() const {
        return protocol + " " +
               std::to_string(responseCode) + " " +
               getCodeMeaning(responseCode) + "\n" +
               headers.str() + "\n" +
               body;
    }
};

class Handler {
private:
    std::string method;
    std::string requestPath;

    bool checkMethod() {
        if (method != "GET" || method != "HEAD") {
            return false;
        } else {
            return true;
        }
    }

public:
    explicit Handler(const std::string &request) {
        method = request.substr(0, request.find(' '));
        requestPath = request.substr(method.length() + 1,
                                     request.rfind(' ') - (method.length() + 1));
        std::cout << method << "\n" << requestPath << std::endl;
    }

    ~Handler() = default;

    plainResponse handle() {
        if (!checkMethod()) {
            return {405, {}, {}};
        }
        auto path = rootPath;
        path.append(requestPath);

        if (!path.has_filename()) {
            path.replace_filename("index.html");
        }

        if (!std::filesystem::exists(path)) {
            return {404, {}, path};
        }

        if (std::filesystem::is_directory(path) ||
            path.string().find(rootDir) == std::string::npos) {
            return {400, {}, path};
        }

        std::ifstream input(path.string(), std::ios::binary);
        std::string str{std::istreambuf_iterator<char>(input),
                        std::istreambuf_iterator<char>()
        };

        return {200, str, path};
    }
};

const short MAX_LINE = 16384;
const char CHUNK_SIZE = 64;

char rot13_char(char c) {
    /* We don't want to use isalpha here; setting the locale would change
     * which characters are considered alphabetical. */
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

void readcb(struct bufferevent *bev, void *ctx) {

    char *getRequest = new char[CHUNK_SIZE];
    std::string request;

    while (bufferevent_read(bev, getRequest, CHUNK_SIZE)) {
        if (const auto pos = std::string(getRequest).find('\n'); pos != -1) {
            request += std::string(getRequest).substr(0, pos);
            break;
        }
        request += getRequest;
    }

    std::cout << std::endl << request << std::endl;
    Handler handler(request);
    Response response(handler.handle());

    const auto strResponse = response.get();

    bufferevent_write(bev, &strResponse, strResponse.length());
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

void writecb(struct bufferevent *bev) {
    struct evbuffer const *output = bufferevent_get_output(bev);


//    if (!evbuffer_get_length(output)) {
//        bufferevent_free(bev);
//    } else {
//        std::cout << "Error" << std::endl;
//    }

//    Response response(404);

//    std::string resp = response.get();
//    std::cout << "WRITE" << resp << std::endl;
//    bufferevent_write(bev, &resp, resp.length());
//    if (!evbuffer_get_length(output)) {
//        bufferevent_free(bev);
//    } else {
//        std::cerr << "bufferevent_free // writecb" << std::endl;
//    }
//    bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short event, void *arg) {
    auto *base = static_cast<event_base *>(arg);
    struct sockaddr_storage ss{};
    socklen_t socketLength = sizeof(ss);
    int fd = accept(listener, (struct sockaddr *) &ss, &socketLength);
    if (fd < 0) {
        std::cerr << "accept";
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        struct bufferevent *bev;
        evutil_make_socket_nonblocking(fd);
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, readcb, reinterpret_cast<bufferevent_data_cb>(writecb), errorcb, base);
        bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
        bufferevent_enable(bev, EV_READ | EV_WRITE);
    }
}

void run() {
    struct sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(8080);

    evutil_socket_t listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);

#ifndef WIN32
    {
        int one = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    }
#endif

    if (bind(listener, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        std::cerr << "bind";
        return;
    }

    if (listen(listener, 16) < 0) {
        std::cerr << "listen";
        return;
    }
    struct event_base *base = event_base_new();
    if (!base) {
        std::cerr << "base";
        return;
    }
//    event_self_cbarg()
    struct event *listener_event = event_new(base, listener, EV_READ | EV_WRITE | EV_PERSIST, do_accept, base);
    if (!listener_event) {
        return;
    }
    event_add(listener_event, nullptr);
    std::cout << "server is starting";
    event_base_dispatch(base);

    std::cout << "server is stopping";
    event_free(listener_event);
    event_base_free(base);
//    libevent_global_shutdown();
}