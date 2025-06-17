# Tic-Tac-Toe Performance Testing Suite

This directory contains a comprehensive performance testing suite for the Tic-Tac-Toe game project. The suite measures key performance metrics including response time, memory usage, and CPU utilization across all major components of the application.

## 🚀 Quick Start

### Prerequisites
- CMake 3.14 or higher
- Qt6 (Core and Widgets)
- SQLite3
- OpenSSL
- C++17 compatible compiler

### Running Performance Tests

#### Linux/macOS
```bash
# From the project root directory
cd Tic-Tac-Toe
chmod +x performance/run_performance_tests.sh
performance/run_performance_tests.sh
```

#### Windows
```cmd
# From the project root directory
cd Tic-Tac-Toe
performance\run_performance_tests.bat
```

#### Manual Build and Run
```bash
# Build the project with performance tests
mkdir build && cd build
cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
cmake --build . --target performance_test --parallel

# Run core performance tests
cd performance
./performance_test

# Run GUI performance tests (requires display)
./gui_performance_test

# Generate HTML report (optional)
cd ..
./performance/generate_report performance_results.json performance_report.html
./performance/generate_report gui_performance_results.json gui_performance_report.html
```

## 📊 What Gets Tested

### Core Game Components
- **Board Operations**: Creation, move validation, game state checking
- **AI Performance**: Move calculation times, decision-making efficiency
- **Authentication**: User registration and login performance
- **Game History**: Database operations, game storage and retrieval

### Performance Metrics
- **Response Time**: Milliseconds for each operation
- **Memory Usage**: RAM consumption in MB
- **CPU Usage**: Processor utilization percentage
- **Success Rate**: Percentage of successful test completions

### Test Categories

#### 1. Board Operations
- Board creation and initialization (10,000 iterations)
- Move validation performance (100,000 iterations)
- Game completion detection (50,000 iterations)
- Full game simulation (1,000 games)

#### 2. AI Performance
- Empty board move calculation (100 iterations)
- Partial board scenarios (100 iterations)
- Endgame move calculation (100 iterations)
- Complete AI vs AI games (50 games)

#### 3. Database Operations
- User registration performance (1,000 users)
- User login performance (1,000 logins)
- Game initialization (1,000 games)
- Move recording (1,000 moves)
- Game retrieval (500 queries)

#### 4. Memory Stress Tests
- Multiple board instances (1,000 boards)
- Game history with large datasets (500 games)

#### 5. Core GUI Component Tests
- Application creation performance
- Basic widget creation and management

#### 6. Comprehensive GUI Performance Tests
- **Window Creation**: MainWindow, LoginPage, GameWindow, GameHistoryGUI
- **Widget Rendering**: Complex layouts, game board updates, window resizing
- **User Interactions**: Button clicks, mouse hover, keyboard input, form validation
- **Large Datasets**: TreeWidget with 1000+ items, TableWidget population, scrolling
- **Navigation**: StackedWidget switching, modal dialogs, page transitions
- **Visual Effects**: Animation simulation, style changes, complex stylesheets
- **Memory Usage**: GUI memory consumption, large data structure display
- **Real Workflows**: Complete login-to-game flow, game history browsing

## 📈 Understanding the Results

### Output Files
The performance tests generate three types of reports:

1. **CSV Report** (`performance_YYYYMMDD_HHMMSS.csv`)
   - Raw data for spreadsheet analysis
   - Contains all test metrics in tabular format

2. **JSON Report** (`performance_YYYYMMDD_HHMMSS.json`)
   - Structured data for programmatic analysis
   - Includes timestamp and detailed test results

3. **HTML Report** (`performance_YYYYMMDD_HHMMSS.html`)
   - Visual dashboard with charts and summaries
   - Color-coded performance indicators
   - Responsive design for all devices

### Performance Indicators

The HTML report uses color-coded indicators:
- 🟢 **Green (Excellent)**: Optimal performance
- 🟣 **Purple (Good)**: Acceptable performance
- 🟡 **Yellow (Fair)**: May need attention
- 🔴 **Red (Poor)**: Requires optimization

#### Response Time Thresholds
- **Excellent**: < 100ms
- **Good**: 100-500ms
- **Fair**: 500-1000ms
- **Poor**: > 1000ms

#### Memory Usage Thresholds
- **Excellent**: < 50MB
- **Good**: 50-100MB
- **Fair**: 100-200MB
- **Poor**: > 200MB

#### CPU Usage Thresholds
- **Excellent**: < 25%
- **Good**: 25-50%
- **Fair**: 50-75%
- **Poor**: > 75%

## 🛠️ Customizing Tests

### Adding New Tests
To add custom performance tests, modify `performance_test.cpp`:

```cpp
void testCustomFeature() {
    std::cout << "Testing Custom Feature..." << std::endl;
    
    {
        PERFORMANCE_TEST(monitor_, "Custom_Test_Name");
        // Your test code here
        for (int i = 0; i < 1000; ++i) {
            // Perform operations to test
        }
    }
}
```

### Configuring Test Parameters
Edit the iteration counts in `performance_test.cpp` to adjust test intensity:
- Increase iterations for more thorough testing
- Decrease for faster execution during development

