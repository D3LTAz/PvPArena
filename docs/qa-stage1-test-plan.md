# PvP Arena — Stage 1 / Milestone 1 Test Plan

**Owner:** qa-engineer
**Scope source:** `/docs/decisions.md` — 2026-09-07 "Milestone 1 definition" entry (locked Stage 1 scope). If that scope changes, this plan must be flagged and re-validated against the new decision, per role rules.
**Status:** Planning document. No running build exists yet in this environment (no UE5 available in this cloud sandbox). Every test case below is written to be executable once game-director's scaffold (UE5 C++ template + Lyra reference, one 3v3 graybox map) exists — each case states preconditions, exact steps, and pass/fail criteria so it can be picked up and run without re-authoring.
**Systems covered (per locked scope):** weapon system, movement under netcode, server-authoritative hit registration + lag compensation, format-picker matchmaking (1v1/2v2/3v3 deathmatch).

**Test environment note:** Any test case tagged `[NET]` must be run under simulated latency, not just on localhost/LAN. Use UE5's network emulation (`net pktlag`, `net pktloss`, `net pktdup`, or the Editor's Net Emulation settings) or an external latency shaper, at minimum these profiles:
- **LAN baseline:** 0–5 ms, 0% loss
- **Regional:** 40–60 ms RTT, 0.5% loss
- **Cross-region:** 120–160 ms RTT, 1–2% loss
- **Degraded:** 200 ms+ RTT, jitter ±50 ms, 3–5% loss

Server-authoritative systems (hit reg, lag comp, movement replication) are considered untested until they've passed at Regional and Cross-region profiles, not just LAN baseline.

---

## 1. Weapon System (damage, fire rate, hit detection)

### TC-WPN-01 — Damage-per-hit matches design value
**Preconditions:** One graybox map loaded, two connected clients (attacker, target), starting weapon equipped, target at full health.
**Steps:**
1. Attacker fires exactly one shot at target's torso hitbox from within point-blank range (no falloff).
2. Record target's health before and after the shot on both the server log/health authority and the target's client HUD.
**Expected:** Health delta equals the weapon's documented base damage value exactly; server-side health value and client HUD value agree (no client-side desync).
**Pass/Fail:** Fail if delta != design value, or if server and client disagree.

### TC-WPN-02 — Fire rate is enforced server-side, not just client-side
**Preconditions:** Same as TC-WPN-01. Weapon's documented fire rate (rounds/sec or ms-between-shots) known.
**Steps:**
1. On the client, modify or simulate an input that spams the fire input faster than the weapon's rate limit (e.g., macro/script sending fire events at 2x the allowed rate, or a debug console command that bypasses client-side cooldown).
2. Observe the number of shots the server actually registers (via server log / hit events) over a fixed time window.
**Expected:** Server accepts only the number of shots consistent with the documented fire rate, regardless of client input rate. Excess client-side fire requests are rejected/ignored server-side.
**Pass/Fail:** Fail if server registers more shots than the fire-rate cap allows — this is a cheat/exploit vector, escalate immediately per role rules if found.

