#pragma once

#include <spdlog/sinks/basic_file_sink.h>

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

// ImGui headers
#include "imgui.h"

namespace logger = SKSE::log;
using namespace std::literals;

using FormID = RE::FormID;
using RefID = RE::FormID;

const RefID player_refid = 20;
