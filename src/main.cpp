#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include "threadpool/threadpool.h"
#include "epollreactor/epollreactor.h"
#include "httpconnection/httpconnection.h"
#include "socketRAII/socketRAII.h"
#include "router/router.h"

constexpr int PORT = 8080;
constexpr int MAX_EVENTS = 1024;

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) throw std::runtime_error("Failed to get flags");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set non-blocking");
    }
}

void setup_routes(Router& router) {
    router.add_route("/", [](const HttpRequest&, HttpResponse& res)
    {
        res.set_body("<html><body><h1>Welcome!</h1></body></html>");
        res.add_header("Content-Type", "text/html");
    });
    router.add_route("/hello", [](const HttpRequest&, HttpResponse& res)
    {
        res.set_body("<html><body><h1>Hello, World!</h1></body></html>");
        res.add_header("Content-Type", "text/html");
    });
}

int main() {
    try
    {
        socketRAII listen_sock(socket(AF_INET, SOCK_STREAM, 0));
        if (!listen_sock.is_alive()) throw std::runtime_error("Failed to create socket");

        int reuse = 1;
        if (setsockopt(listen_sock.get_fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        {
            throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);

        if (bind(listen_sock.get_fd(), (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        {
            throw std::runtime_error("Failed to bind");
        }

        if (listen(listen_sock.get_fd(), SOMAXCONN) < 0)
        {
            throw std::runtime_error("Failed to listen");
        }

        std::cout << "Server listening on port " << PORT << std::endl;

        ThreadPool pool(std::thread::hardware_concurrency());
        EpollReactor reactor(MAX_EVENTS);
        Router router;
        setup_routes(router);

        set_non_blocking(listen_sock.get_fd());
        reactor.add_fd(listen_sock.get_fd(), EPOLLIN | EPOLLET);

        std::map<int, HttpConnection> connections;

        while (true) {
            int n = reactor.wait(-1);
            for (int i = 0; i < n; ++i) {
                const auto& event = reactor.get_event(i);
                int fd = event.data.fd;

                if (fd == listen_sock.get_fd())
                {
                    while (true)
                    {
                        sockaddr_in client_addr{};
                        socklen_t client_len = sizeof(client_addr);
                        int conn_fd = accept(listen_sock.get_fd(), (struct sockaddr*)&client_addr, &client_len);
                        if (conn_fd > 0)
                        {
                            set_non_blocking(conn_fd);
                            connections[conn_fd].init(socketRAII(conn_fd), &reactor, &router);
                        }
                        else
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                            std::cerr << "accept failed" << std::endl;
                            break;
                        }
                    }
                }
                else
                {
                    HttpConnection* conn = &connections.at(fd);
                    pool.enqueue([conn, events = event.events]
                    {
                        conn->handle_events(events); 
                    });
                }
            }

            // Clean up closed connections
            for (auto it = connections.begin(); it != connections.end(); )
            {
                if (it->second.is_to_be_closed())
                {
                    it = connections.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
