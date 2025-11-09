#include "common.h"

#define MSG_LEN_SIZE    4
#define MAX_MSG         128

void read_full(int fd, std::string &out)
{
    std::string len_str(MSG_LEN_SIZE, '\0');

    ssize_t read_b = read_all(fd, len_str.data(), MSG_LEN_SIZE);
    if (read_b < 0) throw_errno("read_all");

    uint32_t len;
    memcpy(&len, len_str.data(), MSG_LEN_SIZE);
    len = ntohl(len);


    out.resize(MAX_MSG, '\0');
    read_b = read_all(fd, out.data(), len);
    if (read_b <= 0) throw_errno("read_all");
}

int main(void) 
{
    try {
        Socket listener(socket(AF_INET, SOCK_STREAM, 0));
        establish_con_server(listener);

        while (true) {
            sockaddr_in cli {};
            socklen_t cli_len = sizeof(cli);

            int client_fd = accept(listener.get(), reinterpret_cast<sockaddr*>(&cli), &cli_len);
            if (client_fd < 0) {
                if (errno == EINTR) continue;
                perror("accept");
                continue;
            }
            
            std::string out;
            read_full(client_fd, out);

            if (out.empty()) {
                std::cout << "[handler] no data received (peer closed or timeout)\n";
            } else {
                std::cout << out;
            }
            close(client_fd);
        }
        
    } catch (const std::system_error &e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
