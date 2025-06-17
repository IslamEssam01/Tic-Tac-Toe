#!/bin/bash

# Dedicated GUI Performance Test Runner for Tic-Tac-Toe
# This script runs only the GUI performance tests with proper display setup

set -e  # Exit on any error

echo "==============================================="
echo "    Tic-Tac-Toe GUI Performance Test Suite"
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

# Check if build directory exists
if [ ! -d "build" ]; then
    print_error "Build directory not found. Please run the main performance test script first."
    print_status "Or create build directory with: mkdir build && cd build && cmake .."
    exit 1
fi

cd build

# Check if GUI performance test is built
if [ ! -f "performance/gui_performance_test" ]; then
    print_status "GUI performance test not found. Building..."
    cmake --build . --target gui_performance_test --parallel
    if [ $? -ne 0 ]; then
        print_error "Failed to build GUI performance test"
        exit 1
    fi
fi

# Setup display environment
DISPLAY_SETUP=false
XVFB_PID=""

print_status "Checking display environment..."

if [ -z "$DISPLAY" ]; then
    print_status "No display found. Setting up virtual display..."

    # Check if Xvfb is available
    if ! command -v Xvfb >/dev/null 2>&1; then
        print_error "Xvfb not found. Please install it:"
        echo "  Ubuntu/Debian: sudo apt-get install xvfb"
        echo "  CentOS/RHEL:   sudo yum install xorg-x11-server-Xvfb"
        echo "  Or run this script on a system with a display"
        exit 1
    fi

    # Start virtual display
    export DISPLAY=:99
    Xvfb :99 -screen 0 1024x768x24 &
    XVFB_PID=$!
    sleep 3
    DISPLAY_SETUP=true
    export QT_QPA_PLATFORM=xcb

    print_success "Virtual display started on :99"
else
    print_success "Using existing display: $DISPLAY"
fi

# Create results directory
RESULTS_DIR="performance_results"
if [ ! -d "$RESULTS_DIR" ]; then
    mkdir "$RESULTS_DIR"
fi

# Generate timestamp for results
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULTS_PREFIX="${RESULTS_DIR}/gui_performance_${TIMESTAMP}"

print_status "Running GUI performance tests..."
echo "Results will be saved with prefix: $RESULTS_PREFIX"
echo

# Function to cleanup display
cleanup_display() {
    if [ "$DISPLAY_SETUP" = true ] && [ ! -z "$XVFB_PID" ]; then
        print_status "Cleaning up virtual display..."
        kill $XVFB_PID 2>/dev/null || true
        sleep 1
    fi
}

# Set trap to cleanup on exit
trap cleanup_display EXIT

# Run the GUI performance tests
cd performance
if [ -f "./gui_performance_test" ]; then
    ./gui_performance_test
    GUI_EXIT_CODE=$?
else
    print_error "GUI performance test executable not found!"
    exit 1
fi

# Move results to timestamped files
if [ -f "gui_performance_results.csv" ]; then
    mv "gui_performance_results.csv" "../${RESULTS_PREFIX}.csv"
    print_success "CSV results saved to: ${RESULTS_PREFIX}.csv"
fi

if [ -f "gui_performance_results.json" ]; then
    mv "gui_performance_results.json" "../${RESULTS_PREFIX}.json"
    print_success "JSON results saved to: ${RESULTS_PREFIX}.json"
fi

# Generate HTML report if generator is available
cd ..
if [ -f "performance/generate_report" ]; then
    if [ -f "${RESULTS_PREFIX}.json" ]; then
        print_status "Generating HTML report..."
        ./performance/generate_report "${RESULTS_PREFIX}.json" "${RESULTS_PREFIX}.html"
        if [ $? -eq 0 ]; then
            print_success "HTML report saved to: ${RESULTS_PREFIX}.html"

            # Try to open the report in the default browser
            if command -v xdg-open >/dev/null 2>&1; then
                print_status "Opening report in browser..."
                xdg-open "${RESULTS_PREFIX}.html" >/dev/null 2>&1 &
            elif command -v open >/dev/null 2>&1; then
                print_status "Opening report in browser..."
                open "${RESULTS_PREFIX}.html" >/dev/null 2>&1 &
            elif command -v start >/dev/null 2>&1; then
                print_status "Opening report in browser..."
                start "${RESULTS_PREFIX}.html" >/dev/null 2>&1 &
            fi
        else
            print_warning "HTML report generation failed"
        fi
    fi
