#include "Buffer.h"

Buffer::Buffer() {}

Buffer::~Buffer() {}

void Buffer::append(const char *data, int size) {
    buf_.append(data, size);
}

void Buffer::erase(int pos, int len) {
    buf_.erase(pos, len);
}

int Buffer::size() {
    return buf_.size();
}

const char* Buffer::data() {
    return buf_.data();
}

void Buffer::clear() {
    buf_.clear();
}