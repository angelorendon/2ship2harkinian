#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include <fstream>

extern "C" {
#include "variables.h"
}

namespace {

constexpr const char* kPortalEventFileName = "projectzelda64_portal_event.json";
constexpr const char* kMmClockTowerDoorEvent = "mm.enter_clock_tower_door";

bool IsClockTowerDoorReturnTarget(s16 sceneId, s8 spawnNum) {
    return sceneId == SCENE_INSIDETOWER && spawnNum == 1;
}

void WriteClockTowerDoorPortalEvent() {
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
