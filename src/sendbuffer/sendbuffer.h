#ifndef SENDBUFFER_H
#define SENDBUFFER_H

#include <vector>
#include <string>
#include <string_view>

class SendBuffer {
public:
    void append(const char* data, size_t len);
    void append(const std::string& str);
    const char* data() const;
    size_t size() const;
    void consume(size_t bytes);
    bool empty() const;

private:
    std::vector<char> buffer_;
};

#endif //SENDBUFFER_H