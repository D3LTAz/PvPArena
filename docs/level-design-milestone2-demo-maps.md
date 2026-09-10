# Level Design — Milestone 2 Demo Maps: "Onboarding Range" (Tutorial) and "The Ring" (1v1 Barrier Map)

> **Sequencing note, added 2026-09-10 (see `docs/decisions.md`):** The repo pivoted to single-player practice-vs-AI on 2026-09-07 and already has a shipped map for that (`Lvl_PracticeRing`) with its own tutorial/HUD flow. Per Daniel's 2026-09-10 call, practice-vs-AI ships first; multiplayer resumes later. "The Ring" spec below may still be useful reference if/when a dedicated 1v1-multiplayer map is built in that later phase — check it against `Lvl_PracticeRing`'s actual layout first rather than building from scratch. "Onboarding Range" is very likely superseded by whatever tutorial flow already shipped with practice-vs-AI — confirm before building it.

**Status:** Paper design — graybox specs, pending game-director sign-off before blockout begins
**Author:** level-designer
**Scope note:** These are two new maps for Milestone 2 (Public Demo), per the 2026-09-08 "Demo build scope" decision — a tutorial map and a small "paintball ring" 1v1 map with barriers. Both are separate from, and do not modify, the existing Stage 1 graybox map "Substation" (`/docs/level-design-stage1-graybox.md`), which remains the 3v3 deathmatch map. Same constraints carry over: server-authoritative hit registration (no spawn point may have an instant sightline into a high-traffic area or an opponent's spawn), and grounded/tactical identity — no jump pads, no verticality gimmicks, stairs/ramps only where any elevation exists at all.

---

# MAP 1 — "Onboarding Range" (Tutorial)

## 1. Overview

Onboarding Range is a small, linear sequence of four enclosed stations, gated in order, that teaches a brand-new player one mechanic at a time — movement/look, then aim/fire, then movement+cover, then a combined practice fight against a simple stationary/patrol target — before they are allowed into 1v1 or 1v1-vs-AI. It is not an open map; it is a single corridor of purpose-built rooms with one-way gates between them, so a first-time player cannot wander into a mechanic before they've been taught it, and cannot get lost.

One-line pitch: one door, one lesson, one gate — the player physically cannot reach the next room until they've demonstrated the previous mechanic.

This map is single-player (PvE against static/simple targets, not other players). The server-authoritative "no instant sightline" constraint is adapted here rather than dropped: no station opens onto its target/threat without a controlled transition, so the player is never shot at (or, for the final station, never standing already-exposed to the practice target) the instant a gate opens. See Section 4.

## 2. Scale Rationale

**Target player count:** 1 (solo onboarding, run before a player's first 1v1 or 1v1-vs-AI match).

**Overall footprint:** ~10-12m wide x 58m long, but built and read as four discrete rooms connected by short gated corridors, not as one continuous space — nothing in this map should feel like a "big map," each station is small and self-contained (8m-14m per side).

**Movement assumption:** same 6 m/s sustained sprint as Substation (UE5 template default ~600 cm/s), though most of this map is walked, not sprinted, since it's paced around reading prompts and shooting at static targets rather than combat urgency.

**Traversal times (walking pace ~3 m/s, gate-to-gate, no dwell time for aiming/reading prompts):**

| Segment | Distance | Time (walk, no tasks) |
|---|---|---|
| Orientation Room (spawn to Gate 1) | ~8m | ~2.7s |
| Firing Range (Gate 1 to Gate 2) | ~20m | ~6.7s |
| Corner/Cover Lane (Gate 2 to Gate 3) | ~14m | ~4.7s |
| Final Practice Arena (Gate 3 to exit) | ~10m | ~3.3s |

Pure walking traversal is under 20 seconds end to end — the map is small on purpose. The actual first-time experience is longer once you add reading the on-screen prompts, taking the aim shots, and clearing the practice target, but there's no target duration to hit; a new player should be able to finish in well under two minutes and never feel like they're being kept in a big space.

**Verticality:** none. Single flat floor level throughout, consistent with the grounded/tactical identity — no stairs, ramps, ladders, or jump pads anywhere on this map, since there's no elevation feature to justify one.

## 3. Map Callouts and Layout

Coordinate system for the blockout artist: **X = 0-10m (width, widens to 0-12m only in the Final Practice Arena), Y = 0-58m (length)**, Y=0 at the player's spawn wall. This map is linear, not symmetric — build it once, front to back, in the order below.

### Top-down layout (not to scale)

```
Y=58  ##########################################
      #      FINAL PRACTICE ARENA (10x10)      #
      #   3 cover pillars, 1 practice target     #
Y=48  ####============ GATE 3 =============#####
      #        CORNER / COVER LANE               #
      #   (S-bend at Y39, 2 cover blocks,        #
Y=32  #    pop-up target near the bend)          #
      ####============ GATE 2 =============#####
      #                                          #
      #           FIRING RANGE                   #
      #   targets at 6m / 12m / 18m from line    #
Y=10  #                                          #
      ####============ GATE 1 =============#####
      #        ORIENTATION ROOM (8x8)            #
Y=0   ##########################################
                    [SPAWN, facing north]
```

### Callout list

| Callout | Bounding area (X, Y in meters) | Description |
|---|---|---|
| **Orientation Room** | X0-8, Y0-8 | Enclosed 8x8m room. Single spawn point at X4, Y2, facing north (into the room, not at the exit door). Teaches basic movement/look via on-screen prompts; walking to a floor marker at roughly X4, Y6 triggers Gate 1 to open. No target, no threat, no sightline out until the gate opens. |
| **Gate 1** | X0-10, Y8-10 | One-way gate (closed door/energy-wall, artist's choice of graybox representation). Closed until the orientation checkpoint is reached. Once open, it stays open — this is a forward-only sequence, no backtracking required. |
| **Firing Range** | X0-8, Y10-30 | Straight, clear lane, 8m wide, 20m long, no cover (deliberately — this station teaches aim/fire only, not cover usage). Firing line at Y11. Three static target silhouettes at Y17 (6m from the line), Y23 (12m), Y29 (18m). Targets are inert until the player crosses the firing line, so nothing is "live" the instant Gate 1 opens. |
| **Gate 2** | X0-10, Y30-32 | Opens once the player has hit a minimum threshold of the three range targets (recommend: all three, since there's no time pressure). |
| **Corner/Cover Lane** | X0-10, Y32-46 | 14m S-bend lane. First leg X2-8, Y32-38; corner wall at Y38-39 forces a blind turn; second leg X2-8, Y39-46. Two waist-high cover blocks flank the bend (one on each side, staggered). A pop-up target activates on a trigger near the bend (roughly Y40) once the player is past the corner, requiring them to peek from cover to engage it rather than walk it down in the open. |
| **Gate 3** | X0-10, Y46-48 | Opens once the pop-up target is defeated. |
| **Final Practice Arena** | X0-12, Y48-58 | 10x10m room (widens slightly to X0-12 for the pillar layout). Three cover pillars arranged so the arena entry point (Y48-49) has no unobstructed line to where the practice target first activates — the target is behind the first pillar and only activates once the player has moved 3-4m into the room, so there's no free shot on either side the instant the gate opens. One stationary or simple-patrol training target (whichever the AI system supports at its simplest tier — see Section 7 risk). Landing the kill triggers the "graduation" event: exit door opens, returning the player to the Title/Menu with Tutorial marked complete, unlocking 1v1 and 1v1-vs-AI. |

## 4. Spawn Point and Gate Logic

**Constraint driving this section (adapted for single-player PvE):** the server-authoritative hit-reg rule that no spawn point may have an instant sightline into a high-traffic area or an opponent's spawn is preserved here in spirit — no station in this map opens onto a live threat without a controlled transition. Concretely:

- The single spawn point (Orientation Room) has zero sightline to anything beyond Gate 1 while it's closed, and the room itself contains no target — there is nothing to be "instantly exposed" to even before the gate opens.
- Firing Range targets are inert (non-threatening, teaching-only) until the player crosses the firing line at Y11 — the player is never shot at on this map; this station is pure aim/fire practice.
- The Corner/Cover Lane's pop-up target only activates after the player is past the blind corner and has cover available — it cannot see or "hit" the player before they've had a chance to use the cover the station is teaching.
- The Final Practice Arena's pillar layout means the practice target never has an unobstructed line to the arena's entry point — the equivalent, for this map, of "no spawn point stares at the opponent's spawn." The player must commit a few meters into the room before any engagement is possible.

**Gate sequencing:** Gates 1-3 are one-way, forward-only trigger volumes (walls/doors that open once their station's checkpoint is met and then stay open for the remainder of that playthrough). There is no spawn rotation logic to design here — this is single-player and linear, unlike Substation's team-based respawn system.

## 5. Sightline Analysis

| Line | Max distance | Why it's set this way |
|---|---|---|
| Orientation Room to Firing Range | 0m (Gate 1 closed) | No threat should be visible before the player has done anything — pure orientation. |
| Firing Range, firing line to farthest target | 18m, fully clear, no cover | Intentional — this station is specifically about aim across distance, so the full lane stays visible. Matches the 18m longest-sightline figure already established as fair on Substation (the Pit ramp-to-ramp line), so a new player's first taste of engagement range matches what they'll see in real matches. |
| Corner/Cover Lane, pre-bend to post-bend | 0m direct (corner wall blocks it) | Forces the player to physically peek rather than walk a straight lane — this is the one mechanic (cover) that Substation's design leans on heavily, so it has to be taught before a real match. |
| Corner/Cover Lane, peek positions from cover | ~4-6m | Short and forgiving — first time using cover shouldn't be punished by a long, precise duel. |
| Final Practice Arena, entry to target's activation point | 0m until player advances 3-4m | See Section 4 — no free shot in either direction on entry. |
| Final Practice Arena, pillar-to-pillar engagement lines | ~6-8m | Small arena, short lines, consistent with "grounded, close, teaching" rather than a long-range test. |

## 6. Practice Flow (solo)

A first-time player spawns in the Orientation Room, reads the movement/look prompts, and walks to the checkpoint marker — Gate 1 opens. They enter the Firing Range, engage the three static targets at increasing distance with no movement pressure, and once the threshold is met, Gate 2 opens. They enter the Corner/Cover Lane, are forced into a blind corner by the S-bend, and must peek from the flanking cover blocks to defeat the pop-up target that activates near the bend — Gate 3 opens. Finally they enter the Final Practice Arena, where cover usage and aim/fire come together against a stationary or simple-patrol target; landing the kill graduates them out to the Title/Menu with Tutorial complete, from which 1v1 and 1v1-vs-AI unlock. The sequence is strictly forward — there is no station where the player can reach a later mechanic before an earlier gate has opened, and no station exposes them to a threat before they've had the room to learn that station's specific skill.

## 7. Suggested Teaching Sequence (explicit)

1. **Orientation Room** — movement + look (WASD/mouse or equivalent), via on-screen prompts. Checkpoint: reach the floor marker. No combat.
2. **Firing Range** — aim + fire, against three static targets at 6m/12m/18m. Checkpoint: hit the required minimum (recommend all three). No movement pressure, no cover, isolates the aim/fire mechanic cleanly.
3. **Corner/Cover Lane** — movement + cover usage. The S-bend forces a blind corner; the pop-up target teaches peek-fire-retreat using the flanking cover blocks. Checkpoint: defeat the pop-up target.
4. **Final Practice Arena** — combines everything (movement, aim/fire, cover) against a stationary or simple-patrol target using the three pillars as cover. Checkpoint (graduation): land the kill. Exits to Title/Menu with Tutorial marked complete.

## 8. Blockout Notes for the Artist

- Build station by station in Y order (Orientation Room first, Final Practice Arena last) — this map has no mirroring or symmetry to exploit, unlike Substation.
- Gates 1-3 can be represented in graybox as simple sliding doors or energy-wall placeholders; the important property is that they're one-way (open and stay open) and driven by the checkpoint triggers described per station, not by a timer.
- All cover blocks, pillars, and corner walls should be simple boxes at this stage — no art pass, per the graybox-first constraint in the level-designer role spec.
- No stairs, ramps, ladders, or elevation changes anywhere on this map — it's flat by design, there's no verticality lesson in scope.
- Target/dummy behavior (static silhouette vs. a simple patrol bot) depends on whatever tier of the AI opponent system (per the Milestone 2 decision) is simplest/available first — flag back to game-director if the AI system's minimum viable behavior doesn't support even a stationary target trigger cleanly; that's an engine-session dependency, not something this paper spec can resolve.

## Deliverable Summary — Map 1

```
MAP           Onboarding Range / Tutorial (solo, Milestone 2 demo)
LAYOUT        Linear 4-station corridor, ~10-12m wide x 58m long, gated
              one-way (Gate 1/2/3) so stations can't be skipped or taken
              out of order. Orientation Room (movement/look, 8x8m) ->
              Firing Range (aim/fire, 3 static targets at 6/12/18m, 20m
              lane, no cover) -> Corner/Cover Lane (movement+cover, S-bend
              + pop-up target, 14m) -> Final Practice Arena (combined
              skills vs. a stationary/simple-patrol target, 3 pillars,
              10x10m, graduation trigger on kill). Single spawn point,
              no rotation logic (solo, linear). No verticality anywhere.
FLOW          Spawn -> orientation checkpoint -> aim/fire vs static
              targets -> movement+cover vs pop-up target -> combined
              practice kill -> graduate to Title/Menu, unlocking 1v1 and
              1v1-vs-AI. Strictly forward, no station exposes the player
              to a threat before its lesson has been taught.
RISK          Gating is new to this map (Substation didn't need one-way
              trigger logic) — if gate checkpoints misfire in-engine, a
              player could get stuck or skip a lesson; needs an engine
              session to validate, this is paper-only. Also: the Final
              Practice Arena's target behavior depends on whichever tier
              of the Milestone-2 AI opponent system is simplest to stand
              up first — flag to game-director if even a stationary-target
              trigger isn't available yet.
DONE WHEN     A blockout artist can build this corridor from this document
              alone; playtest confirms a brand-new player completes all
              four stations in order, cannot skip or bypass a gate, is
              never shot at (or exposed with no cover) the instant a gate
              opens, and graduates in well under two minutes.
```

---

# MAP 2 — "The Ring" (Small 1v1 Barrier Map)

## 1. Overview

The Ring is a small, symmetric, barrier-heavy dueling arena — Daniel's "small paintball ring" reference — built specifically for fast 1v1 combat, not as a scaled-down team map. Two spawn alcoves sit on opposite sides of a compact octagonal ring; between them, mirrored rows of chest-high and full-height barriers break every long sightline into short segments, so the round is decided by close-range peeks and barrier trades rather than a single long duel beam. It is round-based (matching the first-to-3-kills standard mode): both players spawn fresh at the start of each round rather than respawning mid-fight into a live map, so there is no spawn-camping window to design against the way Substation's continuous-respawn logic requires — the design problem here is purely "make sure neither fixed spawn alcove can see, or be seen from, the other, or from the contested center, at round start."

One-line pitch: two mirrored alcoves, a maze of barriers in between, no line long enough to reward standing still.

## 2. Scale Rationale

**Target player count:** 2 (1v1 only — this is not built to flex up to 2v2/3v3 the way Substation is).

**Overall footprint:** ~16m wide x 20m long (spawn wall to spawn wall), octagonal boundary (corners of the rectangle cut at 3m x 3m to read as a "ring" rather than a box). This is dramatically smaller than Substation's 34m x 50m footprint — intentional. A duel map with any dead space just makes players walk toward each other; the whole point of "paintball ring" scale is that the two players are in barrier-trading range almost immediately.

**Movement assumption:** same 6 m/s sustained sprint as Substation, for direct comparability.

**Traversal times (spawn to spawn):**

| Route | Distance | Time (sprint, no combat) |
|---|---|---|
| Straight-line spawn-to-spawn (not a real path — blocked by barriers) | ~18m | ~3.0s (reference figure only) |
| Actual shortest real path (alcove -> Approach -> around Center Cross -> opposite Approach -> alcove) | ~20m | ~3.3s |

This is roughly half of Substation's ~6-7s spawn-to-spawn traversal, as required — noticeably smaller and faster, consistent with a "small paintball ring" identity rather than a team map scaled down.

**Time-to-first-contact target:** 3-6 seconds. Unlike Substation, players don't need to reach the opposite spawn to make contact — the Center Cross barrier cluster sits roughly equidistant between both alcoves, so a typical round's first exchange happens near the center well before either player has covered the full 20m path. This is a deliberately fast, close-quarters pace matching the "small paintball ring" reference.

**Verticality:** none. Single flat floor level, no elevation feature at all (unlike Substation's one deliberate catwalk/pit) — a map this small doesn't need or benefit from a height element, and it would work against the barrier-trading identity Daniel asked for. No stairs, ramps, ladders, or jump pads.

## 3. Map Callouts and Layout

Coordinate system for the blockout artist: **X = 0-16m (width), Y = 0-20m (length)**, Y=0 at Player A's back wall, Y=20 at Player B's back wall. The map is front-back symmetric around the Y=10 centerline (A's half mirrors B's), so build one half and mirror it, same approach as Substation.

### Top-down layout (not to scale, orientation North=B, South=A)

```
Y=20  ****########################################****
      #              B SPAWN ALCOVE (X6-10)           #
Y=18  ##+=====angled exit=====+#######################
      #     NORTH APPROACH (bunkers at X5, X11)        #
Y=14  #----+-----------------------------------+-------#
      #    |  WEST      CENTER CROSS      EAST |       #
      #    |  BUNKER   (4-barrel diamond) BUNKER|       #
Y=10  #    |  ROW       X6-10, Y8-12       ROW  |       #  <- centerline
      #    |  (X0-4)                      (X12-16)     #
Y=6   #----+-----------------------------------+-------#
      #     SOUTH APPROACH (bunkers at X5, X11)         #
Y=2   ##+=====angled exit=====+#######################
      #              A SPAWN ALCOVE (X6-10)            #
Y=0   ****########################################****
```
(`****` marks the cut corners of the octagonal boundary — purely a boundary shape, not gameplay-relevant.)

### Callout list

| Callout | Bounding area (X, Y in meters) | Description |
|---|---|---|
| **A Spawn Alcove** | X6-10, Y0-2 | Small recessed 4m x 2m nook, one spawn point at X8, Y1. Single exit on an angled wall (not facing straight up the ring) leading into South Approach — the angle means the exit doorway itself doesn't line up with anything beyond the first bunker. |
| **B Spawn Alcove** | X6-10, Y18-20 | Mirror of A Spawn Alcove. |
| **South Approach** | X4-12, Y2-6 | Short entry lane immediately outside A's alcove. Two mirrored L-shaped bunkers at X5 and X11 sit right at the mouth of the alcove exit, breaking any sightline further up the ring within the first couple of meters. |
| **North Approach** | X4-12, Y14-18 | Mirror of South Approach, outside B's alcove. |
| **West Bunker Row** | X0-4, Y6-14 | Tall barrier row along the west wall, zigzagged (offset every ~3m) to form a flanking route with no straight run longer than ~4m. |
| **East Bunker Row** | X12-16, Y6-14 | Mirror of West Bunker Row, the flanking route on the east wall. |
| **Center Cross** | X6-10, Y8-12 | Four barrels/crates arranged in a small diamond at the ring's midpoint — the map's contested center. Dense and tall enough that no full corner-to-corner diagonal line through it exceeds ~10m; it caps the map's single longest possible sightline. |

Barrier sizing guidance for the artist: treat "bunkers" and "barrels" as simple boxes/cylinders, roughly 1.5m wide x 1m deep x 1.8m tall (chest-to-head height — blocks sightline standing, allows crouch-peek if that's in scope; flag to game-director if crouch isn't in the Milestone 1 movement set, in which case make them full 2m+ height instead).

## 4. Spawn Point Placement and Round Logic

**Constraint driving this section:** same server-authoritative hit-reg rule as Substation — no spawn point may have line-of-sight, even partial, into a high-traffic area (the Approaches, either Bunker Row, or the Center Cross) or into the opponent's spawn, at the moment a player spawns.

**Structure:** one fixed spawn point per player per round (X8/Y1 for A, X8/Y19 for B) — this is a 1v1 duel map, not a team map with rotating clusters, so there's no contested-fallback logic to design the way Substation needs for 3v3.

**Why this satisfies the constraint by construction:**
- Each alcove's only opening is the single angled exit doorway, and that doorway does not point down a clear lane — the nearest bunker (at the mouth of the Approach) sits directly in the sightline a straight-out exit would otherwise have. A Spawn Alcove to B Spawn Alcove: 0m, fully blocked by the South Approach bunkers, the Center Cross, and the North Approach bunkers in series.
- Spawn Alcove to Center Cross: 0m at spawn (blocked by the Approach bunkers); the player has to physically clear the Approach before the Center Cross comes into view.

**Round logic (first-to-3-kills):** each round, both players spawn simultaneously at their fixed alcove points and the round runs until one player is eliminated (round point awarded), then both reset to their alcoves for the next round — there is no mid-round respawn the way Substation has, so there's no spawn-camping window: nobody is already alive and positioned on the map when a new round begins, both players start from an identical, symmetric, sightline-blind alcove every time. **Fairness measure:** alternate which player spawns at the A alcove vs. the B alcove each round (even though the map is symmetric, this cancels out any residual micro-asymmetry — e.g. handedness of a corner bunker's exact lip — that a graybox pass might introduce before final geometry).

## 5. Sightline Analysis

| Line | Max distance | Cover interval | Why it's fair |
|---|---|---|---|
| A Spawn Alcove to B Spawn Alcove | 0m | N/A | Fully blocked in series by South Approach bunkers, Center Cross, North Approach bunkers. By design — this is what keeps round starts safe. |
| Spawn Alcove to nearest Approach bunker sightline | 0m | Angled exit + bunker at the mouth | No player ever exits their alcove into an open sightline. |
| South/North Approach, bunker to bunker | ~6m | Two mirrored L-bunkers per approach | Short and close-quarters by design, matches the "paintball ring" scale. |
| West/East Bunker Row, full length | ~8m | Zigzag every ~3m, capping any real line to ~4m | Flanking routes stay close-range; nobody gets a long beam down either side wall. |
| Center Cross, longest diagonal corner-to-corner | ~10m | 4-barrel diamond at the middle of the line | This is the single longest sightline on the map, and it's the known, telegraphed centerpiece — both players know it's there, it's symmetric, and at 10m it's still short relative to the single starting weapon's effective range, so it's a fast, fair trade rather than a free kill. |

General fairness notes: the map is front-back symmetric (A's half mirrors B's exactly), and round-based spawning with alternating sides removes any first-round positional edge. Every sightline on the map, including the longest one (Center Cross, ~10m), is shorter than Substation's shortest indoor lane (Warehouse Interior, ~12m) — consistent with "noticeably smaller/faster," this map should never produce an engagement at a range that feels like it belongs on the team map.

## 6. Match Flow (1v1)

A round starts with both players spawning into their alcoves simultaneously, with zero information about the other's position (no sightline out of either alcove). Both push through their Approach — the mirrored bunkers there mean neither player can pre-aim the other's exit — and the round typically resolves at or near the Center Cross, since it's the map's shortest path between the two alcoves and its longest sightline. A player who instead peels wide into a Bunker Row is taking a flanking route with shorter, choppier sightlines and more total distance to cover, trading a slower approach for one the opponent isn't holding an angle on. After a kill, both players reset to their alcoves for the next round rather than continuing to fight in a live space, so there's no post-kill snowball within a round the way a continuous-respawn map has to guard against — the design risk here is round-to-round, not mid-round (see Section 7).

## 7. Blockout Notes for the Artist

- Build Player A's half first (Y0-10), then mirror it across Y=10 for B's half, same approach as Substation — do not hand-build B's half separately.
- All bunkers, barrels, and the alcove walls should be simple boxes/cylinders at this stage — no art pass, no material work, per the graybox-first constraint in the level-designer role spec.
- Corner-cut the 16x20 rectangle's four corners (3m x 3m triangles) for the octagonal "ring" read — this is a boundary-shape note only, it doesn't affect any callout's gameplay footprint above.
- No stairs, ramps, ladders, or elevation changes anywhere on this map — flat by design, consistent with the grounded/tactical identity and with the fact that a map this small has no room for a height feature without it dominating play.
- Flag anything in this doc that doesn't translate cleanly to UE5 geometry back to level-designer/game-director rather than improvising a fix — this is a paper spec written without an engine session to validate against.

## Deliverable Summary — Map 2

```
MAP           The Ring / 1v1 duel with barriers (Milestone 2 demo)
LAYOUT        Front-back symmetric, 16m x 20m octagonal footprint (vs.
              Substation's 34m x 50m). Two recessed spawn alcoves with
              angled exits (0m sightline out) feeding short Approaches
              (mirrored L-bunkers), which open onto a barrier-dense
              center: a West and East zigzag Bunker Row (flanking
              routes, ~8m max, capped to ~4m segments) and a central
              4-barrel "Center Cross" diamond (the map's longest line,
              ~10m). One fixed spawn point per player per round — no
              rotation logic needed since spawning is round-based, not
              mid-fight.
FLOW          Round-based (first-to-3-kills): both players spawn
              simultaneously at their alcove each round, push through a
              blind Approach, and typically meet at the Center Cross
              (shortest path, longest sightline) or trade a slower,
              choppier flank via a Bunker Row. Reset to alcoves after
              each kill; alternate which player spawns at which alcove
              round to round for fairness. Scale rationale: ~3.3s
              spawn-to-spawn (about half of Substation's ~6-7s), 3-6s
              target time-to-first-contact — noticeably faster and
              tighter than the team map, matching the "small paintball
              ring" reference.
RISK          Tight quarters and heavy barrier density mean a single
              choke (especially the Center Cross) could become dominant
              for whichever player reaches it first, and unlike 3v3
              there's no teammate to punish a camped angle — mitigated
              by round-based reset and alternating spawn sides, but
              flag for playtesting. Also: at this close a range, peeker's
              advantage under server-authoritative hit-reg is more
              pronounced than on Substation's longer sightlines — the
              hit-reg/lag-compensation tuning that was fine at 12-22m on
              Substation may need extra scrutiny at this map's 4-10m
              engagement ranges.
DONE WHEN     A blockout artist can build this graybox from this document
              alone; playtest confirms no spawn alcove has line-of-sight
              to the opposite alcove or into the Center Cross at round
              start, no sightline on the map exceeds the ~10m figure
              above, and measured time-to-first-contact across a set of
              1v1 rounds falls in the 3-6s target range.
```
