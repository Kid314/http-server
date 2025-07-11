//
// Created by kid314 on 25-7-11.
//

#ifndef SOCKETRAII_H
#define SOCKETRAII_H

#include <unistd.h>

class SocketRAII
{
private:
    int fd;
public:
    explicit SocketRAII(int);
    ~SocketRAII();

    SocketRAII(const SocketRAII&)=delete;
    SocketRAII& operator=(const SocketRAII&)=delete;

    SocketRAII(SocketRAII&&) noexcept;
    SocketRAII& operator=(SocketRAII&&) noexcept;

    [[nodiscard]] inline int get_fd() const;
    [[nodiscard]] inline int release();
    inline void reset(int);
};



#endif //SOCKETRAII_H
