#include "common.h"



int main(void)
{
    try {
        Socket client_sock(socket(AF_INET, SOCK_STREAM, 0));
        establish_con_client(client_sock, "127.0.0.1");
        
        std::string msg("0019Hello from client\n");
        uint32_t len = static_cast<uint32_t>(msg.size());
        std::cout << len;
        
        ssize_t read_b = write_all(client_sock.get(), msg.data(), msg.size());
        if (read_b < -1) {
            throw_errno("write_all");
        }

    } catch (const std::exception &e) {
        std::cerr << "Client error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
