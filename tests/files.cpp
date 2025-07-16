#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <filesystem>

#include "files.hpp"


void cleanupPath(const std::filesystem::path& path) {
    std::error_code ec;
    if (std::filesystem::is_directory(path, ec)) {
        std::filesystem::remove_all(path, ec);
    } else {
        std::filesystem::remove(path, ec);
    }
}

std::string getFileContent(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) return "";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}


bool test_creation_and_open() {
    std::cout << "--- Test: Creation and Opening ---\n";
    const std::filesystem::path testPath = "test_file.txt";
    cleanupPath(testPath);

    try {
        Harbour::FH::fileHandler handler(testPath, "rw");
        if (!handler.open()) {
            std::cerr << "FAIL: Could not open with valid mode 'rw'.\n";
            return false;
        }
        handler.close();
        std::cout << "PASS: Opened successfully with 'rw'.\n";

        try {
            Harbour::FH::fileHandler invalid_handler(testPath, "rwx");
            std::cerr << "FAIL: Did not throw exception for invalid mode 'rwx'.\n";
            return false;
        } catch (const std::invalid_argument& e) {
            std::cout << "PASS: Correctly threw exception for invalid mode.\n";
        }
    } catch (std::exception e) {
        std::cerr << "FAIL: An unexpected exception occurred: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool test_write_and_read() {
    std::cout << "--- Test: WriteLines Functionality ---\n";
    const std::filesystem::path testPath = "test_file.txt";
    cleanupPath(testPath);
    try {
        Harbour::FH::fileHandler handler(testPath, "w");
        handler.open();
        handler.writeLines({"Hello, World!", "This is a test."});
        handler.close();

        std::string expected = "Hello, World!\nThis is a test.\n";
        std::string actual = getFileContent(testPath);

        if (expected != actual) {
            std::cerr << "FAIL: File content does not match expected content.\n";
            return false;
        }
        std::cout << "PASS: writeLines wrote content correctly.\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: An exception occurred: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool test_seeking_and_jumping() {
    std::cout << "--- Test: Seeking and Jumping ---\n";
    const std::filesystem::path testPath = "test_file.txt";
    cleanupPath(testPath);

    std::ofstream setup(testPath);
    setup << "Line 1\nLine 2\nLine 3: Seek here\nLine 4\nLine 5\n";
    setup.close();

    try {
        Harbour::FH::fileHandler handler(testPath, "r");
        handler.open();

        handler.jumpToLine(3);
        std::string line;
        std::getline(handler.getStream(), line);
        if (line != "Line 3: Seek here") {
            std::cerr << "FAIL: jumpToLine did not land on the correct line.\n";
            return false;
        }
        std::cout << "PASS: jumpToLine works correctly.\n";
        
        handler.seekToByte(7); // Beginning of "Line 2"
        std::getline(handler.getStream(), line);
        if (line != "Line 2") {
            std::cerr << "FAIL: seekToByte did not land on the correct position.\n";
            return false;
        }
        std::cout << "PASS: seekToByte works correctly.\n";
        
        handler.close();
    } catch (const std::exception& e) {
        std::cerr << "FAIL: An exception occurred: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool test_appending() {
    std::cout << "--- Test: Appending ---\n";
    const std::filesystem::path testPath = "test_file.txt";
    cleanupPath(testPath);

    std::ofstream setup(testPath);
    setup << "Initial content.\n";
    setup.close();

    try {
        Harbour::FH::fileHandler handler(testPath, "rw");
        handler.open();
        handler.jumpToEnd();
        handler.getStream() << "Manually appended.\n";
        handler.close();
        
        Harbour::FH::fileHandler append_handler(testPath, "a");
        append_handler.open();
        append_handler.getStream() << "Automatically appended.\n";
        append_handler.close();

        std::string expected = "Initial content.\nManually appended.\nAutomatically appended.\n";
        std::string actual = getFileContent(testPath);

        if (expected != actual) {
            std::cerr << "FAIL: Appending did not produce the correct file content.\n";
            return false;
        }
        std::cout << "PASS: jumpToEnd and append mode work correctly.\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: An exception occurred: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool test_directory_creation() {
    std::cout << "--- Test: Automatic Directory Creation ---\n";
    const std::filesystem::path testPath = "temp_test_dir/nested/test_file.txt";
    cleanupPath("temp_test_dir");

    try {
        Harbour::FH::fileHandler handler(testPath, "w");
        if (!handler.open()) {
            std::cerr << "FAIL: handler.open() failed.\n";
            return false;
        }
        handler.close();
        
        if (!std::filesystem::exists(testPath)) {
            std::cerr << "FAIL: File was not created at the specified path.\n";
            return false;
        }
        std::cout << "PASS: Directory and file were created successfully.\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: An unexpected exception occurred: " << e.what() << "\n";
        return false;
    }
    return true;
}

int main() {
    std::cout << ">>> Running fileHandler Tests <<<\n\n";
    bool all_ok = true;

    all_ok &= test_creation_and_open();
    all_ok &= test_write_and_read();
    all_ok &= test_seeking_and_jumping();
    all_ok &= test_appending();
    all_ok &= test_directory_creation();

    std::cout << "\n-------------------------------------\n";
    if (all_ok) {
        std::cout << ">>> All tests passed successfully! <<<\n";
    } else {
        std::cout << ">>> SOME TESTS FAILED! <<<\n";
    }
    std::cout << "-------------------------------------\n";

    cleanupPath("test_file.txt");
    cleanupPath("temp_test_dir");

    return all_ok ? 0 : 1;
}
