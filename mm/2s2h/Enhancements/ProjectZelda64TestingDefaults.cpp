// ProjectZelda64 testing defaults for 2S2H.
// Keep this file limited to branch-stable CVars and small local hooks used by Angelo's integration flow.

#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64input.h"
}

namespace {
constexpr const char* kEnableTestingDefaultsCVar = "gProjectZelda64.EnableTestingDefaults";
constexpr const char* kSwapABControlsCVar = "gProjectZelda64.SwapABControls";

void SwapButtonMaskAB(uint16_t& buttons) {
    const bool hadA = (buttons & BTN_A) != 0;
    const bool hadB = (buttons & BTN_B) != 0;

    buttons &= ~(BTN_A | BTN_B);

    if (hadA) {
        buttons |= BTN_B;
    }

    if (hadB) {
        buttons |= BTN_A;
    }
}

void RegisterProjectZelda64ABSwap() {
    COND_HOOK(OnPassPlayerInputs, CVarGetInteger(kEnableTestingDefaultsCVar, 1) && CVarGetInteger(kSwapABControlsCVar, 1),
              [](Input* input) {
                  SwapButtonMaskAB(input->cur.button);
                  SwapButtonMaskAB(input->press.button);
                  SwapButtonMaskAB(input->rel.button);
              });
}

void ApplyProjectZelda64TestingDefaults() {
    // Graphics defaults for repeated ProjectZelda64 smoke tests.
    CVarSetInteger("gSettings.MSAAValue", 8);
    CVarSetInteger("gInterpolationFPS", 360);

    // Controller defaults: match Angelo's OoT ProjectZelda64 controller preference.
    CVarSetInteger(kSwapABControlsCVar, 1);

    // Player movement defaults.
    CVarSetInteger("gEnhancements.Player.ClimbSpeed", 5);

    // Speed modifier defaults. Mode 2 is hold-buttons mode; Modifier 1 is the same default button used by 2S2H.
    CVarSetInteger("gCheats.SpeedModifier.Enabled", 1);
    CVarSetInteger("gCheats.SpeedModifier.Mode", 2);
    CVarSetFloat("gCheats.SpeedModifier.Value", 6.0f);
    CVarSetInteger("gCheats.SpeedModifier.Btn", BTN_CUSTOM_MODIFIER1);

    // Keep these compatibility keys set for forks/patches that expose jump-safe speed modifier options.
    CVarSetInteger("gCheats.SpeedModifier.DontAffectJumpVelocity", 1);
    CVarSetInteger("gCheats.SpeedModifier.DoNotAffectJumpVelocity", 1);
    CVarSetInteger("gCheats.SpeedModifier.DontAffectJumpDistance", 1);
    CVarSetInteger("gCheats.SpeedModifier.DoNotAffectJumpDistance", 1);

    // Time Saver defaults to remove repeated setup friction while testing MM portal routing.
    CVarSetInteger("gEnhancements.Cutscenes.SkipEntranceCutscenes", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipFirstCycle", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipIntroSequence", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipIntro", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 1);
    CVarSetInteger("gEnhancements.TimeSavers.SkipCutscene.Intro", 1);
    CVarSetInteger("gEnhancements.TimeSavers.SkipCutscene.Entrances", 1);
    CVarSetInteger("gEnhancements.TimeSavers.SkipOwlInteractions", 1);

    // Dialogue and gameplay defaults.
    CVarSetInteger("gEnhancements.Dialogue.FastText", 1);
    CVarSetInteger("gEnhancements.TextSpeed", 5);
    CVarSetInteger("gEnhancements.SlowTextSpeed", 5);
    CVarSetInteger("gEnhancements.Dpad.DpadEquips", 1);
}

void RegisterProjectZelda64TestingDefaults() {
    if (CVarGetInteger(kEnableTestingDefaultsCVar, 1)) {
        ApplyProjectZelda64TestingDefaults();
    }

    RegisterProjectZelda64ABSwap();
}
} // namespace

static RegisterShipInitFunc initFunc(RegisterProjectZelda64TestingDefaults, { kEnableTestingDefaultsCVar });
