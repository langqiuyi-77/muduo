#include "muduo/net/EventLoop.h"

#include <iostream>
#include <chrono>
#include <cassert>
#include <thread>
#include <algorithm>
#include <numeric>

using namespace muduo::net;

const int NUM_PRODUCERS = 4;
const int NUM_ITEMS = 1000000;

// 测试用例：模拟任务风暴
// 验证 queueInLoop() + doPendingFunctors() 的执行能力
int main (int argc, char* argv[]) {
    EventLoop loop;
    std::atomic<int> count{0};
    
    std::vector<std::thread> threads;
    std::vector<long long> delay_samples;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        threads.emplace_back([&loop, &count, &delay_samples] { 
            for (int j = 0; j < NUM_ITEMS / NUM_PRODUCERS; ++ j) {
                auto now = std::chrono::high_resolution_clock::now();
                loop.queueInLoop([&count, now, &delay_samples]{ 
                    // 收集 delay 统计信息，比如记录最大、平均
                    auto delay = std::chrono::high_resolution_clock::now() - now;
                    delay_samples.push_back(delay.count());
                    ++count; 
                });
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

    //计算平均和 P99 延迟
    std::sort(delay_samples.begin(), delay_samples.end());
    long long total = std::accumulate(delay_samples.begin(), delay_samples.end(), 0LL);
    long long avg = total / delay_samples.size();
    long long p99 = delay_samples[delay_samples.size() * 99 / 100];

    std::cout << "Average delay: " << avg << " ns\n";
    std::cout << "P99 delay: " << p99 << " ns\n";

    return 0;
}