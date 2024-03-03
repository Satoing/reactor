#include <thread>
#include <iostream>
#include <string>
#include <queue>
#include <condition_variable>
#include <mutex>
using namespace std;

class PC {
    mutex m_mutex;
    condition_variable m_cond;
    queue<int> m_que;
public:
    // 生产者向队列中放入num个数
    void producer(int num) {
        while(true) {
            {lock_guard<mutex> lock(m_mutex);
            for(int i = 0; i < num; i++) {
                static int ii = 0;
                m_que.push(ii++);
            }}
            this_thread::sleep_for(chrono::seconds(2));
            m_cond.notify_all();
        }
    }
    void consumer() {
        while(true) {
            unique_lock<mutex> lock(m_mutex);
            while(m_que.empty()) {
                m_cond.wait(lock);
            }

            int product = m_que.front();
            m_que.pop();
            lock.unlock(); // 尽早释放锁

            cout << "线程" << this_thread::get_id() 
                 << "获取到数据: " << product << endl;
            // 模拟消费数据
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

int main() {
    PC pc;
    // 创建三个消费者线程
    thread t1(&PC::consumer, &pc);
    thread t2(&PC::consumer, &pc);
    thread t3(&PC::consumer, &pc);
    // 创建一个生产者线程
    thread t4(&PC::producer, &pc, 2);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}