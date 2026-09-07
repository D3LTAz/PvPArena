# Level Design — Stage 1 Graybox: "Substation"

**Mode:** Deathmatch (1v1 / 2v2 / 3v3, designed at 3v3 scale)
**Status:** Paper design — graybox spec, pending game-director sign-off before blockout begins
**Author:** level-designer
**Scope note:** This map is sized and built for the locked Stage 1 format (3v3 deathmatch, single starting weapon, server-authoritative hit registration). It is not a CTF map and not a sniper map — those are later scope per `/docs/decisions.md` and nothing here should be read as building toward them.

---

## 1. Overview

Substation is a symmetric, mid-size industrial map: two team spawn compounds at opposite ends of a rail/power-yard complex, connected by three parallel routes (a left outdoor lane, a center sunken plaza with an elevated catwalk, and a right indoor warehouse lane). The layout is a compact, readable three-lane shooter map in the CS/Valorant tradition, scaled down for 6 total players instead of 10.

One-line pitch: three short, distinct routes funnel into one central contested plaza, so a 3v3 team either splits across lanes or stacks center — never a single dominant path.

---

## 2. Scale Rationale

**Target player count:** 6 (3v3), also playable at 1v1/2v2 without dead space.

**Overall footprint:** ~34m wide x 50m long (spawn back-wall to spawn back-wall), max height ~6m (catwalk +3m over pit floor at -1.5m, plus roof clearance). This is deliberately smaller than a 5v5 CS map (de_dust2-class maps run ~55-65m per side) — with only 3 attackers per side, a bigger footprint produces dead time with no contact, which reads as empty rather than tense. Everything here is sized to keep first contact fast and repeatable, not to fill space.

**Movement assumption:** ~6 m/s sustained sprint speed (matches the UE5 default ~600 cm/s used by the template/Lyra reference named in decisions.md). All traversal times below use this.

**Traversal times (spawn to spawn, i.e. worst case):**
| Route | Distance | Time (sprint, no combat) |
|---|---|---|
| Left lane (Pipeline), full length | ~34m | ~5.7s |
| Center (through Connector Hub -> Pit -> Connector Hub) | ~40m | ~6.7s |
| Right lane (Warehouse Interior) | ~36m | ~6.0s |

**Time-to-first-contact target:** 8-12 seconds from spawn for players advancing toward center, once you add the hub-room dogleg (no straight sprint is possible — see Section 5) and normal caution/peeking. This is short enough that a 3v3 round stays active, long enough that a fresh spawn is never plunged directly into a firefight (see spawn safety logic, Section 4).

**Verticality:** one deliberate elevation feature (the central catwalk over the sunken Transformer Pit, +3m / -1.5m relative to main ground level), reached by stairs/ramps only — no ladders, no jump pads, no double-jump-reliant geometry. This keeps the map grounded and tactical rather than arena-shooter movement-tech-dependent, per the locked identity.

---

## 3. Map Callouts and Layout

Coordinate system for the blockout artist: **X = 0-34m (width), Y = 0-50m (length)**, Y=0 at Team A's back wall, Y=50 at Team B's back wall. The map mirrors front-to-back around the Y=25 centerline (A's compound is a 180-degree mirror of B's), so only Team A's half needs to be built once and mirrored.

### Top-down layout (not to scale, orientation North=B, South=A)

```
Y=50  ############################################
      #        B SPAWN ROOM (X10-24)             #
Y=44  ############################################
      #   Connector Hub B (hub, no cross-sightline)#
Y=40  ###########+========================+#######
      #          |   B WAREHOUSE ENTRANCE |       #
Y=36  #  B YARD  |------------------------|WAREHOUSE
      # (X0-10)  |   WAREHOUSE INTERIOR   |INTERIOR
Y=30  #----------|  (3 rooms, X24-34)     |--stair-+
      # PIPELINE |                        |        |
      #  LANE    +----- CATWALK (+3m) ----+        |
Y=25  #  (bend    |   over TRANSFORMER PIT |        |  <- centerline
      #  at Y25)  |     (-1.5m) X10-24     |        |
Y=20  #----------+----- stair -------------+--------+
      #  A YARD  |   WAREHOUSE INTERIOR    |
Y=16  #  (X0-10) |------------------------|
      #          |  A WAREHOUSE ENTRANCE  |
Y=10  ###########+========================+#######
      #   Connector Hub A (hub, no cross-sightline)#
Y=6   ############################################
      #        A SPAWN ROOM (X10-24)              #
Y=0   ############################################
```

### Callout list

