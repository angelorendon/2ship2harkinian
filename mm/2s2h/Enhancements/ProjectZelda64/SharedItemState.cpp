#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

extern "C" {
#include "variables.h"
#include "macros.h"
}

namespace {

constexpr const char* kSharedStateFileName = "projectzelda64_shared_state.json";

bool gAppliedSharedItemState = false;

std::string ReadWholeFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return {};
    }

    return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

bool Contains(const std::string& text, const char* value) {
    return text.find(value) != std::string::npos;
}

void AddPathIfUnique(std::vector<std::filesystem::path>& paths, std::set<std::string>& seen,
                     const std::filesystem::path& path) {
    const auto key = path.lexically_normal().string();
    if (seen.insert(key).second) {
        paths.push_back(path.lexically_normal());
    }
}

std::vector<std::filesystem::path> SharedStatePaths() {
    std::vector<std::filesystem::path> paths;
    std::set<std::string> seen;

    std::error_code currentPathError;
    auto base = std::filesystem::current_path(currentPathError);
    if (currentPathError) {
        return paths;
    }

    for (int depth = 0; depth < 8 && !base.empty(); depth++) {
        AddPathIfUnique(paths, seen, base / kSharedStateFileName);
        AddPathIfUnique(paths, seen, base / "x64" / "Release" / kSharedStateFileName);
        AddPathIfUnique(paths, seen, base / "build" / "x64" / "Release" / kSharedStateFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "Shipwright" / "x64" / "Release" / kSharedStateFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "Shipwright" / "build" / "x64" / "Release" / kSharedStateFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "2ship2harkinian" / "x64" / "Release" / kSharedStateFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "2ship2harkinian" / "build" / "x64" / "Release" / kSharedStateFileName);

        const auto parent = base.parent_path();
        if (parent == base) {
            break;
        }
        base = parent;
    }

    return paths;
}

bool SharedStateContains(const char* value) {
    for (const auto& path : SharedStatePaths()) {
        std::error_code existsError;
        if (!std::filesystem::exists(path, existsError) || existsError) {
            continue;
        }

        if (Contains(ReadWholeFile(path), value)) {
            std::cout << "[ProjectZelda64] read shared item state from " << path.string() << '\n';
            return true;
        }
    }

    return false;
}

void ApplySharedItemState(s16) {
    if (gAppliedSharedItemState) {
        return;
    }

    if (!SharedStateContains("\"mm.fierce_deity_mask\": true")) {
        return;
    }

    INV_CONTENT(ITEM_MASK_FIERCE_DEITY) = ITEM_MASK_FIERCE_DEITY;
    gAppliedSharedItemState = true;
    std::cout << "[ProjectZelda64] granted MM Fierce Deity Mask from shared state\n";
}

void RegisterProjectZelda64SharedItemState() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(ApplySharedItemState);
}

} // namespace

static RegisterShipInitFunc initFunc(RegisterProjectZelda64SharedItemState);
