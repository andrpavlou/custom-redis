#include "common.h"

void throw_errno(const char* what) 
{
    throw std::system_error(errno, std::generic_category(), what);
}


ssize_t read_all(int fd, void *out, size_t r_bytes)
{
    size_t total_read = 0;
    char *p = static_cast<char *>(out);

    while (total_read < r_bytes) {
        ssize_t n = read(fd, p + total_read, r_bytes - total_read);
        total_read += static_cast<size_t>(n);

        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) break;
    }
    return static_cast<ssize_t>(total_read);;
}



ssize_t write_all(int fd, const void* data, size_t total_size)
{
    size_t sent = 0;
    const char *p = static_cast<const char *>(data);
    while (sent < total_size) {
        ssize_t n = write(fd, p + sent, total_size - sent);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        sent += static_cast<size_t>(n);
    }
    return sent;
}

ssize_t query(int fd, const void* data)
{
    const char *p = static_cast<const char *>(data);

    uint32_t data_len       = strlen(p);
    uint32_t data_len_bo    = htonl(static_cast<uint32_t>(data_len));
    constexpr size_t LEN_BYTES = sizeof(data_len);
    
    ssize_t sent = write_all(fd, &data_len_bo, LEN_BYTES);
    if (sent != static_cast<ssize_t>(LEN_BYTES)) {
        throw_errno("Query bad data len write");
    }

    sent = write_all(fd, p, data_len);
    if (sent !=static_cast<ssize_t>(data_len)) {
        throw_errno("Query bad data len write");
    }

    return sent;
}


void establish_con_client(const Socket& client_sock, std::string_view ip)
{
    if (client_sock.get() < 0) throw_errno("socket");

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = ntohs(1234);
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
        addr.sin_port = ntohs(1234);
        addr.sin_addr.s_addr = ntohl(INADDR_ANY);

        if (bind(listener.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
            throw_errno("bind");

        if (listen(listener.get(), SOMAXCONN) < 0)
            throw_errno("listen");

        std::cout << "Server listening on 0.0.0.0:1234\n";

}

ssize_t read_full(int fd, void *out)
{
    static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
    constexpr size_t LEN_BYTES = sizeof(uint32_t);

    uint32_t net_len = 0;
    ssize_t read_b = read_all(fd, &net_len, LEN_BYTES);

    if (!read_b) {
        return read_b;
    }

    if (read_b != static_cast<ssize_t>(LEN_BYTES)) {
        throw_errno("read_all");
    }

    uint32_t len = ntohl(net_len);
    if (len > MAX_MSG) {
        return -1;
    }
    
    char *p = static_cast<char *>(out);

    read_b = read_all(fd, p, len);
    if (read_b <= 0) {
        throw_errno("read_all");
    }
    return read_b;
}