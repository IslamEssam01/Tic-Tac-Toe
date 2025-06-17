# 🚀 Quick Start: Performance Testing

This guide will help you quickly run performance tests and generate reports for the Tic-Tac-Toe project.

## ⚡ One-Command Setup

### Linux/macOS
```bash
./performance/run_performance_tests.sh
```

### Windows
```cmd
performance\run_performance_tests.bat
```

That's it! The script will automatically:
- ✅ Build the project
- ✅ Run all performance tests
- ✅ Generate CSV, JSON, and HTML reports
- ✅ Open the HTML report in your browser

## 📊 What You'll Get

### Instant Results
- **Response Time**: How fast each operation completes
- **Memory Usage**: RAM consumption during tests
- **CPU Usage**: Processor utilization
- **Success Rate**: Percentage of tests that pass

### Test Coverage
- 🎮 **Game Logic**: Board operations, AI moves
- 🔐 **Authentication**: Login/registration performance
- 💾 **Database**: Game history storage/retrieval
- 🖥️ **GUI**: Interface creation and management

## 📈 Understanding Results

### Color-Coded Performance
- 🟢 **Green**: Excellent performance
- 🟡 **Yellow**: Acceptable performance
- 🔴 **Red**: Needs optimization

### Key Metrics
- **Response Time < 100ms**: Excellent
- **Memory Usage < 50MB**: Excellent
- **CPU Usage < 25%**: Excellent

## 🔧 Quick Fixes

### If Build Fails
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install cmake build-essential libsqlite3-dev libssl-dev

# Install Qt6
sudo apt-get install qt6-base-dev
```

### If Tests Fail
1. Close other applications to free resources
2. Run as administrator/sudo if needed
3. Check that no antivirus is blocking execution

## 📁 Output Files

All results are saved in `build/performance_results/`:
- `performance_TIMESTAMP.html` - Visual report (recommended)
- `performance_TIMESTAMP.csv` - Raw data
- `performance_TIMESTAMP.json` - Structured data

## 🎯 Performance Targets

### Excellent Performance
- Board operations: < 1ms
- AI moves: < 100ms
- Database queries: < 10ms
- Memory usage: < 50MB

### Red Flags
- Any operation > 1 second
- Memory usage > 200MB
- Consistent test failures

## 🔄 Running Specific Tests

Edit `performance/performance_test.cpp` to focus on specific areas:

```cpp
// Comment out tests you don't need
// testBoardOperations();
testAIPerformance();        // Only test AI
// testAuthenticationPerformance();
```

## 💡 Pro Tips

1. **Run on dedicated hardware** for consistent results
2. **Close unnecessary apps** before testing
3. **Use Release build** for realistic performance
4. **Test regularly** to catch regressions early

## 🆘 Need Help?

1. Check `performance/README.md` for detailed documentation
2. Look at console output for specific error messages
3. Verify all dependencies are installed
4. Make sure you're running from the project root

---

**Quick validation**: If you see a colorful HTML report with green indicators, you're all set! 🎉