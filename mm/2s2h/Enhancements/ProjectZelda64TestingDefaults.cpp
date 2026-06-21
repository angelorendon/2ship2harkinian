// ProjectZelda64 testing defaults for 2S2H.
// Keep this file limited to branch-stable CVars. Do not use input/animation hooks here:
// this branch's hook signatures differ and MSVC treats those warnings as errors.

#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/ShipInit.hpp"

namespace {
constexpr const char* kEnableTestingDefaultsCVar = "gProjectZelda64.EnableTestingDefaults";

void ApplyProjectZelda64TestingDefaults() {
    // Graphics defaults for repeated ProjectZelda64 smoke tests.
    CVarSetInteger("gSettings.MSAAValue", 8);
    CVarSetInteger("gInterpolationFPS", 360);

    // Time Saver defaults to remove repeated setup friction while testing MM portal routing.
    CVarSetInteger("gEnhancements.Cutscenes.SkipEntranceCutscenes", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipFirstCycle", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipIntroSequence", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipIntro", 1);
    CVarSetInteger("gEnhancements.TimeSavers.SkipCutscene.Intro", 1);
    CVarSetInteger("gEnhancements.TimeSavers.SkipCutscene.Entrances", 1);
    CVarSetInteger("gEnhancements.TimeSavers.SkipOwlInteractions", 1);
    CVarSetInteger("gEnhancements.TextSpeed", 5);
    CVarSetInteger("gEnhancements.SlowTextSpeed", 5);

    // Speed modifier defaults.
    CVarSetInteger("gCheats.SpeedModifier.Enabled", 1);
    CVarSetInteger("gCheats.SpeedModifier.Mode", 3);
    CVarSetFloat("gCheats.SpeedModifier.Value", 5.0f);

    // Try both historical jump-safe names used across Shipwright/2S2H forks.
    CVarSetInteger("gCheats.SpeedModifier.DontAffectJumpVelocity", 1);
    CVarSetInteger("gCheats.SpeedModifier.DoNotAffectJumpVelocity", 1);
    CVarSetInteger("gCheats.SpeedModifier.DontAffectJumpDistance", 1);
    CVarSetInteger("gCheats.SpeedModifier.DoNotAffectJumpDistance", 1);
}

void RegisterProjectZelda64TestingDefaults() {
    if (CVarGetInteger(kEnableTestingDefaultsCVar, 1)) {
        ApplyProjectZelda64TestingDefaults();
    }
}
} // namespace

static RegisterShipInitFunc initFunc(RegisterProjectZelda64TestingDefaults, { kEnableTestingDefaultsCVar });
