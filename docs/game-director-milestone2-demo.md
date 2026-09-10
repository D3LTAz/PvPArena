# PvP Arena — Milestone 2 / Public Demo Technical Breakdown

> **Sequencing note, added 2026-09-10 (see `docs/decisions.md`):** This doc was written assuming multiplayer deathmatch (LAN/matchmaking) already existed in the repo. It doesn't — the project pivoted to a single-player practice-vs-AI build on 2026-09-07, which has since shipped and been bug-fixed (see the repo's own decisions log). Per Daniel's 2026-09-10 call, practice-vs-AI ships first; multiplayer resumes as a later phase. Treat everything below as **Phase 3 reference material for when multiplayer work resumes** — not the current build order. The AI-specific sections (difficulty as data, behavior) are still broadly relevant even to the shipped single-player build and worth a skim; the LAN/matchmaking/menu-shell sections are not current work.

**Author:** game-director
**Status:** Ready to execute
**Builds on:** `/docs/game-director-stage1-scaffold.md` (Milestone 1). This document does not replace or relitigate that scaffold — every class in §2 of the M1 doc (GameMode/GameState/PlayerState/PlayerController, `APvPCharacter`, `UPvPHealthComponent`, the full weapon/hit-reg/lag-comp stack in §2.3, and the matchmaking subsystem in §2.4) is reused **as-is**, unchanged, by both LAN and AI modes. Milestone 2 only adds what's net-new: menu/UI flow, local session handling, AI opponents, a settings surface, and a tutorial *flow* (not the tutorial map itself).
**Scope guard:** Per `/docs/decisions.md` (2026-09-08, "Demo build scope"), Milestone 2 covers: title/main menu (Tutorial / 1v1 / 1v1 vs AI / Settings), local LAN play + single-player host, AI opponents with difficulty settings, a minimal settings menu, tutorial mode logic, and first-to-3-kills as the standard win condition. **Art polish, the tutorial map, and the "paintball ring" 1v1 map are explicitly out of scope for this document** — they belong to technical-artist and level-designer respectively. Where this plan touches their work, it names the handoff point and stops.

---

## 0. How to read this document

- §1 explains what's genuinely new vs. what M1 already built, up front, so nothing here gets re-priced as if it were greenfield.
- §2 is the class/system breakdown (same format as M1 scaffold §2).
- §3 is a short design note on the AI system specifically, since it's the one system in this milestone without an M1 precedent.
- §4 is the phase-by-phase task breakdown (same format as M1 scaffold §4).
- §5 is the closing deliverable block in studio-director format.

---

## 1. What's actually new here

Going through the decision log's demo requirements one at a time, against what M1 already built:

| Requirement | Status | Notes |
|---|---|---|
| Title screen + main menu (Tutorial / 1v1 / 1v1 AI / Settings) | **New** | No menu system exists in M1 — M1 has no pre-match UI beyond the format-picker widget (`UPvPFormatPickerWidget`, M1 §2.4), which is itself reused here as the "1v1" path's format confirmation, not replaced. |
| Local LAN play | **Mostly covered, one gap** | M1 §2.4 already built `OnlineSubsystemNull` session create/find with direct-IP connect as the fallback path. `OnlineSubsystemNull` sessions are process-local (LAN discovery is not automatic) — direct-IP connect *is* the LAN story, it just needs a small UI (host/join screen, IP entry) that doesn't exist yet. No new networking architecture required. See §2 `UPvPLANSessionWidget`. |
| Single-player host | **New, small** | "Single-player host" = a listen-server session with only the local player and (for AI mode) bot-controlled opponents — no new networking, just a `UPvPMatchmakingSubsystem` entry point that starts a listen server without queueing for a remote match. |
| AI opponents + difficulty | **New — the real work in this milestone** | No AI controller class exists anywhere in M1. See §3 for the design approach; flagged in RISK (§5) as the least-proven system in the whole plan. |
| Settings menu | **New, kept minimal** | Scoped down in §2 — see rationale there. |
| Tutorial mode flow | **New (logic only)** | The tutorial *map* is level-designer's (per decisions.md). This document owns the state machine that drives a player through it: step sequencing, input prompts, completion detection. Handoff point named explicitly in §2 and §4 Phase L. |
| First-to-3-kills standard mode | **Trivial — not new work** | M1 scaffold §2.1 already specifies a score-cap win condition on `APvPDeathmatchGameMode` (chosen over a time limit specifically because it was simpler to implement). This milestone just sets the cap value to 3 and confirms it end to end. One line of config, already-built code path. Do not let this get re-scoped as a feature — it isn't one. |