### TC-WPN-03 — Hit detection: hitbox accuracy at range
**Preconditions:** Attacker and target on opposite sides of graybox map at three distances: close (~5m), mid (~20m), long (~40m, or map's max sightline).
**Steps:**
1. At each distance, attacker fires at target's head hitbox, then torso, then limb (if separate hitboxes exist), 5 shots each.
2. Record which hits register vs. which are dropped, and whether registered hit location (head/torso/limb) matches the intended aim point.
**Expected:** All shots that visually land on a hitbox in the attacker's client view register a hit on the correct hitbox on the server. No "should have hit" shots are silently dropped at any tested range.
**Pass/Fail:** Fail if hit rate at any range differs from close-range baseline by more than an agreed noise margin, or if hit location is misattributed (e.g., torso shot credited as headshot or vice versa).

### TC-WPN-04 — Damage falloff (if designed) applies correctly by range
**Preconditions:** Weapon's falloff curve/breakpoints documented by game-director/design.
**Steps:**
1. Fire one shot each at documented falloff breakpoint distances (e.g., 0m, 15m, 30m, max range).
2. Record resulting damage at each distance.
**Expected:** Damage values match the documented falloff curve within rounding tolerance.
**Pass/Fail:** Fail on any breakpoint mismatch. If no falloff is in Stage 1 scope, mark N/A explicitly rather than skipping silently.

### TC-WPN-05 — Weapon kill confirms and health floor
**Preconditions:** Target at low health (below one weapon hit's damage).
**Steps:**
1. Attacker fires one shot sufficient to kill.
2. Observe death event firing exactly once, target health does not go negative/wrap, respawn or elimination logic triggers correctly per format rules.
**Expected:** Exactly one death event, health clamps at 0, no double-kill credit, no negative-health carry-over bugs.
**Pass/Fail:** Fail on duplicate death events, health underflow, or missed death trigger.

### TC-WPN-06 `[NET]` — Weapon feel under latency (client-side hit feedback vs. server truth)
**Preconditions:** Regional and Cross-region latency profiles.
**Steps:**
1. Repeat TC-WPN-01 and TC-WPN-03 at each latency profile.
2. Compare client-perceived hit confirmation (hitmarker/sound) timing and correctness against server-authoritative outcome.
**Expected:** Client hit feedback, when present, matches the server's eventual authoritative result (no false-positive hitmarkers with no server damage applied, no missing hitmarkers on shots the server did register).
**Pass/Fail:** Fail on feedback/server mismatch beyond an agreed number of frames; log every mismatch as a candidate hit-reg desync bug (critical, escalate immediately).

---

## 2. Movement Under Netcode (prediction, rubber-banding)

### TC-MOV-01 — Local client-side prediction matches eventual server position (LAN baseline)
**Preconditions:** Single client on graybox map, LAN baseline latency.
**Steps:**
1. Move client through a defined path (straight run, strafe, jump, direction reversal) for 10 seconds.
2. Compare the client's predicted position trace against the server's authoritative position trace for the same time window (via server-side logging/debug draw).
**Expected:** Predicted and authoritative traces match within a small tolerance (e.g., a few cm) at every sampled tick; no visible correction snaps.
**Pass/Fail:** Fail if divergence exceeds tolerance or a correction snap is visible/logged.

### TC-MOV-02 `[NET]` — Rubber-banding does not occur under Regional latency during normal movement
**Preconditions:** Regional latency profile (40–60ms, 0.5% loss), one client.
**Steps:**
1. Repeat the TC-MOV-01 movement path.
2. Watch for visible position "snapping back" (rubber-banding) on the client during normal, uncontested movement (no collisions with other players).
**Expected:** No rubber-banding during normal movement at Regional latency — client prediction reconciles smoothly with server corrections (small, imperceptible adjustments only).
**Pass/Fail:** Fail on any visibly jarring correction (defined as a positional snap greater than an agreed threshold, e.g., >15cm in a single visible correction, or a directional reversal not initiated by the player).

### TC-MOV-03 `[NET]` — Rubber-banding / correction behavior under Cross-region and Degraded latency
**Preconditions:** Cross-region and Degraded profiles.
**Steps:**
1. Repeat movement path at each profile.
2. Record frequency and magnitude of visible corrections.
**Expected:** Corrections may become more frequent at Degraded latency (acceptable/expected), but must remain visually coherent — no teleporting through geometry, no getting stuck, no permanent desync between client and server position.
**Pass/Fail:** Fail if the player becomes permanently desynced (client position and server position diverge and never reconcile), or if correction causes the player to clip through world geometry.

### TC-MOV-04 `[NET]` — Movement prediction during packet loss
**Preconditions:** Any latency profile with 1%+ configured loss.
**Steps:**
1. Move client continuously for 30 seconds while packet loss is active.
2. Observe client movement continuity (no freezing) and post-loss reconciliation with server.
**Expected:** Client continues to predict movement smoothly through lost packets; once packets resume, client reconciles to server truth without a large visible jump.
**Pass/Fail:** Fail on client freeze/hitch beyond an agreed duration, or large reconciliation jump after loss recovery.

### TC-MOV-05 — Collision and geometry interaction under prediction (jump, ledges, obstacles on graybox map)
**Preconditions:** Graybox map's known geometry features (ledges, ramps, obstacles) documented by level-designer.
**Steps:**
1. Client attempts to jump onto/over each documented geometry feature at LAN baseline, then at Regional latency.
2. Record whether client-predicted position and server-authoritative position agree on success/failure of the maneuver (e.g., did the jump succeed on both, or does the client predict success while the server rejects it, teleporting the player back).
**Expected:** Prediction and server authority agree on maneuver outcome in the large majority of attempts; any disagreement resolves as a smooth correction, not a rubber-band snap.
**Pass/Fail:** Fail if server routinely rejects maneuvers the client predicted as successful (indicates prediction/collision logic mismatch between client and server).

---

## 3. Server-Authoritative Hit Registration + Lag Compensation

### TC-HITREG-01 `[NET]` — Lag compensation credits hits on a target the shooter saw, even if target has since moved server-side
**Preconditions:** Two clients, Regional latency profile minimum. Target moving laterally at constant speed across shooter's sightline.
**Steps:**
1. Shooter fires at target's position as rendered on the shooter's own (lagged) client view, aiming exactly at where the target appears.
2. Compare against the target's true server-side position at the time the shot was processed (which will be ahead of what the shooter saw, due to latency).
**Expected:** Server rewinds target's hitbox history to the timestamp of the shooter's client view (favor-the-shooter lag compensation) and registers the hit if the shooter's aim was correct relative to what they saw.
**Pass/Fail:** Fail if shots that were visually accurate on the shooter's screen are not credited due to the target having moved server-side by the time the packet arrives.

### TC-HITREG-02 `[NET]` — Lag compensation does not over-correct ("dying behind cover")
**Preconditions:** Target moving into cover/behind an obstacle on the graybox map, Regional and Cross-region profiles.
**Steps:**
1. Target moves fully behind cover on their own client, confirms cover is reached on their own screen.
2. Shooter fires at the last visible position of the target (per shooter's lagged view) after the target's client shows them in cover.
3. Record whether server credits the hit.
**Expected:** There is a bounded, documented lag-compensation window (per game-director's chosen lag comp implementation). Hits should not be creditable indefinitely after a target's client shows full cover — measure and document the actual window observed, and flag to game-director if it exceeds an agreed maximum (e.g., >~1 RTT + configured rewind cap) as a "died behind cover" complaint risk.
**Pass/Fail:** Fail (flag as design/tuning issue, not necessarily a hard bug) if the observed compensation window is unbounded or clearly excessive relative to configured latency.

### TC-HITREG-03 `[NET]` — Hit registration consistency across simultaneous shooters
**Preconditions:** 2v2 or 3v3 format, multiple clients firing at the same target within the same tick window, Regional latency.
**Steps:**
1. Two shooters fire at the same target within a very short window (<100ms apart) such that both shots should register.
2. Verify server processes and credits both hits (with correct damage/kill attribution) rather than dropping one due to a race condition.
**Expected:** Both hits register, total damage is the sum of both, and kill credit (if the combined damage is lethal) goes to whichever hit's damage crossed the health threshold per documented kill-attribution rule.
**Pass/Fail:** Fail if one hit is silently dropped, or if damage/kill attribution is inconsistent between server log and client-visible results.

### TC-HITREG-04 — Server rejects impossible hits (anti-cheat baseline for Stage 1)
**Preconditions:** Ability to send a manually crafted or debug-forced hit-scan/fire event claiming a hit on a target that is not within the weapon's valid range/line-of-sight (e.g., through a wall, beyond max range).
**Steps:**
1. Attempt to trigger a hit event where the target is behind solid graybox geometry with no line of sight, or beyond the weapon's max effective range.
2. Observe whether the server credits damage.
**Expected:** Server performs its own line-of-sight/range validation and rejects the hit; client-reported hits are never trusted blindly.
**Pass/Fail:** Fail (critical, escalate immediately per role rules — this is an exploit vector) if server credits damage for a hit that should have been geometrically impossible.

### TC-HITREG-05 `[NET]` — Hit registration under Degraded profile does not desync server/client kill state
**Preconditions:** Degraded latency profile, full match (any format) in progress.
**Steps:**
1. Play a short deathmatch segment (5+ kills) entirely under Degraded latency.
2. Compare final kill/death counts as tracked by the server (scoreboard authority) against what each client displayed during the match.
**Expected:** Server-authoritative scoreboard is the source of truth and, once settled, matches what clients display after reconciliation; no phantom kills/deaths, no missing kill credit.
**Pass/Fail:** Fail on any discrepancy between server-authoritative score and final client-displayed score after match end.

---

## 4. Format-Picker Matchmaking Queue (1v1 / 2v2 / 3v3 Deathmatch)

### TC-MM-01 — Queue accepts a valid format selection and enters queued state
**Preconditions:** Matchmaking service/queue running, at least one client connected.
**Steps:**
1. Client selects each format in turn (1v1, 2v2, 3v3) from the format picker.
2. Confirm the client enters a "queued" state with the correct format tracked server-side (not just client UI).
**Expected:** Server-side queue record shows correct requested format for each selection; UI reflects queued state accurately.
**Pass/Fail:** Fail if server queue state doesn't match client selection, or if UI shows queued without a corresponding server-side entry.

### TC-MM-02 — 1v1 match forms with exactly 2 players and starts
**Preconditions:** Two clients queue for 1v1.
**Steps:**
1. Both clients select 1v1 and queue.
2. Observe match formation.
**Expected:** Exactly 2 players are matched, match starts on the graybox map, both clients are placed on opposing sides/teams appropriately for 1v1 deathmatch.
**Pass/Fail:** Fail if match starts with wrong player count, wrong team assignment, or fails to start once 2 valid players are queued.

### TC-MM-03 — 2v2 match forms with exactly 4 players, balanced 2-and-2
**Preconditions:** Four clients queue for 2v2.
**Steps:**
1. All four select 2v2 and queue (test both simultaneous queuing and staggered queuing, e.g., 2 queue immediately, 2 queue 10s later).
2. Observe match formation and team assignment.
**Expected:** Match starts with exactly 4 players split 2v2; no team has 3+ or 1 player at match start.
**Pass/Fail:** Fail on incorrect player count or unbalanced teams.

### TC-MM-04 — 3v3 match forms with exactly 6 players, balanced 3-and-3
**Preconditions:** Six clients queue for 3v3.
**Steps:**
1. All six select 3v3 and queue (simultaneous and staggered variants as in TC-MM-03).
2. Observe match formation and team assignment.
**Expected:** Match starts with exactly 6 players split 3v3.
**Pass/Fail:** Fail on incorrect player count or unbalanced teams.

### TC-MM-05 — Queue does not cross-mix formats
**Preconditions:** Mixed pool: e.g., 2 clients queue 1v1, 4 clients queue 2v2, simultaneously.
**Steps:**
1. Queue the mixed pool as described.
2. Observe which matches form.
**Expected:** A 1v1 match forms only from the two 1v1-queued players; a 2v2 match forms only from the four 2v2-queued players. No player is placed into a format they didn't select.
**Pass/Fail:** Fail if any player is matched into a different format than requested.

### TC-MM-06 — Player can cancel/leave queue before match is found
**Preconditions:** One client queued, no match yet formed (insufficient players for that format).
**Steps:**
1. Client cancels queue via UI.
2. Verify server-side queue record is removed.
3. Have other clients queue for the same format up to the required count; confirm the cancelled client is not included in the resulting match.
**Expected:** Cancelled client is fully removed from server-side queue state; does not get pulled into a later match.
**Pass/Fail:** Fail if a cancelled/left player is still matched, or if server queue retains a stale entry.

### TC-MM-07 — Player disconnects while queued
**Preconditions:** One client queued, then forcibly disconnected (kill client process / drop connection) before a match forms.
**Steps:**
1. Disconnect the queued client.
2. Confirm server-side queue cleans up the entry within a reasonable timeout.
3. Confirm remaining queued players for that format are unaffected and can still form a match once enough valid players remain.
**Expected:** No stale/ghost queue entries block match formation; disconnected player is purged from the queue.
**Pass/Fail:** Fail if a ghost entry persists and either blocks match formation or causes a match to start short a player.

### TC-MM-08 — Player disconnects mid-match (post-formation)
**Preconditions:** A match (any format) has started; one player disconnects mid-match.
**Steps:**
1. Force-disconnect one client mid-match.
2. Observe server/match state handling — remaining players' match continuity, scoreboard, and whether the match ends, continues short-handed, or handles it per documented Stage 1 behavior.
**Expected:** Match behavior matches whatever is documented for Stage 1 (even if that's simply "match continues short-handed, no backfill" — Stage 1 scope does not mention backfill/reconnect). If no behavior is documented, flag this as a scope gap to studio-director/game-director rather than assuming expected behavior.
**Pass/Fail:** N/A pending documented expected behavior — file as a scope clarification request if undocumented, not as a pass/fail bug.

### TC-MM-09 `[NET]` — Matchmaking and match start under Regional/Cross-region latency
**Preconditions:** Simulate Regional and Cross-region latency profiles for all queuing clients (mixed latencies across clients where possible, to mimic a real matched pair/group).
**Steps:**
1. Repeat TC-MM-02 through TC-MM-04 with clients under differing simulated latency profiles (e.g., one client Regional, one Cross-region, in a 1v1 match).
2. Confirm match still forms correctly and both/all clients load into the map and synchronize match start (countdown, spawn) together.
**Expected:** Latency differences between matched players do not prevent correct match formation or cause one client to start the match significantly before/after another.
**Pass/Fail:** Fail on match-start desync (e.g., one client sees the match begin, kills registering, before another client has finished loading in).

### TC-MM-10 — Queue scales correctly when multiple matches of the same format form back-to-back
**Preconditions:** Enough clients queued to form 2+ separate matches of the same format (e.g., 4 players queued for 1v1 → should form 2 separate 1v1 matches, not one 4-player match).
**Steps:**
1. Queue 4 clients for 1v1 simultaneously.
2. Observe resulting match(es).
**Expected:** Two independent 1v1 matches form, each with correct 2-player count; no cross-match player bleed.
**Pass/Fail:** Fail if matches are malformed (e.g., wrong grouping, a player left unmatched despite an even queue, or players from one match incorrectly appearing in another).

---

## Coverage Checklist (Stage 1 locked scope)

| Stage 1 system | Covered by |
|---|---|
| Weapon damage | TC-WPN-01, TC-WPN-04, TC-WPN-05 |
| Weapon fire rate (server-enforced) | TC-WPN-02 |
| Weapon hit detection | TC-WPN-03, TC-WPN-06 |
| Movement prediction | TC-MOV-01, TC-MOV-04, TC-MOV-05 |
| Movement rubber-banding under netcode | TC-MOV-02, TC-MOV-03 |
| Server-authoritative hit registration | TC-HITREG-01, TC-HITREG-03, TC-HITREG-04, TC-HITREG-05 |
| Lag compensation | TC-HITREG-01, TC-HITREG-02 |
| Format-picker matchmaking — 1v1 | TC-MM-02, TC-MM-05, TC-MM-09, TC-MM-10 |
| Format-picker matchmaking — 2v2 | TC-MM-03, TC-MM-05 |
| Format-picker matchmaking — 3v3 | TC-MM-04, TC-MM-05 |
| Matchmaking queue lifecycle (join/cancel/disconnect) | TC-MM-01, TC-MM-06, TC-MM-07, TC-MM-08 |

Every locked Stage 1 system has at least one concrete, executable test case above. `[NET]` tags mark the cases that must not be run only on localhost/LAN — these are the priority pass for server-authoritative validation once a build exists.

---

## Bug Report Template

Reuse verbatim for every bug found against this plan (or any other testing) per role spec:

```
PROBLEM          one line
EXPECTED         what should happen
REPRODUCTION     exact steps, every time
CAUSE            if known, else "unknown"
FIX              proposed, if you have one
REGRESSION TEST  how to make sure it doesn't come back
```

**Reminders (per role rules):**
- Critical bugs (crashes, exploits, hit-reg desync) escalate immediately — do not batch into a routine report.
- No bug closes without a regression test attached.
- All testing here is against the locked Stage 1 scope in `/docs/decisions.md` — flag anything tested against scope that has since changed.
- Route all bug reports through studio-director; qa-engineer does not talk to Daniel directly.
