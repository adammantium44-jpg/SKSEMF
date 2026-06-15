#pragma once

#include "PCH.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

// Simple INI value holder
struct INIValue {
    std::string key;
    std::string value;
    std::string type;  // "bool", "int", "float", "string"

    // Helper methods untuk type conversion
    bool AsBool() const {
        return value == "1" || value == "true" || value == "True" || value == "TRUE";
    }

    int AsInt() const {
        try {
            return std::stoi(value);
        } catch (...) {
            return 0;
        }
    }

    float AsFloat() const {
        try {
            return std::stof(value);
        } catch (...) {
            return 0.0f;
        }
    }

    std::string AsString() const {
        return value;
    }
};

// INI section holder
struct INISection {
    std::string name;
    std::map<std::string, INIValue> values;
};

// Main INI config class
class INIConfig {
public:
    INIConfig(const std::string& filePath);
    ~INIConfig() = default;

    bool LoadFromFile();
    bool SaveToFile();

    const std::vector<INISection>& GetSections() const { return sections; }
    INISection* GetSection(const std::string& sectionName);
    INIValue* GetValue(const std::string& sectionName, const std::string& keyName);

    void SetValue(const std::string& sectionName, const std::string& keyName, const std::string& value);
    void SetValue(const std::string& sectionName, const std::string& keyName, float value);
    void SetValue(const std::string& sectionName, const std::string& keyName, int value);
    void SetValue(const std::string& sectionName, const std::string& keyName, bool value);

    const std::string& GetFilePath() const { return filePath; }
    const std::string& GetFileName() const { return fileName; }

private:
    std::string filePath;
    std::string fileName;
    std::vector<INISection> sections;

    std::string Trim(const std::string& str);
    std::string DetectType(const std::string& value);
};