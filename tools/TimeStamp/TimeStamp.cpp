#include "TimeStamp.h"

TimeStamp::TimeStamp(): secondsince_(time(0)){}

TimeStamp::TimeStamp(time_t secondsince): secondsince_(secondsince) {}

TimeStamp TimeStamp::now() {
    return TimeStamp();
}

int TimeStamp::toint() const {
    return secondsince_;
}

std::string TimeStamp::tostring() const {
    char buf[32] = {0};
    tm *tm_time = localtime(&secondsince_);
    snprintf(buf, 32, "%4d-%02d-%02d %02d:%02d:%02d",
            tm_time->tm_year+1900, tm_time->tm_mon+1, 
            tm_time->tm_mday, tm_time->tm_hour,
            tm_time->tm_min, tm_time->tm_sec);
    return buf;
}

// int main() {
//     TimeStamp ts = TimeStamp::now();
//     std::cout << ts.toint() << std::endl;
//     std::cout << ts.tostring() << std::endl;
// }