### Platform-Specific Monitoring
The performance monitor automatically detects the platform and uses appropriate system calls:
- **Windows**: WinAPI for memory and CPU monitoring
- **Linux**: `/proc` filesystem for system metrics
- **macOS**: Mach kernel APIs for resource monitoring

## 🔧 Troubleshooting

### Common Issues

#### Build Failures
```bash
# Ensure all dependencies are installed
sudo apt-get install libsqlite3-dev libssl-dev  # Ubuntu/Debian
brew install sqlite3 openssl                     # macOS
```

#### Permission Errors
```bash
# Make scripts executable on Unix systems
chmod +x performance/run_performance_tests.sh
```

#### Missing Qt6
Ensure Qt6 is properly installed and `CMAKE_PREFIX_PATH` is set:
```bash
export CMAKE_PREFIX_PATH=/path/to/qt6
```

#### Database Lock Errors
If you encounter database locking issues, ensure no other instances are running:
```bash
# Kill any running instances
pkill -f performance_test
```

### Performance Tuning Tips

#### For Better Performance
1. **Close unnecessary applications** before running tests
2. **Use Release build** (`-DCMAKE_BUILD_TYPE=Release`)
3. **Run on dedicated hardware** for consistent results
4. **Disable antivirus real-time scanning** temporarily

#### For Development
1. **Reduce iteration counts** for faster feedback
2. **Focus on specific test categories** by commenting out others
3. **Use Debug builds** for detailed error information

## 📁 File Structure

```
performance/
├── CMakeLists.txt              # Build configuration
├── performance_monitor.h       # Performance monitoring utilities
├── performance_monitor.cpp     # Platform-specific implementations
├── performance_test.cpp        # Main test suite
├── generate_report.cpp         # HTML report generator
├── run_performance_tests.sh    # Linux/macOS runner script
├── run_performance_tests.bat   # Windows runner script
└── README.md                   # This file
```

## 🎯 Interpreting Results

### Baseline Performance Expectations

#### Typical Response Times
- **Board Operations**: 0.01-1ms per operation
- **AI Moves**: 10-100ms per calculation
- **Database Operations**: 1-10ms per query
- **GUI Creation**: 50-500ms per component

#### Memory Usage Guidelines
- **Base Application**: 20-50MB
- **Per Game**: 1-5MB additional
- **Large History**: Up to 100MB for 1000+ games

### When to Be Concerned
- **Response times > 1 second** for simple operations
- **GUI response times > 100ms** for user interactions
- **Memory usage > 200MB** for basic functionality
- **Consistent test failures** indicating stability issues
- **CPU usage > 80%** during normal operations
- **GUI frame rate < 30 FPS** (>33ms per frame) for animations

## 🔄 Continuous Integration

### Automated Testing
The performance tests can be integrated into CI/CD pipelines:

```yaml
# Example GitHub Actions workflow
- name: Run Performance Tests
  run: |
    chmod +x performance/run_performance_tests.sh
    performance/run_performance_tests.sh
    
- name: Upload Performance Results
  uses: actions/upload-artifact@v3
  with:
    name: performance-results
    path: build/performance_results/
```

### Performance Regression Detection
Compare results over time to detect performance regressions:
1. Store historical performance data
2. Set acceptable thresholds for key metrics
3. Alert on significant degradations

### GUI-Specific Testing Notes

#### Display Requirements
GUI performance tests require a display environment:
- **Linux**: Use `Xvfb` for headless testing in CI
- **Windows**: Tests run normally with desktop
- **macOS**: Tests run normally with desktop

#### Running Headless (Linux CI)
```bash
# Start virtual display
export DISPLAY=:99
Xvfb :99 -screen 0 1024x768x24 &
export QT_QPA_PLATFORM=xcb

# Run GUI tests
./gui_performance_test
```

#### GUI Performance Targets
- **Window Creation**: < 500ms
- **Button Clicks**: < 50ms response time
- **Form Input**: < 100ms per character
- **Large Dataset Display**: < 2 seconds for 1000 items
- **Navigation**: < 200ms between pages
- **Memory Usage**: < 100MB for typical GUI operations

## 📞 Support

For issues or questions about performance testing:
1. Check the troubleshooting section above
2. Review the generated logs in the build directory
3. Examine the detailed CSV/JSON reports for specific failures
4. Ensure all system requirements are met

## 🔮 Future Enhancements

Planned improvements to the performance testing suite:
- **Interactive charts** in HTML reports using Chart.js
- **Performance regression tracking** over time
- **Multi-threaded test scenarios** for concurrency testing
- **Network performance tests** for future multiplayer features
- **Cross-platform performance comparisons**
- **Automated performance alerts** for CI systems
- **GUI rendering profiling** with Qt's built-in profiler
- **Frame rate analysis** for smooth animations
- **Touch/gesture performance** for mobile platforms
- **Accessibility performance** testing
- **Real user monitoring** integration

---

*This performance testing suite helps ensure the Tic-Tac-Toe application maintains optimal performance across all supported platforms and use cases.*