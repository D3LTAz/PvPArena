# PvP Arena — Decisions Log

## 2026-09-07 — Milestone 1 definition
**Decision:** Milestone 1 is multiplayer deathmatch (1v1/2v2/3v3) on a single graybox map, matching the already-finalized Stage 1 scope — not the single-player graybox combat prototype named in the earlier Claude Code kickoff prompt.
**Reason:** Stage 1 scope (UE5 C++ template + Lyra reference, Git/Git LFS, one 3v3 graybox map, single starting weapon, server-authoritative hit registration with basic lag compensation, deathmatch across 1v1/2v2/3v3, minimal format-picker matchmaking) was already locked before the kickoff prompt was drafted, and the Game Director / QA Engineer specs are built around multiplayer from day one. Building single-player first means re-architecting for replication and server authority later — costs more than it saves.
**Who it affects:** game-director (owns the build order), qa-engineer (test plan should assume server-authoritative multiplayer from the start).

## 2026-09-07 — Studio Director stood up
**Decision:** Daniel's studio-director agent spec adopted as the standing point of contact for PvP Arena; began standing up the full specialist roster (game-director, qa-engineer, level-designer, technical-artist, social-media-manager, community-manager, sales-monetization, contracts-manager, finance-ops).
**Reason:** Requested directly.
**Who it affects:** all workstreams.
