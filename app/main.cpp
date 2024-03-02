#include <iostream>
#include <unistd.h>
#include <signal.h>
using namespace std;

void func(int num) {
    cout << "signal = " << num << ", 执行定时任务。" << endl;
    alarm(2);
}

int main(int argc, char *argv[]) {
    signal(14, func);
    alarm(2);
    while(1) {
        cout << "执行一次工作" << endl;
        sleep(1);
    }
}