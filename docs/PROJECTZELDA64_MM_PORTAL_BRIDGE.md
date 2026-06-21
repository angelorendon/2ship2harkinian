# ProjectZelda64 MM portal bridge

2S2H emits a small portal event file when Majora's Mask loads the Clock Tower Interior through the South Clock Town Clock Tower door target.

## Event file

The bridge writes this file in the current 2S2H working directory:

```text
projectzelda64_portal_event.json
```

Example payload:

```json
{
  "schema": 1,
  "sourceGame": "mm",
  "event": "mm.enter_clock_tower_door",
  "sourceEntrance": "ENTRANCE(CLOCK_TOWER_INTERIOR, 1)",
  "sourceEntranceIndex": 49168,
  "targetGame": "oot",
  "targetPortal": "oot.market.happy_mask_shop_exterior"
}
```

ProjectZelda64 consumes this event as `TransitionEvent::MmEnterClockTowerDoor` and routes back to the OoT Market / Happy Mask Shop exterior target.

## Trigger

The hook currently fires on scene initialization for:

```text
SCENE_INSIDETOWER, spawn 1
```

This corresponds to:

```cpp
ENTRANCE(CLOCK_TOWER_INTERIOR, 1)
```

The ProjectZelda64 default watcher includes the 2S2H submodule path:

```powershell
ProjectZelda64.exe --watch-default-events
```

which watches:

```text
extern/2ship2harkinian/projectzelda64_portal_event.json
```

## Current limitation

This bridge only emits the transition event. It does not yet suspend 2S2H, launch/resume Shipwright, or move save state between engines.
