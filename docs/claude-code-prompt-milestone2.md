# Claude Code kickoff prompt — Milestone 2 / Public Demo

> **Sequencing note, added 2026-09-10 (see `docs/decisions.md`):** Do not paste this yet. It assumes multiplayer deathmatch is already built; the repo pivoted to single-player practice-vs-AI on 2026-09-07 instead, which has since shipped. Per Daniel's 2026-09-10 call, practice-vs-AI ships first and multiplayer resumes as a later phase — this prompt is Phase 3 reference for that later resume, not something to run now.

Paste this into Claude Code running on your machine, inside the `PvPArena` repo, once Milestone 1 (Phases A–H) is actually built and playable.

---

You are the engineering lead on PvP Arena, a tactical multiplayer FPS in Unreal Engine 5. Milestone 1 (core multiplayer deathmatch — replication, server-authoritative hit registration, lag compensation, matchmaking) is already built and working in this repo. Do not modify any Milestone 1 class. Your job now is Milestone 2 — the public demo layer — built strictly additively on top of it.

Before writing any code, read these files in the repo in full:
- `docs/decisions.md` — full decision history, especially the "Demo build scope (Milestone 2)" and "New launch requirement" entries
- `docs/game-director-stage1-scaffold.md` — the Milestone 1 architecture you must not touch
- `docs/game-director-milestone2-demo.md` — the complete Milestone 2 spec: this is your primary build order
- `docs/level-design-milestone2-demo-maps.md` — the tutorial map and "The Ring" 1v1 map specs (someone else builds these maps; you build the systems that plug into them)
- `docs/technical-artist-milestone2-art-polish.md` — art scope (not your job; use placeholder art where the menu/AI need visuals)

Hard constraints, non-negotiable:
1. Every Milestone 1 class (GameMode, GameState, PlayerState, PlayerController, `APvPCharacter`, `UPvPHealthComponent`, the full weapon/hit-reg/lag-comp stack, `UPvPMatchmakingSubsystem`) is reused unchanged. If you think one needs to change, stop and flag it — don't modify it silently.
2. AI difficulty is 100% data (`UPvPAIDifficultyProfile`), never a branch in weapon/hit-reg/replication code. A bot's shot is a normal `ServerFire` RPC with aim error applied before the RPC — indistinguishable to the server from imperfect human aim.
3. Don't build `UPvPMenuNavSubsystem` unless the widget-switcher pattern in `UPvPMainMenuWidget` genuinely can't handle back/forward navigation — try the simple version first.
4. Don't build a custom AI perception system — use UE5's built-in `UAIPerceptionComponent` (Sight sense) directly.
5. Settings menu is scoped to exactly four things: mouse sensitivity, master volume, invert-Y toggle, window mode/resolution (via `UGameUserSettings`). No keybind remapping, no graphics presets, no FOV slider, no controller support — if you're tempted to add one, stop and flag it instead.
6. First-to-3-kills is a one-line config change to the score cap already built in Milestone 1 — do not rebuild the win-condition system.
7. "Take Cover" AI behavior is optional — only build it if the Phase K playtest (K9) shows the bot feels too static without it.

Work through the phases in `docs/game-director-milestone2-demo.md` §4 in order, one at a time — don't jump ahead:

- **Phase I** — Menu shell: `UPvPMainMenuWidget` (Tutorial / 1v1 / 1v1 vs AI / Settings), boot-to-menu flow, `UPvPLANSessionWidget`, `UPvPAIMatchSetupWidget`, back navigation.
- **Phase J** — LAN / single-player host wiring: extend `UPvPMatchmakingSubsystem` with `StartListenServer(bool bWithAI)`, confirm direct-IP join, playtest on two machines.
- **Phase K** — AI opponents: `UPvPAIDifficultyProfile`, `APvPAIController`, perception, a 3–4 node behavior tree (Search / Engage / Lost Target, optional Take Cover), GameMode hookup, difficulty playtest pass.
- **Phase L** — Tutorial flow: `UPvPTutorialStepData`, `APvPTutorialGameMode`, `UPvPTutorialHUDWidget`, completion-condition handlers. Stop at the handoff point (L5) — the actual tutorial map content is not yours to author.
- **Phase M** — Settings: `UPvPSettingsSaveGame`, `UPvPSettingsWidget`, wiring to Enhanced Input and sound class, persistence playtest.
- **Phase N** — Win condition: set the score cap to 3, confirm end to end.
- **Phase O** — Full integration playtest: boot → menu → each of the 4 destinations → into a match → 3-kill win → back to menu. Fix what breaks. Sign off against the DONE WHEN in §5 of the spec.

After each phase: run a smoke test before moving to the next one, and tell me what you built, what you tested, and anything you had to deviate from the spec on (and why) before continuing.

If the tutorial map or the 1v1 "Ring" map aren't in the repo yet when you reach Phase K's patrol-point placement or Phase L, use stub placeholder geometry/points and say so — don't block on someone else's deliverable, per the spec's own guidance.

Log every real architecture decision to `docs/decisions.md` the same way the existing entries are written — date, decision, reason, who it affects.
