#pragma once
#include <fstream>
#include <iostream>
#include <string>

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
};
