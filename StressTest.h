#ifndef STRESS_TEST_H
#define STRESS_TEST_H

#include <atomic>

class StressTest {
public:
    struct TestConfig {
        int threadCount;        // 동시 스레드 수
        int requestsPerThread;  // 각 스레드당 요청 수

        TestConfig()
            : threadCount(10)
            , requestsPerThread(20) {
        }
    };

    struct TestResult {
        int totalRequests;
        int successCount;
        int errorCount;

        void print() const;
    };

    StressTest(const TestConfig& config);
    ~StressTest() = default;

    // 동시 IPC 요청 테스트 실행
    TestResult runConcurrentTest();

private:
    TestConfig config_;

    // 단일 IPC 요청 수행
    bool makeIpcRequest(int seq, int opt1, int opt2, int opt3);

    // 각 스레드가 실행할 작업
    void workerThread(
        int threadId,
        std::atomic<int>& successCount,
        std::atomic<int>& errorCount
    );
};

#endif // STRESS_TEST_H