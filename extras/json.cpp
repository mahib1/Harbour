#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    std::ifstream in(".hrbr");
    if (!in) {
        std::cerr << ".hrbr file not found!" << std::endl;
        return 1;
    }
    json j;
    in >> j;

    std::cout << "[info]" << std::endl;
    if (j.contains("info")) {
        auto& info = j["info"];
        std::cout << "  name: " << info.value("name", "") << std::endl;
        std::cout << "  type: " << info.value("type", "") << std::endl;
        std::cout << "  version: " << info.value("version", "") << std::endl;
    }

    std::cout << "\n[scripts]" << std::endl;
    if (j.contains("scripts")) {
        for (auto& [key, val] : j["scripts"].items()) {
            std::cout << "  " << key << ": ";
            if (val.is_string()) {
                std::cout << val.get<std::string>() << std::endl;
            } else if (val.is_array()) {
                std::cout << std::endl;
                for (auto& cmd : val) {
                    std::cout << "    - " << cmd.get<std::string>() << std::endl;
                }
            }
        }
    }

    std::cout << "\n[dependencies]" << std::endl;
    if (j.contains("dependencies")) {
        for (auto& [dep, ver] : j["dependencies"].items()) {
            std::cout << "  " << dep << ": " << ver.get<std::string>() << std::endl;
        }
    }
    return 0;
} 