# Milestone 2 Art Polish — Scope for Public Demo Build

**Context:** The 2026-09-08 "Demo build scope (Milestone 2)" decision calls for "polished character/weapon/bullet visuals" plus start-screen artwork for the public demo (title screen: Tutorial / 1v1 / 1v1-vs-AI / Settings). This is a **direct reversal** of the Milestone 1 rule in `technical-artist-stage1-pipeline.md` ("Placeholder-quality is fine if it unblocks testing"). That M1 pipeline doc scoped exactly one placeholder weapon + a 3-clip animation set and explicitly deferred texturing/rigging complexity. This doc scopes what changes on top of it — it does not redo the M1 pipeline, which still stands as the base process for modeling/import/naming conventions.

**Owner:** technical-artist. Art direction (final character look, weapon silhouette, VFX style) is not decided here — presented to studio-director as tradeoffs, per role spec.

---

## 1. What "polished" concretely means, by asset category

### 1.1 Character model + animation
| | M1 baseline | M2 demo requirement |
|---|---|---|
| Model | None scoped in M1 (M1 is first-person weapon/arms only — no third-person or menu-visible character called out) | At least one full character model, game-ready topology, third-person-visible quality (players will see opponents and themselves in a public demo, including on a title screen or character preview) |
| Texturing | Flat/basic PBR acceptable, authored directly in Blender/UE5 | Real PBR texture set (base color, normal, roughness/metallic at minimum) — flat placeholder materials will read as visibly unfinished on a public-facing build |
| Rigging | N/A (no character rig in M1) | Full humanoid skeleton compatible with UE5's standard bone naming (ideally Epic skeleton / Mannequin-compatible so Lyra animation retargeting is usable), including IK bones for feet/hands if third-person locomotion needs it |
| Animation set | N/A | Basic third-person locomotion set: idle, walk/run (forward + strafe), jump/land, death, plus weapon-holding pose sync with the first-person rig. This is a materially larger animation scope than M1's 3-clip weapon-only set — a full locomotion set is typically 8-15 clips vs. M1's 3. |
| Rig complexity vs. M1 weapon rig | M1 rig: single root + weapon bone, no IK, no physics-driven secondary motion — deliberately minimal because it only needed to hold and fire a gun on screen | M2 character rig: full humanoid hierarchy, needs IK for foot placement/aim offsets, likely needs a retargeting step if animations are sourced from Mixamo/marketplace rather than hand-keyed. This is a different order of rigging work, not a bigger version of the same task. |

**Feasibility:** Not reasonably solo within the demo timeline if animation quality needs to look "polished" rather than functional. Modeling one polished character solo is plausible if Daniel already has modeling chops; full biped rigging + a clean locomotion animation set is where solo time balloons — this is commonly the single most time-expensive art task on an indie team, often several weeks of dedicated work even for one character. **Recommend contractor: 3D character artist** (model + texture) and/or a **rigger/animator** if that's a separate skillset from whoever models it — these are commonly split roles.