else
    print_warning "HTML report generator not available"
fi

# Create symlinks to latest results
if [ -f "${RESULTS_PREFIX}.csv" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}.csv)" "${RESULTS_DIR}/latest_gui.csv"
fi

if [ -f "${RESULTS_PREFIX}.json" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}.json)" "${RESULTS_DIR}/latest_gui.json"
fi

if [ -f "${RESULTS_PREFIX}.html" ]; then
    ln -sf "$(basename ${RESULTS_PREFIX}.html)" "${RESULTS_DIR}/latest_gui.html"
fi

echo
echo "==============================================="
echo "         GUI Performance Test Summary"
echo "==============================================="

# Display basic statistics from the results
if [ -f "${RESULTS_PREFIX}.csv" ]; then
    echo "Quick Statistics:"
    echo "----------------"

    # Count total tests
    TOTAL_TESTS=$(tail -n +2 "${RESULTS_PREFIX}.csv" | wc -l)
    echo "Total GUI Tests: $TOTAL_TESTS"

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
    echo "GUI Performance Metrics:"
    echo "-----------------------"

    # Extract performance metrics if awk is available
    if command -v awk >/dev/null 2>&1; then
        AVG_TIME=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' '{sum+=$2; count++} END {if(count>0) printf "%.2f", sum/count; else print "N/A"}')
        MAX_TIME=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' 'BEGIN{max=0} {if($2>max) max=$2} END {printf "%.2f", max}')
        echo "Average Response Time: ${AVG_TIME} ms"
        echo "Maximum Response Time: ${MAX_TIME} ms"

        # Extract memory usage
        AVG_MEM=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' '{sum+=$3; count++} END {if(count>0) printf "%.1f", sum/count; else print "N/A"}')
        MAX_MEM=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' 'BEGIN{max=0} {if($3>max) max=$3} END {printf "%.1f", max}')
        echo "Average Memory Usage: ${AVG_MEM} MB"
        echo "Peak Memory Usage: ${MAX_MEM} MB"
    fi
fi

echo
echo "Available Reports:"
echo "-----------------"
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
echo "Latest Reports (symlinks):"
echo "-------------------------"
if [ -L "${RESULTS_DIR}/latest_gui.csv" ]; then
    echo "• Latest CSV: ${RESULTS_DIR}/latest_gui.csv"
fi
if [ -L "${RESULTS_DIR}/latest_gui.json" ]; then
    echo "• Latest JSON: ${RESULTS_DIR}/latest_gui.json"
fi
if [ -L "${RESULTS_DIR}/latest_gui.html" ]; then
    echo "• Latest HTML: ${RESULTS_DIR}/latest_gui.html"
fi

echo
echo "GUI Performance Insights:"
echo "------------------------"
if [ -f "${RESULTS_PREFIX}.csv" ] && command -v awk >/dev/null 2>&1; then
    # Analyze specific GUI metrics
    SLOW_TESTS=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' '$2 > 100 {count++} END {print count+0}')
    HIGH_MEM_TESTS=$(tail -n +2 "${RESULTS_PREFIX}.csv" | awk -F',' '$3 > 100 {count++} END {print count+0}')

    if [ $SLOW_TESTS -gt 0 ]; then
        echo "• $SLOW_TESTS tests took longer than 100ms (consider optimization)"
    fi

    if [ $HIGH_MEM_TESTS -gt 0 ]; then
        echo "• $HIGH_MEM_TESTS tests used more than 100MB memory (check for leaks)"
    fi

    echo "• For smooth GUI: aim for <16ms response time (60 FPS)"
    echo "• For good UX: keep user interactions under 100ms"
    echo "• Monitor memory usage trends for potential leaks"
fi

echo
if [ $GUI_EXIT_CODE -eq 0 ]; then
    print_success "GUI Performance testing completed successfully!"
    echo
    echo "To view the detailed report, open: ${RESULTS_PREFIX}.html"
    echo "To analyze raw data, check: ${RESULTS_PREFIX}.csv"
else
    print_warning "GUI Performance testing completed with some issues (exit code: $GUI_EXIT_CODE)"
fi

echo
echo "==============================================="

exit $GUI_EXIT_CODE
