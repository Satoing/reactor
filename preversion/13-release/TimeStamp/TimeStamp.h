#pragma once

#include <string>
#include <iostream>
#include <time.h>

class TimeStamp {
private:
    time_t secondsince_;

public:
    TimeStamp();
    TimeStamp(time_t secondsince);

    static TimeStamp now();

    int toint() const;
    std::string tostring() const;
};