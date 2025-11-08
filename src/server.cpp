#include "common.h"



void handle_client(int client_fd, sockaddr_in client_addr) 
{
    try {        
        std::string line;
        read_line(client_fd, &line);

        if (line.empty()) {
            std::cout << "[handler] no data received (peer closed or timeout)\n";
        } else {
            std::cout << line;
        }

    } catch (const std::system_error &e) {
        std::cerr << "[handler] error: " << e.what() << "\n";
    }
    close(client_fd);
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
            handle_client(client_fd, cli);
        }
        
    } catch (const std::system_error &e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
