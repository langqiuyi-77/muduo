#include "muduo/net/EventLoop.h"
 
 #include <iostream>
 #include <chrono>
 #include <cassert>
 
 using namespace muduo::net;
 
 // 测试用例：模拟任务风暴
 // 验证 queueInLoop() + doPendingFunctors() 的执行能力
 int main (int argc, char* argv[]) {
     EventLoop loop;
     std::atomic<int> count{0};
     
     auto start = std::chrono::high_resolution_clock::now();
     for (int i = 0; i < 1000000; ++i) {
         loop.queueInLoop([&count]{ ++count; });
     }
     loop.doPendingFunctors();  // 只测 functor 执行效率
     assert(count == 1000000);
     
     auto end = std::chrono::high_resolution_clock::now();
     std::cout << "Throughput: " 
               << 1000000 * 1e9 / 
                 static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count())
               << " ops/sec\n";
     return 0;
 }