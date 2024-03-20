#pragma once

#include <iostream>
#include <string>

class Buffer {
private:
    std::string buf_;
public:
    Buffer();
    ~Buffer();

    void append(const char *data, int size);
    void appendwithhead(const char *data, int size);
    void erase(int pos, int len);
    int size();
    const char *data();
    void clear();
};