//
// Created by kid314 on 25-7-7.
//

#ifndef SOCKETRAII_H
#define SOCKETRAII_H

#include <unistd.h> // For close()

class socketRAII
{
public:
    explicit socketRAII(int new_fd = -1);
    ~socketRAII();

    socketRAII(const socketRAII&) = delete;
    socketRAII& operator=(const socketRAII&) = delete;

    socketRAII(socketRAII&& other) noexcept;
    socketRAII& operator=(socketRAII&& other) noexcept;

    [[nodiscard]] int get_fd() const;
    [[nodiscard]] bool is_alive() const;

    // Resets the socket, closing the old one if present
    void reset(int new_fd = -1);

private:
    int fd_; // Renamed to avoid conflict with the method name
};

#endif //SOCKETRAII_H