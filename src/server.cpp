#include "common.h"



void handle_client(int client_fd, sockaddr_in client_addr) 
{
    try {
        std::string ip(INET_ADDRSTRLEN, '\0');
        if (inet_ntop(AF_INET, &client_addr.sin_addr, ip.data(), ip.size()) == nullptr) {
            throw_errno("inet_ntop");
        }
        std::cout << "[handler] client " << ip << ":" << ntohs(client_addr.sin_port) << "\n";

        int val = 1;
        if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
            std::perror("[handler] setsockopt SO_RCVTIMEO");
        }

        std::string line;
        read_line(client_fd, &line);

        if (line.empty()) {
            std::cout << "[handler] no data received (peer closed or timeout)\n";
        } else {
            std::cout << "[handler] received: " << line;
        }

        send_all(client_fd, "Hello from server\n");
        std::cout << "[handler] reply sent\n";
    } catch (const std::system_error &e) {
        std::cerr << "[handler] error: " << e.what() << "\n";
    }
    close(client_fd);
    std::cout << "[handler] connection closed\n";
}

int main(void) 
{
    try {
        Socket listener(socket(AF_INET, SOCK_STREAM, 0));
        if (listener.get() < 0) throw_errno("socket");

        int yes = 1;
        if (setsockopt(listener.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            throw_errno("setsockopt");

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(listener.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
            throw_errno("bind");

        if (listen(listener.get(), SOMAXCONN) < 0)
            throw_errno("listen");

        std::cout << "Server listening on 0.0.0.0:1234\n";

        while (true) {
            sockaddr_in cli {};
            socklen_t cli_len = sizeof(cli);

            int client_fd = accept(listener.get(), reinterpret_cast<sockaddr*>(&cli), &cli_len);
            if (client_fd < 0) {
                if (errno == EINTR) continue;
                perror("accept");
                continue;
            }
            handle_client(client_fd, cli);
        }
    } catch (const std::system_error &e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
