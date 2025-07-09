#include "sendbuffer.h"
#include <algorithm>

void SendBuffer::append(const char* data, size_t len) {
    buffer_.insert(buffer_.end(), data, data + len);
}

void SendBuffer::append(const std::string& str) {
    append(str.data(), str.size());
}

const char* SendBuffer::data() const {
    return buffer_.data();
}

size_t SendBuffer::size() const {
    return buffer_.size();
}

void SendBuffer::consume(size_t bytes) {
    if (bytes >= buffer_.size()) {
        buffer_.clear();
    } else {
        buffer_.erase(buffer_.begin(), buffer_.begin() + bytes);
    }
}

bool SendBuffer::empty() const {
    return buffer_.empty();
}