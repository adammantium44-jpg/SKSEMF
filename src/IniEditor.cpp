#include "IniEditor.h"
#include "IniFile.h"
#include "IniBrowser.h"
#include <imgui/imgui.h>

static IniFile g_ini;
static bool g_loaded = false;
static std::string g_currentPath;
static std::vector<std::string> g_files;
static int g_selectedIndex = -1;

void IniEditor::Render() {
    if (g_files.empty()) {
        g_files = GetIniFilesUnderSKSEPlugins();
        if (!g_files.empty())
            g_selectedIndex = 0;
    }

    ImGui::Text("INI Editor (Data/SKSE/Plugins)");
    ImGui::Separator();

    if (g_files.empty()) {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "No .ini files found.");
        return;
    }

    const char* current = g_selectedIndex >= 0 ? g_files[g_selectedIndex].c_str() : "None";

    if (ImGui::BeginCombo("INI File", current)) {
        for (int i = 0; i < static_cast<int>(g_files.size()); ++i) {
            bool sel = (i == g_selectedIndex);
            if (ImGui::Selectable(g_files[i].c_str(), sel)) {
                g_selectedIndex = i;
            }
            if (sel)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("Rescan")) {
        g_files.clear();
        g_loaded = false;
        g_selectedIndex = -1;
    }

    if (g_selectedIndex < 0)
        return;

    if (ImGui::Button("Load")) {
        g_currentPath = g_files[g_selectedIndex];
        g_loaded = g_ini.Load(g_currentPath);
    }

    if (!g_loaded) {
        ImGui::SameLine();
        ImGui::TextDisabled("No INI loaded.");
        return;
    }

    ImGui::Text("Editing: %s", g_currentPath.c_str());
    ImGui::Spacing();

    ImGui::BeginChild("IniScroll", ImVec2(0, -40.0f), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& [sectionName, section] : g_ini.GetSections()) {
        if (!sectionName.empty()) {
            ImGui::Separator();
            ImGui::Text("[%s]", sectionName.c_str());
        }
        for (const auto& [key, value] : section) {
            ImGui::PushID((sectionName + ":" + key).c_str());
            ImGui::Text("%s", key.c_str());
            ImGui::SameLine(250.0f);
            char buf[256];
            std::snprintf(buf, sizeof(buf), "%s", value.c_str());
            if (ImGui::InputText("##value", buf, sizeof(buf))) {
                g_ini.Set(sectionName, key, buf);
            }
            ImGui::PopID();
        }
    }

    ImGui::EndChild();

    ImGui::Separator();
    if (ImGui::Button("Save")) {
        g_ini.Save(g_currentPath);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload from disk")) {
        g_ini.Load(g_currentPath);
    }
}
