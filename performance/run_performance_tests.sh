#!/bin/bash

# Tic-Tac-Toe Performance Test Runner
# This script runs the performance tests and generates a comprehensive report

set -e  # Exit on any error

echo "==============================================="
echo "    Tic-Tac-Toe Performance Test Suite"
echo "==============================================="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the correct directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "Please run this script from the Tic-Tac-Toe root directory"
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    print_status "Creating build directory..."
    mkdir build
fi

cd build

# Configure CMake if needed
if [ ! -f "Makefile" ] && [ ! -f "build.ninja" ]; then
    print_status "Configuring CMake..."
    cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
    if [ $? -ne 0 ]; then
        print_error "CMake configuration failed"
        exit 1
    fi
fi

# Build the project including performance tests
print_status "Building project and performance tests..."
cmake --build . --target performance_test --parallel
if [ $? -ne 0 ]; then
    print_error "Build failed"
    exit 1
fi

# Build GUI performance tests
print_status "Building GUI performance tests..."
cmake --build . --target gui_performance_test --parallel
if [ $? -ne 0 ]; then
    print_warning "GUI performance test build failed, continuing with core tests only"
    GUI_TESTS_AVAILABLE=false
else
    GUI_TESTS_AVAILABLE=true
fi

# Build the report generator
print_status "Building HTML report generator..."
cmake --build . --target generate_report --parallel
if [ $? -ne 0 ]; then
    print_warning "HTML report generator build failed, continuing with basic report only"
fi

# Create performance results directory
RESULTS_DIR="performance_results"
if [ ! -d "$RESULTS_DIR" ]; then
    mkdir "$RESULTS_DIR"
fi

# Get current timestamp for file naming
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULTS_PREFIX="${RESULTS_DIR}/performance_${TIMESTAMP}"

print_status "Running performance tests..."
echo "Results will be saved with prefix: $RESULTS_PREFIX"
echo

# Setup display for GUI tests if available
DISPLAY_SETUP=false
if [ "$GUI_TESTS_AVAILABLE" = true ]; then
    if [ -z "$DISPLAY" ]; then
        print_status "Setting up virtual display for GUI tests..."
        export DISPLAY=:99
        Xvfb :99 -screen 0 1024x768x24 &
        XVFB_PID=$!
        sleep 3
        DISPLAY_SETUP=true
        export QT_QPA_PLATFORM=xcb
    fi
fi

# Run the core performance tests
cd performance
if [ -f "./performance_test" ]; then
    print_status "Running core performance tests..."
    ./performance_test
    PERF_EXIT_CODE=$?
else
    print_error "Performance test executable not found!"
    exit 1
fi

# Run GUI performance tests if available
if [ "$GUI_TESTS_AVAILABLE" = true ] && [ -f "./gui_performance_test" ]; then
    print_status "Running GUI performance tests..."
    ./gui_performance_test
    GUI_EXIT_CODE=$?
    if [ $GUI_EXIT_CODE -ne 0 ]; then
        print_warning "GUI performance tests completed with issues (exit code: $GUI_EXIT_CODE)"
    fi
else
    print_warning "GUI performance tests not available"
    GUI_EXIT_CODE=0
fi

# Cleanup virtual display if we set it up
if [ "$DISPLAY_SETUP" = true ] && [ ! -z "$XVFB_PID" ]; then
    kill $XVFB_PID 2>/dev/null || true
fi

# Move core performance results to timestamped files
if [ -f "performance_results.csv" ]; then
    mv "performance_results.csv" "../${RESULTS_PREFIX}.csv"
    print_success "Core CSV results saved to: ${RESULTS_PREFIX}.csv"
fi

if [ -f "performance_results.json" ]; then
    mv "performance_results.json" "../${RESULTS_PREFIX}.json"
    print_success "Core JSON results saved to: ${RESULTS_PREFIX}.json"
fi

# Move GUI performance results to timestamped files
if [ -f "gui_performance_results.csv" ]; then
    mv "gui_performance_results.csv" "../${RESULTS_PREFIX}_gui.csv"
    print_success "GUI CSV results saved to: ${RESULTS_PREFIX}_gui.csv"
fi

if [ -f "gui_performance_results.json" ]; then
    mv "gui_performance_results.json" "../${RESULTS_PREFIX}_gui.json"
    print_success "GUI JSON results saved to: ${RESULTS_PREFIX}_gui.json"
fi