Net: three real new systems (menu/UI shell, LAN host/join UI, AI opponents), one small addition (settings data + widget), one flow (tutorial), one config value (kill cap). Everything else in this milestone is UI wrapped around M1 architecture that already exists.

---

## 2. Class / System Breakdown

New classes/assets only — nothing in M1 §2 is modified in place. Where an M2 class *talks to* an M1 class, that's called out in Responsibility.

### 2.1 Menu / UI Shell

| Class | Base | Responsibility |
|---|---|---|
| `UPvPMainMenuWidget` | `UUserWidget` (UMG) | Title screen. Four buttons: Tutorial, 1v1, 1v1 vs AI, Settings. Owns top-level navigation state (which sub-screen is active) via a simple widget-switcher pattern — no separate state machine class needed, this is UI-local state. Tutorial → loads tutorial flow (§2.4). 1v1 → `UPvPLANSessionWidget`. 1v1 vs AI → `UPvPAIMatchSetupWidget`. Settings → `UPvPSettingsWidget`. |
| `UPvPLANSessionWidget` | `UUserWidget` | Host / Join screen for local play. Host: starts a listen server via `UPvPMatchmakingSubsystem` (new entry point, §2.3) using the existing `FPvPLocalMatchProvider` (M1 §2.4) — no new session-management code. Join: text-entry IP field + Connect button, calls the standard `APlayerController::ClientTravel`/`ExecuteConsoleCommand("open <ip>")` path. This is the "direct-IP connect" fallback M1 already priced in as a matchmaking option — it just didn't have a UI in front of it yet. |
| `UPvPAIMatchSetupWidget` | `UUserWidget` | 1v1-vs-AI setup screen: difficulty selector (Easy/Normal/Hard — see §3.3), Start button. Starts a listen server with zero remote players and one `APvPAIController`-possessed opponent spawned by the GameMode (§2.3). |
| `UPvPSettingsWidget` | `UUserWidget` | See §2.2 for scoped fields. Persists via `UPvPSettingsSaveGame`. |
| `UPvPMenuNavSubsystem` (optional — see note) | `UGameInstanceSubsystem` | **Only build this if back/forward navigation between more than 2 menu levels turns out to need shared state** (e.g. remembering "which screen to return to on Back" across widgets). If the widget-switcher pattern in `UPvPMainMenuWidget` covers it — likely, given only 4 top-level destinations — skip this class entirely. Flagged here so it isn't invented mid-implementation without a reason. |

### 2.2 Settings — scoped minimally

Decision: a demo does not need an options menu with graphics presets, keybind remapping, audio mixer graphs, etc. Daniel is not shipping this to a marketplace with support obligations — it's a public demo build people play once or twice on the website. Scope to what actually breaks the experience if missing:

| Setting | Why it's in scope | Why it's *not* gold-plated further |
|---|---|---|
| Mouse sensitivity (single float, applied to Enhanced Input scale) | A shooter demo is unplayable for a meaningful fraction of players at a fixed sensitivity — this is table stakes, not polish. | One slider, not per-axis/per-weapon sensitivity curves. |
| Master volume | Same bar — silence/audio-blast on load is a bad first impression for a public download. | One slider. No separate music/SFX/voice buses — there's no music or voice in this build yet (confirm with social/community before adding scope here). |
| Invert Y-axis toggle | Cheap, binary, some players consider it non-negotiable. | Toggle only, no per-axis granularity. |
| Window mode (Fullscreen/Windowed) + resolution dropdown | UE5 exposes this almost for free via `UGameUserSettings` — skipping it actively hurts a downloadable demo running on unknown hardware. | Use UE5's built-in `UGameUserSettings` API directly; do not build a custom resolution-detection system. |

