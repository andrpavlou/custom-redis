#include "common.h"



int main(void)
{
    try {
        Socket client_sock(socket(AF_INET, SOCK_STREAM, 0));
        establish_con_client(client_sock, "127.0.0.1");
        
        const std::string msg("Hello from client\n");
        send_all(client_sock.get(), msg);
        
        const std::string msg2("Hello from client2\n");
        send_all(client_sock.get(), msg2);

    } catch (const std::exception &e) {
        std::cerr << "Client error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
