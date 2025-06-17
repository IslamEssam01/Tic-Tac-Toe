#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <chrono>
#include <string>
#include <vector>
#include <memory>

struct PerformanceMetrics {
    std::string testName;
    double responseTimeMs;
    double memoryUsageMB;
    double cpuUsagePercent;
    bool success;
    std::string errorMessage;
    
    PerformanceMetrics() : responseTimeMs(0), memoryUsageMB(0), cpuUsagePercent(0), success(true) {}
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring a test
    void startTest(const std::string& testName);
    
    // Stop monitoring and record results
    PerformanceMetrics stopTest();
    
    // Get current memory usage in MB
    double getCurrentMemoryUsage();
    
    // Get current CPU usage percentage
    double getCurrentCPUUsage();
    
    // Add a completed test result
    void addResult(const PerformanceMetrics& metrics);
    
    // Get all test results
    const std::vector<PerformanceMetrics>& getResults() const;
    
    // Clear all results
    void clearResults();
    
    // Generate summary statistics
    struct Summary {
        double avgResponseTime;
        double maxResponseTime;
        double minResponseTime;
        double avgMemoryUsage;
        double maxMemoryUsage;
        double avgCPUUsage;
        double maxCPUUsage;
        int totalTests;
        int successfulTests;
        int failedTests;
    };
    
    Summary generateSummary() const;
    
    // Export results to CSV
    void exportToCSV(const std::string& filename) const;
    
    // Export results to JSON
    void exportToJSON(const std::string& filename) const;

private:
    std::string currentTestName;
    std::chrono::high_resolution_clock::time_point startTime;
    double initialMemoryUsage;
    std::vector<PerformanceMetrics> results;
    
    // Platform-specific memory usage methods
    double getMemoryUsageWindows();
    double getMemoryUsageLinux();
    double getMemoryUsageMac();
    
    // Platform-specific CPU usage methods
    double getCPUUsageWindows();
    double getCPUUsageLinux();
    double getCPUUsageMac();
    
    // Helper methods
    std::string getCurrentTimestamp() const;
    void logMetric(const std::string& message) const;
};

// Utility class for automatic timing
class ScopedTimer {
public:
    ScopedTimer(PerformanceMonitor& monitor, const std::string& testName);
    ~ScopedTimer();
    
    // Mark test as failed
    void markFailed(const std::string& errorMessage);

private:
    PerformanceMonitor& monitor_;
    bool completed_;
    std::string errorMessage_;
};

// Macro for easy timing
#define PERFORMANCE_TEST(monitor, name) ScopedTimer timer(monitor, name)

#endif // PERFORMANCE_MONITOR_H