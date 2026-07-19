#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include <filesystem>
#include <fstream>
#include <set>
#include <vector>

extern "C" {
#include "sfx.h"
#include "variables.h"
}

namespace {

constexpr const char* kPortalEventFileName = "projectzelda64_portal_event.json";
constexpr const char* kSharedRupeesFileName = "projectzelda64_shared_rupees.json";
constexpr const char* kMmClockTowerDoorEvent = "mm.enter_clock_tower_door";

bool IsClockTowerDoorReturnTarget(s16 sceneId, s8 spawnNum) {
    return sceneId == SCENE_INSIDETOWER && spawnNum == 1;
}

void AddPathIfUnique(std::vector<std::filesystem::path>& paths, std::set<std::string>& seen,
                     const std::filesystem::path& path) {
    const auto key = path.lexically_normal().string();
    if (seen.insert(key).second) {
        paths.push_back(path.lexically_normal());
    }
}

std::vector<std::filesystem::path> SharedRupeePaths() {
    std::vector<std::filesystem::path> paths;
    std::set<std::string> seen;

    std::error_code currentPathError;
    auto base = std::filesystem::current_path(currentPathError);
    if (currentPathError) {
        return paths;
    }

    for (int depth = 0; depth < 8 && !base.empty(); depth++) {
        AddPathIfUnique(paths, seen, base / kSharedRupeesFileName);
        AddPathIfUnique(paths, seen, base / "x64" / "Release" / kSharedRupeesFileName);
        AddPathIfUnique(paths, seen, base / "build" / "x64" / "Release" / kSharedRupeesFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "Shipwright" / "x64" / "Release" / kSharedRupeesFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "Shipwright" / "build" / "x64" / "Release" / kSharedRupeesFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "2ship2harkinian" / "x64" / "Release" / kSharedRupeesFileName);
        AddPathIfUnique(paths, seen, base / "extern" / "2ship2harkinian" / "build" / "x64" / "Release" / kSharedRupeesFileName);

        const auto parent = base.parent_path();
        if (parent == base) {
            break;
        }
        base = parent;
    }

    return paths;
}

void WriteSharedRupees() {
    const int rupees = gSaveContext.save.saveInfo.playerData.rupees;

    for (const auto& path : SharedRupeePaths()) {
        const auto parent = path.parent_path();
        std::error_code existsError;
        if (!parent.empty() && (!std::filesystem::exists(parent, existsError) || existsError)) {
            continue;
        }

        std::ofstream output(path, std::ios::trunc);
        if (!output.is_open()) {
            continue;
        }

        output << "{\n"
               << "  \"schema\": 1,\n"
               << "  \"sourceGame\": \"mm\",\n"
               << "  \"sharedRupees\": " << rupees << "\n"
               << "}\n";
    }
}

void WriteClockTowerDoorPortalEvent() {
    Audio_PlaySfx(NA_SE_OC_SECRET_WARP_OUT);
    WriteSharedRupees();

    std::ofstream output(kPortalEventFileName, std::ios::trunc);
    if (!output) {
        return;
    }

    output << "{\n"
           << "  \"schema\": 1,\n"
           << "  \"sourceGame\": \"mm\",\n"
           << "  \"event\": \"" << kMmClockTowerDoorEvent << "\",\n"
           << "  \"sourceEntrance\": \"ENTRANCE(CLOCK_TOWER_INTERIOR, 1)\",\n"
           << "  \"sourceEntranceIndex\": " << ENTRANCE(CLOCK_TOWER_INTERIOR, 1) << ",\n"
           << "  \"sharedRupees\": " << gSaveContext.save.saveInfo.playerData.rupees << ",\n"
           << "  \"targetGame\": \"oot\",\n"
           << "  \"targetPortal\": \"oot.market.happy_mask_shop_exterior\"\n"
           << "}\n";
}

void OnSceneInit(s16 sceneId, s8 spawnNum) {
    if (!IsClockTowerDoorReturnTarget(sceneId, spawnNum)) {
        return;
    }

    WriteClockTowerDoorPortalEvent();
}

void RegisterProjectZelda64PortalBridge() {
    COND_ID_HOOK(OnSceneInit, SCENE_INSIDETOWER, true, OnSceneInit);
}

} // namespace

static RegisterShipInitFunc initFunc(RegisterProjectZelda64PortalBridge);
