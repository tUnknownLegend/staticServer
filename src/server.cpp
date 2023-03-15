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
#include <csignal>
#include <ctime>

#include "net.h"
#include "utils.h"
#include "handler.h"

std::string read(struct bufferevent *bev) {
    auto *getRequest = new char[CHUNK_SIZE];
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
        std::cerr << "Error bufferevent_free; \n";
    }
}

void doaccept(struct evconnlistener *listener, evutil_socket_t fd,
                       struct sockaddr *sa, int sockLength, void *arg) {
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
            bufferevent_setwatermark(bev, EV_READ, 10, MAX_LINE);
            bufferevent_enable(bev, EV_READ);
        } else {
            std::cerr << "do_accept+bev; ";
            event_base_loopbreak(base);
            return;
        }
    }
}

void eventCallback(evutil_socket_t sig, short events, void *user_data) {
    auto *callbackBase = static_cast<event_base *>(user_data);
    struct timeval delay = {1, 0};
    std::cerr << "exiting; \n";
    event_base_loopexit(callbackBase, &delay);
}

void runServer() {
    for (int i = 1; i < conf.processLimit; i++) {
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

    struct event *listener_event = event_new(base,
                                             SIGINT, EV_SIGNAL | EV_PERSIST, eventCallback,
                                             base);
    if (!listener_event) {
        std::cerr << "event_new; ";
        return;
    }

    struct sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(conf.port);

    struct evconnlistener *listener =
            evconnlistener_new_bind(base, doaccept, (void *) base,
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
}
