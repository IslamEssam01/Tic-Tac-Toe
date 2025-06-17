#include "performance_monitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#elif __linux__
#include <unistd.h>
#include <sys/resource.h>
#include <fstream>
#include <string>
#elif __APPLE__
#include <mach/mach.h>
#include <sys/resource.h>
#endif

PerformanceMonitor::PerformanceMonitor() 
    : initialMemoryUsage(0) {
    initialMemoryUsage = getCurrentMemoryUsage();
    // Warm up memory monitoring to get accurate readings
    for (int i = 0; i < 3; ++i) {
        getCurrentMemoryUsage();
        getCurrentCPUUsage();
    }
}

PerformanceMonitor::~PerformanceMonitor() {
    // Cleanup if needed
}

void PerformanceMonitor::startTest(const std::string& testName) {
    currentTestName = testName;
    startTime = std::chrono::high_resolution_clock::now();
    
    // Take multiple memory readings for accuracy
    double memSum = 0;
    for (int i = 0; i < 3; ++i) {
        memSum += getCurrentMemoryUsage();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    initialMemoryUsage = memSum / 3.0;
    
    logMetric("Starting test: " + testName);
}

PerformanceMetrics PerformanceMonitor::stopTest() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Take multiple memory readings for accuracy
    double memSum = 0;
    for (int i = 0; i < 3; ++i) {
        memSum += getCurrentMemoryUsage();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    double finalMemory = memSum / 3.0;
    
    PerformanceMetrics metrics;
    metrics.testName = currentTestName;
    metrics.responseTimeMs = duration.count() / 1000.0; // Convert to milliseconds
    metrics.memoryUsageMB = std::max(finalMemory, initialMemoryUsage); // Report peak memory
    metrics.cpuUsagePercent = getCurrentCPUUsage();
    metrics.success = true;
    
    results.push_back(metrics);
    
    logMetric("Completed test: " + currentTestName + 
              " (Time: " + std::to_string(metrics.responseTimeMs) + "ms, " +
              "Memory: " + std::to_string(metrics.memoryUsageMB) + "MB)");
    
    return metrics;
}

double PerformanceMonitor::getCurrentMemoryUsage() {
#ifdef _WIN32
    return getMemoryUsageWindows();
#elif __linux__
    return getMemoryUsageLinux();
#elif __APPLE__
    return getMemoryUsageMac();
#else
    return 0.0; // Fallback for unsupported platforms
#endif
}

double PerformanceMonitor::getCurrentCPUUsage() {
#ifdef _WIN32
    return getCPUUsageWindows();
#elif __linux__
    return getCPUUsageLinux();
#elif __APPLE__
    return getCPUUsageMac();
#else
    return 0.0; // Fallback for unsupported platforms
#endif
}

void PerformanceMonitor::addResult(const PerformanceMetrics& metrics) {
    results.push_back(metrics);
}

const std::vector<PerformanceMetrics>& PerformanceMonitor::getResults() const {
    return results;
}

void PerformanceMonitor::clearResults() {
    results.clear();
}

PerformanceMonitor::Summary PerformanceMonitor::generateSummary() const {
    Summary summary;
    
    if (results.empty()) {
        return summary;
    }
    
    summary.totalTests = results.size();
    summary.successfulTests = 0;
    summary.failedTests = 0;
    
    double totalResponseTime = 0;
    double totalMemoryUsage = 0;
    double totalCPUUsage = 0;
    
    summary.minResponseTime = results[0].responseTimeMs;
    summary.maxResponseTime = results[0].responseTimeMs;
    summary.maxMemoryUsage = results[0].memoryUsageMB;
    summary.maxCPUUsage = results[0].cpuUsagePercent;
    
    for (const auto& result : results) {
        if (result.success) {
            summary.successfulTests++;
        } else {
            summary.failedTests++;
        }
        
        totalResponseTime += result.responseTimeMs;
        totalMemoryUsage += result.memoryUsageMB;
        totalCPUUsage += result.cpuUsagePercent;
        
        summary.minResponseTime = std::min(summary.minResponseTime, result.responseTimeMs);
        summary.maxResponseTime = std::max(summary.maxResponseTime, result.responseTimeMs);
        summary.maxMemoryUsage = std::max(summary.maxMemoryUsage, result.memoryUsageMB);
        summary.maxCPUUsage = std::max(summary.maxCPUUsage, result.cpuUsagePercent);
    }
    
    summary.avgResponseTime = totalResponseTime / summary.totalTests;
    summary.avgMemoryUsage = totalMemoryUsage / summary.totalTests;
    summary.avgCPUUsage = totalCPUUsage / summary.totalTests;
    
    return summary;
}

void PerformanceMonitor::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for CSV export: " << filename << std::endl;
        return;
    }
    
    // Write header
    file << "Test Name,Response Time (ms),Memory Usage (MB),CPU Usage (%),Success,Error Message\n";
    
    // Write data
    for (const auto& result : results) {
        file << result.testName << ","
             << result.responseTimeMs << ","
             << result.memoryUsageMB << ","
             << result.cpuUsagePercent << ","
             << (result.success ? "true" : "false") << ","
             << result.errorMessage << "\n";
    }
    
    file.close();
}

