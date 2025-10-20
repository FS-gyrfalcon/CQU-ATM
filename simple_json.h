#ifndef SIMPLE_JSON_H
#define SIMPLE_JSON_H

#include <string>
#include <map>
#include <fstream>

class SimpleJson {
private:
    std::map<std::string, std::string> data;

public:
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    bool hasKey(const std::string& key) const;
    void clear();
    std::string findAccountByIdCard(const std::string& idCard);
};

#endif