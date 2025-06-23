#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class ReadProvider {
  protected:
    template <typename T>
    T readValue(const std::string &path, const T &default_value = T{}) const {
        std::ifstream file(path);
        if (!file.is_open()) {
            return default_value;
        }

        T value;
        if (!(file >> value)) {
            return default_value;
        }

        return value;
    }

    std::string readString(const std::string &path,
                           const std::string &default_value = "") const {
        std::ifstream file(path);
        if (!file.is_open()) {
            return default_value;
        }

        std::string value;
        if (!std::getline(file, value)) {
            return default_value;
        }

        return value;
    }

    std::string findDirByType(const std::string &base_path,
                              const std::vector<std::string> &keywords) const {
        namespace fs = std::filesystem;
        for (const auto &entry : fs::directory_iterator(base_path)) {
            if (!entry.is_directory())
                continue;

            auto type_path = entry.path() / "type";
            std::ifstream type_file(type_path);
            if (!type_file.is_open())
                continue;

            std::string type;
            std::getline(type_file, type);

            for (const auto &keyword : keywords) {
                if (type.find(keyword) != std::string::npos) {
                    return entry.path().string();
                }
            }
        }
        return "";
    }
};
