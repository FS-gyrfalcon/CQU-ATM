#include "simple_json.h"
#include <algorithm>

void SimpleJson::set(const std::string& key, const std::string& value) {
    data[key] = value;
}

std::string SimpleJson::get(const std::string& key) const {
    auto it = data.find(key);
    if (it != data.end()) {
        return it->second;
    }
    return "";
}

bool SimpleJson::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    data.clear();
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            key.erase(0, key.find_first_not_of(" \t\""));
            key.erase(key.find_last_not_of(" \t\"") + 1);
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\"") + 1);
            data[key] = value;
        }
    }
    file.close();
    return true;
}

bool SimpleJson::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "{\n";
    for (auto it = data.begin(); it != data.end(); ++it) {
        file << "  \"" << it->first << "\": \"" << it->second << "\"";
        if (std::next(it) != data.end()) file << ",";
        file << "\n";
    }
    file << "}";
    file.close();
    return true;
}

bool SimpleJson::hasKey(const std::string& key) const {
    return data.find(key) != data.end();
}

void SimpleJson::clear() {
    data.clear();
}
