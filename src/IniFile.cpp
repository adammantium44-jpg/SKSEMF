#include "IniFile.h"
#include <fstream>
#include <sstream>

static void Trim(std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) {
        s.clear();
    } else {
        s = s.substr(start, end - start + 1);
    }
}

bool IniFile::Load(const std::string& path) {
    sections.clear();

    std::ifstream file(path);
    if (!file.is_open())
        return false;

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) {
        Trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            Trim(currentSection);
            continue;
        }

        auto eqPos = line.find('=');
        if (eqPos == std::string::npos)
            continue;

        std::string key   = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        Trim(key);
        Trim(value);

        sections[currentSection][key] = value;
    }

    return true;
}

bool IniFile::Save(const std::string& path) const {
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open())
        return false;

    bool firstSection = true;
    for (const auto& [sectionName, section] : sections) {
        if (!firstSection)
            file << "\n";
        firstSection = false;

        if (!sectionName.empty())
            file << "[" << sectionName << "]\n";

        for (const auto& [key, value] : section) {
            file << key << " = " << value << "\n";
        }
    }

    return true;
}

std::optional<std::string> IniFile::Get(const std::string& section, const std::string& key) const {
    auto itSec = sections.find(section);
    if (itSec == sections.end())
        return std::nullopt;
    auto itKey = itSec->second.find(key);
    if (itKey == itSec->second.end())
        return std::nullopt;
    return itKey->second;
}

void IniFile::Set(const std::string& section, const std::string& key, const std::string& value) {
    sections[section][key] = value;
}
