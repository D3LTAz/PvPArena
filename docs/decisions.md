# PvP Arena — Decisions Log

## 2026-09-07 — Milestone 1 definition
**Decision:** Milestone 1 is multiplayer deathmatch (1v1/2v2/3v3) on a single graybox map, matching the already-finalized Stage 1 scope — not the single-player graybox combat prototype named in the earlier Claude Code kickoff prompt.
**Reason:** Stage 1 scope (UE5 C++ template + Lyra reference, Git/Git LFS, one 3v3 graybox map, single starting weapon, server-authoritative hit registration with basic lag compensation, deathmatch across 1v1/2v2/3v3, minimal format-picker matchmaking) was already locked before the kickoff prompt was drafted, and the Game Director / QA Engineer specs are built around multiplayer from day one. Building single-player first means re-architecting for replication and server authority later — costs more than it saves.
**Who it affects:** game-director (owns the build order), qa-engineer (test plan should assume server-authoritative multiplayer from the start).

## 2026-09-07 — Studio Director stood up
**Decision:** Daniel's studio-director agent spec adopted as the standing point of contact for PvP Arena; began standing up the full specialist roster (game-director, qa-engineer, level-designer, technical-artist, social-media-manager, community-manager, sales-monetization, contracts-manager, finance-ops).
**Reason:** Requested directly.
**Who it affects:** all workstreams.

## 2026-09-07 — Engine pinned to UE 5.8; build toolchain is VS2022, not VS2026
**Decision:** `EngineAssociation` is `5.8` (per §0 of the scaffold, log the exact version at kickoff — done here). C++ project-file generation must target **Visual Studio 2022**, installed alongside whatever newer VS version is on a given machine — do not rely on the newest installed VS.
**Reason:** UE 5.8's `UnrealBuildTool` doesn't recognize Visual Studio 2026 (v18) as a supported project-file format. With only VS2026 installed, project generation failed with `Unexpected ProjectFileFormat 'Default'` (UBT detects the installed IDE, can't map it to a known format, and falls back to the literal unhandled string `"Default"`). Installing VS2022 side-by-side (Community edition, "Game development with C++" workload) resolved it.
**Who it affects:** game-director (build order assumes C++ compiles), anyone setting up the project on a new machine — check installed VS version before assuming project generation will "just work" on the newest VS available. `Config/DefaultEditorPerProjectUserSettings.ini` is now gitignored so a machine's resolved IDE/editor preference doesn't get committed and break other machines the way this did.

## 2026-09-07 — APvPDeathmatchGameMode drives its own match-state machine (AGameModeBase has none)
**Decision:** `APvPDeathmatchGameMode::SetMatchState()` is a small hand-rolled state setter that writes `EPvPMatchState` (`Source/PvPArena/Core/PvPMatchState.h`) onto `APvPDeathmatchGameState::CurrentMatchState`, rather than overriding `HandleMatchIsWaitingToStart`/`HandleMatchHasStarted`/`HandleMatchHasEnded`.
**Reason:** Those `Handle*` hooks only exist on the fuller `AGameMode`, not on `AGameModeBase` — which is the base class §2.1 of the scaffold explicitly calls for. Assumed otherwise while writing the B3 skeleton; the compiler caught it (`C2039: is not a member of 'AGameModeBase'`) before it shipped. Switching to `AGameMode` would have been the "easy" fix but silently pulls in its lobby/timeout machinery, which isn't a decision to make unilaterally.
**Who it affects:** Phase E (win condition / `EndMatch`) — call `SetMatchState(EPvPMatchState::InProgress)` / `SetMatchState(EPvPMatchState::Ending)` from there rather than expecting `AGameMode`-style virtuals to fire.
