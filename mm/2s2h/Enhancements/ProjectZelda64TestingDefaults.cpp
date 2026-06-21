#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

namespace {
constexpr const char* kEnableTestingDefaultsCVar = "gProjectZelda64.EnableTestingDefaults";
constexpr const char* kSwapAAndBCVar = "gProjectZelda64.SwapAAndB";

void ApplyProjectZelda64TestingDefaults() {
    // Graphics defaults for repeated ProjectZelda64 smoke tests.
    CVarSetInteger("gSettings.MSAAValue", 8);
    CVarSetInteger("gInterpolationFPS", 360);

    // Time Saver defaults to remove repeated setup friction while testing MM portal routing.
    CVarSetInteger("gEnhancements.Cutscenes.SkipEntranceCutscenes", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipFirstCycle", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipIntroSequence", 1);

    // Keep the input swap discoverable/disableable without depending on controller profile serialization.
    CVarSetInteger(kSwapAAndBCVar, 1);
}

void SwapAAndBButtons(Input* input) {
    if (input == nullptr) {
        return;
    }

    auto swapButtons = [](decltype(input->cur.button)& buttons) {
        const bool hasA = (buttons & BTN_A) != 0;
        const bool hasB = (buttons & BTN_B) != 0;

        buttons &= static_cast<decltype(buttons)>(~(BTN_A | BTN_B));
        if (hasA) {
            buttons |= BTN_B;
        }
        if (hasB) {
            buttons |= BTN_A;
        }
    };

    swapButtons(input->cur.button);
    swapButtons(input->press.button);
    swapButtons(input->rel.button);
}

void RegisterProjectZelda64TestingDefaults() {
    if (CVarGetInteger(kEnableTestingDefaultsCVar, 1)) {
        ApplyProjectZelda64TestingDefaults();
    }

    COND_HOOK(OnPassPlayerInputs, CVarGetInteger(kSwapAAndBCVar, 1), [](Input* input) { SwapAAndBButtons(input); });
}
} // namespace

static RegisterShipInitFunc initFunc(RegisterProjectZelda64TestingDefaults,
                                     { kEnableTestingDefaultsCVar, kSwapAAndBCVar });
