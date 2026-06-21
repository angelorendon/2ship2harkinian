#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
#include "overlays/gamestates/ovl_select/z_select.h"
#include "overlays/gamestates/ovl_title/z_title.h"

extern GameState* gGameState;
extern SaveContext gSaveContext;
}

namespace {

constexpr const char* kLaunchIntentFileName = "projectzelda64_launch_intent.json";
constexpr int kSouthClockTownSpawn = 0;

bool gProjectZelda64IntentConsumed = false;

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

bool ConsumeLaunchIntentFromPath(const std::filesystem::path& path) {
    std::error_code existsError;
    if (!std::filesystem::exists(path, existsError) || existsError) {
        return false;
    }

    const std::string intent = ReadWholeFile(path);

    const bool isMm =
        Contains(intent, "\"targetGame\": \"mm\"") ||
        Contains(intent, "\"targetGame\":\"mm\"");

    const bool isClockTownTarget =
        Contains(intent, "ENTR_SCENE_SOUTH_CLOCK_TOWN") ||
        Contains(intent, "mm.clock_town") ||
        Contains(intent, "Clock Town");

    if (!isMm || !isClockTownTarget) {
        return false;
    }

    std::error_code removeError;
    std::filesystem::remove(path, removeError);

    std::cout << "[ProjectZelda64] consumed MM launch intent: " << path.string() << '\n';
    return true;
}

bool TryConsumeClockTownLaunchIntent() {
    if (gProjectZelda64IntentConsumed) {
        return false;
    }

    const auto currentPath = std::filesystem::current_path();

    const bool consumed =
        ConsumeLaunchIntentFromPath(currentPath / kLaunchIntentFileName) ||
        ConsumeLaunchIntentFromPath(currentPath / "x64" / "Release" / kLaunchIntentFileName) ||
        ConsumeLaunchIntentFromPath(currentPath / "build" / "x64" / "Release" / kLaunchIntentFileName) ||
        ConsumeLaunchIntentFromPath(currentPath / "extern" / "2ship2harkinian" / "x64" / "Release" / kLaunchIntentFileName) ||
        ConsumeLaunchIntentFromPath(currentPath / "extern" / "2ship2harkinian" / "build" / "x64" / "Release" / kLaunchIntentFileName);

    if (consumed) {
        gProjectZelda64IntentConsumed = true;
    }

    return consumed;
}

void PrepareClockTownSaveState() {
    Sram_InitNewSave();

    gSaveContext.gameMode = GAMEMODE_NORMAL;
    gSaveContext.sceneLayer = 0;
    gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, kSouthClockTownSpawn);
    gSaveContext.save.cutsceneIndex = 0;
    gSaveContext.save.hasTatl = true;
    gSaveContext.save.isFirstCycle = true;
    gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
    gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
    gSaveContext.save.saveInfo.playerData.threeDayResetCount = 1;
    gSaveContext.save.saveInfo.inventory.questItems |= (1 << QUEST_SONG_TIME) | (1 << QUEST_SONG_HEALING);
    gSaveContext.save.linkAge = 0;
    gSaveContext.save.time = CLOCK_TIME(8, 0);
    gSaveContext.save.day = 1;

    for (size_t i = 0; i < ARRAY_COUNT(gSaveContext.eventInf); i++) {
        gSaveContext.eventInf[i] = 0;
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
        gSaveContext.cycleSceneFlags[i].chest = gSaveContext.save.saveInfo.permanentSceneFlags[i].chest;
        gSaveContext.cycleSceneFlags[i].switch0 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0;
        gSaveContext.cycleSceneFlags[i].switch1 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1;
        gSaveContext.cycleSceneFlags[i].clearedRoom = gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom;
        gSaveContext.cycleSceneFlags[i].collectible = gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible;
    }

    gSaveContext.cycleSceneFlags[SCENE_INSIDETOWER].switch0 |= (1 << 0);
    gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].switch0 |= (1 << 2);
    gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].switch0 |= (1 << 0);

    SET_WEEKEVENTREG(WEEKEVENTREG_59_04);
    SET_WEEKEVENTREG(WEEKEVENTREG_31_04);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_EAST_CLOCK_TOWN);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WEST_CLOCK_TOWN);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_NORTH_CLOCK_TOWN);

    gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = Entrance_Create(ENTR_SCENE_SOUTH_CLOCK_TOWN, kSouthClockTownSpawn, 0);
    gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D);
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
    gSaveContext.respawnFlag = -8;
}

void ApplyClockTownLaunchIntentOnSaveLoad(s16) {
    if (TryConsumeClockTownLaunchIntent()) {
        PrepareClockTownSaveState();
        std::cout << "[ProjectZelda64] applied MM launch intent on save load\n";
    }
}

void BootDirectlyToClockTownIfIntentExists() {
    if (!TryConsumeClockTownLaunchIntent()) {
        return;
    }

    ConsoleLogoState* consoleLogoState = (ConsoleLogoState*)gGameState;

    if (!consoleLogoState->exit) {
        consoleLogoState->timer = 0;
        consoleLogoState->coverAlpha = 255;
        consoleLogoState->addAlpha = 255;
        consoleLogoState->visibleDuration = 0;
        consoleLogoState->exit = true;
    }

    PrepareClockTownSaveState();

    // Use dummy file number pattern used by 2S2H's WarpPoint boot helper.
    gSaveContext.fileNum = 0xFE;

    STOP_GAMESTATE(gGameState);
    MapSelect_LoadGame((MapSelectState*)gGameState, ENTRANCE(SOUTH_CLOCK_TOWN, kSouthClockTownSpawn), 0);

    gSaveContext.fileNum = 0xFF;

    GameInteractor_ExecuteOnSaveInit(gSaveContext.fileNum);
    GameInteractor_ExecuteOnSaveLoad(gSaveContext.fileNum);

    std::cout << "[ProjectZelda64] booting MM directly to South Clock Town / Clock Tower exterior\n";
}

void RegisterProjectZelda64LaunchIntent() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnConsoleLogoUpdate>(BootDirectlyToClockTownIfIntentExists);

    // Keep fallback behavior in case direct boot does not fire on a given startup path.
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(ApplyClockTownLaunchIntentOnSaveLoad);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(ApplyClockTownLaunchIntentOnSaveLoad);
}

static RegisterShipInitFunc initFunc(RegisterProjectZelda64LaunchIntent);

} // namespace
