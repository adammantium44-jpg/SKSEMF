#include "IniBrowser.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

std::vector<std::string> GetIniFilesUnderSKSEPlugins() {
    std::vector<std::string> result;

    fs::path root = fs::current_path();                // game root
    fs::path base = root / "Data" / "SKSE" / "Plugins";

    if (!fs::exists(base) || !fs::is_directory(base))
        return result;

    for (const auto& entry : fs::recursive_directory_iterator(base)) {
        if (!entry.is_regular_file())
            continue;
        if (entry.path().extension() == ".ini") {
            fs::path rel = fs::relative(entry.path(), root);
            result.push_back(rel.string());
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}