# Generate HTML reports if generator is available
cd ..
if [ -f "performance/generate_report" ]; then
    # Generate core performance report
    if [ -f "${RESULTS_PREFIX}.json" ]; then
        print_status "Generating core performance HTML report..."
        ./performance/generate_report "${RESULTS_PREFIX}.json" "${RESULTS_PREFIX}.html"
        if [ $? -eq 0 ]; then
            print_success "Core HTML report saved to: ${RESULTS_PREFIX}.html"
        else
            print_warning "Core HTML report generation failed"
        fi
    fi

    # Generate GUI performance report
    if [ -f "${RESULTS_PREFIX}_gui.json" ]; then
        print_status "Generating GUI performance HTML report..."
        ./performance/generate_report "${RESULTS_PREFIX}_gui.json" "${RESULTS_PREFIX}_gui.html"
        if [ $? -eq 0 ]; then
            print_success "GUI HTML report saved to: ${RESULTS_PREFIX}_gui.html"
        else
            print_warning "GUI HTML report generation failed"
        fi
    fi

    # Try to open the main report in the default browser (optional)
    if [ -f "${RESULTS_PREFIX}.html" ]; then
        if command -v xdg-open >/dev/null 2>&1; then
            print_status "Opening core report in browser..."
            xdg-open "${RESULTS_PREFIX}.html" >/dev/null 2>&1 &
        elif command -v open >/dev/null 2>&1; then
            print_status "Opening core report in browser..."
            open "${RESULTS_PREFIX}.html" >/dev/null 2>&1 &
        elif command -v start >/dev/null 2>&1; then
            print_status "Opening core report in browser..."
            start "${RESULTS_PREFIX}.html" >/dev/null 2>&1 &
        fi
    fi

    # Try to open the gui report in the default browser (optional)
    if [ -f "${RESULTS_PREFIX}.html" ]; then
        if command -v xdg-open >/dev/null 2>&1; then
            print_status "Opening gui report in browser..."
            xdg-open "${RESULTS_PREFIX}_gui.html" >/dev/null 2>&1 &
        elif command -v open >/dev/null 2>&1; then
            print_status "Opening gui report in browser..."
            open "${RESULTS_PREFIX}_gui.html" >/dev/null 2>&1 &
        elif command -v start >/dev/null 2>&1; then
            print_status "Opening gui report in browser..."
            start "${RESULTS_PREFIX}_gui.html" >/dev/null 2>&1 &
        fi
    fi
else
    print_warning "HTML report generator not available"
fi

# Create symlinks to latest results for convenience
if [ -f "${RESULTS_PREFIX}.csv" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}.csv)" "${RESULTS_DIR}/latest.csv"
fi

if [ -f "${RESULTS_PREFIX}.json" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}.json)" "${RESULTS_DIR}/latest.json"
fi

if [ -f "${RESULTS_PREFIX}.html" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}.html)" "${RESULTS_DIR}/latest.html"
fi

# Create symlinks for GUI results
if [ -f "${RESULTS_PREFIX}_gui.csv" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}_gui.csv)" "${RESULTS_DIR}/latest_gui.csv"
fi

if [ -f "${RESULTS_PREFIX}_gui.json" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}_gui.json)" "${RESULTS_DIR}/latest_gui.json"
fi

if [ -f "${RESULTS_PREFIX}_gui.html" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}_gui.html)" "${RESULTS_DIR}/latest_gui.html"
fi

echo
echo "==============================================="
echo "           Performance Test Summary"
echo "==============================================="