| Callout | Bounding area (X, Y in meters) | Description |
|---|---|---|
| **A Spawn Room** | X10-24, Y0-6 | Enclosed room, one back wall, two forward doorways both leading only into Connector Hub A. No window/sightline to anything outside the room. |
| **B Spawn Room** | X10-24, Y44-50 | Mirror of A Spawn Room. |
| **Connector Hub A** | X10-24, Y6-10 | Small hub room (~14m x 4m) with an internal dividing wall so the west doorway (to A Yard) and east doorway (to A Warehouse Entrance) do not see each other. Third doorway (north, center) leads to the Transformer Pit's south approach. This hub is the spawn buffer — see Section 4. |
| **Connector Hub B** | X10-24, Y40-44 | Mirror of Connector Hub A. |
| **A Yard / B Yard** | X0-10, Y10-20 and Y30-40 | Outdoor yard, crate-cluster cover, opens north/south into Pipeline Lane. |
| **Pipeline Lane** | X0-10, Y16-34 | Long left-side outdoor lane. Contains a dogleg bend centered at Y25 (the lane jogs ~3m in X at the midpoint) — this caps any straight sightline down the lane at ~18m instead of the full 34m length. Pipe-stack cover clusters every 6-8m along its length. Side door at Y23 connects to the Catwalk's west stair. |
| **A/B Warehouse Entrance** | X24-34, Y10-18 and Y32-40 | Indoor entry rooms into the Warehouse Interior. |
| **Warehouse Interior** | X24-34, Y14-36 | Right-side indoor lane, broken into three connected rooms (storage, office, loading bay) by doorways and half-walls, so no sightline inside exceeds ~12m. Side door near Y29 connects to the Catwalk's east stair. |
| **Transformer Pit** | X10-24, Y18-32, floor at -1.5m | Central sunken plaza, main deathmatch hotspot. Reached from both Connector Hubs via ramped approaches (not stairs — server-authoritative hit-reg favors predictable, un-obstructed movement over ladder-clutter). Scattered transformer/crate cover blocks. |
| **Catwalk** | X14-20, Y18-32, elevated +3m over the Pit | Metal catwalk spanning the Pit lengthwise, connecting Pipeline Lane (west stair) to Warehouse Interior (east stair). Grants high ground over the Pit but is itself exposed from both Pit approach ramps — see sightline table. |

---

## 4. Spawn Point Placement and Rotation Logic

**Constraint driving this section:** hit registration is server-authoritative — a spawn cannot rely on client-side prediction to paper over a player being shot before they've meaningfully reacted. So no spawn point may have line-of-sight, even partial, into a high-traffic lane (Pipeline, Warehouse Interior, or the Pit) at the moment a player spawns.

**Structure per team:** 6 total spawn points, in two clusters of 3.

- **Primary cluster (3 points):** inside the team's Spawn Room (X10-24, Y0-6 for A / Y44-50 for B). Points are spread ~2m apart in a shallow arc facing the room's forward doorways. The Spawn Room has zero external sightlines — its only openings are the two doorways into the Connector Hub, and the Hub itself is compartmentalized (Section 3) so nothing beyond the Hub's near wall is visible from a spawn point. This satisfies the no-instant-exposure constraint by construction: a spawning player cannot see, and cannot be seen from, any lane.
- **Secondary cluster (3 points):** inside the Connector Hub itself (X10-24, Y6-10 for A / Y40-44 for B), tucked against the hub's dividing wall, out of view of all three of the hub's doorways. Used as fallback when the primary cluster is contested (below).

**Selection logic per respawn (server-side, evaluated against live enemy positions):**

