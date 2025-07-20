#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <windows.h>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <chrono>

// Simple testing framework for Win32 applications
class TestFramework {
public:
    struct TestResult {
        std::string testName;
        bool passed;
        std::string errorMessage;
        long long executionTimeMs;
    };

    struct TestSuite {
        std::string suiteName;
        std::vector<std::function<bool()>> tests;
        std::vector<std::string> testNames;
    };

private:
    std::vector<TestSuite> testSuites;
    std::vector<TestResult> results;
    int totalTests = 0;
    int passedTests = 0;

public:
    // Add a test suite
    void AddSuite(const std::string& suiteName) {
        TestSuite suite;
        suite.suiteName = suiteName;
        testSuites.push_back(suite);
    }

    // Add a test to the current suite
    void AddTest(const std::string& testName, std::function<bool()> testFunc) {
        if (!testSuites.empty()) {
            testSuites.back().testNames.push_back(testName);
            testSuites.back().tests.push_back(testFunc);
        }
    }

    // Run all tests
    void RunAllTests() {
        std::cout << "\n=== Modern Paint Studio Pro - Test Suite ===" << std::endl;
        std::cout << "Starting automated tests...\n" << std::endl;

        auto startTime = std::chrono::high_resolution_clock::now();

        for (const auto& suite : testSuites) {
            RunTestSuite(suite);
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        PrintSummary(duration.count());
    }

private:
    void RunTestSuite(const TestSuite& suite) {
        std::cout << "Running " << suite.suiteName << " tests:" << std::endl;
        
        for (size_t i = 0; i < suite.tests.size(); i++) {
            RunSingleTest(suite.testNames[i], suite.tests[i]);
        }
        std::cout << std::endl;
    }

    void RunSingleTest(const std::string& testName, std::function<bool()> testFunc) {
        TestResult result;
        result.testName = testName;

        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            result.passed = testFunc();
            if (!result.passed) {
                result.errorMessage = "Test assertion failed";
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
        } catch (...) {
            result.passed = false;
            result.errorMessage = "Unknown exception occurred";
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        results.push_back(result);
        totalTests++;
        if (result.passed) passedTests++;

        // Print result
        std::cout << "  " << (result.passed ? "✓ PASS" : "✗ FAIL") 
                  << " - " << testName 
                  << " (" << result.executionTimeMs << "ms)";
        if (!result.passed) {
            std::cout << " - " << result.errorMessage;
        }
        std::cout << std::endl;
    }

    void PrintSummary(long long totalTimeMs) {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Success Rate: " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%" << std::endl;
        std::cout << "Total Time: " << totalTimeMs << "ms" << std::endl;

        if (passedTests == totalTests) {
            std::cout << "\n🎉 All tests passed! Modern Paint Studio Pro is working perfectly!" << std::endl;
        } else {
            std::cout << "\n⚠️  Some tests failed. Check the results above." << std::endl;
        }
    }
};

// Test assertion macros
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        return false; \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        return false; \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        return false; \
    }

#define ASSERT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        return false; \
    }

#endif // TEST_FRAMEWORK_H