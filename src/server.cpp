/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>

#include <coroutine>

#define MAX_LINE 16384

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
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);
    size_t n;
    char *line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF);
    while (line) {
        for (unsigned int i = 0; i < n; ++i)
            line[i] = rot13_char(line[i]);
        evbuffer_add(output, line, n);
        evbuffer_add(output, "\n", 1);
        free(line);
        line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF);
    }

    if (evbuffer_get_length(input) >= MAX_LINE) {
        /* Too long; just process what there is and go on so that the buffer
         * doesn't grow infinitely long. */
        std::string buf;
        buf.resize(1024);
        while (evbuffer_get_length(input)) {
            size_t n = evbuffer_remove(input, &buf, buf.size());
            for (unsigned int i = 0; i < n; ++i)
                buf[i] = rot13_char(buf[i]);
            evbuffer_add(output, &buf, n);
        }
        evbuffer_add(output, "\n", 1);
    }
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

void write_end_callback(struct bufferevent *bev) {
    struct evbuffer const *output = bufferevent_get_output(bev);
    if (!evbuffer_get_length(output)) {
        bufferevent_free(bev);
    } else {
        std::cout << "Error" << std::endl;
    }
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
        bufferevent_setcb(bev, readcb, nullptr, errorcb, base);
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
    struct event *listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, base);
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