# Display basic statistics from the results
if [ -f "${RESULTS_PREFIX}.csv" ]; then
    echo "Quick Statistics:"
    echo "----------------"

    # Count total tests
    TOTAL_TESTS=$(tail -n +2 "${RESULTS_PREFIX}.csv" | wc -l)
    echo "Total Tests: $TOTAL_TESTS"

    # Count successful tests
    SUCCESS_TESTS=$(tail -n +2 "${RESULTS_PREFIX}.csv" | grep -c ",true," || true)
    echo "Successful Tests: $SUCCESS_TESTS"

    # Count failed tests
    FAILED_TESTS=$((TOTAL_TESTS - SUCCESS_TESTS))
    echo "Failed Tests: $FAILED_TESTS"

    if [ $TOTAL_TESTS -gt 0 ]; then
        SUCCESS_RATE=$(echo "scale=1; $SUCCESS_TESTS * 100 / $TOTAL_TESTS" | bc -l 2>/dev/null || echo "N/A")
        echo "Success Rate: ${SUCCESS_RATE}%"
    fi

    echo
    echo "Performance Metrics:"
    echo "-------------------"

    # Extract response times (2nd column)
    if command -v awk >/dev/null 2>&1; then
        AVG_TIME=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' '{sum+=$2; count++} END {if(count>0) printf "%.2f", sum/count; else print "N/A"}')
        MAX_TIME=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' 'BEGIN{max=0} {if($2>max) max=$2} END {printf "%.2f", max}')
        echo "Average Response Time: ${AVG_TIME} ms"
        echo "Maximum Response Time: ${MAX_TIME} ms"

        # Extract memory usage (3rd column)
        AVG_MEM=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' '{sum+=$3; count++} END {if(count>0) printf "%.1f", sum/count; else print "N/A"}')
        MAX_MEM=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' 'BEGIN{max=0} {if($3>max) max=$3} END {printf "%.1f", max}')
        echo "Average Memory Usage: ${AVG_MEM} MB"
        echo "Peak Memory Usage: ${MAX_MEM} MB"
    fi
fi

echo
echo "Available Reports:"
echo "-----------------"
echo "Core Performance:"
if [ -f "${RESULTS_PREFIX}.csv" ]; then
    echo "• CSV Report: ${RESULTS_PREFIX}.csv"
fi
if [ -f "${RESULTS_PREFIX}.json" ]; then
    echo "• JSON Report: ${RESULTS_PREFIX}.json"
fi
if [ -f "${RESULTS_PREFIX}.html" ]; then
    echo "• HTML Report: ${RESULTS_PREFIX}.html"
fi

echo
echo "GUI Performance:"
if [ -f "${RESULTS_PREFIX}_gui.csv" ]; then
    echo "• GUI CSV Report: ${RESULTS_PREFIX}_gui.csv"
fi
if [ -f "${RESULTS_PREFIX}_gui.json" ]; then
    echo "• GUI JSON Report: ${RESULTS_PREFIX}_gui.json"
fi
if [ -f "${RESULTS_PREFIX}_gui.html" ]; then
    echo "• GUI HTML Report: ${RESULTS_PREFIX}_gui.html"
fi

echo
echo "Latest Reports (symlinks):"
echo "-------------------------"
echo "Core Performance:"
if [ -L "${RESULTS_DIR}/latest.csv" ]; then
    echo "• Latest CSV: ${RESULTS_DIR}/latest.csv"
fi
if [ -L "${RESULTS_DIR}/latest.json" ]; then
    echo "• Latest JSON: ${RESULTS_DIR}/latest.json"
fi
if [ -L "${RESULTS_DIR}/latest.html" ]; then
    echo "• Latest HTML: ${RESULTS_DIR}/latest.html"
fi

echo
echo "GUI Performance:"
if [ -L "${RESULTS_DIR}/latest_gui.csv" ]; then
    echo "• Latest GUI CSV: ${RESULTS_DIR}/latest_gui.csv"
fi
if [ -L "${RESULTS_DIR}/latest_gui.json" ]; then
    echo "• Latest GUI JSON: ${RESULTS_DIR}/latest_gui.json"
fi
if [ -L "${RESULTS_DIR}/latest_gui.html" ]; then
    echo "• Latest GUI HTML: ${RESULTS_DIR}/latest_gui.html"
fi

echo
OVERALL_EXIT_CODE=0
if [ $PERF_EXIT_CODE -eq 0 ] && [ $GUI_EXIT_CODE -eq 0 ]; then
    print_success "All performance testing completed successfully!"
    echo
    echo "To view core performance report: ${RESULTS_PREFIX}.html"
    echo "To view GUI performance report: ${RESULTS_PREFIX}_gui.html"
    echo "To analyze raw data: ${RESULTS_PREFIX}.csv and ${RESULTS_PREFIX}_gui.csv"
elif [ $PERF_EXIT_CODE -ne 0 ]; then
    print_warning "Core performance testing completed with issues (exit code: $PERF_EXIT_CODE)"
    OVERALL_EXIT_CODE=$PERF_EXIT_CODE
elif [ $GUI_EXIT_CODE -ne 0 ]; then
    print_warning "GUI performance testing completed with issues (exit code: $GUI_EXIT_CODE)"
    echo "Core performance tests passed successfully"
    OVERALL_EXIT_CODE=$GUI_EXIT_CODE
fi

echo
echo "==============================================="

exit $OVERALL_EXIT_CODE
