#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <iostream>

class HTMLReportGenerator {
private:
    static std::string generateChartDataJSON(const QJsonArray& results) {
        QJsonObject chartData;
        QJsonArray categories, responseTimes, memoryUsage, cpuUsage;
        QJsonArray performanceLabels, performanceCounts;
        QJsonArray radarData;
        
        // Group results by category
        QMap<QString, QList<double>> categoryResponseTimes;
        QMap<QString, QList<double>> categoryMemoryUsage;
        
        int excellentCount = 0, goodCount = 0, fairCount = 0, poorCount = 0;
        double totalResponseTime = 0, totalMemory = 0, totalCPU = 0;
        int validResults = 0;
        
        for (const auto& value : results) {
            QJsonObject test = value.toObject();
            QString testName = test["testName"].toString();
            double responseTime = test["responseTimeMs"].toDouble();
            double memory = test["memoryUsageMB"].toDouble();
            double cpu = test["cpuUsagePercent"].toDouble();
            bool success = test["success"].toBool();
            
            if (!success) continue;
            validResults++;
            
            // Extract category from test name
            QString category = testName.split("_")[0];
            if (category.isEmpty()) category = "Other";
            
            categoryResponseTimes[category].append(responseTime);
            categoryMemoryUsage[category].append(memory);
            
            totalResponseTime += responseTime;
            totalMemory += memory;
            totalCPU += cpu;
            
            // Count performance levels
            if (responseTime < 100) excellentCount++;
            else if (responseTime < 500) goodCount++;
            else if (responseTime < 1000) fairCount++;
            else poorCount++;
        }
        
        // Generate category averages
        for (auto it = categoryResponseTimes.begin(); it != categoryResponseTimes.end(); ++it) {
            categories.append(it.key());
            
            double avgResponse = 0;
            for (double time : it.value()) avgResponse += time;
            avgResponse /= it.value().size();
            responseTimes.append(avgResponse);
            
            double avgMemory = 0;
            const auto& memList = categoryMemoryUsage[it.key()];
            for (double mem : memList) avgMemory += mem;
            avgMemory /= memList.size();
            memoryUsage.append(avgMemory);
        }
        
        // Performance distribution
        performanceLabels.append("Excellent");
        performanceLabels.append("Good");
        performanceLabels.append("Fair");
        performanceLabels.append("Poor");
        
        performanceCounts.append(excellentCount);
        performanceCounts.append(goodCount);
        performanceCounts.append(fairCount);
        performanceCounts.append(poorCount);
        
        // Radar chart data (normalized to 0-100)
        double avgResponseTime = validResults > 0 ? totalResponseTime / validResults : 0;
        double avgMemory = validResults > 0 ? totalMemory / validResults : 0;
        double avgCPU = validResults > 0 ? totalCPU / validResults : 0;
        
        radarData.append(std::max(0.0, 100.0 - (avgResponseTime / 10.0))); // Response Time
        radarData.append(std::max(0.0, 100.0 - avgMemory)); // Memory Efficiency
        radarData.append(std::max(0.0, 100.0 - avgCPU)); // CPU Efficiency
        radarData.append(validResults > 0 ? (double)excellentCount / validResults * 100 : 0); // Stability
        radarData.append(validResults > 0 ? std::min(100.0, validResults * 2.0) : 0); // Throughput
        
        chartData["categories"] = categories;
        chartData["responseTimes"] = responseTimes;
        chartData["memoryUsage"] = memoryUsage;
        chartData["performanceLabels"] = performanceLabels;
        chartData["performanceCounts"] = performanceCounts;
        chartData["radarData"] = radarData;
        
        QJsonDocument doc(chartData);
        return doc.toJson(QJsonDocument::Compact).toStdString();
    }

public:
    static bool generateReport(const QString& jsonFile, const QString& outputFile) {
        QFile file(jsonFile);
        if (!file.open(QIODevice::ReadOnly)) {
            std::cerr << "Could not open JSON file: " << jsonFile.toStdString() << std::endl;
            return false;
        }
        
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            std::cerr << "Invalid JSON format" << std::endl;
            return false;
        }
        
