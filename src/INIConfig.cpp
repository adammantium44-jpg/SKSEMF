#include "INIConfig.h"
#include <algorithm>

INIConfig::INIConfig(const std::string& filePath)
    : filePath(filePath) {
    // Extract filename from path
    size_t lastSlash = filePath.find_last_of("\\/");
    fileName = (lastSlash == std::string::npos) ? filePath : filePath.substr(lastSlash + 1);
}

bool INIConfig::LoadFromFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        logger::warn("Failed to open INI file: {}", filePath);
        return false;
    }

    sections.clear();
    std::string line;
    INISection* currentSection = nullptr;

    while (std::getline(file, line)) {
        line = Trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        // Check for section header [SectionName]
        if (line[0] == '[' && line[line.length() - 1] == ']') {
            std::string sectionName = line.substr(1, line.length() - 2);
            sections.emplace_back();
            currentSection = &sections.back();
            currentSection->name = sectionName;
        }
        // Parse key=value pair
        else if (currentSection != nullptr) {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = Trim(line.substr(0, eqPos));
                std::string value = Trim(line.substr(eqPos + 1));

                INIValue iniValue;
                iniValue.key = key;
                iniValue.value = value;
                iniValue.type = DetectType(value);

                currentSection->values[key] = iniValue;
            }
        }
    }

    file.close();
    logger::info("Successfully loaded INI file: {} with {} sections", fileName, sections.size());
    return true;
}

bool INIConfig::SaveToFile() {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        logger::warn("Failed to open INI file for writing: {}", filePath);
        return false;
    }

    for (const auto& section : sections) {
        file << "[" << section.name << "]\n";

        for (const auto& [key, value] : section.values) {
            file << key << "=" << value.value << "\n";
        }

        file << "\n";
    }

    file.close();
    logger::info("Successfully saved INI file: {}", fileName);
    return true;
}

INISection* INIConfig::GetSection(const std::string& sectionName) {
    for (auto& section : sections) {
        if (section.name == sectionName) {
            return &section;
        }
    }
    return nullptr;
}

INIValue* INIConfig::GetValue(const std::string& sectionName, const std::string& keyName) {
    auto* section = GetSection(sectionName);
    if (section != nullptr) {
        auto it = section->values.find(keyName);
        if (it != section->values.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

void INIConfig::SetValue(const std::string& sectionName, const std::string& keyName, const std::string& value) {
    auto* section = GetSection(sectionName);
    if (section != nullptr) {
        INIValue& val = section->values[keyName];
        val.key = keyName;
        val.value = value;
        val.type = DetectType(value);
    }
}

void INIConfig::SetValue(const std::string& sectionName, const std::string& keyName, float value) {
    SetValue(sectionName, keyName, std::to_string(value));
}

void INIConfig::SetValue(const std::string& sectionName, const std::string& keyName, int value) {
    SetValue(sectionName, keyName, std::to_string(value));
}

void INIConfig::SetValue(const std::string& sectionName, const std::string& keyName, bool value) {
    SetValue(sectionName, keyName, value ? "1" : "0");
}

std::string INIConfig::Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string INIConfig::DetectType(const std::string& value) {
    // Check if it's a boolean
    if (value == "0" || value == "1" || value == "true" || value == "false" ||
        value == "True" || value == "False" || value == "TRUE" || value == "FALSE") {
        return "bool";
    }

    // Check if it's a float
    try {
        size_t idx;
        std::stof(value, &idx);
        if (idx == value.length()) {
            if (value.find('.') != std::string::npos) {
                return "float";
            }
            return "int";
        }
    } catch (...) {
    }

    return "string";
}