#include "common.h"

void throw_errno(const char* what) 
{
    throw std::system_error(errno, std::generic_category(), what);
}


void read_line(int fd, std::string *out) 
{
    out->reserve(128);
    std::string buf(128, '\0');

    while (true) {
        ssize_t n = read(fd, buf.data(), buf.size());
        if (n < 0) {
            if (errno == EINTR) continue;
            throw_errno("recv");
        }
        if (n == 0) break;
        out->append(buf.data(), static_cast<size_t>(n));
        if (out->find('\n') != std::string::npos) break;
    }
}

void send_all(int fd, std::string data) 
{
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = write(fd, data.data() + sent, data.size() - sent);
        if (n < 0) {
            if (errno == EINTR) continue;
            throw_errno("send");
        }
        sent += static_cast<size_t>(n);
    }
}

void establish_con_client(const Socket& client_sock, std::string_view ip)
{
    if (client_sock.get() < 0) throw_errno("socket");

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(1234);
    srv.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);

    if (connect(client_sock.get(), reinterpret_cast<sockaddr*>(&srv), sizeof(srv)) < 0)
        throw_errno("connect");
}

void establish_con_server(const Socket& listener) 
{
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

}