### 1.2 Weapon model + VFX
| | M1 baseline | M2 demo requirement |
|---|---|---|
| Model | Placeholder-quality single weapon, flat materials acceptable | Same weapon (or a polish pass on it) brought to game-ready textured quality — this can reuse the M1 rig/socket/naming pipeline unchanged, it's a texture/material quality upgrade plus optional geo detail pass, not a new pipeline |
| Muzzle flash | Not scoped in M1 | Real VFX: Niagara particle system (UE5's VFX system) — flash sprite/mesh, light flicker, timed to the fire animation frame |
| Tracer | Not scoped in M1 | Niagara ribbon/beam effect from muzzle to impact point, timed to projectile travel or hitscan trace |
| Impact VFX | Not scoped in M1 | Niagara particle burst + decal on surface hit (spark/dust depending on surface), plus impact sound hook (audio is not this role's scope but the VFX timing needs to leave a hook for it) |

**Pipeline implication:** This is the first time Niagara enters the pipeline — M1's dependency list only covers Blender/UE5/Git LFS for meshes. **New dependency: UE5 Niagara VFX system** (built into UE5, no extra license cost, but is a distinct skillset from modeling/rigging — particle system authoring, not DCC mesh work).

**Feasibility:** The weapon model polish pass is plausibly solo (it's a texture upgrade on an existing rigged asset, following the existing M1 pipeline). The three VFX effects (muzzle flash, tracer, impact) are a different skillset — Niagara authoring is closer to technical VFX than to modeling, and doing all three well (readable, not distracting, hardware-cheap) is its own specialty. **Recommend contractor: VFX artist**, unless Daniel already has Niagara experience — flag this as a judgment call for studio-director rather than assuming solo-infeasible, since it's more scriptable/parameterizable than character animation and a motivated solo dev could plausibly get a "good enough for demo" version working from Niagara templates/examples.

### 1.3 Bullet/tracer visual treatment
Covered above under weapon VFX (tracer + impact), but calling out separately since it's also a **gameplay-readability decision**, not purely an art one: hitscan vs. visible-projectile changes what the tracer represents (a instantaneous line-render effect vs. an actual traveling mesh/particle). This should be confirmed with game-director/level-designer before VFX authoring starts, since M1's hit registration is server-authoritative hitscan with lag compensation — the tracer is a purely visual representation layered on top of that, not a physical projectile, and needs to stay consistent with the netcode model already locked in M1.

### 1.4 Title screen / start-screen artwork
| | Requirement |
|---|---|
| Asset type | 2D/UI illustration — title screen background art, menu layout art (Tutorial / 1v1 / 1v1-vs-AI / Settings), possibly a logo/wordmark |
| Skillset | **This is a different discipline from 3D character/weapon art.** 3D modeling/rigging/VFX skills do not transfer to 2D illustration, UI layout, or typography/branding work. Flagging explicitly per the task brief: this likely needs a **separate contractor — a 2D/UI illustrator or UI artist**, not the same person doing character/weapon work, unless Daniel is deliberately hiring one generalist who covers both (uncommon and worth confirming rather than assuming). |
| Pipeline implication | This doesn't touch the M1/M2 3D pipeline (Blender/FBX/UE5 import) at all — it's UMG (UE5's UI system) + source art authored in a 2D tool (Photoshop/Affinity/Krita, outside this role's current tooling list). **New dependency: UE5 UMG** for wiring the art into the actual title screen widget, plus whatever 2D authoring tool the illustrator uses (their choice, not prescribed here). |

---

## 2. Pipeline/tooling changes from the M1 doc

The M1 pipeline (Blender → FBX → UE5 import, naming convention, socket-based weapon attachment) **still applies unchanged** for the weapon model polish pass and reused as the base process for the character model. What's new for M2:

1. **Niagara VFX pipeline** (muzzle flash, tracer, impact) — not covered in M1 at all, needs its own asset naming convention (recommend `NS_<Effect>_<Variant>` per UE5 convention, e.g. `NS_MuzzleFlash_Rifle01`) and its own documented process once someone (contractor or Daniel) starts building them — out of scope to fully spec here until it's confirmed who's doing the work and what their Niagara experience level is.
2. **Full humanoid rig + retargeting step** for the character — a materially different rigging approach than M1's flat weapon-only skeleton (Section 3 of the M1 doc explicitly says "Stage 1 does not need a full character/IK rig" — M2 does). If animations come from an external source (Mixamo, marketplace) rather than hand-keyed, add a retargeting pass to the Epic skeleton as a new pipeline step.
3. **UMG + 2D art import** for the title screen — a new, separate track from the 3D pipeline; doesn't block or depend on the character/weapon work and could proceed in parallel with a different contractor.
4. **Git LFS scope grows**: character textures, Niagara assets, and 2D UI art all add binary asset weight — no process change needed (M1 already put Git LFS in place for this reason) but worth noting for finance-ops/game-director's repo size expectations.

---

## 3. Deliverable summary

```
ASSET         Milestone 2 art polish scope: (1) one polished third-person character model
              + basic locomotion animation set, (2) polish pass on the existing M1 weapon
              model, (3) three Niagara VFX effects (muzzle flash, tracer, impact) plus
              bullet/tracer visual treatment consistent with M1's hitscan netcode, (4)
              title-screen/start-screen 2D art (Tutorial/1v1/1v1-AI/Settings menu).

PIPELINE      Weapon polish reuses the M1 Blender->FBX->UE5 pipeline unchanged (texture/
              material upgrade only). Character model uses the same DCC/export process
              but needs a full humanoid rig (Epic-skeleton-compatible for retargeting) in
              place of M1's flat weapon-only skeleton -- new pipeline step. VFX is a new
              track entirely: UE5 Niagara, no naming convention or process yet documented
              (deferred until a VFX contractor/owner is confirmed). Title screen art is a
              fully separate 2D/UMG track, independent of the 3D pipeline, that can run in
              parallel once a 2D/UI contractor is engaged.

DEPENDENCIES  Everything in the M1 doc (UE5, Blender, Git LFS) plus: UE5 Niagara (built-in,
              no license cost, but a distinct authoring skillset), UE5 UMG for wiring title-
              screen art into the menu, a 2D authoring tool for the UI illustrator (contractor's
              choice), and likely an Epic-skeleton-compatible animation source if character
              locomotion isn't hand-keyed from scratch.

RISK          This is a real step up from M1's "placeholder is fine" rule, not a small
              extension of it -- full character rigging/animation and a from-scratch VFX
              set are each substantial scope on their own, and title-screen art needs an
              entirely different skillset (2D/UI illustration) than the 3D/VFX work. Likely
              outcome is significant solo time (multi-week, most acutely on character
              rigging/animation) or contractor budget across up to three specialties
              (3D character artist/rigger, VFX artist, 2D/UI illustrator) -- not one hire.
              Scoping this as one undifferentiated "art polish" line item risks under-
              budgeting it.

DONE WHEN     Character model + locomotion set imports and animates in-engine at correct
              scale (same scale-mismatch risk as M1, now compounded by a full skeleton);
              weapon polish pass imports without breaking the existing M1 socket/animation
              setup; all three VFX effects play correctly timed to fire/impact events in
              a Niagara-driven test scene; title screen art is wired into UMG and the
              Tutorial/1v1/1v1-AI/Settings menu renders correctly at the target resolution.
```

---

## NEEDS FROM FINANCE-OPS

This is flagged as a **real budget item, not a scheduling item**, before any contractor work starts:

- A cost estimate is needed for up to **three separate contractor engagements**: (1) 3D character artist + rigger (model, texture, rig, locomotion animation for one character), (2) VFX artist (Niagara muzzle flash/tracer/impact — or confirm Daniel/technical-artist role can cover this solo, which is more plausible than the character work), (3) 2D/UI illustrator (title screen + menu art).
- No dollar figures are proposed here — that's finance-ops's call once scope is priced against actual contractor quotes or market rates.
- Recommend finance-ops loop in **contracts-manager** for engagement/scope-of-work templates as soon as even one of these is confirmed as contractor-bound, so contract paperwork isn't the bottleneck once budget is approved.
- Until this estimate lands, studio-director should treat Milestone 2's art polish timeline as **unscheduled** — the M1 pipeline's weapon-model reuse can proceed on the existing (solo, no-budget) process in parallel, but character/VFX/UI art should not be assumed to ship on a specific date until contractor budget (or a confirmed solo-timeline tradeoff) is settled.
