#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <unistd.h>

class Socket {
public:
    explicit Socket(int fd = -1) noexcept : fd_(fd) {}
    ~Socket() { if (fd_ >= 0) close(fd_); }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket& operator=(Socket&& o) noexcept {
        if (this != &o) {
            if (fd_ >= 0) close(fd_);
            fd_ = o.fd_;
            o.fd_ = -1;
        }
        return *this;
    }
    int get() const noexcept { return fd_; }
    
    int release() noexcept 
    { 
        int t = fd_; 
        fd_ = -1; 
        return t;
    }

    void reset(int fd = -1) noexcept 
    {
        if (fd >= 0) close(fd_); 
        fd_ = fd;
    }
    
private:
    int fd_;
};


void throw_errno(const char* what);
ssize_t read_all(int fd, void *out, size_t r_bytes);
ssize_t write_all(int fd, void* data, size_t total_size);
void establish_con_client(const Socket& client_sock, std::string_view ip);
void establish_con_server(const Socket& listener);