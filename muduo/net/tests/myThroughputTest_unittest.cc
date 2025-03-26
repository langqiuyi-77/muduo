#include "muduo/net/EventLoop.h"

#include <iostream>
#include <chrono>
#include <cassert>
#include <thread>

using namespace muduo::net;

const int NUM_PRODUCERS = 4;
const int NUM_ITEMS = 1000000;

// 测试用例：模拟任务风暴
// 验证 queueInLoop() + doPendingFunctors() 的执行能力
int main (int argc, char* argv[]) {
    EventLoop loop;
    std::atomic<int> count{0};
    
    std::vector<std::thread> threads;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        threads.emplace_back([&loop, &count] { 
            for (int j = 0; j < NUM_ITEMS / NUM_PRODUCERS; ++j) {
                loop.queueInLoop([&count]{ ++count; });
            }
        });
    }

    std::thread consumerThread([&loop, &count] { 
        while (count != NUM_ITEMS) {
            loop.doPendingFunctors();  // 只测 functor 执行效率
        }
    });

    // 等待所有线程结束
    for (auto& t : threads) {
        t.join();  
    }
    consumerThread.join();
    
    assert(count == 1000000);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Throughput: " 
              << 1000000 * 1e9 / 
                static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count())
              << " ops/sec\n";
    return 0;
}
