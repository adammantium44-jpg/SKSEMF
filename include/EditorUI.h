#pragma once

#include "PCH.h"
#include "INIConfig.h"
#include <memory>
#include <vector>
#include "imgui.h"

class EditorUI {
public:
    static EditorUI& GetSingleton() {
        static EditorUI singleton;
        return singleton;
    }

    void Initialize();
    void Draw();
    void Toggle();
    bool IsVisible() const { return visible; }

    void LoadConfigFromFile(const std::string& filePath);
    void RefreshConfigList();

private:
    EditorUI() = default;
    ~EditorUI() = default;

    EditorUI(const EditorUI&) = delete;
    EditorUI(EditorUI&&) = delete;
    EditorUI& operator=(const EditorUI&) = delete;
    EditorUI& operator=(EditorUI&&) = delete;

    bool visible = false;
    int selectedConfigIndex = -1;
    int selectedSectionIndex = -1;

    std::vector<std::string> configFiles;  // List of INI files
    std::unique_ptr<INIConfig> currentConfig;

    // UI drawing functions
    void DrawMainWindow();
    void DrawConfigFileList();
    void DrawConfigEditor();
    void DrawSectionTabs();
    void DrawPropertyPanel();
    void DrawActionButtons();

    // Helper
    void DrawINIValue(INISection* section, INIValue& value);
    std::string GetPluginsFolder();
};
