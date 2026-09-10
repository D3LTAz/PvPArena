# Practice vs AI — shared-file audit + local rebuild

**Author:** studio-director (code audit; no UE Editor on this machine)
**Audience:** game-director
**Repo tip this report assumes:** `origin/master` at `7523847` (FIRE BUILD 4) or later
**Cannot do here:** compile or PIE. Unreal Engine 5.8 is not on the Linux cloud agent. The binary rebuild is a **Windows + VS2022** job.

---

## FEATURE

Confirm 1v1 vs AI uses the **same** character, weapons, input, HUD, and fire code as the rest of the practice slice — not a leftover Blueprint pawn or a second fire path — and tell game-director how to rebuild so PIE actually loads that C++.

## ARCHITECTURE

Practice vs AI **is** the game, not a parallel stack.

| Role | Class / asset | Same as player? |
| --- | --- | --- |
| Default map | `Content/Maps/Lvl_PracticeRing.umap` (`DefaultEngine.ini`) | n/a — only map |
| GameMode | `APvPPracticeGameMode` (`GlobalDefaultGameMode=/Script/PvPArena.PvPPracticeGameMode`) | n/a |
| Human pawn | `APvPCharacter` | — |
| Bot pawn | `BotPawnClass = APvPCharacter::StaticClass()` | **yes, same class** |
| Human controller | `APvPPlayerController` | — |
| Bot controller | `APvPAIController` (Tick aims + calls `Weapon->Fire()`) | different controller, **same pawn/weapon** |
| Health | `UPvPHealthComponent` on `APvPCharacter` | shared |
| Weapons | `UPvPWeaponComponent` + `DA_Weapon_Pistol` / `Rifle` / `Sniper` loaded in `APvPCharacter` ctor | **same component and data assets** |
| Fire | `UPvPWeaponComponent::Fire()` | player Tick / BindKey **and** AI Tick both call this |
| Input maps | `/Game/Input/IMC_Default`, `IMC_MouseLook`, `IA_*` via ConstructorHelpers | player only; bot never uses IMC |
| HUD / tutorial | `UPvPHUDWidget` / `UPvPTutorialWidget` created from `APvPPlayerController` | C++ widgets, not a BP HUD |

There is **no** `BP_ThirdPersonCharacter` / `BP_ThirdPersonPlayerController` in the live GameMode constructor. A stale comment in `PvPPracticeGameMode.h` still mentioned those Blueprint classes; that comment is wrong and is corrected in this change.

`APvPDeathmatchGameMode` only sets GameState/PlayerState. It does **not** spawn a different pawn.

Player fire (FIRE BUILD 4): `APvPCharacter::Tick` calls `Fire()` while the match is `InProgress` if LMB or F is held (BindKey + `IsInputKeyDown` + Slate pressed buttons). AI never goes through Tick fire-hold; it calls `Fire()` from `APvPAIController::Tick` after range/delay/cooldown, and `CycleWeapon()` when `CurrentAmmo <= 0`.

## TASKS

1. **Pull `master`** on `C:\Users\djkin\Documents\GitHub\PvPArena`. Confirm `git log -1 --oneline` shows `7523847` or a later merge that includes FIRE BUILD 4.
2. **Fully quit** Unreal Editor (hot-reload of a live `PvPArena` Editor module is how previous PIE sessions kept showing old fire/ammo).
3. Generate / open **Visual Studio 2022** (not VS2026). Engine pin is **UE 5.8**.
4. **Rebuild** target `PvPArenaEditor` Win64 Development (Clean + Rebuild if the HUD still lacks the BUILD 4 string).
5. Launch editor from that binary. PIE `Lvl_PracticeRing`.
6. Path: main menu → **03 1V1 VS AI** → difficulty → tutorial. Proof you have the new DLL:
   - Tutorial line: `FIRE BUILD 4 — press F to shoot (LMB too)`
   - In-match HUD: `Ammo: n / max   [BUILD4 F=fire]`
7. Press Enter to start. Confirm **F** and **LMB** decrement HUD ammo and spawn `FIRE ammo=` on-screen debug. Bot should still shoot (same `Fire()`).
8. If those strings are **missing**, the Editor is still running a pre–BUILD 4 module. Do not debug input until the fingerprint is visible.

## RISK

A cloud agent cannot Rebuild here. If Windows PIE still has no BUILD 4 fingerprint, the local binary is stale — not a second vs-AI code path.

## DONE WHEN

- Tutorial + HUD show BUILD 4 strings after a VS2022 rebuild.
- Player ammo decreases on F and LMB during `InProgress`.
- Bot still fires via `APvPAIController` → `UPvPWeaponComponent::Fire()` on the same `APvPCharacter`.
