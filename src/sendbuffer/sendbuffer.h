#ifndef SENDBUFFER_H
#define SENDBUFFER_H

#include <vector>
#include <string>
#include <string_view>

class SendBuffer
{
public:
    void consume(size_t bytes);
    void append(const char* data, size_t len);
    void append(const std::string& str);
    [[nodiscard]] const char* data() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool empty() const;
private:
    std::vector<char> buffer_;
};

#endif //SENDBUFFER_H