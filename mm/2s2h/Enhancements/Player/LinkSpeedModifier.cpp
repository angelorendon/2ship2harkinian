#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/BenPort.h"

extern "C" {
#include "variables.h"
extern Input* sPlayerControlInput;
}

#define CVAR_SPEED_MODIFIER_MODE_NAME "gCheats.SpeedModifier.Mode"
#define CVAR_SPEED_MODIFIER_TOGGLE_NAME "gCheats.SpeedModifier.Toggle"
#define CVAR_SPEED_MODIFIER_VALUE_NAME "gCheats.SpeedModifier.Value"
#define CVAR_SPEED_MODIFIER_BTN_NAME "gCheats.SpeedModifier.Btn"
#define CVAR_SPEED_MODIFIER_DOESNT_CHANGE_JUMP_NAME "gCheats.SpeedModifier.DoesntChangeJump"
#define CVAR_SPEED_MODIFIER_ROLL_MOMENTUM_JUMP_NAME "gCheats.SpeedModifier.RollMomentumJump"
#define CVAR_SPEED_MODIFIER_MODE CVarGetInteger(CVAR_SPEED_MODIFIER_MODE_NAME, 0)
#define CVAR_SPEED_MODIFIER_TOGGLE CVarGetInteger(CVAR_SPEED_MODIFIER_TOGGLE_NAME, 0)
#define CVAR_SPEED_MODIFIER_VALUE CVarGetFloat(CVAR_SPEED_MODIFIER_VALUE_NAME, 1.0f)
#define CVAR_SPEED_MODIFIER_BTN CVarGetInteger(CVAR_SPEED_MODIFIER_BTN_NAME, BTN_CUSTOM_MODIFIER1)
#define CVAR_SPEED_MODIFIER_DOESNT_CHANGE_JUMP CVarGetInteger(CVAR_SPEED_MODIFIER_DOESNT_CHANGE_JUMP_NAME, 0)
#define CVAR_SPEED_MODIFIER_ROLL_MOMENTUM_JUMP CVarGetInteger(CVAR_SPEED_MODIFIER_ROLL_MOMENTUM_JUMP_NAME, 0)

bool btnHeldOrToggled = false;

static bool IsSpeedModifierActive() {
    return CVAR_SPEED_MODIFIER_MODE == 1 || btnHeldOrToggled;
}

void RegisterLinkSpeedModifier() {
    // Reset in case they disabled while toggled
    btnHeldOrToggled = false;

    COND_VB_SHOULD(VB_SPEED_MODIFIER_WALK, CVAR_SPEED_MODIFIER_MODE, {
        f32* speedTarget = va_arg(args, f32*);

        if (IsSpeedModifierActive()) {
            *speedTarget *= CVAR_SPEED_MODIFIER_VALUE;
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_SWIM, CVAR_SPEED_MODIFIER_MODE, {
        f32* incrStep = va_arg(args, f32*);
        f32* maxSpeed = va_arg(args, f32*);
        f32* speed = va_arg(args, f32*);
        f32* speedTarget = va_arg(args, f32*);
        f32 swimMod = 1.0f;

        // sControlInput is NULL to prevent inputs while surfacing after obtaining an underwater item so we want
        // to ignore it for that case
        if (sPlayerControlInput == NULL) {
            return;
        }

        if (IsSpeedModifierActive()) {
            swimMod *= CVAR_SPEED_MODIFIER_VALUE;
            *maxSpeed *= swimMod;
            Math_AsymStepToF(speed, *speedTarget * 0.8f * swimMod, *incrStep, (fabsf(*speed) * 0.02f) + 0.05f);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_JUMP, CVAR_SPEED_MODIFIER_MODE && CVAR_SPEED_MODIFIER_DOESNT_CHANGE_JUMP, {
        f32* speedXZ = va_arg(args, f32*);

        if (IsSpeedModifierActive() && CVAR_SPEED_MODIFIER_VALUE != 0.0f) {
            // Preserve vanilla jump distance by removing the speed modifier's multiplier before jump physics runs.
            *speedXZ /= CVAR_SPEED_MODIFIER_VALUE;
        }

        if (CVAR_SPEED_MODIFIER_ROLL_MOMENTUM_JUMP) {
            // This hook runs immediately before the jump launch is applied. The first implementation only raised
            // speedXZ just above the vanilla fast-jump threshold, which chooses the fast jump path but does not
            // add the extra horizontal distance a roll carries into a ledge jump. Use the same 1.5x horizontal
            // momentum scale used by rolling so ordinary ledge jumps behave like Link rolled into them, without
            // applying the speed modifier multiplier itself.
            const f32 fastJumpThreshold = IREG(66) / 100.0f;
            const f32 rollJumpMomentum = MAX(fastJumpThreshold * 1.5f, fastJumpThreshold + 0.01f);

            if (*speedXZ < rollJumpMomentum) {
                *speedXZ = rollJumpMomentum;
            }
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_SHORT_DROP, CVAR_SPEED_MODIFIER_MODE && CVAR_SPEED_MODIFIER_DOESNT_CHANGE_JUMP, {
        f32* speedXZ = va_arg(args, f32*);
        f32* actorSpeed = va_arg(args, f32*);

        if (IsSpeedModifierActive() && CVAR_SPEED_MODIFIER_VALUE != 0.0f) {
            // Short drops do not go through the normal ledge-jump launch path, so remove the active speed
            // modifier directly when the game enters the short-drop / landing-wait path.
            *speedXZ /= CVAR_SPEED_MODIFIER_VALUE;
            *actorSpeed /= CVAR_SPEED_MODIFIER_VALUE;
        }
    });

    COND_HOOK(OnPassPlayerInputs, CVAR_SPEED_MODIFIER_MODE >= 2, [](Input* input) {
        const s32 modMask = CVAR_SPEED_MODIFIER_BTN;

        if (modMask == 0) {
            btnHeldOrToggled = false;
            return;
        }

        if (CVAR_SPEED_MODIFIER_MODE == 2) {
            btnHeldOrToggled = CHECK_BTN_ANY(input->cur.button, modMask);
        } else if (CHECK_BTN_ANY(input->press.button, modMask)) {
            btnHeldOrToggled = !btnHeldOrToggled;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterLinkSpeedModifier, { CVAR_SPEED_MODIFIER_MODE_NAME });
