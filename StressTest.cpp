#include "pch.h"
#include "StressTest.h"
#include "IpcClient.h"
#include <thread>
#include <vector>
#include <iostream>

StressTest::StressTest(const TestConfig& config)
    : config_(config) {
}

bool StressTest::makeIpcRequest(int seq, int opt1, int opt2, int opt3) {
    IpcClient client;
    return client.SendOptions(opt1, opt2, opt3, seq);
}

void StressTest::workerThread(
    int threadId,
    std::atomic<int>& successCount,
    std::atomic<int>& errorCount
) {
    for (int i = 0; i < config_.requestsPerThread; ++i) {
        int seq = threadId * 1000 + i;
        int opt1 = (i % 2);
        int opt2 = ((i / 2) % 2);
        int opt3 = ((i / 3) % 2);

        bool success = makeIpcRequest(seq, opt1, opt2, opt3);

        if (success) {
            successCount++;
        }
        else {
            errorCount++;
        }
    }
}

StressTest::TestResult StressTest::runConcurrentTest() {
    std::cout << "\n========== IPC 동시 요청 테스트 시작 ==========\n";
    std::cout << "스레드 수: " << config_.threadCount << "\n";
    std::cout << "스레드당 요청: " << config_.requestsPerThread << "\n";
    std::cout << "총 요청 수: " << (config_.threadCount * config_.requestsPerThread) << "\n";
    std::cout << "=============================================\n\n";

    TestResult result;
    result.totalRequests = config_.threadCount * config_.requestsPerThread;

    std::atomic<int> successCount{ 0 };
    std::atomic<int> errorCount{ 0 };

    std::vector<std::thread> threads;
    threads.reserve(config_.threadCount);

    for (int i = 0; i < config_.threadCount; ++i) {
        threads.emplace_back(
            &StressTest::workerThread,
            this,
            i,
            std::ref(successCount),
            std::ref(errorCount)
        );
    }

    for (auto& t : threads) {
        t.join();
    }

    result.successCount = successCount.load();
    result.errorCount = errorCount.load();

    return result;
}

void StressTest::TestResult::print() const {
    std::cout << "\n========== 테스트 결과 ==========\n";
    std::cout << "총 요청 수: " << totalRequests << "\n";
    std::cout << "성공: " << successCount << "\n";
    std::cout << "실패: " << errorCount << "\n";
    std::cout << "에러율: " << (totalRequests > 0 ? (errorCount * 100.0 / totalRequests) : 0) << "%\n";
    std::cout << "===============================\n";
}