Explicitly **out of scope** for M2 (flag if Daniel asks — price separately, don't build quietly): keybind remapping, graphics quality presets/scalability sliders, FOV slider, colorblind modes, controller support. None of these are needed to prove the demo; all are real work if added.

| Class | Base | Responsibility |
|---|---|---|
| `UPvPSettingsSaveGame` | `USaveGame` | Four fields: `float MouseSensitivity`, `float MasterVolume`, `bool bInvertY`, plus window mode/resolution are read/written straight through `UGameUserSettings` (already persists itself, no custom SaveGame needed for those two). Loaded on `UPvPGameInstance::Init` (M1 class, one new call added — not a new class), applied to Enhanced Input scale and the sound class volume. |

### 2.3 AI Opponents

Full design rationale in §3. Class list:

| Class | Base | Responsibility |
|---|---|---|
| `APvPAIController` | `AAIController` | Possesses an `APvPCharacter` (M1 class, unchanged) exactly like a human `APvPPlayerController` would — the character/health/weapon components have no concept of "am I controlled by a human or a bot," which is what keeps this additive. Runs a `UBehaviorTree`-driven decision loop (§3.2). Reads a `UPvPAIDifficultyProfile` data asset (below) to scale reaction time, aim error, and decision frequency. Calls `UPvPWeaponComponent::Fire()` and the character's movement input exactly the way a player's input would — **no new fire/damage/replication path**, the AI is just another client of the existing M1 input surface, running on the server (bots are server-side, never need their own replicated connection). |
| `UPvPAIDifficultyProfile` | `UPrimaryDataAsset` | Blueprint-authorable tuning surface: `ReactionTimeSeconds`, `AimErrorDegrees` (cone half-angle applied to aim direction before firing), `DecisionIntervalSeconds` (how often the behavior tree re-evaluates target/cover choice), `EngagementRangePreference`. Three instances authored for Easy/Normal/Hard — see §3.3 for starting values. **This is the entire "difficulty settings" feature** — no code branches on difficulty, only data. |
| `UPvPAIPerceptionSetup` (or reuse UE's `AIPerceptionComponent` directly — see §3.1) | `UActorComponent` | Thin wrapper (if needed at all) around UE5's built-in `UAIPerceptionComponent` (Sight sense) attached to `APvPAIController`. Prefer using the engine component directly with per-difficulty sight radius/FOV pulled from `UPvPAIDifficultyProfile`; only wrap it in a custom class if the setup logic gets repetitive enough to justify one class over per-controller Blueprint config. Default to *not* building this class — call it out here so nobody invents a wrapper before checking whether raw `UAIPerceptionComponent` config is enough. |

### 2.4 Tutorial Flow

| Class | Base | Responsibility |
|---|---|---|
| `APvPTutorialGameMode` | `AGameModeBase` | Server-only (single-player, so "server" is the local listen-server host). Drives a linear step sequence (see §3 note below — this is intentionally simple, not a generic scripting system). Each step = a trigger volume or objective placed **in the tutorial map** (level-designer's deliverable) paired with an on-screen prompt widget and a completion condition (enter volume / land N hits / reload once / etc.). |
| `UPvPTutorialStepData` | `UPrimaryDataAsset` (array of structs, or one asset per step — pick one at implementation time, not an architecture decision) | Ordered list of steps: prompt text, completion condition type (enum: `EnterVolume`, `FireWeapon`, `Reload`, `Kill`), reference to the trigger actor in the map. Blueprint-authored so level-designer can iterate on step content without touching C++. |
| `UPvPTutorialHUDWidget` | `UUserWidget` | Displays current step's prompt text + a simple progress indicator (e.g. "Step 2 of 5"). Listens to `APvPTutorialGameMode`'s step-advance delegate. |

**Handoff point, stated plainly:** `APvPTutorialGameMode` and `UPvPTutorialStepData` define *what a step is and how completion is detected* — generic, map-agnostic. level-designer owns the actual tutorial map layout and places the trigger volumes/objective actors that steps 1..N point at. Do not let map layout decisions (what order, what teaches what) get made in this document — that's level-designer's call once they have this step-data contract to build against.

---

## 3. AI Opponent Design Note

This is the one system in Milestone 2 without an M1 precedent, so it gets its own section rather than just a table row. The goal: **a bot that's a believable 1v1 opponent in a hitscan tactical shooter, built with the smallest system that gets there** — not a general-purpose game AI framework.

### 3.1 Perception — use the engine, don't build one

UE5's `UAIPerceptionComponent` with a Sight sense covers everything needed: has the bot seen the player, is the player still in view, last-known location when line of sight breaks. Configure sight radius, FOV half-angle, and "lose sight after N seconds" per difficulty profile (§2.3). **Do not build a custom line-of-trace perception system** — that's reinventing a solved engine feature for no gain here.

### 3.2 Decision-making — a small behavior tree, not a planner

A `UBehaviorTree` with a short, fixed set of tasks is enough for a 1v1 hitscan opponent:

- **Search** — no target perceived: patrol between a small set of designer-placed points (reuse `PlayerStart`-style markers, or a simple point array — level-designer can place these in the 1v1 map alongside the "paintball ring" barriers, since the barriers themselves are natural AI cover/patrol anchors).
- **Engage** — target perceived and in engagement range (from difficulty profile): face target, apply aim error (§3.3), call `Fire()` through the character's existing `UPvPWeaponComponent` at the profile's decision interval.
- **Take Cover** (optional, add only if Engage alone feels too static in playtest — flag as a possible follow-up, not a day-one requirement) — move toward nearest cover barrier when health drops below a threshold.
- **Lost Target** — move to target's last-known location, then fall back to Search if nothing found.

This is a 4-node tree (3 if Take Cover is deferred). **Explicit scope guard: no squad tactics, no communication between bots (M2 is 1v1 vs AI only — one bot, no coordination problem exists), no pathing around player-predicted positions, no difficulty-scaled "cheating" via perfect info.** The bot only ever acts on what its `UAIPerceptionComponent` actually perceives — this keeps it fair-feeling and keeps the system small.

### 3.3 Difficulty — tunable data, not new code paths

This is the requirement worth being explicit about, because the wrong implementation (branching weapon/hit-reg code per difficulty) would violate the "reuse M1 architecture as-is" constraint. The right implementation: **difficulty is entirely a `UPvPAIDifficultyProfile` data asset, consumed by `APvPAIController` — `UPvPWeaponComponent`, `UPvPHealthComponent`, and the server-authoritative hit-reg/lag-comp pipeline are completely unaware AI exists.** A bot's shot is a normal `ServerFire` RPC with a direction vector that has aim error applied *before* the RPC is sent — from the weapon/hit-reg system's point of view, it's indistinguishable from a human client's imperfect aim.

Starting values (tune during playtest, per Phase L below — these are reasonable starting points, not final-tuned numbers):

| Profile | Reaction Time | Aim Error (cone half-angle) | Decision Interval | Sight Range |
|---|---|---|---|---|
| Easy | 0.6s | 4.0° | 0.8s | Short |
| Normal | 0.35s | 2.0° | 0.5s | Medium |
| Hard | 0.15s | 0.5° | 0.25s | Long |

**Reaction time** = delay between perception acquiring a target and Engage state starting to fire (implemented as a simple timer, not a new system). **Aim error** = random cone applied to the fire direction each shot, re-rolled per shot, magnitude in degrees. **Decision interval** = how often the behavior tree re-evaluates (lower = more responsive to the player peeking/repositioning, reads as "sharper"). All four values live on the data asset — adding a fourth difficulty tier later, or retuning existing ones, is a data change, never a code change.

### 3.4 What this deliberately does not do

- No machine-learned/trained AI (RL, imitation learning) — completely out of proportion to a demo's needs.
- No difficulty auto-adjustment (rubber-banding) — static per-match selection only, matches the menu's Easy/Normal/Hard picker.
- No bot support for 2v2/3v3 — decision log's demo scope is 1v1 vs AI specifically; multi-bot coordination is a different (harder) problem, not opened here.

---

## 4. Engineering Task Breakdown

Same relative sizing convention as M1 §4: **XS** = well under a sitting, **S** = one focused sitting, **M** = multiple sittings, **L** = should probably be broken down further before starting if it's still L once you're in it. All tasks require UE5 on Daniel's machine, same as M1 — no exceptions, not called out per-row again.

### Phase I — Menu shell
| # | Task | Size |
|---|---|---|
| I1 | `UPvPMainMenuWidget` UMG layout: title art placeholder (real art is technical-artist's pass), four nav buttons, widget-switcher wiring to sub-screens | S |
| I2 | Wire GameInstance/level-load flow: game boots to main menu level, not directly into a match (new small `LevelSequence`/`GameMode` for a menu-only persistent level) | S |
| I3 | `UPvPLANSessionWidget` — Host button (starts listen server via `UPvPMatchmakingSubsystem`), Join screen (IP text field + Connect) | M |
| I4 | `UPvPAIMatchSetupWidget` — difficulty selector UI (3 buttons/dropdown bound to `UPvPAIDifficultyProfile` assets), Start button | S |
| I5 | Back navigation from every sub-screen to main menu; confirm widget-switcher handles it without a nav subsystem (build `UPvPMenuNavSubsystem` only if this proves insufficient, per §2.1 note) | S |

### Phase J — LAN / single-player host wiring
| # | Task | Size |
|---|---|---|
| J1 | `UPvPMatchmakingSubsystem` (M1 class) — add `StartListenServer(bool bWithAI)` entry point alongside the existing queue-based flow; no changes to `FPvPLocalMatchProvider` internals | S |
| J2 | Confirm direct-IP join path (`ClientTravel`/`open <ip>`) against a listen server started via J1, on two machines on the same LAN | S |
| J3 | Single-player host path: `StartListenServer(bWithAI=false)` with zero required remote connections — starts and plays solo against nothing (sanity check before AI exists) | XS |
| J4 | LAN playtest: two machines, Host/Join UI end to end, into a working M1 deathmatch match | S |

### Phase K — AI opponents
| # | Task | Size |
|---|---|---|
| K1 | `UPvPAIDifficultyProfile` data asset class (fields only, per §3.3) | XS |
| K2 | Author Easy/Normal/Hard data asset instances with §3.3 starting values | XS |
| K3 | `APvPAIController` skeleton — possesses `APvPCharacter`, holds a reference to its assigned `UPvPAIDifficultyProfile`, no behavior yet | S |
| K4 | Attach/configure `UAIPerceptionComponent` (Sight) on the controller, radius/FOV pulled from the difficulty profile | S |
| K5 | Behavior tree: Search state — patrol between designer-placed points (stub points acceptable until level-designer's 1v1 map lands; do not block on the map) | M |
| K6 | Behavior tree: Engage state — face target, apply aim-error cone, call `Fire()` at the profile's decision interval | M |
| K7 | Behavior tree: Lost Target state — path to last-known location, fall back to Search | S |
| K8 | GameMode hookup: 1v1-vs-AI match spawns one `APvPCharacter` possessed by `APvPAIController` instead of a second human player, using the selected difficulty profile from `UPvPAIMatchSetupWidget` | S |
| K9 | Playtest pass per difficulty tier: does Easy feel beatable, does Hard feel sharp-but-fair (not aimbot-perfect); adjust §3.3 values as needed — **this is the task most likely to take longer than it looks, see RISK** | L |
| K10 | (Optional, only if K9 playtest flags it) Take Cover behavior tree state | M |

### Phase L — Tutorial flow
| # | Task | Size |
|---|---|---|
| L1 | `UPvPTutorialStepData` asset structure (ordered steps: prompt text, completion-condition enum, trigger reference) | S |
| L2 | `APvPTutorialGameMode` — step sequencing, advance-on-completion, step-advance delegate | S |
| L3 | `UPvPTutorialHUDWidget` — prompt text + step counter, bound to the delegate | XS |
| L4 | Completion-condition handlers: `EnterVolume` (trigger overlap), `FireWeapon`/`Reload` (hook existing `UPvPWeaponComponent` events — M1 class, read-only hookup, no changes to it), `Kill` (hook `UPvPHealthComponent::OnDeath`, M1 class, same read-only hookup) | S |
| L5 | **Handoff to level-designer**: author `UPvPTutorialStepData` content once the tutorial map's trigger volumes exist — this task is level-designer's, listed here only to mark the dependency boundary | — (not game-director's task) |
| L6 | End-to-end tutorial playtest once map + step content are both in (blocked on L5 + level-designer's map) | S |

### Phase M — Settings
| # | Task | Size |
|---|---|---|
| M1 | `UPvPSettingsSaveGame` — 4 fields, save/load via `USaveGame` API | XS |
| M2 | `UPvPSettingsWidget` UMG — sensitivity slider, volume slider, invert-Y toggle, window mode + resolution (via `UGameUserSettings`) | S |
| M3 | Wire sensitivity to Enhanced Input scale, volume to the sound class, on `UPvPGameInstance::Init` and on live change from the widget | S |
| M4 | Playtest: change each setting, confirm it persists across a relaunch | XS |

### Phase N — Win condition confirmation (trivial, listed for completeness)
| # | Task | Size |
|---|---|---|
| N1 | Set `APvPDeathmatchGameMode`'s score-cap value to 3 (already-built config field per M1 §2.1 — this is a value change, not new code) | XS |
| N2 | End-to-end confirm: a 1v1 match (LAN or vs AI) ends at 3 kills, results state fires correctly | XS |

### Phase O — Milestone 2 integration playtest
| # | Task | Size |
|---|---|---|
| O1 | Full loop: boot → main menu → each of the 4 destinations → into a match → 3-kill win → back to menu | M |
| O2 | Fix list from O1, re-test | M |
| O3 | Milestone 2 DONE WHEN sign-off (§5) | S |

---

## 5. Deliverable Summary

```
FEATURE      Milestone 2 / Public Demo: title/main menu (Tutorial, 1v1, 1v1 vs
             AI, Settings), local LAN play + single-player host, AI opponents
             with tunable difficulty, a minimal settings menu, tutorial mode
             flow (logic only — map is level-designer's), and confirmation of
             the first-to-3-kills win condition. Built additively on Milestone
             1's engineering per decisions.md (2026-09-08).

ARCHITECTURE Every M1 class (GameMode/GameState/PlayerState/PlayerController,
             character, health, weapon, hit-reg, lag-comp, matchmaking
             subsystem) is reused unchanged. New surface area: a UMG menu
             shell (§2.1) sitting in front of M1's existing direct-IP connect
             path (no new networking code — LAN was already priced into M1
             §2.4, it just lacked a UI); an `APvPAIController` that drives an
             unmodified `APvPCharacter` through the same input/fire surface a
             human player uses, so hit-reg/replication never learn AI exists
             (§3); difficulty as pure data (`UPvPAIDifficultyProfile`, §3.3),
             never a weapon/hit-reg code branch; a small map-agnostic tutorial
             step-sequencer that level-designer's map plugs into (§2.4); and a
             4-field settings surface scoped deliberately small for a one-shot
             public demo, not a shipping options menu (§2.2).

TASKS        ~35 tasks across 6 new phases (I: menu shell, J: LAN/host wiring,
             K: AI opponents, L: tutorial flow, M: settings, N: win-condition
             confirmation, O: integration playtest) — see §4 for the full
             table with relative sizing. Phase N is two XS tasks; everything
             else is genuinely new work.

RISK         AI opponent quality (Phase K) is the least-proven system in this
             entire plan and the most likely to need iteration passes beyond
             what's scoped here — "does Hard feel sharp but fair, does Easy
             feel beatable but not insulting" is a playtest-feel judgment that
             can't be fully nailed by tuning table values in advance (K9 is
             sized L specifically because of this). Because difficulty lives
             entirely in `UPvPAIDifficultyProfile` data (§3.3) and the
             behavior tree is isolated to `APvPAIController`, iteration stays
             contained to that one system — it cannot cascade into a rewrite
             of weapon/hit-reg/replication, which is the load-bearing
             architecture protected in M1. Second, smaller risk: LAN Join UI
             (I3/J2) has not been tested against real cross-machine network
             conditions (NAT/firewall quirks even on a LAN are possible) —
             flagged, not expected to be a real blocker for same-network play.

DONE WHEN    From the main menu, a player can: (1) run the tutorial flow to
             completion once level-designer's map lands (L6), (2) host or join
             a LAN 1v1 match that plays to a 3-kill win (J4, N2), (3) start a
             solo match against an AI opponent at each of the three difficulty
             tiers and have Easy/Normal/Hard feel distinguishable in playtest
             (K9), and (4) change and persist each of the four settings
             (M4) — all using only this document plus the M1 scaffold, with no
             further architecture decisions required along the way. Points
             still open are named, not silently resolved: level-designer's
             tutorial-map and 1v1-map delivery timing (blocks L5/L6 and the
             AI's patrol-point placement in K5), and technical-artist's art
             pass (menu/character/weapon visuals — explicitly out of scope
             here, per decisions.md).
```
