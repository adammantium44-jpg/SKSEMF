#pragma once
#include <string>
#include <map>
#include <optional>

class IniFile {
public:
    using Section = std::map<std::string, std::string>;

    bool Load(const std::string& path);
    bool Save(const std::string& path) const;

    std::optional<std::string> Get(const std::string& section, const std::string& key) const;
    void Set(const std::string& section, const std::string& key, const std::string& value);

    const std::map<std::string, Section>& GetSections() const { return sections; }

private:
    std::map<std::string, Section> sections;
};