void PerformanceMonitor::exportToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for JSON export: " << filename << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    file << "  \"results\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "    {\n";
        file << "      \"testName\": \"" << result.testName << "\",\n";
        file << "      \"responseTimeMs\": " << result.responseTimeMs << ",\n";
        file << "      \"memoryUsageMB\": " << result.memoryUsageMB << ",\n";
        file << "      \"cpuUsagePercent\": " << result.cpuUsagePercent << ",\n";
        file << "      \"success\": " << (result.success ? "true" : "false") << ",\n";
        file << "      \"errorMessage\": \"" << result.errorMessage << "\"\n";
        file << "    }";
        if (i < results.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
}

// Platform-specific implementations
#ifdef _WIN32
double PerformanceMonitor::getMemoryUsageWindows() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / (1024.0 * 1024.0); // Convert to MB
    }
    return 0.0;
}

double PerformanceMonitor::getCPUUsageWindows() {
    static PDH_HQUERY cpuQuery = nullptr;
    static PDH_HCOUNTER cpuTotal = nullptr;
    static bool initialized = false;
    
    if (!initialized) {
        PdhOpenQuery(nullptr, 0, &cpuQuery);
        PdhAddEnglishCounterA(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuTotal);
        PdhCollectQueryData(cpuQuery);
        initialized = true;
        return 0.0; // First call returns 0 as we need a baseline
    }
    
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, nullptr, &counterVal);
    return counterVal.doubleValue;
}
#endif

#ifdef __linux__
double PerformanceMonitor::getMemoryUsageLinux() {
    std::ifstream file("/proc/self/status");
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::istringstream iss(line.substr(6));
            double memory_kb;
            iss >> memory_kb;
            return memory_kb / 1024.0; // Convert to MB
        }
    }
    return 0.0;
}

double PerformanceMonitor::getCPUUsageLinux() {
    static long long lastTotalUser = 0, lastTotalUserLow = 0, lastTotalSys = 0, lastTotalIdle = 0;
    
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return 0.0;
    }
    
    std::string line;
    std::getline(file, line);
    
    std::istringstream iss(line);
    std::string cpu;
    long long totalUser, totalUserLow, totalSys, totalIdle;
    
    iss >> cpu >> totalUser >> totalUserLow >> totalSys >> totalIdle;
    
    if (lastTotalUser == 0) {
        lastTotalUser = totalUser;
        lastTotalUserLow = totalUserLow;
        lastTotalSys = totalSys;
        lastTotalIdle = totalIdle;
        return 0.0;
    }
    
    long long totalDiff = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) + (totalSys - lastTotalSys);
    long long idleDiff = totalIdle - lastTotalIdle;
    long long totalTime = totalDiff + idleDiff;
    
    double percent = 0.0;
    if (totalTime > 0) {
        percent = (double)totalDiff / (double)totalTime * 100.0;
    }
    
    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;
    
    return percent;
}
#endif

#ifdef __APPLE__
double PerformanceMonitor::getMemoryUsageMac() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss / (1024.0 * 1024.0); // Convert to MB
    }
    return 0.0;
}

double PerformanceMonitor::getCPUUsageMac() {
    // Simplified CPU usage for macOS - returns 0 for now
    // A more complex implementation would use host_processor_info
    return 0.0;
}
#endif

// Fallback implementations for unsupported platforms
#if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
double PerformanceMonitor::getMemoryUsageWindows() { return 0.0; }
double PerformanceMonitor::getMemoryUsageLinux() { return 0.0; }
double PerformanceMonitor::getMemoryUsageMac() { return 0.0; }
double PerformanceMonitor::getCPUUsageWindows() { return 0.0; }
double PerformanceMonitor::getCPUUsageLinux() { return 0.0; }
double PerformanceMonitor::getCPUUsageMac() { return 0.0; }
#endif

std::string PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void PerformanceMonitor::logMetric(const std::string& message) const {
    std::cout << "[" << getCurrentTimestamp() << "] " << message << std::endl;
}

// ScopedTimer implementation
ScopedTimer::ScopedTimer(PerformanceMonitor& monitor, const std::string& testName)
    : monitor_(monitor), completed_(false) {
    monitor_.startTest(testName);
}

ScopedTimer::~ScopedTimer() {
    if (!completed_) {
        auto metrics = monitor_.stopTest();
        if (!errorMessage_.empty()) {
            metrics.success = false;
            metrics.errorMessage = errorMessage_;
            // Update the result in the monitor
            auto& results = const_cast<std::vector<PerformanceMetrics>&>(monitor_.getResults());
            if (!results.empty()) {
                results.back() = metrics;
            }
        }
    }
}

void ScopedTimer::markFailed(const std::string& errorMessage) {
    errorMessage_ = errorMessage;
}