1. Build the candidate list from the team's 6 points (primary + secondary).
2. Discard any candidate with an enemy within 15m in any direction, or with existing line-of-sight to any living enemy (checked at spawn-point-to-player, not just point-to-point, since enemies move).
3. Of the remaining candidates, weight toward the primary cluster first (it's deeper from the action); only fall back to the secondary (Hub) cluster if all three primary points are discarded in step 2.
4. If every one of the 6 points is discarded (all contested) — this should be rare given the map's size relative to 3 enemies — fall back to whichever single point has the greatest aggregate distance from all living enemies, ignoring the LoS check as a last resort, and log it; this case should not occur in normal 3v3 play and its occurrence during playtesting is itself a signal the map or player-count-to-space ratio needs revisiting.
5. Within whatever cluster is chosen, distribute respawning teammates across its 3 points rather than stacking them on one, so a team that just wiped doesn't reform as one clumped target.

**Why this avoids spawn-camping specifically:** because both clusters sit behind the compartmentalized Hub with no sightline into any lane, an enemy cannot set up on a lane entrance and get a first-look advantage on a spawning player — they'd have to physically push into the Hub itself, at which point they're exposed on both of *its* doorways to anyone in the Yard or Warehouse Entrance, which is a bad trade for the attacker. There is no position anywhere on the map with sightline into a spawn point, so this rule is structural, not just a spawn-selection heuristic.

---

## 5. Sightline Analysis

| Line | Max distance | Cover interval | Why it's fair |
|---|---|---|---|
| Pipeline Lane (either half, spawn side of the Y25 dogleg) | ~18m | Pipe cover every 6-8m | Capped by the dogleg — nobody gets a spawn-to-spawn beam down this lane. Matches the single starting weapon's effective range without turning the lane into a free-kill funnel. |
| Warehouse Interior (any single room) | ~12m | Doorway/half-wall every room | Indoor compartmentalization keeps this the shortest-range lane on the map — favors close-range engagement, gives teams a reason to actually choose this route instead of it being strictly worse than Pipeline. |
| Transformer Pit, ramp-to-ramp (Connector Hub A approach to Connector Hub B approach) | ~22m | Transformer/crate blocks at roughly the thirds | Longest sightline on the map, but it's the map's known, telegraphed centerpiece — both teams know it's there and it's symmetric, so it's a contested resource, not a trap either side falls into blind. |
| Catwalk down into Pit | ~15m (catwalk player has height advantage over Pit-floor players) | None (catwalk is intentionally exposed) | Height advantage is real but the catwalk player is silhouetted against the roof/sky from both Pit ramps and both stair entries — it's a strong position, not an unanswerable one. No cover on the catwalk itself is deliberate: holding it is a commitment, not a safe camp. |
| Connector Hub, doorway to doorway | 0m (internal dividing wall blocks it) | N/A | By design — this is what keeps spawns safe (Section 4). |
| Spawn Room to anything outside | 0m | N/A | By design — no line of sight leaves the Spawn Room. |

General fairness notes: the map is front-back symmetric (A's approach mirrors B's exactly), so no sightline table entry above favors one team over the other. The longest line on the map (Pit, ~22m) is still short relative to typical AR-class engagement ranges, consistent with the single-starting-weapon constraint — there is no sightline on this map long enough to reward a weapon class Stage 1 doesn't have.

---

## 6. Match Flow (3v3)

A typical 3v3 round: both teams clear their Connector Hub within the first few seconds (no reason to linger — nothing is visible from there). From the Hub, a team of 3 generally splits 1-1-1 or 2-1 across the three routes rather than stacking all three on one lane, since each lane dead-ends into the same central Pit/Catwalk — stacking one lane just delays the fight without controlling more space. Pipeline and Warehouse are where flanks and skirmishes happen; the Pit and Catwalk are where the round's central fight usually resolves, since both remaining routes feed into it. After a kill, the respawn logic (Section 4) puts the respawning player back into a route rather than directly into the fight that just happened, so momentum doesn't compound into an unanswerable snowball within a single life — but a team that's controlling the Pit/Catwalk consistently is a legitimate, readable win condition, not an exploit.

---

## 7. Blockout Notes for the Artist

- Build to the X/Y/height figures in Sections 3 and this section; treat the ASCII diagram as topology, not final proportions — use the meter figures for actual placement.
- All walls, doorways, and cover blocks should be simple boxes/wedges at this stage — no art pass, no material work, per the graybox-first constraint in the level-designer role spec.
- Build Team A's half first (Y0-25), then mirror it across Y=25 for Team B's half to guarantee the symmetry this whole design depends on. Do not hand-build B's half separately.
- Stairs/ramps only for the Catwalk and Pit access — no ladders, no jump pads, confirmed by the "grounded, not arena-shooter" identity constraint.
- Flag anything in this doc that doesn't translate cleanly to UE5 geometry back to level-designer/game-director rather than improvising a fix — this is a paper spec written without an engine session to validate against.

---

## Deliverable Summary

```
MAP           Substation / 3v3 deathmatch (Stage 1 single map)
LAYOUT        Front-back symmetric, 34m x 50m footprint. Two enclosed spawn
              rooms (no external sightlines) each feeding a compartmentalized
              Connector Hub, which opens onto three routes: Pipeline Lane
              (outdoor, dogleg-capped at ~18m sightline), Warehouse Interior
              (indoor, 3 rooms, ~12m max sightline), and a central sunken
              Transformer Pit (~22m max sightline) with an elevated Catwalk
              (+3m, no cover, height-advantage-but-exposed). 6 spawn points
              per team (3 in Spawn Room, 3 in Hub as contested fallback),
              server-side selection excludes any point within 15m or LoS of
              a living enemy. Scale rationale: sized for 6 total players
              (3v3), ~6-7s spawn-to-spawn traversal, 8-12s target
              time-to-first-contact.
FLOW          Teams clear their Hub immediately (nothing visible from it),
              split across the three routes rather than stacking one, and
              the round's central fight resolves at the Pit/Catwalk since
              all three routes feed into it. Respawns re-enter via a route,
              not directly into an active fight.
RISK          The Pit/Catwalk is the map's one long-ish sightline (~22m) and
              its main power position — if 3v3 playtesting shows one team
              can hold it uncontested for an entire life cycle, it will read
              as a spawn-adjacent camp risk even though it's not literally
              spawn-adjacent, and the Pit cover density may need to increase.
DONE WHEN     A blockout artist can build this graybox from this document
              without further design decisions; playtest confirms no spawn
              point has line-of-sight into Pipeline Lane, Warehouse Interior,
              or the Transformer Pit, and no observed spawn-kill lines in a
              3v3 session.
```
