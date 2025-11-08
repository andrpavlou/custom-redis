#include "common.h"



int main(void)
{
    try {
        Socket client_sock(socket(AF_INET, SOCK_STREAM, 0));
        if (client_sock.get() < 0) throw_errno("socket");

        sockaddr_in srv{};
        srv.sin_family = AF_INET;
        srv.sin_port = htons(1234);
        if (inet_pton(AF_INET, "127.0.0.1", &srv.sin_addr) != 1) {
            throw std::runtime_error("invalid address");
        }

        if (connect(client_sock.get(), reinterpret_cast<sockaddr*>(&srv), sizeof(srv)) < 0)
            throw_errno("connect");

        int yes = 1;
        if (setsockopt(client_sock.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            std::perror("setsockopt SO_RCVTIMEO (client)");
        
        
        const std::string msg("Hello from client\n");
        ssize_t w = send(client_sock.get(), msg.data(), msg.size(), 0);
        if (w < 0) throw_errno("send");
        std::cout << "Client sent " << w << " bytes\n";


        std::string line;
        read_line(client_sock.get(), &line);

        if (line.empty()) {
            std::cout << "[handler] no data received (peer closed or timeout)\n";
        } else {
            std::cout << "[handler] received: " << line;
        }

    } catch (const std::exception &e) {
        std::cerr << "Client error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
