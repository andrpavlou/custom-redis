#include "common.h"




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
                char out[MAX_MSG + 1];
                ssize_t read_b = read_full(conn_fd, out);
                out[read_b] = '\0';
                
                if (!read_b) {
                    break; // connection closed
                } 

                if (read_b < 0) {
                    throw_errno("read_full");
                }
            

                std::cout << out;
                
            }

            
            close(conn_fd);
        }
        
    } catch (const std::system_error &e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
