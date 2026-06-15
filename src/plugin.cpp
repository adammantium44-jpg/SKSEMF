#include "Plugin.h"
#include "IniEditor.h"            // <-- tambah ini
#include "SKSEMenuFramework.h"    // <-- header dari SKSE Menu Framework (sudah ada di template)

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        // Contoh bawaan template
        UI::Example1::LookupForm();

        // Daftarkan section dan item baru di SKSE Menu Framework
        SKSEMenuFramework::SetSection("INI Tools");
        SKSEMenuFramework::AddSectionItem("SKSE Plugins INI Editor", IniEditor::Render);
    }

    if (message->type == SKSE::MessagingInterface::kPostLoad) {
        // kosong gapapa
    }
}

// Pastikan makro / signature SKSEPluginLoad yang benar di project kamu.
// Biasanya di Plugin.h sudah didefinisikan SKSEPluginLoad.
SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");

    UI::Register(); // bawaan template, biarin

    return true;
}
