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
        ssize_t n = ::recv(fd, buf.data(), sizeof(buf), 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            throw_errno("recv");
        }
        if (n == 0) break;
        out->append(buf.data(), static_cast<size_t>(n));
        if (out->find('\n') != std::string::npos) break;
    }
}

void send_all(int fd, std::string_view data) 
{
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = send(fd, data.data() + sent, data.size() - sent, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            throw_errno("send");
        }
        sent += static_cast<size_t>(n);
    }
}