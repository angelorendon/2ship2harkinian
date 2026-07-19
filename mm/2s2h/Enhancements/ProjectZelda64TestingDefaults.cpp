// ProjectZelda64 testing defaults for 2S2H.
// Keep this file limited to branch-stable CVars used by Angelo's integration flow.

#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/ShipInit.hpp"

namespace {
constexpr const char* kEnableTestingDefaultsCVar = "gProjectZelda64.EnableTestingDefaults";

void ApplyProjectZelda64TestingDefaults() {
    // Graphics defaults for repeated ProjectZelda64 smoke tests.
    CVarSetInteger("gSettings.MSAAValue", 8);
    CVarSetInteger("gInterpolationFPS", 300);
    CVarSetInteger("gSdlWindowedFullscreen", 1);

    // Audio defaults: 2S2H is naturally louder than OoT in the ProjectZelda64 flow.
    // These are best-effort until we identify the exact 2S2H volume config key.
    CVarSetInteger("gSettings.Audio.MasterVolume", 80);
    CVarSetInteger("gSettings.Audio.MainVolume", 80);
    CVarSetInteger("gSettings.Audio.Volume", 80);
    CVarSetInteger("gAudio.MasterVolume", 80);
    CVarSetInteger("gAudio.MainVolume", 80);
    CVarSetInteger("gAudio.Volume", 80);
    CVarSetInteger("gMainVolume", 80);
    CVarSetInteger("gMasterVolume", 80);

    // Controller defaults: set likely A/B swap CVars without depending on branch-specific input headers.
    // The earlier custom input hook used z64input.h / OnPassPlayerInputs, which is not stable in this 2S2H branch.
    CVarSetInteger("gProjectZelda64.SwapABControls", 1);
    CVarSetInteger("gEnhancements.Controller.SwapAB", 1);
    CVarSetInteger("gEnhancements.Controls.SwapAB", 1);
    CVarSetInteger("gEnhancements.Input.SwapAB", 1);

    // Player movement defaults.
    CVarSetInteger("gEnhancements.Player.ClimbSpeed", 5);

    // Speed modifier defaults. Leave the button binding alone for now: in this 2S2H branch,
    // the guessed button CVars map to C-buttons rather than Modifier 2.
    CVarSetInteger("gCheats.SpeedModifier.Enabled", 1);
    CVarSetInteger("gCheats.SpeedModifier.Mode", 2);
    CVarSetFloat("gCheats.SpeedModifier.Value", 6.0f);

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
    CVarSetInteger("gEnhancements.Cutscenes.HideTitleCards", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipEnemyCutscenes", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 2);
    CVarSetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipOnePointCutscenes", 1);
    CVarSetInteger("gEnhancements.Cutscenes.SkipToFileSelect", 1);
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
}
} // namespace

static RegisterShipInitFunc initFunc(RegisterProjectZelda64TestingDefaults, { kEnableTestingDefaultsCVar });
