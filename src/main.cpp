#include "calculator.hpp"
#include <iostream>
#include <string>

void printUsage() {
    std::cout << "Usage: calculator <operation> <number1> <number2>\n";
    std::cout << "Operations: add, sub, mul, div\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage();
        return 1;
    }

    std::string operation = argv[1];
    double num1, num2;
    
    try {
        num1 = std::stod(argv[2]);
        num2 = std::stod(argv[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid numbers provided\n";
        return 1;
    }

    Calculator calc;
    double result;

    try {
        if (operation == "add") {
            result = calc.add(num1, num2);
        } else if (operation == "sub") {
            result = calc.subtract(num1, num2);
        } else if (operation == "mul") {
            result = calc.multiply(num1, num2);
        } else if (operation == "div") {
            result = calc.divide(num1, num2);
        } else {
            std::cerr << "Error: Invalid operation\n";
            printUsage();
            return 1;
        }

        std::cout << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
