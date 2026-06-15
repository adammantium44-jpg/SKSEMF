#include "PCH.h"
#include "EditorUI.h"

namespace {
    void MessageHandler(SKSE::MessagingInterface::Message* message) {
        switch (message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            logger::info("Game data loaded");
            EditorUI::GetSingleton().Initialize();
            break;
        }
    }

    void OnKeyEvent(RE::ButtonEvent* event) {
        if (event && event->IsPressed()) {
            // F2 to toggle editor (keycode 0x71)
            if (event->GetKeyCode() == 0x71) {
                EditorUI::GetSingleton().Toggle();
                logger::info("Editor toggle key pressed");
            }
        }
    }
}

extern "C" {
    BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
        if (reason == DLL_PROCESS_ATTACH) {
#ifdef _DEBUG
            while (!IsDebuggerPresent()) {
                Sleep(100);
            }
#endif
        }
        return TRUE;
    }

    DLLEXPORT constinit auto SKSEPlugin_Version = []() {
        SKSE::PluginVersionData v;
        v.PluginVersion({0, 2, 0});
        v.PluginName("SKSE INI Config Editor");
        v.AuthorName("AdamMantium");
        v.UsesAddressLibrary(true);
        v.UsesStructsDB(true);
        v.CompatibleVersions({SKSE::RUNTIME_1_6_640});
        return v;
    }();

    DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse) {
        SKSE::Init(skse);

        auto* msg = SKSE::GetMessagingInterface();
        if (!msg->RegisterListener(MessageHandler)) {
            logger::error("Failed to register message listener");
            return false;
        }

        auto* input = SKSE::GetInputInterface();
        if (!input->AddEventSink<RE::ButtonEvent>(OnKeyEvent)) {
            logger::error("Failed to register input sink");
            return false;
        }

        logger::info("SKSE INI Config Editor loaded successfully!");
        logger::info("Press F2 to toggle the editor");
        return true;
    }

    DLLEXPORT constinit auto SKSEPlugin_Query = []() {
        auto query = SKSE::QueryInterface<0>();
        return query;
    }();
}
