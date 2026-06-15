#include "EditorUI.h"
#include <filesystem>
#include <Windows.h>

namespace fs = std::filesystem;

void EditorUI::Initialize() {
    logger::info("EditorUI initialized");
    RefreshConfigList();
}

void EditorUI::Toggle() {
    visible = !visible;
    logger::info("Editor toggled: {}", visible ? "ON" : "OFF");
}

std::string EditorUI::GetPluginsFolder() {
    // Get the SKSE plugins folder
    // Typically: <Skyrim Data>/SKSE/Plugins
    char skyrimPath[MAX_PATH];
    DWORD size = GetEnvironmentVariableA("SKYRIM_MODS_FOLDER", skyrimPath, MAX_PATH);
    
    if (size > 0) {
        return std::string(skyrimPath) + "\\SKSE Menu Framework\\SKSE\\Plugins";
    }

    // Fallback: try to find from mod folder
    return "";
}

void EditorUI::RefreshConfigList() {
    configFiles.clear();
    selectedConfigIndex = -1;

    std::string pluginsFolder = GetPluginsFolder();
    if (pluginsFolder.empty()) {
        logger::warn("Could not determine plugins folder");
        return;
    }

    try {
        if (fs::exists(pluginsFolder)) {
            for (const auto& entry : fs::directory_iterator(pluginsFolder)) {
                if (entry.is_regular_file() && entry.path().extension() == ".ini") {
                    configFiles.push_back(entry.path().string());
                    logger::info("Found INI file: {}", entry.path().filename().string());
                }
            }
        }
    } catch (const std::exception& e) {
        logger::error("Error scanning plugins folder: {}", e.what());
    }
}

void EditorUI::LoadConfigFromFile(const std::string& filePath) {
    currentConfig = std::make_unique<INIConfig>(filePath);
    if (currentConfig->LoadFromFile()) {
        logger::info("Loaded config: {}", currentConfig->GetFileName());
    } else {
        logger::error("Failed to load config");
        currentConfig.reset();
    }
}

void EditorUI::Draw() {
    if (!visible) return;

    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("INI Config Editor", &visible, ImGuiWindowFlags_NoMove)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

        // Main layout: File list on left, Editor on right
        if (ImGui::BeginTable("EditorLayout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersV, ImVec2(0, -60))) {
            ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthFixed, 250);
            ImGui::TableSetupColumn("Editor", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            DrawConfigFileList();

            ImGui::TableSetColumnIndex(1);
            DrawConfigEditor();

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        DrawActionButtons();

        ImGui::End();
    }
}

void EditorUI::DrawConfigFileList() {
    ImGui::Text("Config Files");
    ImGui::Separator();

    if (ImGui::BeginListBox("##configlist", ImVec2(-1, -40))) {
        for (int i = 0; i < configFiles.size(); i++) {
            bool is_selected = (selectedConfigIndex == i);
            std::string display_name = fs::path(configFiles[i]).filename().string();

            if (ImGui::Selectable(display_name.c_str(), is_selected)) {
                selectedConfigIndex = i;
                LoadConfigFromFile(configFiles[i]);
                selectedSectionIndex = 0;
            }
        }
        ImGui::EndListBox();
    }

    // Refresh button
    if (ImGui::Button("Refresh##files", ImVec2(-1, 0))) {
        RefreshConfigList();
    }
}

void EditorUI::DrawConfigEditor() {
    if (!currentConfig) {
        ImGui::TextDisabled("Select a config file to edit");
        return;
    }

    ImGui::Text("File: %s", currentConfig->GetFileName().c_str());
    ImGui::Separator();

    DrawSectionTabs();
    ImGui::Separator();
    DrawPropertyPanel();
}

void EditorUI::DrawSectionTabs() {
    const auto& sections = currentConfig->GetSections();

    if (ImGui::BeginTabBar("##sections", ImGuiTabBarFlags_None)) {
        for (int i = 0; i < sections.size(); i++) {
            if (ImGui::BeginTabItem(sections[i].name.c_str())) {
                selectedSectionIndex = i;
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

void EditorUI::DrawPropertyPanel() {
    if (!currentConfig || selectedSectionIndex < 0) {
        return;
    }

    const auto& sections = currentConfig->GetSections();
    if (selectedSectionIndex >= sections.size()) {
        return;
    }

    auto* section = const_cast<INISection*>(&sections[selectedSectionIndex]);

    ImGui::BeginChild("Properties", ImVec2(0, 0), true);
    {
        for (auto& [key, value] : section->values) {
            DrawINIValue(section, value);
        }
    }
    ImGui::EndChild();
}

void EditorUI::DrawINIValue(INISection* section, INIValue& value) {
    ImGui::PushID(value.key.c_str());

    if (value.type == "bool") {
        bool bValue = value.AsBool();
        if (ImGui::Checkbox(value.key.c_str(), &bValue)) {
            section->values[value.key].value = bValue ? "1" : "0";
        }
    } else if (value.type == "int") {
        int iValue = value.AsInt();
        if (ImGui::SliderInt(value.key.c_str(), &iValue, -1000, 1000)) {
            section->values[value.key].value = std::to_string(iValue);
        }
    } else if (value.type == "float") {
        float fValue = value.AsFloat();
        if (ImGui::SliderFloat(value.key.c_str(), &fValue, -100.0f, 100.0f)) {
            section->values[value.key].value = std::to_string(fValue);
        }
    } else {
        static std::map<std::string, std::array<char, 256>> stringBuffers;
        
        if (stringBuffers.find(value.key) == stringBuffers.end()) {
            auto& buffer = stringBuffers[value.key];
            strcpy_s(buffer.data(), buffer.size(), value.value.c_str());
        }

        auto& buffer = stringBuffers[value.key];
        if (ImGui::InputText(value.key.c_str(), buffer.data(), buffer.size())) {
            section->values[value.key].value = std::string(buffer.data());
        }
    }

    ImGui::PopID();
}

void EditorUI::DrawActionButtons() {
    float buttonWidth = ImGui::GetContentRegionAvail().x / 3 - 4;

    if (ImGui::Button("Save", ImVec2(buttonWidth, 0))) {
        if (currentConfig) {
            currentConfig->SaveToFile();
            logger::info("Config saved");
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reload", ImVec2(buttonWidth, 0))) {
        if (currentConfig) {
            currentConfig->LoadFromFile();
            logger::info("Config reloaded");
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
        Toggle();
    }
}