        QJsonObject root = doc.object();
        QJsonArray results = root["results"].toArray();
        QString timestamp = root["timestamp"].toString();
        
        // Generate HTML content
        QString html = generateHTML(results, timestamp);
        
        // Write to output file
        QFile outFile(outputFile);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            std::cerr << "Could not create output file: " << outputFile.toStdString() << std::endl;
            return false;
        }
        
        QTextStream out(&outFile);
        out << html;
        outFile.close();
        
        return true;
    }

private:
    static QString generateHTML(const QJsonArray& results, const QString& timestamp) {
        QString html = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tic-Tac-Toe Performance Report</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
            color: #333;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            overflow: hidden;
        }
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        .header h1 {
            margin: 0;
            font-size: 2.5em;
            font-weight: 300;
        }
        .header .subtitle {
            margin-top: 10px;
            opacity: 0.9;
            font-size: 1.2em;
            font-weight: 300;
        }
        .header .timestamp {
            margin-top: 5px;
            opacity: 0.8;
            font-size: 1.0em;
        }
        .summary {
            padding: 30px;
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
        }
        .summary-card {
            background: #f8f9fa;
            border-radius: 8px;
            padding: 20px;
            text-align: center;
            border-left: 4px solid #667eea;
        }
        .summary-card h3 {
            margin: 0 0 10px 0;
            color: #667eea;
            font-size: 1.2em;
        }
        .summary-card .value {
            font-size: 2em;
            font-weight: bold;
            color: #333;
        }
        .summary-card .unit {
            font-size: 0.9em;
            color: #666;
            margin-left: 5px;
        }
        .summary-card .description {
            font-size: 0.85em;
            color: #777;
            margin-top: 5px;
            line-height: 1.3;
        }
        .charts-section {
            padding: 30px;
            border-top: 1px solid #eee;
        }
        .charts-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
            gap: 30px;
            margin-top: 20px;
        }
        .chart-container {
            background: #f8f9fa;
            border-radius: 8px;
            padding: 20px;
        }
        .chart-container h3 {
            margin: 0 0 20px 0;
            color: #333;
            text-align: center;
        }
        .chart-canvas {
            width: 100% !important;
            height: 300px !important;
        }
        .table-section {
            padding: 30px;
            border-top: 1px solid #eee;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
            background: white;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 1px 3px rgba(0,0,0,0.1);
        }
        th, td {
            padding: 12px 15px;
            text-align: left;
            border-bottom: 1px solid #eee;
        }
        th {
            background: #f8f9fa;
            font-weight: 600;
            color: #333;
            text-transform: uppercase;
            font-size: 0.9em;
            letter-spacing: 0.5px;
        }
        tr:hover {
            background: #f8f9fa;
        }
        .status-pass {
            color: #28a745;
            font-weight: bold;
        }
        .status-fail {
            color: #dc3545;
            font-weight: bold;
        }
        .metric-good {
            color: #28a745;
        }
        .metric-warning {
            color: #ffc107;
        }
        .metric-poor {
            color: #dc3545;
        }
        .section-title {
            font-size: 1.8em;
            color: #333;
            margin-bottom: 10px;
            border-bottom: 2px solid #667eea;
            padding-bottom: 10px;
        }
        .chart-placeholder {
            height: 300px;
            background: linear-gradient(45deg, #f0f0f0 25%, transparent 25%), 
                        linear-gradient(-45deg, #f0f0f0 25%, transparent 25%), 
                        linear-gradient(45deg, transparent 75%, #f0f0f0 75%), 
                        linear-gradient(-45deg, transparent 75%, #f0f0f0 75%);
            background-size: 20px 20px;
            background-position: 0 0, 0 10px, 10px -10px, -10px 0px;
            border-radius: 4px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: #666;
            font-style: italic;
        }
        .performance-indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 8px;
        }
        .perf-excellent { background-color: #28a745; }
        .perf-good { background-color: #6f42c1; }
        .perf-fair { background-color: #ffc107; }
        .perf-poor { background-color: #dc3545; }
        
        @media (max-width: 768px) {
            .summary {
                grid-template-columns: 1fr;
            }
            .charts-grid {
                grid-template-columns: 1fr;
            }
            table {
                font-size: 0.9em;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 Tic-Tac-Toe Performance Dashboard</h1>
            <div class="subtitle">Real-time performance metrics and analysis</div>
            <div class="timestamp">Generated on: )" + timestamp + R"(</div>
        </div>
)";

        // Calculate summary statistics
        double totalResponseTime = 0, totalMemory = 0, totalCPU = 0;
        double minTime = 999999, maxTime = 0, maxMemory = 0, maxCPU = 0;
        int totalTests = results.size();
        int passedTests = 0, failedTests = 0;
        
        for (const auto& value : results) {
            QJsonObject test = value.toObject();
            double responseTime = test["responseTimeMs"].toDouble();
            double memory = test["memoryUsageMB"].toDouble();
            double cpu = test["cpuUsagePercent"].toDouble();
            bool success = test["success"].toBool();
            
            totalResponseTime += responseTime;
            totalMemory += memory;
            totalCPU += cpu;
            
            minTime = std::min(minTime, responseTime);
            maxTime = std::max(maxTime, responseTime);
            maxMemory = std::max(maxMemory, memory);
            maxCPU = std::max(maxCPU, cpu);
            
            if (success) passedTests++;
            else failedTests++;
        }
        
        double avgTime = totalTests > 0 ? totalResponseTime / totalTests : 0;
        double avgMemory = totalTests > 0 ? totalMemory / totalTests : 0;
        double avgCPU = totalTests > 0 ? totalCPU / totalTests : 0;
        
        // Summary section
        html += R"(
        <div class="summary">
            <div class="summary-card">
                <h3>Operations Tested</h3>
                <div class="value">)" + QString::number(totalTests) + R"(</div>
                <div class="description">Performance measurements across all components</div>
            </div>
            <div class="summary-card">
                <h3>Avg Response Time</h3>
                <div class="value">)" + QString::number(avgTime, 'f', 2) + R"(<span class="unit">ms</span></div>
                <div class="description">Average time for all operations</div>
            </div>
            <div class="summary-card">
                <h3>Peak Memory Usage</h3>
                <div class="value">)" + QString::number(maxMemory, 'f', 1) + R"(<span class="unit">MB</span></div>
                <div class="description">Maximum memory consumption</div>
            </div>
            <div class="summary-card">
                <h3>Peak CPU Usage</h3>
                <div class="value">)" + QString::number(maxCPU, 'f', 1) + R"(<span class="unit">%</span></div>
                <div class="description">Maximum processor utilization</div>
            </div>
        </div>
)";

        // Charts section with real data
        html += R"(
        <div class="charts-section">
            <h2 class="section-title">📊 Performance Metrics</h2>
            <div class="charts-grid">
                <div class="chart-container">
                    <h3>Response Time Distribution</h3>
                    <canvas id="responseTimeChart" class="chart-canvas"></canvas>
                </div>
                <div class="chart-container">
                    <h3>Memory Usage Overview</h3>
                    <canvas id="memoryChart" class="chart-canvas"></canvas>
                </div>
            </div>
        </div>
)";

        // Detailed results table
        html += R"(
        <div class="table-section">
            <h2 class="section-title">📋 Detailed Performance Metrics</h2>
            <table>
                <thead>
                    <tr>
                        <th>Operation</th>
                        <th>Response Time</th>
                        <th>Memory Usage</th>
                        <th>CPU Usage</th>
                        <th>Performance</th>
                    </tr>
                </thead>
                <tbody>
)";

        // Add table rows
        for (const auto& value : results) {
            QJsonObject test = value.toObject();
            QString testName = test["testName"].toString();
            double responseTime = test["responseTimeMs"].toDouble();
            double memory = test["memoryUsageMB"].toDouble();
            double cpu = test["cpuUsagePercent"].toDouble();
            bool success = test["success"].toBool();
            QString errorMsg = test["errorMessage"].toString();
            
            // Determine performance indicators
            QString timeClass = responseTime < 100 ? "perf-excellent" : 
                               responseTime < 500 ? "perf-good" : 
                               responseTime < 1000 ? "perf-fair" : "perf-poor";
            
            QString memClass = memory < 50 ? "perf-excellent" : 
                              memory < 100 ? "perf-good" : 
                              memory < 200 ? "perf-fair" : "perf-poor";
            
            QString cpuClass = cpu < 25 ? "perf-excellent" : 
                              cpu < 50 ? "perf-good" : 
                              cpu < 75 ? "perf-fair" : "perf-poor";
            
            html += R"(
                    <tr>
                        <td>)" + testName + R"(</td>
                        <td>
                            <span class="performance-indicator )" + timeClass + R"("></span>
                            )" + QString::number(responseTime, 'f', 2) + R"( ms
                        </td>
                        <td>
                            <span class="performance-indicator )" + memClass + R"("></span>
                            )" + QString::number(memory, 'f', 1) + R"( MB
                        </td>
                        <td>
                            <span class="performance-indicator )" + cpuClass + R"("></span>
                            )" + QString::number(cpu, 'f', 1) + R"( %
                        </td>
                        <td class=")" + (success ? "status-pass" : "status-fail") + R"(">
                            )" + (success ? (responseTime < 100 ? "EXCELLENT" : responseTime < 500 ? "GOOD" : "SLOW") : "ERROR") + R"(
                        </td>
                    </tr>
)";
        }
        
        html += R"(
                </tbody>
            </table>
        </div>

        <div style="padding: 30px; border-top: 1px solid #eee; text-align: center; color: #666;">
            <p>Performance Indicators:</p>
            <span class="performance-indicator perf-excellent"></span> Excellent (&lt;100ms)
            <span class="performance-indicator perf-good" style="margin-left: 15px;"></span> Good (100-500ms)
            <span class="performance-indicator perf-fair" style="margin-left: 15px;"></span> Fair (500-1000ms)
            <span class="performance-indicator perf-poor" style="margin-left: 15px;"></span> Poor (&gt;1000ms)
            <br><br>
            <small>Generated by Tic-Tac-Toe Performance Test Suite</small>
        </div>
    </div>

    <script>
        // Parse the results data from JSON
        const resultsData = )" + QString::fromStdString(generateChartDataJSON(results)) + R"(;
        
        // Response Time Chart
        const responseCtx = document.getElementById('responseTimeChart').getContext('2d');
        new Chart(responseCtx, {
            type: 'bar',
            data: {
                labels: resultsData.categories,
                datasets: [{
                    label: 'Response Time (ms)',
                    data: resultsData.responseTimes,
                    backgroundColor: 'rgba(101, 173, 226, 0.8)',
                    borderColor: 'rgba(101, 173, 226, 1)',
                    borderWidth: 1
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Milliseconds'
                        }
                    }
                }
            }
        });

        // Memory Usage Chart
        const memoryCtx = document.getElementById('memoryChart').getContext('2d');
        new Chart(memoryCtx, {
            type: 'line',
            data: {
                labels: resultsData.categories,
                datasets: [{
                    label: 'Memory Usage (MB)',
                    data: resultsData.memoryUsage,
                    backgroundColor: 'rgba(88, 214, 141, 0.2)',
                    borderColor: 'rgba(88, 214, 141, 1)',
                    borderWidth: 2,
                    fill: true
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Megabytes'
                        }
                    }
                }
            }
        });

    </script>
</body>
</html>
)";

        return html;
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    QString jsonFile = "performance_results.json";
    QString outputFile = "performance_report.html";
    
    // Check if custom file names were provided
    if (argc >= 2) {
        jsonFile = QString::fromLocal8Bit(argv[1]);
    }
    if (argc >= 3) {
        outputFile = QString::fromLocal8Bit(argv[2]);
    }
    
    std::cout << "Generating HTML performance report..." << std::endl;
    std::cout << "Input file: " << jsonFile.toStdString() << std::endl;
    std::cout << "Output file: " << outputFile.toStdString() << std::endl;
    
    if (HTMLReportGenerator::generateReport(jsonFile, outputFile)) {
        std::cout << "HTML report generated successfully!" << std::endl;
        std::cout << "Open " << outputFile.toStdString() << " in your web browser to view the report." << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to generate HTML report." << std::endl;
        return 1;
    }
}
