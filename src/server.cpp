#include "common.h"


ssize_t read_full(int fd, std::string &out)
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
    out.resize(len, '\0');

    read_b = read_all(fd, out.data(), len);
    if (read_b <= 0) {
        throw_errno("read_all");
    }
    return read_b;
}

int main(void) 
{
    try {
        Socket listener(socket(AF_INET, SOCK_STREAM, 0));
        establish_con_server(listener);
            sockaddr_in cli {};
            socklen_t cli_len = sizeof(cli);

            
        while (true) {
            int conn_fd = accept(listener.get(), reinterpret_cast<sockaddr*>(&cli), &cli_len);
            if (conn_fd < 0) {
                if (errno == EINTR) continue;
                perror("accept");
                continue;
            }
            
            while (true) {
                std::string out;
                ssize_t read_b = read_full(conn_fd, out);
                if (!read_b) {
                    break; // connection closed
                }

                if (out.empty()) {
                    std::cout << "[handler] no data received (peer closed or timeout)\n";
                } else {
                    std::cout << out;
                }
            }

            
            close(conn_fd);
        }
        
    } catch (const std::system_error &e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
