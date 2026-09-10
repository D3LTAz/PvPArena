# Claude Code kickoff prompt — Free Asset Integration

Paste this into Claude Code running on your machine, inside the `PvPArena` repo, once you've pulled the assets into the project via Fab (Add to Library → Add to Project, or manual Content Browser import).

---

You are the engineering lead on PvP Arena, a tactical multiplayer FPS in Unreal Engine 5. Your job right now is a narrow, content-only task: wire the free community assets that have already been imported into the project's Content folder into the existing C++ data-asset and component architecture. This is not a gameplay-logic task — no netcode, replication, hit-registration, or AI behavior should change.

Before touching anything, read these files in the repo in full:
- `docs/free-community-assets-milestone2.md` — the full list of what's approved, what's a backup option, and what's explicitly rejected (there is a rejected asset in here for IP reasons — do not use it or anything like it even if it's sitting in the Content folder)
- `docs/decisions.md` — search for the 2026-09-09 entries, which log exactly what was live-verified free, what license terms apply, and any caveats (poly count, missing rig, UE version compatibility) for each asset
- `docs/game-director-stage1-scaffold.md` — the `UPvPWeaponData`, `UPvPWeaponComponent`, `APvPCharacter`, `UPvPHealthComponent` class definitions these assets plug into
- `docs/game-director-milestone2-demo.md` — if any AI/menu-facing widget needs placeholder art, check this spec for what's expected there

Hard constraints, non-negotiable:
1. Every Milestone 1 class signature stays unchanged. You are assigning mesh/material/Niagara references inside existing data assets and Blueprints, not modifying C++ class structure. If a class genuinely needs a new field to hold a reference, stop and flag it — don't add it silently.
2. Use only assets confirmed free in `docs/free-community-assets-milestone2.md`. If you find something in the Content folder that isn't listed there, stop and ask before wiring it in — don't assume it's cleared.
3. Do not use anything resembling the rejected "CS2/CS:GO Printstream" asset or its sibling listings (Asiimov/Blaze/Cyrex) — these replicate a competitor's specific paid cosmetic and are flagged as an IP risk, not a style choice.
4. The Deadghost "FPS Weapon Bundle" is the primary weapon source; the ALowPolyCount rifle/SMG are fallbacks only if the Deadghost rig doesn't import cleanly. Don't blend both into the same weapon slot without flagging why.
5. The Quantum Modular Character Free Sample is the primary character mesh; Manny/Quinn + Mixamo is the documented fallback if the Quantum rig's animation retargeting doesn't work cleanly with the existing `APvPCharacter` anim setup.
6. If any asset's import breaks compilation, a smoke-test playthrough, or existing animation blueprints, stop and report it — don't work around it by modifying Milestone 1 systems.

Work order:
1. **Character**: Import the Quantum Modular Character Free Sample's skeletal mesh into `APvPCharacter`'s mesh component. Confirm it binds to the existing animation blueprint (retarget via IK Retargeter if the skeleton doesn't match Manny/Quinn directly). If it doesn't work cleanly within reasonable effort, fall back to Manny/Quinn + Mixamo and say so.
2. **Weapon**: Create/update a `UPvPWeaponData` instance pointing at the Deadghost FPS Weapon Bundle's primary rifle mesh, and wire its muzzle socket to the Fab Niagara muzzle flash system on fire. Confirm the mesh's rig is compatible with the existing first-person arms/weapon-attach socket setup.
3. **VFX**: Confirm the muzzle flash Niagara system fires correctly through `UPvPWeaponComponent`'s existing fire-effect hook (or the nearest equivalent) — this should be a reference swap, not new VFX logic.
4. **Environment props**: Place the free industrial barrel and (if claimed before the 09/22/2026 deadline) the Sierra Division "Industrial Infrastructure" kit pieces into the Substation and/or Ring map files as static mesh actors — this is level-designer's map, so add props without altering existing geometry/blockout unless asked.
5. **Smoke test**: boot into a match, confirm the character renders and animates, the weapon fires with the muzzle VFX visible, and nothing from Milestone 1's netcode/replication broke.

After finishing: tell me what got wired in, what fell back to the backup option (and why), and anything that needs a real decision (e.g., "Quantum rig didn't retarget cleanly, used Manny/Quinn instead — needs technical-artist to confirm this is fine going forward").

Log every real decision (fallback used, asset swapped, anything skipped) to `docs/decisions.md` the same way the existing entries are written — date, decision, reason, who it affects.
